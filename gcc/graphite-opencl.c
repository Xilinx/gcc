/* GRAPHITE-OpenCL pass.
   Copyright (C) 2009, 2010 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* This pass implements transformation of perfect loop nests without
   dependencies to OpenCL kernels.

   For example, loop nest like this

   | for (s_i = lb_i; s_i <= ub_i; s_i += stride_i)
   |  for (s_{i_1} = lb_{i+1}; s_{i+1} <= ub_{i+1}; s_{i+1} += stride_{i+1})
   |   ...
   |    for (s_j = lb_j; s_j <= ub_j; s_j += stride_j)
   |     {
   |       STMT(s_i, s_{i+1}, ..., s_j);
   |     }

   will be transformed to this OpenCL kernel and all required OpenCL
   calls will be created.

   | __kernel void opencl_auto_function_N
   |   (base_i, mod_i, step_i, first_i,
   |    ...,
   |    base_j, mod_j, step_i, first_j,
   |    ...)
   | {
   |   unsigned int global_id = get_global_id (0);
   |   unsigned int s_i = ((global_id / base_i) % mod_i) * step_i + first_i;
   |   unsigned int s_j = ((global_id / base_j) % mod_j) * step_j + first_j;
   |   STMT(s_i, s_{i+1}, ..., s_j);
   | }
*/

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tree-flow.h"
#include "cfgloop.h"
#include "tree-chrec.h"
#include "tree-data-ref.h"
#include "tree-scalar-evolution.h"
#include "tree-pass.h"
#include "sese.h"
#include "output.h"

/* Variable, which holds OpenCL context.  */
static GTY(()) tree h_context;

/* Variable, which holds OpenCL command queue.  */
static GTY(()) tree h_cmd_queue;

/* Variable, which holds OpenCL program for current function.  */
static GTY(()) tree h_program;

#ifdef HAVE_cloog
#include "cloog/cloog.h"
#include "ppl_c.h"
#include "graphite-ppl.h"
#include "graphite-poly.h"
#include "graphite-clast-to-gimple.h"
#include "graphite-dependences.h"
#include "dyn-string.h"
#include "graphite-opencl.h"

/* Data structure to be used in data_reference_p to opencl_data hash
   table.  */
struct map_ref_to_data_def
{
  data_reference_p key;
  opencl_data value;
};

typedef struct map_ref_to_data_def *map_ref_to_data;

/* Calculate hash value from map_ref_to_data.  */

static hashval_t
map_ref_to_data_to_hash (const void *data)
{
  const struct map_ref_to_data_def *obj
    = (const struct map_ref_to_data_def *) data;

  return htab_hash_pointer (obj->key);
}

/* Compare to map_ref_to_data pointers.  */

static int
map_ref_to_data_cmp (const void *v1, const void *v2)
{
  const struct map_ref_to_data_def *obj_1
    = (const struct map_ref_to_data_def *) v1;
  const struct map_ref_to_data_def *obj_2
    = (const struct map_ref_to_data_def *) v2;

  return (obj_1->key == obj_2->key);
}

/* Create new map_ref_to_data with NEW_KEY as key and NEW_VALUE as value.  */

static map_ref_to_data
map_ref_to_data_create (data_reference_p new_key,
                        opencl_data new_value)
{
  map_ref_to_data tmp = XNEW (struct map_ref_to_data_def);

  tmp->key = new_key;
  tmp->value = new_value;
  return tmp;
}

/* Data structure to be used in tree to opencl_data hash table.  */

struct map_tree_to_data_def
{
  tree key;
  opencl_data value;
};

typedef struct map_tree_to_data_def *map_tree_to_data;

/* Calculate hash value from map_tree_to_data.  */

static hashval_t
map_tree_to_data_to_hash (const void *data)
{
  const struct map_tree_to_data_def *obj
    = (const struct map_tree_to_data_def *) data;

  return htab_hash_pointer (obj->key);
}

/* Compare to map_tree_to_data pointers.  */

static int
map_tree_to_data_cmp (const void *v1, const void *v2)
{
  const struct map_tree_to_data_def *obj_1
    = (const struct map_tree_to_data_def *) v1;
  const struct map_tree_to_data_def *obj_2
    = (const struct map_tree_to_data_def *) v2;

  return (obj_1->key == obj_2->key);
}

/* Create new map_tree_to_data with NEW_KEY as key and NEW_VALUE as value.  */

static map_tree_to_data
map_tree_to_data_create (tree new_key,
                         opencl_data new_value)
{
  map_tree_to_data tmp = XNEW (struct map_tree_to_data_def);

  tmp->key = new_key;
  tmp->value = new_value;
  return tmp;
}

/* Create and init new temporary variable with name NAME and
   type TYPE.  */

static tree
opencl_create_tmp_var (tree type, const char *name)
{
  tree tmp = create_tmp_var (type, name);

  TREE_ADDRESSABLE (tmp) = 1;
  return tmp;
}

/* Create new var in basic block DEST to store EXPR and return it.  */

tree
opencl_tree_to_var (basic_block dest, tree expr)
{
  tree type = TREE_TYPE (expr);
  tree var = opencl_create_tmp_var (type, "__ocl_general_tmp_var");
  gimple_stmt_iterator g_iter = gsi_last_bb (dest);

  tree call = build2 (MODIFY_EXPR, type, var, expr);

  force_gimple_operand_gsi (&g_iter, call, true, NULL, false,
                            GSI_CONTINUE_LINKING);

  return var;
}

/* Set rw flags to false for all datas, referenced in CODE_GEN.  */

static void
opencl_fflush_rw_flags (opencl_main code_gen)
{
  VEC (opencl_data, heap) *datas = code_gen->opencl_function_data;
  int i;
  opencl_data curr;

  FOR_EACH_VEC_ELT (opencl_data, datas, i, curr)
    {
      curr->written_in_current_body = false;
      curr->read_in_current_body = false;
      curr->privatized = false;
    }
}

/* Create new basic block on CODE_GEN->main edge and update it.  */

basic_block
opencl_create_bb (opencl_main code_gen)
{
  basic_block tmp = split_edge (code_gen->main_edge);

  code_gen->main_edge = single_succ_edge (tmp);
  return tmp;
}

/* All kernels of current function.  */
static VEC(tree, heap) *opencl_function_kernels;

/* OpenCL code for all kernels of current function.  */
static dyn_string_t main_program_src;

/* Delete clast meta DATA.  */

static void
opencl_clast_meta_delete (opencl_clast_meta data)
{
  if (!data)
    return;

  opencl_clast_meta_delete (data->body);
  opencl_clast_meta_delete (data->next);
  BITMAP_FREE (data->modified_on_device);
  BITMAP_FREE (data->modified_on_host);

  if (data->access != NULL)
    BITMAP_FREE (data->access);

  if (data->can_be_private)
    BITMAP_FREE (data->can_be_private);

  free (data);
}

static inline void
opencl_verify (void)
{
#ifdef ENABLE_CHECKING
  verify_loop_structure ();
  verify_dominators (CDI_DOMINATORS);
  verify_loop_closed_ssa (true);
#endif
}

/* OpenCL definitions.  */
#define CL_CONTEXT_PLATFORM 0x1084
#define CL_CONTEXT_DEVICES 0x1081
#define CL_DEVICE_TYPE_CPU (1 << 1)
#define CL_DEVICE_TYPE_GPU (1 << 2)
#define CL_MEM_COPY_HOST_PTR (1 << 5)
#define CL_MEM_USE_HOST_PTR (1 << 3)
#define CL_MEM_READ_WRITE (1 << 0)
#define CL_MEM_WRITE_ONLY (1 << 1)
#define CL_MEM_READ_ONLY (1 << 2)
#define CL_TRUE 1

#define DEFOPENCLCODE(CODE, FN_NAME) CODE,

/* Enum for all OpenCL functions used in GRAPHITE-OpenCL.  */
enum OPENCL_FUNCTIONS
  {
#include "graphite-opencl-functions.def"
    STATIC_INIT
  };

#undef DEFOPENCLCODE

#define DEFOPENCLCODE(CODE, FN_NAME) FN_NAME,

/* Names of all OpenCL functions, used in GRAPHITE-OpenCL.  */
static const char *opencl_function_names[] =
  {
#include "graphite-opencl-functions.def"
  };

#undef DEFOPENCLCODE

/* This vector holds opencl_data, which represents arrays.
   Arrays have constant sizes, so buffers for each of them can
   be created only once.  */
static VEC (opencl_data, heap) *opencl_array_data;

/* Hash table, which maps opencl_data, related to arrays, to
	  trees, which represents corresponding array.  */
static htab_t array_data_to_tree;

/* Check whether VAR is a zero dimension array.  */

static bool
zero_dim_array_p (tree var)
{
  tree type = TREE_TYPE (var);
  tree domain;
  tree up_bound;

  if (TREE_CODE (type) != ARRAY_TYPE
      || TREE_CODE (TREE_TYPE (type)) == ARRAY_TYPE
      || (domain = TYPE_DOMAIN (type)) == NULL)
    return false;

  up_bound = TYPE_MAX_VALUE (domain);

  if (TREE_CODE (up_bound) != INTEGER_CST)
    return false;

  return TREE_INT_CST_LOW (up_bound) == 0;
}

/* Check whether NAME is the name of the artificial array, which can be
   privatized.  */

static bool
opencl_private_var_name_p (const char *name)
{
  static const char *general_reduction = "General_Reduction";
  static const char *close_phi = "Close_Phi";
  static const char *cross_bb = "Cross_BB_scalar_dependence";
  static const char *commutative = "Commutative_Associative_Reduction";

  if (!name)
    return false;

  return
    ((strstr (name, general_reduction) == name)
     || (strstr (name, close_phi) == name)
     || (strstr (name, commutative) == name)
     || (strstr (name, cross_bb) == name));
}

/* Check whether VAR is an artificial array, which can be privatized.  */

static bool
graphite_artificial_array_p (tree var)
{
  tree name;

  if (TREE_CODE (var) != VAR_DECL
      || !zero_dim_array_p (var)
      || !(name = DECL_NAME (var)))
    return false;

  return opencl_private_var_name_p (IDENTIFIER_POINTER (name));
}

/* Get depth of type TYPE scalar (base) part.  */

static int
opencl_get_non_scalar_type_depth (tree type)
{
  int count = 0;

  while (TREE_CODE (type) == ARRAY_TYPE
         || TREE_CODE (type) == POINTER_TYPE)
    {
      count++;
      type = TREE_TYPE (type);
    }

  return count;
}

/* Constructors & destructors.
   <name>_create - creates a new object of such type and returns it.
   <name>_delete - delete object (like destructor).  */

static opencl_data
opencl_data_create (tree var, tree size)
{
  opencl_data tmp = XNEW (struct opencl_data_def);
  tree type = TREE_TYPE (var);

  tmp->can_be_private = graphite_artificial_array_p (var);
  tmp->exact_object = var;

  tmp->supported = TREE_CODE (var) == VAR_DECL || TREE_CODE (var) == SSA_NAME;

  if (TREE_CODE (type) == ARRAY_TYPE)
    var = build_addr (var, current_function_decl);

  tmp->data_dim = opencl_get_non_scalar_type_depth (type);
  tmp->object = var;

  tmp->size_value = size;
  tmp->size_variable
    = opencl_create_tmp_var (size_type_node, "__opencl_data_size");

  tmp->up_to_date_on_host = true;
  tmp->up_to_date_on_device = true;
  tmp->used_on_device = false;
  tmp->ever_read_on_device = false;
  tmp->ever_written_on_device = false;
  return tmp;
}

static void
opencl_data_delete (opencl_data data)
{
  free (data);
}

static opencl_main
opencl_main_create (CloogNames *names, sese region, edge main_edge,
                    htab_t params_index)
{
  opencl_main tmp = XNEW (struct graphite_opencl_creator);

  tmp->root_names = names;
  tmp->defined_vars = NULL;
  tmp->global_defined_vars = NULL;
  tmp->region = region;
  tmp->main_edge = main_edge;
  tmp->main_program = dyn_string_new (OPENCL_INIT_BUFF_SIZE);
  tmp->current_body = NULL;
  tmp->clast_meta = NULL;
  tmp->curr_meta = NULL;
  tmp->params_index = params_index;
  tmp->newivs_index = htab_create (10, clast_name_index_elt_info,
                                   eq_clast_name_indexes, free);
  tmp->ref_to_data = htab_create (10, map_ref_to_data_to_hash,
                                  map_ref_to_data_cmp, free);
  tmp->tree_to_data = htab_create (10, map_tree_to_data_to_hash,
                                   map_tree_to_data_cmp, free);
  tmp->newivs = VEC_alloc (tree, heap, 10);
  tmp->context_loop = SESE_ENTRY (region)->src->loop_father;
  tmp->opencl_function_data = VEC_alloc (opencl_data, heap,
					 OPENCL_INIT_BUFF_SIZE);
  return tmp;
}

static void
opencl_main_delete (opencl_main data)
{
  int i;
  opencl_data curr;

  dyn_string_delete (data->main_program);
  htab_delete (data->newivs_index);
  htab_delete (data->ref_to_data);
  htab_delete (data->tree_to_data);
  opencl_clast_meta_delete (data->clast_meta);

  FOR_EACH_VEC_ELT (opencl_data, data->opencl_function_data, i, curr)
    if (!curr->is_static)
      opencl_data_delete (curr);

  VEC_free (tree, heap, data->newivs);
  VEC_free (opencl_data, heap, data->opencl_function_data);
  free (data);
}

/* Add function call CALL to edge SRC.  If FLAG_GRAPHITE_OPENCL_DEBUG is
   enabled, then add the following:

   | int result = call ();
   | if (call == 0 != ZERO_RETURN)
   |   abort ();

   Otherwise just add CALL as function call.  */

static edge
opencl_add_safe_call_on_edge (tree call, bool zero_return, edge src)
{
  if (!flag_graphite_opencl_debug)
    {
      basic_block bb = split_edge (src);
      gimple_stmt_iterator g_iter = gsi_last_bb (bb);

      force_gimple_operand_gsi (&g_iter, call, true, NULL, false,
                                GSI_CONTINUE_LINKING);
      return single_succ_edge (bb);
    }
  else
    {
      tree cmp;
      edge result;
      basic_block abort_bb;
      tree abort_funtion;
      tree abort_call;
      gimple_stmt_iterator g_iter;

      if (zero_return)
        {
          tree correct_result = build1 (CONVERT_EXPR, TREE_TYPE (call),
                                        integer_zero_node);
          cmp = build2 (EQ_EXPR, boolean_type_node,
			call, correct_result);
        }
      else
        {
          tree incorrect_result = build1 (CONVERT_EXPR, TREE_TYPE (call),
                                          null_pointer_node);

          cmp = build2 (NE_EXPR, boolean_type_node,
			call, incorrect_result);
        }

      result = create_empty_if_region_on_edge (src, cmp);
      abort_bb =  get_false_edge_from_guard_bb (src->dest)->dest;
      abort_funtion = build_fn_decl ("abort", build_function_type_list
				     (void_type_node, NULL_TREE));

      abort_call = build_call_expr (abort_funtion, 0);

      g_iter = gsi_last_bb (abort_bb);
      force_gimple_operand_gsi (&g_iter, abort_call, true, NULL, false,
                                GSI_CONTINUE_LINKING);
      return result;
    }
}

/* Add call CALL to the current edge from CODE_GEN.  ZERO_RETURN indicates
   whether NULL or 0 is the success return value for CALL.  */

static void
opencl_add_safe_call (opencl_main code_gen, tree call, bool zero_return)
{
  code_gen->main_edge
    = opencl_add_safe_call_on_edge (call, zero_return, code_gen->main_edge);
}

/* Get base object for OBJ.  */

tree
opencl_get_base_object_by_tree (tree obj)
{
  while (TREE_CODE (obj) == INDIRECT_REF
         || TREE_CODE (obj) == ARRAY_REF)
    obj = TREE_OPERAND (obj, 0);

  return obj;
}

/* Get base object from data reference DR.  */

tree
dr_outermost_base_object (data_reference_p dr)
{
  tree addr = DR_BASE_ADDRESS (dr);

  if (!addr)
    {
      /* In case, we don't know base object.  For example:

         | void foo (int *a)
         | {
         |   int *b = a + 1;
         |   *b = 0;
         | }

         Just return the innermost object when the base address is unknown.  */
      tree ref = DR_REF (dr);

      return opencl_get_base_object_by_tree (ref);
    }

  if (TREE_CODE (addr) == ADDR_EXPR)
    addr = TREE_OPERAND (addr, 0);

  return addr;
}

/* Get correct basic block for data with DATA_ID transfer.  If DEVICE
   is true, then it's host to device transfer, otherwise it's device
   to host transfer.  CODE_GEN holds information related to code
   generation.  */

static edge
opencl_get_edge_for_init (opencl_main code_gen, int data_id, bool device)
{
  opencl_clast_meta curr = code_gen->curr_meta;

  if (!curr)
    return NULL;

  while (curr->parent)
    {
      opencl_clast_meta parent = curr->parent;
      bitmap curr_bitmap
	= device ? parent->modified_on_host : parent->modified_on_device;

      if (bitmap_bit_p (curr_bitmap, data_id))
        break;

      curr = curr->parent;
    }

  return curr->init_edge;
}

/* Return tree, which represents function selected by ID.
   If ID is STATIC_INIT, init all required data.  */

static tree
opencl_create_function_decl (enum OPENCL_FUNCTIONS id)
{
  static tree create_context_from_type_decl = NULL;
  static tree get_context_info_decl = NULL;
  static tree create_command_queue_decl = NULL;
  static tree create_program_with_source_decl = NULL;
  static tree build_program_decl = NULL;
  static tree create_kernel_decl = NULL;
  static tree create_buffer_decl = NULL;
  static tree set_kernel_arg_decl = NULL;
  static tree enqueue_nd_range_kernel_decl = NULL;
  static tree enqueue_read_buffer_decl = NULL;
  static tree enqueue_write_buffer_decl = NULL;
  static tree release_memory_obj_decl = NULL;
  static tree release_context_decl = NULL;
  static tree release_command_queue_decl = NULL;
  static tree release_program_decl = NULL;
  static tree release_kernel_decl = NULL;
  static tree get_platform_ids_decl = NULL;
  static tree get_wait_for_events_decl = NULL;

  switch (id)
    {
    case STATIC_INIT:
      {
	tree const_char_type = build_qualified_type (char_type_node,
						     TYPE_QUAL_CONST);
	tree const_char_ptr = build_pointer_type (const_char_type);
	tree const_char_ptr_ptr = build_pointer_type (const_char_ptr);

	tree const_size_t = build_qualified_type (size_type_node,
						  TYPE_QUAL_CONST);
	tree const_size_t_ptr = build_pointer_type (const_size_t);

	tree size_t_ptr = build_pointer_type (size_type_node);

	tree cl_device_type = integer_type_node;
	tree cl_context_info = unsigned_type_node;
	tree cl_command_queue_properties = long_unsigned_type_node;
	tree cl_mem_flags = long_unsigned_type_node;

	tree cl_context = ptr_type_node;
	tree cl_context_properties = ptr_type_node;
	tree cl_command_queue = ptr_type_node;
	tree cl_device_id = ptr_type_node;
	tree cl_program = ptr_type_node;
	tree cl_kernel = ptr_type_node;
	tree cl_event = ptr_type_node;
	tree cl_mem = ptr_type_node;

	tree const_cl_event = build_qualified_type (cl_event,
						    TYPE_QUAL_CONST);
	tree cl_event_ptr = build_pointer_type (cl_event);
	tree const_cl_event_ptr = build_pointer_type (const_cl_event);

	tree const_cl_device_id = build_qualified_type (cl_device_id,
							TYPE_QUAL_CONST);
	tree const_cl_device_id_ptr = build_pointer_type (const_cl_device_id);

	tree cl_platford_id = long_integer_type_node;
	tree cl_platford_id_ptr = build_pointer_type (cl_platford_id);

	tree function_type;
	/* | cl_context
	   | clCreateContextFromType (cl_context_properties *properties,
	   |                          cl_device_type device_type,
	   |                          void (*pfn_notify) (const char *errinfo,
	   |                          const void *private_info, size_t cb,
	   |                          void *user_data),
	   |                          void *user_data,
	   |                          cl_int *errcode_ret)  */
	function_type
	  = build_function_type_list (cl_context,
				      cl_context_properties,
				      cl_device_type,
				      ptr_type_node,
				      ptr_type_node,
				      integer_ptr_type_node,
				      NULL_TREE);
	create_context_from_type_decl
	  = build_fn_decl (opencl_function_names[CREATE_CONTEXT_FROM_TYPE],
			   function_type);

	/* | cl_int clGetContextInfo (cl_context context,
	   |                          cl_context_info param_name,
	   |                          size_t param_value_size,
	   |                          void *param_value,
	   |                          size_t *param_value_size_ret)  */
	function_type
	  = build_function_type_list (integer_type_node,
				      cl_context,
				      cl_context_info,
				      size_type_node,
				      ptr_type_node,
				      size_t_ptr,
				      NULL_TREE);
	get_context_info_decl
	  = build_fn_decl (opencl_function_names[GET_CONTEXT_INFO],
			   function_type);

	/* | cl_command_queue
	   | clCreateCommandQueue (cl_context context,
	   |                       cl_device_id device,
	   |                       cl_command_queue_properties properties,
	   |                       cl_int *errcode_ret)  */
	function_type
	  = build_function_type_list (cl_command_queue,
				      cl_context,
				      cl_device_id,
				      cl_command_queue_properties,
				      integer_ptr_type_node,
				      NULL_TREE);
	create_command_queue_decl
	  = build_fn_decl (opencl_function_names[CREATE_COMMAND_QUEUE],
			   function_type);

	/* | cl_program clCreateProgramWithSource (cl_context context,
	   |                                       cl_uint count,
	   |                                       const char **strings,
	   |                                       const size_t *lengths,
	   |                                       cl_int *errcode_ret)  */
	function_type
	  = build_function_type_list (cl_program,
				      cl_context,
				      unsigned_type_node,
				      const_char_ptr_ptr,
				      const_size_t_ptr,
				      integer_ptr_type_node,
				      NULL_TREE);
	create_program_with_source_decl
	  = build_fn_decl (opencl_function_names[CREATE_PROGRAM_WITH_SOURCE],
			   function_type);

	/* | cl_int
	   | clBuildProgram (cl_program program,
	   |                 cl_uint num_devices,
	   |                 const cl_device_id *device_list,
	   |                 const char *options,
	   |                 void (*pfn_notify) (cl_program, void *user_data),
	   |                 void *user_data)  */
	function_type
	  = build_function_type_list (integer_type_node,
				      cl_program,
				      unsigned_type_node,
				      const_cl_device_id_ptr,
				      const_char_ptr,
				      ptr_type_node,
				      ptr_type_node,
				      NULL_TREE);
	build_program_decl
	  = build_fn_decl (opencl_function_names[BUILD_PROGRAM],
			   function_type);

	/* | cl_kernel clCreateKernel (cl_program program,
	   |                           const char *kernel_name,
	   |                           cl_int *errcode_ret)  */
	function_type
	  = build_function_type_list (cl_kernel,
				      cl_program,
				      const_char_ptr,
				      integer_ptr_type_node,
				      NULL_TREE);

	create_kernel_decl
	  = build_fn_decl (opencl_function_names[CREATE_KERNEL],
			   function_type);

	/* | cl_mem clCreateBuffer (cl_context context,
	   |                        cl_mem_flags flags,
	   |                        size_t size,
	   |                        void *host_ptr,
	   |                        cl_int *errcode_ret)  */

	function_type
	  = build_function_type_list (cl_mem,
				      cl_context,
				      cl_mem_flags,
				      size_type_node,
				      ptr_type_node,
				      integer_ptr_type_node,
				      NULL_TREE);
	create_buffer_decl
	  = build_fn_decl (opencl_function_names[CREATE_BUFFER],
			   function_type);


	/* | cl_int clSetKernelArg (cl_kernel kernel,
	   |                        cl_uint arg_index,
	   |                        size_t arg_size,
	   |                        const void *arg_value)  */

	function_type
	  = build_function_type_list (integer_type_node,
				      cl_kernel,
				      unsigned_type_node,
				      size_type_node,
				      const_ptr_type_node,
				      NULL_TREE);
	set_kernel_arg_decl
	  = build_fn_decl (opencl_function_names[SET_KERNEL_ARG],
			   function_type);

	/* | cl_int clEnqueueNDRangeKernel (cl_command_queue command_queue,
	   |                                cl_kernel kernel,
	   |                                cl_uint work_dim,
	   |                                const size_t *global_work_offset,
	   |                                const size_t *global_work_size,
	   |                                const size_t *local_work_size,
	   |                                cl_uint num_events_in_wait_list,
	   |                                const cl_event *event_wait_list,
	   |                                cl_event *event)  */

	function_type
	  = build_function_type_list (integer_type_node,
				      cl_command_queue,
				      cl_kernel,
				      unsigned_type_node,
				      const_size_t_ptr,
				      const_size_t_ptr,
				      const_size_t_ptr,
				      unsigned_type_node,
				      const_cl_event_ptr,
				      cl_event_ptr,
				      NULL_TREE);

	enqueue_nd_range_kernel_decl
	  = build_fn_decl (opencl_function_names[ENQUEUE_ND_RANGE_KERNEL],
			   function_type);

	/* | cl_int clEnqueueReadBuffer (cl_command_queue command_queue,
	   |                             cl_mem buffer,
	   |                             cl_bool blocking_read,
	   |                             size_t offset,
	   |                             size_t cb,
	   |                             void *ptr,
	   |                             cl_uint num_events_in_wait_list,
	   |                             const cl_event *event_wait_list,
	   |                             cl_event *event)  */

	function_type
	  = build_function_type_list (integer_type_node,
				      cl_command_queue,
				      cl_mem,
				      unsigned_type_node,
				      size_type_node,
				      size_type_node,
				      ptr_type_node,
				      unsigned_type_node,
				      const_cl_event_ptr,
				      cl_event_ptr,
				      NULL_TREE);

	enqueue_read_buffer_decl
	  = build_fn_decl (opencl_function_names[ENQUEUE_READ_BUFFER],
			   function_type);

	/* | cl_int clEnqueueWriteBuffer (cl_command_queue command_queue,
	   |                              cl_mem buffer,
	   |                              cl_bool blocking_write,
	   |                              size_t offset,
	   |                              size_t cb,
	   |                              const void *ptr,
	   |                              cl_uint num_events_in_wait_list,
	   |                              const cl_event *event_wait_list,
	   |                              cl_event *event)  */

	function_type
	  = build_function_type_list (integer_type_node,
				      cl_command_queue,
				      cl_mem,
				      unsigned_type_node,
				      size_type_node,
				      size_type_node,
				      const_ptr_type_node,
				      unsigned_type_node,
				      const_cl_event_ptr,
				      cl_event_ptr,
				      NULL_TREE);

	enqueue_write_buffer_decl
	  = build_fn_decl (opencl_function_names[ENQUEUE_WRITE_BUFFER],
			   function_type);


	/* cl_int clReleaseMemObject (cl_mem memobj)  */

	function_type
	  = build_function_type_list (integer_type_node, cl_mem, NULL_TREE);

	release_memory_obj_decl
	  = build_fn_decl (opencl_function_names[RELEASE_MEMORY_OBJ],
			   function_type);


	/* cl_int clReleaseContext (cl_context context)  */
	function_type
	  = build_function_type_list (integer_type_node, cl_context,
				      NULL_TREE);

	release_context_decl
	  = build_fn_decl (opencl_function_names[RELEASE_CONTEXT],
			   function_type);

	/* cl_int clReleaseCommandQueue (cl_command_queue command_queue)  */
	function_type
	  = build_function_type_list (integer_type_node, cl_command_queue,
				      NULL_TREE);

	release_command_queue_decl
	  = build_fn_decl (opencl_function_names[RELEASE_COMMAND_QUEUE],
			   function_type);

	/* cl_int clReleaseProgram (cl_program program)  */
	function_type
	  = build_function_type_list (integer_type_node, cl_program,
				      NULL_TREE);

	release_program_decl
	  = build_fn_decl (opencl_function_names[RELEASE_PROGRAM],
			   function_type);

	/* cl_int clReleaseKernel (cl_kernel kernel)  */
	function_type
	  = build_function_type_list (integer_type_node, cl_kernel, NULL_TREE);

	release_kernel_decl
	  = build_fn_decl (opencl_function_names[RELEASE_KERNEL],
			   function_type);

	/* | cl_int clGetPlatformIDs (cl_uint num_entries,
	   |                          cl_platform_id *platforms,
	   |                          cl_uint *num_platforms)  */


	function_type
	  = build_function_type_list (integer_type_node,
				      unsigned_type_node,
				      cl_platford_id_ptr,
				      build_pointer_type (unsigned_type_node),
				      NULL_TREE);
	get_platform_ids_decl
	  = build_fn_decl (opencl_function_names [GET_PLATFORM_IDS],
			   function_type);


	/* | cl_int clWaitForEvents (cl_uint num_events,
	   |                         const cl_event *event_list)  */

	function_type
	  = build_function_type_list (integer_type_node,
				      unsigned_type_node,
				      const_cl_event_ptr,
				      NULL_TREE);

	get_wait_for_events_decl
	  = build_fn_decl (opencl_function_names [WAIT_FOR_EVENTS],
			   function_type);

	return NULL_TREE;
      }

    case CREATE_CONTEXT_FROM_TYPE:
      return create_context_from_type_decl;

    case GET_CONTEXT_INFO:
      return get_context_info_decl;

    case CREATE_COMMAND_QUEUE:
      return create_command_queue_decl;

    case CREATE_PROGRAM_WITH_SOURCE:
      return create_program_with_source_decl;

    case BUILD_PROGRAM:
      return build_program_decl;

    case CREATE_KERNEL:
      return create_kernel_decl;

    case CREATE_BUFFER:
      return create_buffer_decl;

    case SET_KERNEL_ARG:
      return set_kernel_arg_decl;

    case ENQUEUE_ND_RANGE_KERNEL:
      return enqueue_nd_range_kernel_decl;

    case ENQUEUE_READ_BUFFER:
      return enqueue_read_buffer_decl;

    case ENQUEUE_WRITE_BUFFER:
      return enqueue_write_buffer_decl;

    case RELEASE_MEMORY_OBJ:
      return release_memory_obj_decl;

    case RELEASE_CONTEXT:
      return release_context_decl;

    case RELEASE_COMMAND_QUEUE:
      return release_command_queue_decl;

    case RELEASE_PROGRAM:
      return release_program_decl;

    case RELEASE_KERNEL:
      return release_kernel_decl;

    case GET_PLATFORM_IDS:
      return get_platform_ids_decl;

    case WAIT_FOR_EVENTS:
      return get_wait_for_events_decl;

    default: gcc_unreachable ();
    }
}

/* Add clWaitForEvent (1, EVENT_VAR); call to CODE_GEN->main_edge.  */

static void
opencl_wait_for_event (opencl_main code_gen, tree event_var)
{
  tree function = opencl_create_function_decl (WAIT_FOR_EVENTS);
  tree call = build_call_expr (function, 2,
                               integer_one_node,
                               event_var);

  opencl_add_safe_call (code_gen, call, true);
}

/* Add host to device memory transfer.  DATA - data, which must be
   transfered to device.  CODE_GEN holds information related to code
   generation.  */

static tree
opencl_pass_to_device (opencl_main code_gen, opencl_data data)
{
  edge init_edge;
  tree function;
  tree call;

  tree index_type = build_index_type (build_int_cst (NULL_TREE, 2));
  tree array_type = build_array_type (ptr_type_node, index_type);
  tree var = opencl_create_tmp_var (array_type, "wait_event");
  tree event_call;

  TREE_STATIC (var) = 1;
  assemble_variable (var, 1, 0, 1);

  /* If data is wrutten in device, mark it is not up to date on host.  */
  if (data->written_in_current_body)
    data->up_to_date_on_host = false;

  /* If data is up to date on device, but it was initialized befor
     current loop, then mark it as initialized in current loop and
     store it.

     Consider an example: D - device, H - host, W - write, R - read.

     | HW(1) -- LOOP
     |          /\
     |         /  \
     |        /    \
     |      HR(2)  DW(3)

     While analyzing statement (2), data will be up to date on host
     because of statement (1), but while executing after (3) in loop,
     (2) will read incorrect data.

     So, we have to add device to host memory transfer after statement (3).
  */
  if (flag_graphite_opencl_cpu)
    return data->device_object;

  if (data->up_to_date_on_device)
    {
      if (!data->inited_in_current_loop_on_device
	  && code_gen && code_gen->curr_meta
	  && code_gen->curr_meta->parent)
	VEC_safe_push (opencl_data, heap,
		       code_gen->curr_meta->parent->post_pass_to_device,
		       data);

      data->inited_in_current_loop_on_device = true;
      return data->device_object;
    }

  data->inited_in_current_loop_on_device = true;
  init_edge = opencl_get_edge_for_init (code_gen, data->id, true);

  /* Add gimple.  */
  function = opencl_create_function_decl (ENQUEUE_WRITE_BUFFER);

  event_call = build4 (ARRAY_REF, ptr_type_node, var,
                       integer_zero_node, NULL_TREE, NULL_TREE);
  event_call = build_addr (event_call, current_function_decl);
  call = build_call_expr (function, 9,
                          h_cmd_queue,
                          data->device_object,
                          build_int_cst (NULL_TREE, CL_TRUE),
                          integer_zero_node,
                          data->size_variable,
                          data->object,
                          integer_zero_node,
                          null_pointer_node,
                          event_call);

  if (init_edge)
    opencl_add_safe_call_on_edge (call, true, init_edge);
  else
    opencl_add_safe_call (code_gen, call, true);

  data->up_to_date_on_device = true;
  opencl_wait_for_event (code_gen, event_call);
  return data->device_object;
}

/* Add device to host memory transfer.  DATA - data, which must be
   transfered to host.  CODE_GEN holds information related to code
   generation.  */

static void
opencl_pass_to_host (opencl_main code_gen, opencl_data data)
{
  edge init_edge;
  tree function;
  tree curr_type;
  tree curr;
  tree call;
  tree index_type = build_index_type (build_int_cst (NULL_TREE, 2));
  tree array_type = build_array_type (ptr_type_node, index_type);
  tree var = opencl_create_tmp_var (array_type, "wait_event");
  tree event_call;

  TREE_STATIC (var) = 1;
  assemble_variable (var, 1, 0, 1);

  if (data->written_in_current_body)
    data->up_to_date_on_device = false;

  if (data->up_to_date_on_host)
    {
      if (!data->inited_in_current_loop_on_host
	  && code_gen && code_gen->curr_meta &&
	  code_gen->curr_meta->parent)
	VEC_safe_push (opencl_data, heap,
		       code_gen->curr_meta->parent->post_pass_to_host, data);

      data->inited_in_current_loop_on_host = true;
      return;
    }

  data->inited_in_current_loop_on_host = true;

  if (flag_graphite_opencl_cpu
      || data->privatized)
    return;

  init_edge = opencl_get_edge_for_init (code_gen, data->id, false);

  function = opencl_create_function_decl (ENQUEUE_READ_BUFFER);
  curr_type =  TREE_TYPE (data->object);
  curr = data->object;

  if (TREE_CODE (curr_type) == ARRAY_TYPE)
    curr = build_addr (curr, current_function_decl);

  event_call = build4 (ARRAY_REF, ptr_type_node, var,
                       integer_zero_node, NULL_TREE, NULL_TREE);
  event_call = build_addr (event_call, current_function_decl);

  call = build_call_expr (function, 9,
                          h_cmd_queue,
                          data->device_object,
                          build_int_cst (NULL_TREE, CL_TRUE),
                          integer_zero_node,
                          data->size_variable,
                          curr, integer_zero_node,
                          null_pointer_node,
                          event_call);

  if (init_edge)
    opencl_add_safe_call_on_edge (call, true, init_edge);
  else
    opencl_add_safe_call (code_gen, call, true);

  opencl_wait_for_event (code_gen, event_call);
  data->up_to_date_on_host = true;
}

/* Pass all data from device to host.  This function must be called when
   we need all data to be up to date on host.  CODE_GEN holds information
   related to code generation.  */

static void
opencl_fflush_all_device_buffers_to_host (opencl_main code_gen)
{
  VEC (opencl_data, heap) *datas = code_gen->opencl_function_data;
  int i;
  opencl_data curr;
  tree function = opencl_create_function_decl (RELEASE_MEMORY_OBJ);

  FOR_EACH_VEC_ELT (opencl_data, datas, i, curr)
    {
      curr->written_in_current_body = true;
      opencl_pass_to_host (code_gen, curr);
    }

  FOR_EACH_VEC_ELT (opencl_data, datas, i, curr)
    if (curr->used_on_device && !curr->is_static)
      {
	tree var = curr->device_object;
	tree call = build_call_expr (function, 1, var);

	opencl_add_safe_call (code_gen, call, true);
      }
}

/* Calculate correct flags for clCreateBuffer.  READ means, that
   buffer must be readable on device, WRITE - that buffer must be
   writable on device.  */

static int
opencl_get_mem_flags (bool read, bool write)
{
  int rw_flags;
  int location_flags;

  gcc_assert (read || write);

  if (write && read)
    rw_flags = CL_MEM_READ_WRITE;
  else
    {
      if (read)
        rw_flags = CL_MEM_READ_ONLY;
      else
        rw_flags = CL_MEM_WRITE_ONLY;
    }

  if (flag_graphite_opencl_cpu)
    location_flags = CL_MEM_USE_HOST_PTR;
  else
    location_flags = CL_MEM_COPY_HOST_PTR;

  return location_flags | rw_flags;
}

/* Create memory on device for DATA and init it by data from host.
   ptr is pointer to host memory location.  Function returns tree,
   corresponding to memory location on device.  */

static tree
opencl_create_memory_for_pointer (opencl_data data)
{
  tree ptr = data->object;
  tree arr_size = data->size_variable;
  tree function = opencl_create_function_decl (CREATE_BUFFER);
  bool ever_read = data->ever_read_on_device;
  bool ever_written = data->ever_written_on_device;
  tree mem_flags = build_int_cst (NULL_TREE,
				  opencl_get_mem_flags (ever_read,
							ever_written));
  if (TREE_CODE (TREE_TYPE (ptr)) == ARRAY_TYPE)
    ptr = build_addr (ptr, current_function_decl);

  if (flag_graphite_opencl_debug)
    {
      tree result = opencl_create_tmp_var (integer_type_node,
                                           "__opencl_create_buffer_result");

      return build_call_expr (function, 5,
                              h_context,  mem_flags,
                              arr_size, ptr,
                              build1 (ADDR_EXPR,
                                      integer_ptr_type_node,
                                      result));
    }
  else
    return build_call_expr (function, 5,
                            h_context,  mem_flags,
                            arr_size, ptr, null_pointer_node);
}

/* Create memory buffers on host for all required host memory objects.
   CODE_GEN holds information related to code generation.  */

static void
opencl_init_all_device_buffers (opencl_main code_gen)
{
  VEC (opencl_data, heap) *datas = code_gen->opencl_function_data;
  int i;
  opencl_data curr;
  edge data_init_edge = single_succ_edge (code_gen->data_init_bb);

  FOR_EACH_VEC_ELT (opencl_data, datas, i, curr)
    {
      tree tmp;

      if (!curr->used_on_device || curr->is_static)
        continue;

      tmp = opencl_create_memory_for_pointer (curr);
      tmp = build2 (MODIFY_EXPR, ptr_type_node, curr->device_object, tmp);
      data_init_edge = opencl_add_safe_call_on_edge (tmp, false,
                                                     data_init_edge);
    }
}

/* Create new static void * variable with name __ocl_ + NAME.  */

static tree
opencl_create_static_ptr_variable (const char *name)
{
  const char *id_name = concat ("__ocl_",name,  NULL);
  tree var =  build_decl (UNKNOWN_LOCATION, VAR_DECL,
                          create_tmp_var_name (id_name), ptr_type_node);
  TREE_STATIC (var) = 1;
  TREE_PUBLIC (var) = 0;
  DECL_ARTIFICIAL (var) = 1;
  TREE_USED (var) = 1;
  TREE_ADDRESSABLE (var) = 1;
  DECL_INITIAL (var) = null_pointer_node;
  assemble_variable (var, 1, 0, 1);

  return var;
}

/* Insert several opencl calls to output program.  */

/* | cl_program h_program;
   | h_program = clCreateProgramWithSource (h_context, 1,
   | 				          sProgramSource, 0, 0);

   SRC is the program source code and DEST is the edge where
   call must be inserted.  */

static edge
opencl_insert_create_program_with_source_call (const char *src, edge base)
{
  /* Get tree with function definition.  */
  tree function = opencl_create_function_decl (CREATE_PROGRAM_WITH_SOURCE);
  tree code_tree = build_string_literal (strlen (src) + 1, src);
  tree call;
  basic_block bb = split_edge (base);
  tree tmp_var = opencl_tree_to_var (bb, code_tree);

  call = build_call_expr (function, 5, h_context,
                          integer_one_node,
                          build_addr (tmp_var, current_function_decl),
                          null_pointer_node,
                          null_pointer_node);

  call = build2 (MODIFY_EXPR, ptr_type_node,
                 h_program, call);
  return opencl_add_safe_call_on_edge (call, false, single_succ_edge (bb));
}

/* clBuildProgram (h_program, 0, 0, 0, 0, 0);
   BASE is the edge where call must be inserted.  */

static edge
opencl_insert_build_program_call (edge base)
{
  tree function = opencl_create_function_decl (BUILD_PROGRAM);
  tree call = build_call_expr (function, 6,
                               h_program,
                               integer_zero_node, null_pointer_node,
                               null_pointer_node, null_pointer_node,
                               null_pointer_node);
  return opencl_add_safe_call_on_edge (call, true, base);
}

/* cl_kernel tmm_kernel;
   tmp_kernel = clCreateKernel (h_program, func_name, 0);
   FUNCTION_NAME is the name of the kernel function,
   CODE_GEN holds information related to code generation.  */

static tree
opencl_insert_create_kernel_call (opencl_main code_gen,
				  const char *function_name)
{
  tree new_kernel_var;
  basic_block bb;
  gimple_stmt_iterator g_iter;
  tree function;
  tree kernel_name;
  tree call;
  tree tmp_tree;

  new_kernel_var = opencl_create_tmp_var (ptr_type_node, function_name);
  bb = split_edge (code_gen->kernel_edge);
  g_iter = gsi_last_bb (bb);
  function = opencl_create_function_decl (CREATE_KERNEL);
  kernel_name = build_string_literal (strlen (function_name) + 1,
                                      function_name);
  call = build_call_expr (function, 3, h_program, kernel_name,
                          null_pointer_node);
  tmp_tree = build2 (MODIFY_EXPR, ptr_type_node,
                     new_kernel_var, call);

  code_gen->kernel_edge = single_succ_edge (bb);
  force_gimple_operand_gsi (&g_iter, tmp_tree, true, NULL, false,
                            GSI_CONTINUE_LINKING);
  VEC_safe_push (tree, heap, opencl_function_kernels, new_kernel_var);
  code_gen->kernel_edge
    = opencl_add_safe_call_on_edge (new_kernel_var, false,
				    code_gen->kernel_edge);
  return new_kernel_var;
}

/* Init memory on device.  Only one levell of pointers are suppoted.
   So in case of char ** only array of char * will be created.
   Function return tree, corresponding to new pointer (pointer
   on device).

   | cl_mem clCreateBuffer (cl_context context,
   |                        cl_mem_flags flags,
   |                        size_t size,
   |                        void *host_ptr,
   |                        cl_int *errcode_ret)  */

/* Calculate size of data reference, represented by REF.  PTR is a
   base object of data reference.  */

static tree
opencl_get_indirect_size (tree ptr, poly_dr_p ref)
{
  ptr = TREE_TYPE (ptr);

  switch (TREE_CODE (ptr))
    {
    case ARRAY_TYPE:
      return TYPE_SIZE_UNIT (ptr);

    case POINTER_TYPE:
      {
	tree inner_type = TREE_TYPE (ptr);
	tree t = graphite_outer_subscript_bound (ref, false);
	tree inner_type_size = TYPE_SIZE_UNIT (inner_type);

	if (inner_type_size == NULL)
	  return NULL;

	if (DECL_P (inner_type_size))
	  add_referenced_var (inner_type_size);

	gcc_assert (t);
	t = fold_build2 (TRUNC_DIV_EXPR, sizetype, t, inner_type_size);
	t = fold_build2 (PLUS_EXPR, sizetype, t, size_one_node);
	t = fold_build2 (MULT_EXPR, sizetype, t, inner_type_size);
	return t;
      }

    default:
      return NULL_TREE;
    }

  gcc_unreachable ();
}

/* Create variables for kernel KERNEL arguments.  Each argument is
   represented by new variable with it's value and it's size.  If arg
   is a pointer or array, it's represented by device buffer with data
   from host memory.  CODE_GEN holds information related to code
   generation.  */

static void
opencl_init_local_device_memory (opencl_main code_gen, opencl_body kernel)
{
  VEC (tree, heap) **args = &kernel->function_args;
  VEC (tree, heap) **args_to_pass = &kernel->function_args_to_pass;
  VEC (opencl_data, heap) **refs = &kernel->data_refs;
  tree curr;
  opencl_data curr_data;
  int i;
  basic_block bb = opencl_create_bb (code_gen);
  basic_block kernel_bb = split_edge (code_gen->kernel_edge);

  code_gen->kernel_edge = single_succ_edge (kernel_bb);

  FOR_EACH_VEC_ELT (tree, *args, i, curr)
    {
      gimple_stmt_iterator g_iter = gsi_last_bb (bb);
      gimple_stmt_iterator kernel_g_iter = gsi_last_bb (kernel_bb);
      tree curr_type = TREE_TYPE (curr);
      tree new_type;
      tree tmp_var;
      tree mov;
      tree curr_var = opencl_create_tmp_var (curr_type, "__ocl_iv");

      if (TREE_CODE (curr) != PARM_DECL
          && TREE_CODE (curr) != VAR_DECL)
        {
          mov = build2 (MODIFY_EXPR, curr_type, curr_var, curr);

          force_gimple_operand_gsi (&g_iter, mov, false, NULL, false,
                                    GSI_CONTINUE_LINKING);
        }
      else
	force_gimple_operand_gsi (&g_iter, curr, false, curr_var, false,
				  GSI_CONTINUE_LINKING);
      curr = curr_var;

      new_type = build_pointer_type (curr_type);
      tmp_var = opencl_create_tmp_var (new_type, "__opencl_scalar_arg");
      mov = build1 (ADDR_EXPR, new_type, curr);

      mov = build2 (MODIFY_EXPR, new_type, tmp_var, mov);

      force_gimple_operand_gsi (&kernel_g_iter, mov, false, NULL, false,
                                GSI_CONTINUE_LINKING);
      VEC_safe_push (tree, heap, *args_to_pass, tmp_var);
    }

  FOR_EACH_VEC_ELT (opencl_data, *refs, i, curr_data)
    {
      gimple_stmt_iterator kernel_g_iter = gsi_last_bb (kernel_bb);
      tree new_type;
      tree tmp_var;
      tree mov;
      tree curr = opencl_pass_to_device (code_gen, curr_data);
      tree curr_type = ptr_type_node;

      new_type = build_pointer_type (curr_type);
      tmp_var = opencl_create_tmp_var (new_type, "__opencl_non_scalar_arg");
      mov = build1 (ADDR_EXPR, new_type, curr);

      mov = build2 (MODIFY_EXPR, new_type, tmp_var, mov);

      force_gimple_operand_gsi (&kernel_g_iter, mov, false, NULL, false,
                                GSI_CONTINUE_LINKING);
      VEC_safe_push (tree, heap, *args_to_pass, tmp_var);

    }
}

/* cl_int clSetKernelArg (cl_kernel kernel,
   cl_uint arg_index,
   size_t arg_size,
   const void *arg_value)

   Set all kernel args for OpenCL kernel, represented by KERNEL_VAR.
   KERNEL holds all data, related to given kernel.
   CODE_GEN holds information related to code generation.
   All arguments are passed by pointer.  */

static void
opencl_pass_kernel_arguments (opencl_main code_gen, opencl_body kernel,
                              tree kernel_var)
{
  VEC (tree, heap) *args_to_pass = kernel->function_args_to_pass;
  tree arg;
  int i;
  tree function = opencl_create_function_decl (SET_KERNEL_ARG);

  FOR_EACH_VEC_ELT (tree, args_to_pass, i, arg)
    {
      tree call
	= build_call_expr (function, 4, kernel_var,
			   build_int_cst (NULL_TREE, i),
			   TYPE_SIZE_UNIT (TREE_TYPE (TREE_TYPE (arg))),
			   arg);
      opencl_add_safe_call (code_gen, call, true);
    }
}

/* clEnqueueNDRangeKernel (h_cmd_queue, hKernel, 1, 0,
   &cnDimension, 0, 0, 0, 0);

   Execute kernel, represented by KERNEL_VAR in NUM_OF_EXEC threads.
   Use EVENT_VAR as event variable for asynchronous call.
   CODE_GEN holds information related to code generation.  */

static void
opencl_execute_kernel (opencl_main code_gen, tree num_of_exec,
                       tree kernel_var, tree event_var)
{
  tree function = opencl_create_function_decl (ENQUEUE_ND_RANGE_KERNEL);
  tree num_of_threads = opencl_create_tmp_var (integer_type_node,
					       "__opencl_num_of_exec");
  gimple_stmt_iterator g_iter = gsi_last_bb (opencl_create_bb (code_gen));
  tree call;

  TREE_STATIC (num_of_threads) = 1;
  assemble_variable (num_of_threads, 1, 0, 1);

  call = build2 (MODIFY_EXPR, integer_type_node, num_of_threads, num_of_exec);

  force_gimple_operand_gsi (&g_iter, call, true, NULL, false,
                            GSI_CONTINUE_LINKING);

  call = build1 (ADDR_EXPR, integer_ptr_type_node, num_of_threads);

  call = build_call_expr (function, 9,
                          h_cmd_queue,
                          kernel_var,
                          integer_one_node,
                          null_pointer_node,
                          call,
                          null_pointer_node,
                          integer_zero_node,
                          null_pointer_node,
                          event_var);

  opencl_add_safe_call (code_gen, call, true);
}

/* Place building program from single source string to edge BASE.
   Current implementation performs single build per function.
   String contains kernels from all scops of current function.
   Functions returns true if any kernel has been created.  */

static edge
opencl_create_function_call (edge base)
{
  edge new_edge;
  const char *src;

  /* Required for addressing types with size less then 4 bytes.  */
  dyn_string_prepend_cstr
    (main_program_src,
     "#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable\n");
  /* Required for double type.  */
  dyn_string_prepend_cstr (main_program_src,
			   "#if defined(cl_khr_fp64)\n"
			   "#pragma OPENCL EXTENSION cl_khr_fp64 : enable\n"
			   "#elif defined(cl_amd_fp64)\n"
			   "#pragma OPENCL EXTENSION cl_amd_fp64 : enable\n"
			   "#endif\n");

  src = dyn_string_buf (main_program_src);

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "\nGenerated OpenCL code: \n");
      fprintf (dump_file, "%s", src);
    }

  new_edge = opencl_insert_create_program_with_source_call (src, base);

  return opencl_insert_build_program_call (new_edge);
}

/* Mark privatizable data for current loop nest.  Information where
   given data can be privatized is taken from meta information of
   current loop nest, which is stored in CODE_GEN.  */

static void
opencl_mark_privatized_data (opencl_main code_gen)
{
  VEC (opencl_data, heap) *datas = code_gen->opencl_function_data;
  int i;
  opencl_data curr;
  bitmap can_be_private = code_gen->curr_meta->can_be_private;

  FOR_EACH_VEC_ELT (opencl_data, datas, i, curr)
    curr->privatized = bitmap_bit_p (can_be_private, curr->id);
}

/* Store calculated sizes of all pointers or arrays to variables.
   CODE_GEN holds information related to code generation.  */

static void
opencl_set_data_size (opencl_main code_gen)
{
  VEC (opencl_data, heap) *datas = code_gen->opencl_function_data;
  int i;
  opencl_data curr;
  gimple_stmt_iterator g_iter = gsi_last_bb (code_gen->data_init_bb);

  FOR_EACH_VEC_ELT (opencl_data, datas, i, curr)
    {
      tree call;

      if (curr->is_static
	  || !curr->used_on_device
	  || curr->size_value == NULL)
        continue;

      call = build2 (MODIFY_EXPR, size_type_node,
                     curr->size_variable, curr->size_value);

      force_gimple_operand_gsi (&g_iter, call, true, NULL, false,
                                GSI_CONTINUE_LINKING);
    }
}

/* Find opencl_data which represents array VAR.  */

static opencl_data
opencl_get_static_data_by_tree (tree var)
{
  map_tree_to_data tmp = map_tree_to_data_create (var, NULL);
  map_tree_to_data *slot
    = (map_tree_to_data *) htab_find_slot (array_data_to_tree,
					   tmp, INSERT);
  if (*slot == NULL)
    return NULL;

  return (*slot)->value;

}

/* Create required OpenCL variable for given DATA.  */

static void
opencl_data_init_object (opencl_data data)
{
  if (TREE_CODE (TREE_TYPE (data->exact_object)) == POINTER_TYPE)
    {
      data->device_object
	= opencl_create_tmp_var (ptr_type_node, "__opencl_data");
      data->is_static = false;
    }
  else
    {
      /* (TREE_CODE (TREE_TYPE (data->exact_object)) == ARRAY_TYPE) */
      map_tree_to_data tree_ptr
	= map_tree_to_data_create (data->exact_object, data);
      map_tree_to_data *tree_slot
	= (map_tree_to_data *) htab_find_slot (array_data_to_tree,
					       tree_ptr, INSERT);

      gcc_assert (*tree_slot == NULL);
      *tree_slot = tree_ptr;

      data->device_object
	= opencl_create_static_ptr_variable ("__opencl_data");
      data->is_static = true;
      data->size_variable = data->size_value;
      VEC_safe_push (opencl_data, heap, opencl_array_data, data);
    }
}

/* Register reference to DATA via data reference REF_KEY and
   variable TREE_KEY in CODE_GEN structures.  */

static void
opencl_register_data (opencl_main code_gen, opencl_data data,
                      tree tree_key, data_reference_p ref_key)
{
  htab_t ref_to_data = code_gen->ref_to_data;
  htab_t tree_to_data = code_gen->tree_to_data;
  map_ref_to_data ref_ptr = map_ref_to_data_create (ref_key, data);
  map_tree_to_data tree_ptr = map_tree_to_data_create (tree_key, data);
  map_ref_to_data *ref_slot;
  map_tree_to_data *tree_slot;

  ref_slot
    = (map_ref_to_data *) htab_find_slot (ref_to_data, ref_ptr, INSERT);
  gcc_assert (*ref_slot == NULL);
  *ref_slot = ref_ptr;

  tree_slot
    = (map_tree_to_data *) htab_find_slot (tree_to_data, tree_ptr, INSERT);
  gcc_assert (*tree_slot == NULL || (*tree_slot)->value == data);
  *tree_slot = tree_ptr;
}

/* Analyze single data reference REF and update CODE_GEN structures.
   If it access data, which has been accessed in data references
   before, update it's size.  Otherwise add data to array.  */

static void
opencl_parse_single_data_ref (poly_dr_p ref, opencl_main code_gen)
{
  data_reference_p d_ref = (data_reference_p) PDR_CDR (ref);
  tree data_ref_tree = dr_outermost_base_object (d_ref);
  opencl_data curr = opencl_get_data_by_tree (code_gen, data_ref_tree);
  tree size = opencl_get_indirect_size (data_ref_tree, ref);

  if (curr)
    {
      if (!curr->is_static)
        {
          if (!size || !curr->size_value)
            curr->size_value = NULL;
          else
            curr->size_value = fold_build2 (MAX_EXPR, sizetype,
                                            size, curr->size_value);
        }
    }
  else
    {
      curr = opencl_get_static_data_by_tree (data_ref_tree);

      if (!curr)
        {
          curr = opencl_data_create (data_ref_tree, size);
          opencl_data_init_object (curr);
        }

      curr->id = VEC_length (opencl_data, code_gen->opencl_function_data);
      VEC_safe_push (opencl_data, heap, code_gen->opencl_function_data, curr);
    }

  opencl_register_data (code_gen, curr, data_ref_tree, d_ref);
}

/* Analyse all data reference for poly basic block PBB and update CODE_GEN
   structures.  */

static void
opencl_parse_data_refs (poly_bb_p pbb, opencl_main code_gen)
{
  VEC (poly_dr_p, heap) *drs = PBB_DRS (pbb);
  int i;
  poly_dr_p curr;

  FOR_EACH_VEC_ELT (poly_dr_p, drs, i, curr)
    opencl_parse_single_data_ref (curr, code_gen);
}

/* Analyse all data reference for scop M_SCOP and update
   CODE_GEN structures.  */

static void
opencl_init_data (scop_p m_scop, opencl_main code_gen)
{
  VEC (poly_bb_p, heap) *bbs = SCOP_BBS (m_scop);
  int i;
  poly_bb_p curr;

  FOR_EACH_VEC_ELT (poly_bb_p, bbs, i, curr)
    opencl_parse_data_refs (curr, code_gen);
}

/* Init basic block in CODE_GEN structures.  */

static void
opencl_init_basic_blocks (opencl_main code_gen)
{
  code_gen->data_init_bb = opencl_create_bb (code_gen);
  code_gen->kernel_edge = code_gen->main_edge;
}

/* Add function calls to create and launch kernel KERNEL to
   CODE_GEN->main_edge.  */

static void
opencl_create_gimple_for_body (opencl_body kernel, opencl_main code_gen)
{
  tree num_of_exec = kernel->num_of_exec;
  tree call;

  tree kernel_var
    = opencl_insert_create_kernel_call (code_gen, (const char *) kernel->name);

  tree index_type = build_index_type (build_int_cst (NULL_TREE, 2));
  tree array_type = build_array_type (ptr_type_node, index_type);
  tree var = opencl_create_tmp_var (array_type, "wait_event");

  TREE_STATIC (var) = 1;
  assemble_variable (var, 1, 0, 1);

  call = build4 (ARRAY_REF, ptr_type_node, var,
		 integer_zero_node, NULL_TREE, NULL_TREE);
  call = build_addr (call, current_function_decl);

  opencl_init_local_device_memory (code_gen, kernel);
  opencl_pass_kernel_arguments (code_gen, kernel, kernel_var);

  opencl_execute_kernel (code_gen, num_of_exec, kernel_var, call);
  opencl_wait_for_event (code_gen, call);
}

/* Prepare memory for gimple (host) statement, represented by PBB.
   Copy memory from device to host if it's nessesary.
   CODE_GEN holds information related to code generation.  */

static void
opencl_prepare_memory_for_gimple_stmt (poly_bb_p pbb, opencl_main code_gen)
{
  VEC (poly_dr_p, heap) *drs = PBB_DRS (pbb);
  int i;
  poly_dr_p curr;

  FOR_EACH_VEC_ELT (poly_dr_p, drs, i, curr)
    {
      data_reference_p d_ref = (data_reference_p) PDR_CDR (curr);
      opencl_data data;
      bool is_read;

      /* Scalar variables can be passed directly.  */
      data = opencl_get_data_by_data_ref (code_gen, d_ref);

      /* Private variables should not be passed from device to host.  */
      if (data->privatized)
        continue;

      is_read = DR_IS_READ (d_ref);
      gcc_assert (data);

      data->read_in_current_body = is_read;
      data->written_in_current_body = !is_read;
      opencl_pass_to_host (code_gen, data);

      if (!is_read)
        bitmap_set_bit (code_gen->curr_meta->modified_on_host, data->id);
    }
}

/* Add basic block from clast_user_stmt STMT to gimple.
   CODE_GEN holds information related to code generation.  */

static void
opencl_add_gimple_for_user_stmt (struct clast_user_stmt *stmt,
				 opencl_main code_gen)
{
  gimple_bb_p gbb;
  CloogStatement *cs = stmt->statement;
  poly_bb_p pbb = (poly_bb_p) cloog_statement_usr (cs);
  sese region = code_gen->region;
  int nb_loops = number_of_loops ();
  int i;
  VEC (tree, heap) *iv_map = VEC_alloc (tree, heap, nb_loops);
  htab_t newivs_index = code_gen->newivs_index;
  VEC (tree, heap) *newivs = code_gen->newivs;

  /* Get basic block to add.  */
  gbb = PBB_BLACK_BOX (pbb);

  if (GBB_BB (gbb) == ENTRY_BLOCK_PTR)
    return;

  /*Reset flags.  */
  opencl_fflush_rw_flags (code_gen);

  /* Pass all required memory to host.  */
  opencl_prepare_memory_for_gimple_stmt (pbb, code_gen);

  for (i = 0; i < nb_loops; i++)
    VEC_quick_push (tree, iv_map, NULL_TREE);

  build_iv_mapping (iv_map, region, newivs, newivs_index,
                    stmt, code_gen->params_index);
  code_gen->main_edge
    = copy_bb_and_scalar_dependences (GBB_BB (gbb), region,
                                      code_gen->main_edge, iv_map);
  VEC_free (tree, heap, iv_map);
  recompute_all_dominators ();
  update_ssa (TODO_update_ssa);

  opencl_verify ();
}

/* Delete opencl_body DATA.  */

static void
opencl_body_delete (opencl_body data)
{
  dyn_string_delete (data->body);
  dyn_string_delete (data->header);
  dyn_string_delete (data->pre_header);
  dyn_string_delete (data->non_scalar_args);
  VEC_free (tree, heap, data->function_args);
  VEC_free (tree, heap, data->function_args_to_pass);
  VEC_free (opencl_data, heap, data->data_refs);
  free (data);
}

/* Reset data structures before processing loop, represented by META.
   CODE_GEN holds information related to code generation.  */

static void
opencl_init_new_loop (opencl_clast_meta meta, opencl_main code_gen)
{
  opencl_data curr;
  unsigned i;

  meta->post_pass_to_host
    = VEC_alloc (opencl_data, heap, OPENCL_INIT_BUFF_SIZE);
  meta->post_pass_to_device
    = VEC_alloc (opencl_data, heap, OPENCL_INIT_BUFF_SIZE);

  FOR_EACH_VEC_ELT (opencl_data, code_gen->opencl_function_data, i, curr)
    {
      curr->inited_in_current_loop_on_host = false;
      curr->inited_in_current_loop_on_device = false;
    }
}

/* Post loop init.  Loop through all data stored in POST_PASS_TO_HOST
   and POST_PASS_TO_DEVICE vectors in META.  This data must be up to
   date on host or device respectively at the end of current loop.
   CODE_GEN holds information related to code generation.  */

static void
opencl_postpass_data (opencl_main code_gen, opencl_clast_meta meta)
{
  opencl_data curr;
  unsigned i;

  FOR_EACH_VEC_ELT (opencl_data, meta->post_pass_to_host, i, curr)
    {
      curr->written_in_current_body = false;
      opencl_pass_to_host (code_gen, curr);
    }

  if (!flag_graphite_opencl_cpu)
    FOR_EACH_VEC_ELT (opencl_data, meta->post_pass_to_device, i, curr)
      {
        curr->written_in_current_body = false;
        opencl_pass_to_device (code_gen, curr);
      }

  if (meta->parent)
    {
      VEC (opencl_data, heap) *parent_vec_host
	= meta->parent->post_pass_to_host;
      VEC (opencl_data, heap) *parent_vec_device
	= meta->parent->post_pass_to_device;

      FOR_EACH_VEC_ELT (opencl_data, meta->post_pass_to_host, i, curr)
	VEC_safe_push (opencl_data, heap, parent_vec_host, curr);

      FOR_EACH_VEC_ELT (opencl_data, meta->post_pass_to_device, i, curr)
	VEC_safe_push (opencl_data, heap, parent_vec_device, curr);
    }

  VEC_free (opencl_data, heap, meta->post_pass_to_host);
  VEC_free (opencl_data, heap, meta->post_pass_to_device);
}

static void opencl_transform_stmt_list (struct clast_stmt *, opencl_main, int);

/* Add loop body, of the loop, represented by S, on host.
   Loop body can contain device code.
   DEPTH contains depth of given loop in current loop nest.
   DEPENDENCY indicates where given loop has any dependencies.
   CODE_GEN holds information related to code generation.  */

static void
opencl_add_gimple_for_loop (struct clast_for *s, opencl_main code_gen,
			    int depth, bool dependency)
{
  loop_p old_parent = code_gen->context_loop;
  loop_p new_loop = graphite_create_new_loop
    (code_gen->region, code_gen->main_edge, s, code_gen->context_loop,
     &code_gen->newivs, code_gen->newivs_index, code_gen->params_index, depth);
  edge last_e = single_exit (new_loop);
  edge to_body = single_succ_edge (new_loop->header);
  basic_block after = to_body->dest;
  opencl_clast_meta parent = code_gen->curr_meta->parent;

  last_e = single_succ_edge (split_edge (last_e));

  code_gen->context_loop = new_loop;
  code_gen->main_edge = single_succ_edge (new_loop->header);

  opencl_init_new_loop (parent, code_gen);
  opencl_transform_stmt_list (s->body, code_gen, depth + 1);

  code_gen->context_loop = old_parent;

  redirect_edge_succ_nodup (code_gen->main_edge, after);
  set_immediate_dominator (CDI_DOMINATORS, code_gen->main_edge->dest,
                           code_gen->main_edge->src);

  opencl_postpass_data (code_gen, parent);
  code_gen->main_edge = last_e;

  if (flag_loop_parallelize_all && !dependency)
    new_loop->can_be_parallel = true;

  opencl_verify ();
}

/* Add loop, represented by S, on host.  Loop body can contain device code.
   DEPTH contains depth of given loop in current loop nest.
   DEPENDENCY indicates where given loop has any dependencies.
   CODE_GEN holds information related to code generation.  */

static void
opencl_add_gimple_for_stmt_for (struct clast_for *s, opencl_main code_gen,
				int depth, bool dependency)
{
  edge last_e = graphite_create_new_loop_guard (code_gen->region,
                                                code_gen->main_edge,
                                                s, code_gen->newivs,
                                                code_gen->newivs_index,
                                                code_gen->params_index);
  edge true_e = get_true_edge_from_guard_bb (code_gen->main_edge->dest);

  code_gen->main_edge = true_e;
  opencl_add_gimple_for_loop (s, code_gen, depth, dependency);
  code_gen->main_edge = last_e;
}

/* Calculate parent data access flags in META based on children.
   parent->modified_on_host = OR_{forall children} child->modified_on_host.
   parent->modified_on_device = OR_{forall children} child->modified_on_device.
*/

static void
opencl_fix_meta_flags (opencl_clast_meta meta)
{
  opencl_clast_meta curr = meta->body;

  while (curr)
    {
      bitmap_ior_into (meta->modified_on_host, curr->modified_on_host);
      bitmap_ior_into (meta->modified_on_device, curr->modified_on_device);
      curr = curr->next;
    }
}

/* Add if statement, represented by S to current gimple.
   CODE_GEN holds information related to code generation.  */

static void
opencl_add_gimple_for_stmt_guard (struct clast_guard *s,
                                  opencl_main code_gen, int depth)
{
  edge last_e = graphite_create_new_guard (code_gen->region,
                                           code_gen->main_edge, s,
                                           code_gen->newivs,
                                           code_gen->newivs_index,
                                           code_gen->params_index);
  edge true_e = get_true_edge_from_guard_bb (code_gen->main_edge->dest);

  code_gen->main_edge = true_e;
  opencl_transform_stmt_list (s->then, code_gen, depth);
  code_gen->main_edge = last_e;

  recompute_all_dominators ();
  opencl_verify ();
}

/* Parse clast statement list S, located on depth DEPTH in current loop nest.
   This function generates gimple from clast statements, but in case of
   stmt_for either host or device code can be generated.
   CODE_GEN holds information related to code generation.  */

static void
opencl_transform_stmt_list (struct clast_stmt *s, opencl_main code_gen,
			    int depth)
{
  bool dump_p = dump_file && (dump_flags & TDF_DETAILS);

  for ( ; s; s = s->next)
    {
      opencl_clast_meta tmp = code_gen->curr_meta;

      if (CLAST_STMT_IS_A (s, stmt_root))
        continue;

      else if (CLAST_STMT_IS_A (s, stmt_user))
        {
          code_gen->curr_meta->init_edge = code_gen->main_edge;
          opencl_add_gimple_for_user_stmt ((struct clast_user_stmt *) s,
                                           code_gen);
          code_gen->curr_meta = code_gen->curr_meta->next;
        }
      else if (CLAST_STMT_IS_A (s, stmt_for))
        {
          opencl_clast_meta current_clast  = code_gen->curr_meta;
          struct clast_for *for_stmt = (struct clast_for *) s;
          bool dependency = false;
          bool parallel = false;

          /* If there are dependencies in loop, it can't be parallelized.  */
          if (!flag_graphite_opencl_no_dep_check &&
              dependency_in_clast_loop_p (code_gen, current_clast,
                                          for_stmt, depth))
            {
	      if (dump_p)
		fprintf (dump_file, "dependency in loop\n");

	      dependency = true;
            }

          if (!dependency)
            parallel = opencl_should_be_parallel_p (code_gen, current_clast,
                                                    depth);

          /* Create init block for memory transfer befor loop.  */
          current_clast->init_edge = code_gen->main_edge;

          if (parallel && !dependency)
            {
              opencl_body current_body;

              opencl_fflush_rw_flags (code_gen);
              opencl_mark_privatized_data (code_gen);
              current_clast->on_device = true;
              current_body
		= opencl_clast_to_kernel (for_stmt, code_gen, depth);

              if (current_body->num_of_data_writes)
                {
                  dyn_string_t header = current_body->header;
                  dyn_string_t pre_header = current_body->pre_header;
                  dyn_string_t body = current_body->body;

                  dyn_string_append (code_gen->main_program, header);
                  dyn_string_append (code_gen->main_program, pre_header);
                  dyn_string_append (code_gen->main_program, body);

                  opencl_create_gimple_for_body (current_body, code_gen);
                }
              htab_delete (code_gen->global_defined_vars);
              update_ssa (TODO_update_ssa);
              opencl_verify ();
              opencl_body_delete (current_body);
              code_gen->current_body = NULL;
            }
          else
            {
              code_gen->curr_meta = code_gen->curr_meta->body;
              opencl_add_gimple_for_stmt_for (for_stmt, code_gen,
                                              depth, dependency);
            }

          opencl_fix_meta_flags (current_clast);
          code_gen->curr_meta = current_clast->next;
        }
      else if (CLAST_STMT_IS_A (s, stmt_guard))
        opencl_add_gimple_for_stmt_guard ((struct clast_guard *) s,
                                          code_gen, depth);
      else if (CLAST_STMT_IS_A (s, stmt_block))
        opencl_transform_stmt_list (((struct clast_block *) s)->body,
                                    code_gen, depth);
      else
        gcc_unreachable ();

      if (tmp->parent)
        opencl_fix_meta_flags (tmp->parent);
    }
}

/* Transform clast statement DATA from scop SCOP to OpenCL calls
   in region REGION.  Place all calls to edge MAIN.  PARAM_INDEX
   holds external scop params.  */

void
opencl_transform_clast (struct clast_stmt *data, sese region,
                        edge main, scop_p scop, htab_t params_index)
{
  opencl_main code_gen;
  /* Create main data struture for code generation.  */

  if (dump_file && (dump_flags & TDF_DETAILS))
    {
      fprintf (dump_file, "\nGenerating OpenCL code for SCoP: \n");
      print_scop (dump_file, scop, 0);
    }

  code_gen = opencl_main_create (((struct clast_root *) data)->names,
                                 region, main, params_index);

  opencl_init_basic_blocks (code_gen);
  opencl_init_data (scop, code_gen);

  code_gen->clast_meta = opencl_create_meta_from_clast (code_gen, data, 1,
							NULL);
  code_gen->curr_meta = code_gen->clast_meta;

  opencl_transform_stmt_list (data, code_gen, 1);

  if (dyn_string_length (code_gen->main_program) != 0)
    {
      dyn_string_append (main_program_src, code_gen->main_program);
      opencl_set_data_size (code_gen);
      opencl_init_all_device_buffers (code_gen);
      opencl_fflush_all_device_buffers_to_host (code_gen);
    }

  recompute_all_dominators ();
  update_ssa (TODO_update_ssa);
  opencl_main_delete (code_gen);
}

/* Find opencl_data object by host object OBJ in CODE_GEN hash maps.  */

opencl_data
opencl_get_data_by_tree (opencl_main code_gen, tree obj)
{
  map_tree_to_data tmp = map_tree_to_data_create (obj, NULL);
  map_tree_to_data *slot
    = (map_tree_to_data *) htab_find_slot (code_gen->tree_to_data,
					   tmp, INSERT);
  if (*slot == NULL)
    return NULL;

  return (*slot)->value;
}

/* Find opencl_data object by data reference REF in CODE_GEN hash maps.  */

opencl_data
opencl_get_data_by_data_ref (opencl_main code_gen, data_reference_p ref)
{
  map_ref_to_data tmp = map_ref_to_data_create (ref, NULL);
  map_ref_to_data *slot
    = (map_ref_to_data *) htab_find_slot (code_gen->ref_to_data,
					  tmp, INSERT);
  if (*slot == NULL)
    return NULL;

  return (*slot)->value;
}

/* Create global variables for opencl code.  */

static void
opencl_create_gimple_variables (void)
{
  static bool opencl_var_created = false;

  if (opencl_var_created)
    return;

  opencl_var_created = true;

  /* cl_context h_context */
  h_context = opencl_create_static_ptr_variable ("__ocl_h_context");

  /* cl_command_queue h_cmd_queue */
  h_cmd_queue = opencl_create_static_ptr_variable ("__ocl_h_cmd_queue");
}

/* Create call
   | clGetContextInfo (h_context, CL_CONTEXT_DEVICES, 0, 0,
   |                   &n_context_descriptor_size);

   POINTER_TO_SIZE if &n_context_descriptor_size.  */

static tree
opencl_create_clGetContextInfo_1 (tree pointer_to_size)
{
  tree function = opencl_create_function_decl (GET_CONTEXT_INFO);
  tree zero_pointer = null_pointer_node;
  tree cl_contex_devices = build_int_cst (NULL_TREE, CL_CONTEXT_DEVICES);
  tree context_var = h_context;

  return build_call_expr (function, 5,
			  context_var,
			  cl_contex_devices,
			  integer_zero_node,
			  zero_pointer,
			  pointer_to_size);
}

/* Create call
   | clGetContextInfo (h_context, CL_CONTEXT_DEVICES,
   |                   n_context_descriptor_size, A_DEVICES, 0);

   POINTER_TO_SIZE if &n_context_descriptor_size.  */

static tree
opencl_create_clGetContextInfo_2 (tree size, tree a_devices)
{
  tree function = opencl_create_function_decl (GET_CONTEXT_INFO);
  tree zero_pointer = null_pointer_node;
  tree cl_contex_devices = build_int_cst (NULL_TREE, CL_CONTEXT_DEVICES);
  tree context_var = h_context;

  return build_call_expr (function, 5,
			  context_var,
			  cl_contex_devices,
			  size,
			  a_devices,
			  zero_pointer);
}

/* Create context_properties array variable.  */

static tree
opencl_create_context_properties (void)
{
  tree cl_context_properties_type = long_integer_type_node;

  tree index_type = build_index_type (build_int_cst (NULL_TREE, 3));
  tree array_type = build_array_type (cl_context_properties_type,
                                      index_type);

  return opencl_create_tmp_var (array_type, "context_properties");
}

/* Place calls to obtain current platform id to INIT_EDGE.
   Place obtained id to VAR.  */

static edge
opencl_set_context_properties (edge init_edge, tree var)
{
  tree function = opencl_create_function_decl (GET_PLATFORM_IDS);
  tree cl_context_properties_type = long_integer_type_node;
  tree call;
  tree call2;
  gimple_stmt_iterator g_iter;

  basic_block bb = split_edge (init_edge);

  init_edge = single_succ_edge (bb);

  g_iter = gsi_last_bb (bb);
  call = build4 (ARRAY_REF, cl_context_properties_type,
		 var, integer_zero_node, NULL_TREE, NULL_TREE);
  call2 = build_int_cst (NULL_TREE, CL_CONTEXT_PLATFORM);
  call2 = build1 (CONVERT_EXPR, cl_context_properties_type, call2);

  call = build2 (MODIFY_EXPR, cl_context_properties_type,
                 call, call2);

  force_gimple_operand_gsi (&g_iter, call, true, NULL, false,
                            GSI_CONTINUE_LINKING);
  g_iter = gsi_last_bb (bb);

  call = build4 (ARRAY_REF, cl_context_properties_type,
                 var, integer_one_node, NULL_TREE, NULL_TREE);
  call2 = build_call_expr (function, 3,
                           integer_one_node,
                           build_addr (call, current_function_decl),
                           null_pointer_node);
  force_gimple_operand_gsi (&g_iter, call2, true, NULL, false,
                            GSI_CONTINUE_LINKING);

  call = build4 (ARRAY_REF, cl_context_properties_type,
                 var, build_int_cst (NULL_TREE, 2), NULL_TREE, NULL_TREE);
  call = build2 (MODIFY_EXPR, cl_context_properties_type,
                 call, fold_convert (cl_context_properties_type,
                                     integer_zero_node));
  force_gimple_operand_gsi (&g_iter, call, true, NULL, false,
                            GSI_CONTINUE_LINKING);
  return init_edge;
}

/* Create call
   clCreateContextFromType (PROPERTIES, CL_DEVICE_TYPE_GPU, 0, 0, 0);  */

static tree
opencl_create_clCreateContextFromType (tree properties)
{
  tree function = opencl_create_function_decl (CREATE_CONTEXT_FROM_TYPE);
  tree zero_pointer = null_pointer_node;
  tree device
    = build_int_cst (NULL_TREE, flag_graphite_opencl_cpu
		     ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU);

  return build_call_expr (function, 5,
                          build_addr (properties, current_function_decl),
                          device,
                          zero_pointer,
                          zero_pointer,
                          zero_pointer);
}

/* Create call
   clCreateCommandQueue (h_context, DEV_ID, 0, 0);  */

static tree
opencl_create_clCreateCommandQueue (tree dev_id)
{

  tree function = opencl_create_function_decl (CREATE_COMMAND_QUEUE);
  tree zero_pointer = null_pointer_node;
  tree context = h_context;

  return build_call_expr (function, 4,
			  context,
			  dev_id,
			  zero_pointer,
			  zero_pointer);
}

/* Create call malloc (ARG).  */

static tree
opencl_create_malloc_call (tree arg)
{
  tree function_type = build_function_type_list (ptr_type_node,
						 integer_type_node,
						 NULL_TREE);
  tree function = build_fn_decl ("malloc", function_type);

  return build_call_expr (function, 1, arg);
}

/* Generate calls for opencl init functions and place them to INIT_EDGE.
   Must be called only once in each function.  */

static edge
opencl_create_init_context (edge init_edge)
{
  tree tmp_type;
  tree call;
  tree n_context_descriptor_size
    = opencl_create_tmp_var (size_type_node, "__ocl_nContextDescriptorSize");
  tree a_devices = opencl_create_tmp_var (build_pointer_type (ptr_type_node),
					  "__ocl_a_devices");
  tree properties = opencl_create_context_properties ();

  init_edge = opencl_set_context_properties (init_edge, properties);
  call = opencl_create_clCreateContextFromType (properties);
  call = build2 (MODIFY_EXPR, TREE_TYPE (h_context),
                 h_context, call);
  init_edge = opencl_add_safe_call_on_edge (call, false, init_edge);
  tmp_type = build_pointer_type
    (TREE_TYPE (n_context_descriptor_size));
  call = build1 (ADDR_EXPR, tmp_type,
                 n_context_descriptor_size);
  call = opencl_create_clGetContextInfo_1 (call);
  init_edge = opencl_add_safe_call_on_edge (call, true, init_edge);
  call = opencl_create_malloc_call (n_context_descriptor_size);
  call = fold_convert (TREE_TYPE (a_devices), call);
  call = build2 (MODIFY_EXPR, TREE_TYPE (a_devices), a_devices, call);
  init_edge = opencl_add_safe_call_on_edge (call, false, init_edge);
  call = opencl_create_clGetContextInfo_2 (n_context_descriptor_size,
                                           a_devices);
  init_edge = opencl_add_safe_call_on_edge (call, true, init_edge);
  tmp_type = TREE_TYPE (TREE_TYPE (a_devices));
  call = build1 (INDIRECT_REF, tmp_type, a_devices);
  call = opencl_create_clCreateCommandQueue (call);
  call = build2 (MODIFY_EXPR, TREE_TYPE (h_cmd_queue),
                 h_cmd_queue, call);
  init_edge = opencl_add_safe_call_on_edge (call, false, init_edge);
  return init_edge;
}

/* Fill array VEC with all poly basic blocks in clast statement ROOT.  */

static void
build_poly_bb_vec (struct clast_stmt *root,
                   VEC (poly_bb_p, heap) **vec)
{
  while (root)
    {
      if (CLAST_STMT_IS_A (root, stmt_user))
        {
          poly_bb_p tmp
	    = (poly_bb_p) cloog_statement_usr
	    (((struct clast_user_stmt *) root)->statement);

          VEC_safe_push (poly_bb_p, heap, *vec, tmp);
        }

      else if (CLAST_STMT_IS_A (root, stmt_for))
        build_poly_bb_vec (((struct clast_for *) root)->body, vec);

      else if (CLAST_STMT_IS_A (root, stmt_guard))
        build_poly_bb_vec (((struct clast_guard *) root)->then, vec);

      else if (CLAST_STMT_IS_A (root, stmt_block))
        build_poly_bb_vec (((struct clast_block *) root)->body, vec);

      root = root->next;
    }
}

/* Check whether there is a dependency between PBB1 and PBB2 on level LEVEL.
   CAN_BE_PRIVATE indicates which variables can be privatizated.
   CODE_GEN holds information related to code generation.  */

static bool
opencl_dependency_between_pbbs_p (opencl_main code_gen, poly_bb_p pbb1,
                                  poly_bb_p pbb2, int level,
                                  bitmap can_be_private)
{
  int i, j;
  poly_dr_p pdr1, pdr2;

  timevar_push (TV_GRAPHITE_DATA_DEPS);

  FOR_EACH_VEC_ELT (poly_dr_p, PBB_DRS (pbb1), i, pdr1)
    {
      data_reference_p ref1 = (data_reference_p)PDR_CDR (pdr1);
      opencl_data data_1 = opencl_get_data_by_data_ref (code_gen, ref1);

      if (bitmap_bit_p (can_be_private, data_1->id))
        continue;

      FOR_EACH_VEC_ELT (poly_dr_p, PBB_DRS (pbb2), j, pdr2)
        {
          data_reference_p ref2 = (data_reference_p)PDR_CDR (pdr2);

          opencl_data data_2 = opencl_get_data_by_data_ref (code_gen, ref2);

          if (bitmap_bit_p (can_be_private, data_2->id))
            continue;

          if (graphite_carried_dependence_level_k (pdr1, pdr2, level))
            {
              timevar_pop (TV_GRAPHITE_DATA_DEPS);
              return true;
            }
        }
    }

  timevar_pop (TV_GRAPHITE_DATA_DEPS);
  return false;
}

/* Returns true, if there is dependency in clast loop STMT on depth DEPTH.
   CODE_GEN holds information related to code generation.  */

bool
dependency_in_clast_loop_p (opencl_main code_gen, opencl_clast_meta meta,
                            struct clast_for *stmt, int depth)
{
  VEC (poly_bb_p, heap) *pbbs = VEC_alloc (poly_bb_p, heap, 10);
  int level = get_scattering_level (depth);
  int i;
  poly_bb_p pbb1;
  bitmap can_be_private;

  build_poly_bb_vec (stmt->body, &pbbs);
  can_be_private = meta->can_be_private;

  FOR_EACH_VEC_ELT (poly_bb_p, pbbs, i, pbb1)
    {
      int j;
      poly_bb_p pbb2;

      FOR_EACH_VEC_ELT (poly_bb_p, pbbs, j, pbb2)
        if (opencl_dependency_between_pbbs_p (code_gen, pbb1, pbb1,
                                              level, can_be_private))
          {
            VEC_free (poly_bb_p, heap, pbbs);
            return true;
          }
    }

  VEC_free (poly_bb_p, heap, pbbs);
  return false;
}

/* Init graphite-opencl pass.  Must be called in each function before
   any scop processing.  */

void
graphite_opencl_init (void)
{
  opencl_create_gimple_variables ();

  /* cl_program h_program */
  h_program
    = opencl_create_static_ptr_variable ("__ocl_h_program");

  opencl_function_kernels = VEC_alloc (tree, heap, OPENCL_INIT_BUFF_SIZE);
  main_program_src = dyn_string_new (100);

  opencl_array_data = VEC_alloc (opencl_data, heap, OPENCL_INIT_BUFF_SIZE);
  array_data_to_tree = htab_create (10, map_tree_to_data_to_hash,
                                    map_tree_to_data_cmp, free);

  opencl_create_function_decl (STATIC_INIT);
}

/* Create calls to initialize static data for current function and
   place them to INIT_EDGE.  */

static edge
opencl_init_static_data (edge init_edge)
{
  int i;
  opencl_data curr;

  FOR_EACH_VEC_ELT (opencl_data, opencl_array_data, i, curr)
    {
      tree tmp;

      if (!curr->used_on_device)
        continue;

      tmp = opencl_create_memory_for_pointer (curr);
      tmp = build2 (MODIFY_EXPR, ptr_type_node, curr->device_object, tmp);
      init_edge = opencl_add_safe_call_on_edge (tmp, false, init_edge);
    }

  return init_edge;
}

/* Finalize graphite-opencl pass for current function.  Place all required
   calls to STATIC_INIT_EDGE.  Must be called after all scop processing
   in current function.  */

void
graphite_opencl_finalize (edge static_init_edge)
{
  int i;
  opencl_data curr;

  if (dyn_string_length (main_program_src) != 0)
    {
      tree call = build2 (EQ_EXPR, boolean_type_node,
                          h_program, null_pointer_node);
      basic_block buff_init_block = split_edge (static_init_edge);
      edge before_init;
      edge init_edge;

      static_init_edge = single_succ_edge (buff_init_block);

      create_empty_if_region_on_edge (static_init_edge, call);

      static_init_edge = opencl_create_function_call
	(get_true_edge_from_guard_bb (static_init_edge->dest));
      static_init_edge = opencl_init_static_data (static_init_edge);
      before_init = single_pred_edge (buff_init_block);
      call = build2 (EQ_EXPR, boolean_type_node,
                     h_context, null_pointer_node);

      create_empty_if_region_on_edge (before_init, call);
      init_edge = get_true_edge_from_guard_bb (before_init->dest);
      init_edge = opencl_create_init_context (init_edge);
    }

  dyn_string_delete (main_program_src);

  FOR_EACH_VEC_ELT (opencl_data, opencl_array_data, i, curr)
    opencl_data_delete (curr);

  VEC_free (tree, heap, opencl_function_kernels);
  VEC_free (opencl_data, heap, opencl_array_data);
  recompute_all_dominators ();
  update_ssa (TODO_update_ssa);
}

/* Debug functions for deined data structures.  */

static void
dump_flag_to_file (const char *name, bool cond,
                   FILE *file, int indent)
{
  indent_to (file, indent);
  fprintf (file, "%s = %s", name, cond? "true" : "false");
}

void
dump_opencl_data (opencl_data data, FILE *file, bool verbose)
{
  fprintf (file, "Data id = %d\n", data->id);
  fprintf (file, "Data dimension = %d\n", data->data_dim);
  fprintf (file, "Data depth = %d\n", data->depth);
  fprintf (file, "Flags");
  indent_to (file, 2);
  fprintf (file, "Global");
  dump_flag_to_file ("Static", data->is_static, file, 4);
  dump_flag_to_file ("Can be private", data->can_be_private, file, 4);
  dump_flag_to_file ("Used on device", data->used_on_device, file, 4);
  dump_flag_to_file ("Ever read on device",
                     data->ever_read_on_device, file, 4);

  dump_flag_to_file ("Ever written on device",
                     data->ever_written_on_device, file, 4);

  dump_flag_to_file ("Supported", data->supported, file, 4);
  indent_to (file, 2);
  fprintf (file, "Local");

  dump_flag_to_file ("Up to date on device",
                     data->up_to_date_on_device, file, 4);
  dump_flag_to_file ("Up to date on host",
                     data->up_to_date_on_host, file, 4);

  dump_flag_to_file ("Inited in current loop on host",
                     data->inited_in_current_loop_on_host, file, 4);

  dump_flag_to_file ("Inited in current loop on device",
                     data->inited_in_current_loop_on_device, file, 4);

  dump_flag_to_file ("Written in current body",
                     data->written_in_current_body, file, 4);

  dump_flag_to_file ("Read in current body",
                     data->read_in_current_body, file, 4);
  dump_flag_to_file ("Privatized", data->privatized, file, 4);

  fprintf (file, "\n");

  if (verbose)
    {
      fprintf (file, "\nObject\n");
      print_node_brief (file, "", data->object, 2);

      fprintf (file, "\nDevice object\n");
      print_node_brief (file, "", data->device_object, 2);

      fprintf (file, "\nSize value\n");
      print_node_brief (file, "", data->size_value, 2);

      fprintf (file, "\nSize variable\n");
      print_node_brief (file, "", data->size_variable, 2);

      fprintf (file, "\nExact object\n");
      print_node_brief (file, "", data->exact_object, 2);
    }
}

DEBUG_FUNCTION void
debug_opencl_data (opencl_data data, bool verbose)
{
  dump_opencl_data (data, stderr, verbose);
}

void
dump_opencl_body (opencl_body body, FILE *file, bool verbose)
{
  fprintf (file, "\n%s\n\n", body->name);
  fprintf (file, "First iterator: %s\n", body->first_iter);
  fprintf (file, "Last iterator: %s\n", body->last_iter);
  fprintf (file, "Number of data writes = %d\n\n", body->num_of_data_writes);
  fprintf (file, "Function header::\n");
  fprintf (file, "%s\n\n", dyn_string_buf (body->header));
  fprintf (file, "Non scalar args::\n");
  fprintf (file, "%s\n\n", dyn_string_buf (body->non_scalar_args));
  fprintf (file, "Pre header::\n");
  fprintf (file, "%s\n\n", dyn_string_buf (body->pre_header));
  fprintf (file, "Body::\n");
  fprintf (file, "%s\n\n", dyn_string_buf (body->body));

  fprintf (file, "Number of executions::\n");
  print_node_brief (file, "", body->num_of_exec, 2);

  if (verbose)
    print_clast_stmt (file, body->clast_body);
}

DEBUG_FUNCTION void
debug_opencl_body (opencl_body body, bool verbose)
{
  dump_opencl_body (body, stderr, verbose);
}

void
dump_opencl_clast_meta (opencl_clast_meta meta, FILE *file,
                        bool verbose, int indent)
{
  if (!verbose)
    /* Just print structure of meta.  */
    {
      while (meta)
        {
          indent_to (file, indent);
          fprintf (file, "<in = %d, out = %d, dev = %s, ok = %s>",
                   meta->in_depth, meta->out_depth,
                   meta->on_device?"true":"false",
                   meta->access_unsupported?"false":"true");
          dump_opencl_clast_meta (meta->body, file, false, indent + 4);
          meta = meta->next;
        }
    }
  else
    {
      fprintf (file, "<in = %d, out = %d, dev = %s, ok = %s>",
               meta->in_depth, meta->out_depth,
               meta->on_device?"true":"false",
               meta->access_unsupported?"false":"true");

      fprintf (file, "\nModified on host::\n");
      debug_bitmap_file (file, meta->modified_on_host);

      fprintf (file, "\nModified on device::\n");
      debug_bitmap_file (file, meta->modified_on_device);

      fprintf (file, "\nAccess::\n");
      debug_bitmap_file (file, meta->access);

      fprintf (file, "\nCan be private::\n");
      debug_bitmap_file (file, meta->can_be_private);
    }
}

DEBUG_FUNCTION void
debug_opencl_clast_meta (opencl_clast_meta meta, bool verbose)
{
  dump_opencl_clast_meta (meta, stderr, verbose, 0);
}

static int
print_char_p_htab (void **h, void *v)
{
  char **ptr = (char **) h;
  FILE *file = (FILE *) v;

  fprintf (file, "  %s\n", *ptr);
  return 1;
}

static int
print_tree_to_data_htab (void **h, void *v)
{
  map_tree_to_data *map = (map_tree_to_data *) h;
  FILE *file = (FILE *) v;
  tree key = (*map)->key;
  opencl_data data = (*map)->value;

  print_node_brief (file, "key = ", key, 2);
  fprintf (file, " data_id =  %d\n", data->id);
  return 1;
}

static int
print_ref_to_data_htab (void **h, void *v)
{
  map_ref_to_data *map = (map_ref_to_data *) h;
  FILE *file = (FILE *) v;
  data_reference_p key = (*map)->key;
  opencl_data data = (*map)->value;

  fprintf (file, "key::\n");
  dump_data_reference (file, key);
  fprintf (file, "data_id =  %d\n\n", data->id);
  return 1;
}

void
dump_opencl_main (opencl_main code_gen, FILE *file, bool verbose)
{
  fprintf (file, "Current meta::\n");
  dump_opencl_clast_meta (code_gen->curr_meta, file, false, 2);
  fprintf (file, "\n");

  if (code_gen->current_body)
    {
      fprintf (file, "Current body::\n");
      dump_opencl_body (code_gen->current_body, file, verbose);
    }

  fprintf (file, "\n\nData init basic block::\n");
  dump_bb (code_gen->data_init_bb, stderr, 0);

  if (code_gen->defined_vars)
    {
      fprintf (file, "Defined variables::\n");
      htab_traverse_noresize (code_gen->defined_vars, print_char_p_htab,
                              file);
    }

  if (code_gen->global_defined_vars)
    {
      fprintf (file, "Global defined variables::\n");
      htab_traverse_noresize (code_gen->global_defined_vars,
                              print_char_p_htab, file);
    }

  fprintf (file, "Refs to data::\n");
  htab_traverse_noresize (code_gen->ref_to_data,
                          print_ref_to_data_htab, file);

  fprintf (file, "Trees to data::\n");
  htab_traverse_noresize (code_gen->tree_to_data,
                          print_tree_to_data_htab, file);

  if (verbose)
    fprintf (file, "%s\n", dyn_string_buf (code_gen->main_program));
}

DEBUG_FUNCTION void
debug_opencl_main (opencl_main code_gen, bool verbose)
{
  dump_opencl_main (code_gen, stderr, verbose);
}

DEBUG_FUNCTION void
debug_opencl_program (void)
{
  fprintf (stderr, "%s", dyn_string_buf (main_program_src));
}

#endif
#include "gt-graphite-opencl.h"
