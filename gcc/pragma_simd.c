/* Routines to handle Pragma SIMD capabilities by the vectorizer
 * Copyright (C) 2011
 * Contributed by Balaji V. Iyer <balaji.v.iyer@intel.com>
 *
 */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tree.h"
#include "langhooks.h"
#include "c-tree.h"
#include "c-lang.h"
#include "flags.h"
#include "output.h"
#include "intl.h"
#include "target.h"
#include "tree-iterator.h"
#include "bitmap.h"
#include "gimple.h"
#include "cilk.h"
#include "c-family/c-objc.h"
		

struct pragma_simd_values *psv_head;
static void find_var_decl (tree t, const char *var_name, tree *var);
static void change_var_decl (tree *t, tree new_var, tree var);

bool
clause_resolved_p (enum pragma_simd_kind clause_type, int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  
  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return true;

  if (psv_head == NULL)
    return true;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->pragma_encountered == true) &&
	(ps_iter->index == pragma_simd_index))
    {
      switch (clause_type) {
      case P_SIMD_VECTORLENGTH:
	return ps_iter->vlength_OK;
      case P_SIMD_PRIVATE:
	return ps_iter->pvars_OK;
      case P_SIMD_LINEAR:
	return ps_iter->lvars_OK;
      case P_SIMD_REDUCTION:
	return ps_iter->rvars_OK;
      default:
	return false;
      }
    }
  }

  /* if we can't find the certain pragma, then just return true to make sure
   * we do not assert/halt and continue */
  return true;
}

void
set_OK_for_certain_clause (enum pragma_simd_kind clause_type, bool set_value,
			   int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return;

  if (psv_head == NULL)
    return;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->pragma_encountered == true) &&
	(ps_iter->index == pragma_simd_index))
      break;
  }
  if (ps_iter == NULL)
    return;

  
  
  switch (clause_type)
  {
  case P_SIMD_VECTORLENGTH:
    ps_iter->vlength_OK = set_value;
  case P_SIMD_PRIVATE:
    ps_iter->pvars_OK = set_value;
  case P_SIMD_LINEAR:
    ps_iter->lvars_OK = set_value;
  case P_SIMD_REDUCTION:
    ps_iter->rvars_OK = set_value;
  default:
    break;
  }

  return;
}

bool
all_reductions_satisfied_p (int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  struct reduction_values *rv_iter = NULL;

  if (psv_head == NULL)
    return true;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return true;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    for (rv_iter = ps_iter->reduction_vals; rv_iter != NULL;
	 rv_iter = rv_iter->ptr_next)
    {
      if (rv_iter->not_reduced != 0)
	return false;
    }
  }
  return true;
}
  
  
    

struct pragma_simd_values *
psv_find_node(int psv_index)
{
  struct pragma_simd_values *ps_iter = NULL;

  if (psv_index <= INVALID_PRAGMA_SIMD_SLOT)
    return NULL;

  if (psv_head == NULL)
    return NULL;
  
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->index == psv_index) &&
	(ps_iter->pragma_encountered == true))
    {
      return ps_iter;
    }
  }

  /* you should not get here */
  return NULL;
}

int
psv_head_insert(struct pragma_simd_values local_simd_values)
{
  struct pragma_simd_values *ps_iter = NULL;
 
  struct reduction_values *rv_iter = NULL;

  gcc_assert (local_simd_values.pragma_encountered == true);
  if (psv_head == NULL)
  {
    psv_head = (struct pragma_simd_values *)
      xmalloc (sizeof(struct pragma_simd_values));
    gcc_assert(psv_head != NULL);
    psv_head->pragma_encountered  = local_simd_values.pragma_encountered;
    /* we keep the head pointer index to be invalid pragma simd slot + 1. This
     * is done before fi we want to debug then we can set invalid pragma
     * simd_slot value to some arbitary number and then see if we are
     * catching the pragmas correctly */
    psv_head->index = INVALID_PRAGMA_SIMD_SLOT + 1;
    psv_head->types = local_simd_values.types;
    psv_head->vectorlength = local_simd_values.vectorlength;
    if (local_simd_values.vectorlength != NULL_TREE)
      psv_head->vlength_OK = false;
    else
      psv_head->vlength_OK = true;
    
    psv_head->private_vars = local_simd_values.private_vars;
    if (local_simd_values.private_vars != NULL_TREE)
      psv_head->pvars_OK = false;
    else
      psv_head->pvars_OK = true;
    
    psv_head->linear_vars = local_simd_values.linear_vars;
    if (local_simd_values.linear_vars != NULL_TREE)
      psv_head->lvars_OK = false;
    else
      psv_head->lvars_OK = true;
    
    psv_head->linear_steps = local_simd_values.linear_steps;
    if (local_simd_values.reduction_vals == NULL)
      psv_head->reduction_vals = NULL;
    
    psv_head->reduction_vals = NULL;
    for (rv_iter = local_simd_values.reduction_vals; rv_iter != NULL;
	 rv_iter = rv_iter->ptr_next)
    {
      insert_reduction_values (&psv_head->reduction_vals,
			       rv_iter->reduction_operator,
			       rv_iter->tree_reduction_var_list);
    }

    if (local_simd_values.reduction_vals != NULL)
      psv_head->rvars_OK = false;
    else
      psv_head->rvars_OK = true;
    
    psv_head->ptr_next = NULL;
    return psv_head->index;
  }
  
  for (ps_iter = psv_head; ps_iter->ptr_next != NULL;
       ps_iter = ps_iter->ptr_next)
  {
    ;
  }

  ps_iter->ptr_next = (struct pragma_simd_values *)
    xmalloc(sizeof(struct pragma_simd_values));
  gcc_assert(ps_iter->ptr_next != NULL);
 
  ps_iter->ptr_next->pragma_encountered = local_simd_values.pragma_encountered;
  ps_iter->ptr_next->index = ps_iter->index+1;
  ps_iter->ptr_next->types = local_simd_values.types;
  ps_iter->ptr_next->vectorlength = local_simd_values.vectorlength;
  ps_iter->ptr_next->private_vars = local_simd_values.private_vars;
  ps_iter->ptr_next->linear_vars = local_simd_values.linear_vars;
  ps_iter->ptr_next->linear_steps = local_simd_values.linear_steps;
  ps_iter->ptr_next->ptr_next = NULL;

  ps_iter->ptr_next->reduction_vals = NULL;
  for (rv_iter = local_simd_values.reduction_vals; rv_iter != NULL;
       rv_iter = rv_iter->ptr_next)
  {
    insert_reduction_values (&ps_iter->ptr_next->reduction_vals,
			     rv_iter->reduction_operator,
			     rv_iter->tree_reduction_var_list);
  }

  return ps_iter->ptr_next->index;
}

bool
pragma_simd_assert_requested_p(int ps_index)
{
  struct pragma_simd_values *ps_iter = NULL;

  if (ps_index == 0)
  {
    return 0;
  }

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->pragma_encountered == true) &&
	(ps_iter->index == ps_index))
    {
      if ((ps_iter->types & P_SIMD_NOASSERT))
	return false;
      else
	return true;
    }
  }
  return false;
}

bool
pragma_simd_acceptable_vlength_p(int ps_index,
				 int possible_vectorization_factor,
				 tree scalar_type)
{
  tree size = NULL_TREE;
  struct pragma_simd_values *ps_iter = NULL;
  int possible_vector_length = 0;
  tree ii_tree = NULL_TREE;
  tree ii_value = NULL_TREE;
  tree vl_tree = NULL_TREE;
  size = size_in_bytes(scalar_type);

  if (ps_index <= INVALID_PRAGMA_SIMD_SLOT)
  {
    return true;
  }

  if (TREE_CODE(size) == INTEGER_CST)
  {
    ;
  }
  else
  {
    error ("Unable to determine size of scalar type!");
  }

  possible_vector_length = possible_vectorization_factor;

  vl_tree = build_int_cst(integer_type_node, possible_vector_length);
  
  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->pragma_encountered == true) &&
	(ps_iter->index == ps_index))
    {
      break;
    }
  }

  gcc_assert(ps_iter != NULL);

  if (list_length(ps_iter->vectorlength) == 0)
  {
    /* the user has not given any vectorlength, so whatever the vectorizer
     * found is good enough for us
     */
    return true;
  }

  for (ii_tree = ps_iter->vectorlength; ii_tree != NULL_TREE;
       ii_tree = TREE_CHAIN(ii_tree))
  {
    ii_value = TREE_VALUE(ii_tree);
    if ((TREE_CODE(ii_value) == INTEGER_CST)  &&
	(tree_int_cst_equal ((const_tree)ii_value, (const_tree)vl_tree)))
    {
      /* we found a match, so we give the YES! */
      return true;
    }
  }

  /* not found a match */
  return false;
}


bool
pragma_simd_vectorize_loop_p (int ps_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  
  if (ps_index <= INVALID_PRAGMA_SIMD_SLOT)
  {
    return false;
  }

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if (ps_iter->index == ps_index)
      return ps_iter->pragma_encountered;
  }

  return false;
}

tree
pragma_simd_create_private_vars(tree body,
				tree *reset_stmt_list,
				struct pragma_simd_values ps_info)
{
  tree_stmt_iterator ii_tree;
  tree ii_value = NULL_TREE, ii_priv_list = NULL_TREE;
  tree ii_priv_value = NULL_TREE;
  tree inside_body = NULL_TREE;
  tree var = NULL_TREE;
  char name[40];
  int count = 0;
  tree var_clone = NULL_TREE;
  tree var_orig  = NULL_TREE;
  tree set_stmt  = NULL_TREE;
  tree body_list = NULL_TREE;
  tree reset_stmt= NULL_TREE;
  
  if (body == NULL_TREE)
  {
    return body;
  }

  if (ps_info.private_vars == NULL_TREE)
  {
    return body;
  }
  
  if (TREE_CODE(body) == BIND_EXPR)
    inside_body = BIND_EXPR_BODY(body);
  else
    inside_body = body;

  for (ii_priv_list = ps_info.private_vars; ii_priv_list != NULL_TREE;
       ii_priv_list = TREE_CHAIN(ii_priv_list))
  {
    ii_priv_value = TREE_VALUE(ii_priv_list);
    var = NULL_TREE;
    find_var_decl (body,
		   IDENTIFIER_POINTER((ii_priv_value)),
		   &var);
    if (var == NULL_TREE)
      continue; /* this means the variable got optimized out */
    else
    {
      sprintf(name, "_p_simd_new_var_%d", ++count);
      var_clone = build_decl (UNKNOWN_LOCATION, VAR_DECL,
			      get_identifier(name), TREE_TYPE(var));
      var_orig  = build_decl (UNKNOWN_LOCATION, VAR_DECL,
			      get_identifier(IDENTIFIER_POINTER(DECL_NAME(var))), TREE_TYPE(var));
  
      
      set_stmt = build2(INIT_EXPR, TREE_TYPE(var), var_clone,
			unshare_expr(var_orig));
      reset_stmt = build2 (INIT_EXPR, TREE_TYPE(var), unshare_expr (var_orig),
			   var_clone);
      if (*reset_stmt_list == NULL_TREE)
      {
	*reset_stmt_list = alloc_stmt_list();
      }
      append_to_statement_list_force (reset_stmt, reset_stmt_list);
      
      if (TREE_CODE(inside_body) == STATEMENT_LIST)
      {
	ii_tree = tsi_start (inside_body);
	tsi_link_before (&ii_tree, set_stmt, TSI_NEW_STMT);
      

	/* after we have added the new initialize expr, we need to go through
	 * all the statements after the statements we just added */

   
      
	/* now we skip the recently added statement */
	ii_tree = tsi_start (inside_body);
	tsi_next(&ii_tree);

	while (tsi_end_p(ii_tree) == 0)
	{
	  ii_value = *tsi_stmt_ptr (ii_tree);
	  change_var_decl (&ii_value, var_clone, var);
	  tsi_next(&ii_tree);
	}
      }
      else
      {
	body_list = alloc_stmt_list();
	change_var_decl (&inside_body, var_clone, var);
	append_to_statement_list_force (set_stmt, &body_list);
	append_to_statement_list_force (inside_body, &body_list);
	body = build3(BIND_EXPR, void_type_node,var_clone,
		      body_list, NULL_TREE);
      }
    }
  }
		   
  ps_info.pvars_OK = true;
  return body;
}
  
				

static void
find_var_decl (tree t, const char *var_name, tree *var)
{
  enum tree_code code;
  bool is_expr;

  

  /* Skip empty subtrees.  */
  if (t == NULL_TREE)
    return;

  code = TREE_CODE (t);
  is_expr = IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (code));
  if (is_expr)
    find_var_decl (TREE_TYPE (t),var_name, var);

  switch (code)
  {
  case ERROR_MARK:
  case IDENTIFIER_NODE:
  case INTEGER_CST:
  case REAL_CST:
  case FIXED_CST:
  case STRING_CST:
  case BLOCK:
  case PLACEHOLDER_EXPR:
  case FIELD_DECL:
  case VOID_TYPE:
  case REAL_TYPE:
    /* These do not contain variable references. */
    return;

  case SSA_NAME:
    find_var_decl (SSA_NAME_VAR (t), var_name, var);
    return;

  case LABEL_DECL:
    return;

  case RESULT_DECL:
  case VAR_DECL:
  case PARM_DECL:
    if (strcmp(IDENTIFIER_POINTER(DECL_NAME(t)),var_name) == 0)
    {
      *var = t;
    }
    return;
  case NON_LVALUE_EXPR:
  case CONVERT_EXPR:
  case NOP_EXPR:
    find_var_decl (TREE_OPERAND (t, 0), var_name, var);
    return;

  case INIT_EXPR:
  
    find_var_decl (TREE_OPERAND(t, 0), var_name, var);
    find_var_decl (TREE_OPERAND (t, 1), var_name, var);
    
   

  case MODIFY_EXPR:
  case PREDECREMENT_EXPR:
  case PREINCREMENT_EXPR:
  case POSTDECREMENT_EXPR:
  case POSTINCREMENT_EXPR:
    /* These write their result. */

    find_var_decl (TREE_OPERAND (t, 0), var_name, var);
    find_var_decl (TREE_OPERAND (t, 1), var_name, var);
    return;

  case ADDR_EXPR:
    find_var_decl (TREE_OPERAND (t, 0), var_name, var);
   
    return;

  case ARRAY_REF:
  case BIT_FIELD_REF:
    find_var_decl (TREE_OPERAND (t, 0), var_name, var);
    find_var_decl (TREE_OPERAND (t, 1), var_name, var);
    find_var_decl (TREE_OPERAND (t, 2), var_name, var);
    return;

  case TREE_LIST:
    find_var_decl (TREE_PURPOSE (t), var_name, var);
    find_var_decl (TREE_VALUE (t), var_name, var);
    find_var_decl (TREE_CHAIN (t), var_name, var);
    return;

  case TREE_VEC:
  {
    int len = TREE_VEC_LENGTH (t);
    int i;
    for (i = 0; i < len; i++)
      find_var_decl (TREE_VEC_ELT (t, i), var_name, var);
    return;
  }

  case VECTOR_CST:
    find_var_decl (TREE_VECTOR_CST_ELTS (t),var_name,  var);
    break;

  case COMPLEX_CST:
    find_var_decl (TREE_REALPART (t), var_name, var);
    find_var_decl (TREE_IMAGPART (t), var_name, var);
    return;

  case CONSTRUCTOR:
  {
    unsigned HOST_WIDE_INT idx;
    constructor_elt *ce;

    for (idx = 0;
	 VEC_iterate(constructor_elt, CONSTRUCTOR_ELTS (t), idx, ce);
	 idx++)
      find_var_decl (ce->value, var_name, var);
    return;
  }

  case BIND_EXPR:
  {
    tree decl;
    for (decl = BIND_EXPR_VARS (t); decl; decl = TREE_CHAIN (decl))
    {
      find_var_decl(decl,var_name, var);
    }
    find_var_decl (BIND_EXPR_BODY (t),var_name, var);
    return;
  }

  case STATEMENT_LIST:
  {
    tree_stmt_iterator i;
    for (i = tsi_start (t); !tsi_end_p (i); tsi_next (&i))
      find_var_decl (*tsi_stmt_ptr (i), var_name, var);
    return;
  }

  case OMP_PARALLEL:
  case OMP_TASK:
  case OMP_FOR:
  case OMP_SINGLE:
  case OMP_SECTION:
  case OMP_SECTIONS:
  case OMP_MASTER:
  case OMP_ORDERED:
  case OMP_CRITICAL:
  case OMP_ATOMIC:
  case OMP_CLAUSE:
    break;

  case TARGET_EXPR:
  {
    find_var_decl (TREE_OPERAND (t, 0),var_name, var);
    find_var_decl (TREE_OPERAND (t, 1), var_name, var);
    find_var_decl (TREE_OPERAND (t, 2), var_name, var);
    if (TREE_OPERAND (t, 3) != TREE_OPERAND (t, 1))
      find_var_decl (TREE_OPERAND (t, 3), var_name,  var);
    return;
  }

  case RETURN_EXPR:
    return;

  case DECL_EXPR:
    return;

  case INTEGER_TYPE:
  case ENUMERAL_TYPE:
  case BOOLEAN_TYPE:
    find_var_decl (TYPE_MIN_VALUE (t), var_name, var);
    find_var_decl (TYPE_MAX_VALUE (t), var_name, var);
    return;

  case POINTER_TYPE:
    find_var_decl (TREE_TYPE (t), var_name, var);
    break;

  case ARRAY_TYPE:
    find_var_decl (TREE_TYPE (t), var_name, var);
    find_var_decl (TYPE_DOMAIN (t), var_name, var);
    return;

  case RECORD_TYPE:
    find_var_decl(TYPE_FIELDS(t),var_name, var);
    return;
    
  case METHOD_TYPE:
    find_var_decl(TYPE_ARG_TYPES(t), var_name, var);
    find_var_decl(TYPE_METHOD_BASETYPE(t), var_name, var);

  case AGGR_INIT_EXPR:
  case CALL_EXPR:
  {
    int len = 0;
    int ii = 0;
    if (TREE_CODE(TREE_OPERAND(t,0)) == INTEGER_CST)
    {
      len = TREE_INT_CST_LOW(TREE_OPERAND(t,0));

      for (ii = 0; ii < len; ii++)
      {
	find_var_decl(TREE_OPERAND(t,ii), var_name, var);
      }
      find_var_decl(TREE_TYPE(t), var_name, var);
    }
    break;
  }

  default:
    if (is_expr)
    {
      int i, len;

      /* Walk over all the sub-trees of this operand.  */
      len = TREE_CODE_LENGTH (code);

      /* Go through the subtrees.  We need to do this in forward order so
	 that the scope of a FOR_EXPR is handled properly.  */
      for (i = 0; i < len; ++i)
	find_var_decl (TREE_OPERAND (t, i), var_name, var);
    }
    return;
  }
}


				

static void
change_var_decl (tree *t, tree new_var, tree var)
{
  tree nt = NULL_TREE;
  enum tree_code code;
  bool is_expr = false;
  tree nt2 = NULL_TREE, nt3 = NULL_TREE, nt4 = NULL_TREE;
  

  /* Skip empty subtrees.  */
  if ((t == NULL) || (*t == NULL_TREE))
    return;

  code = TREE_CODE (*t);
  is_expr = IS_EXPR_CODE_CLASS (TREE_CODE_CLASS (code));
  if (is_expr)
  {
    nt = TREE_TYPE(*t);
    change_var_decl (&nt, new_var, var);
    TREE_TYPE(*t) = nt;
  }
  switch (code)
  {
  case ERROR_MARK:
  case IDENTIFIER_NODE:
  case INTEGER_CST:
  case REAL_CST:
  case FIXED_CST:
  case STRING_CST:
  case BLOCK:
  case PLACEHOLDER_EXPR:
  case FIELD_DECL:
  case VOID_TYPE:
  case REAL_TYPE:
    /* These do not contain variable references. */
    return;

  case SSA_NAME:
    nt = SSA_NAME_VAR(*t);
    change_var_decl (&nt, new_var, var);
    SSA_NAME_VAR(*t) = nt;
    return;

  case LABEL_DECL:
    return;

  case RESULT_DECL:
  case VAR_DECL:
  case PARM_DECL:
    if (strcmp(IDENTIFIER_POINTER(DECL_NAME(*t)),
	       IDENTIFIER_POINTER(DECL_NAME(var))) == 0)
    {
      *t = new_var;
    }
    return;
  case NON_LVALUE_EXPR:
  case CONVERT_EXPR:
  case NOP_EXPR:
    nt = TREE_OPERAND(*t, 0);
    change_var_decl (&nt, new_var, var);
    TREE_OPERAND(*t, 0) = nt;
    return;

  case INIT_EXPR:
    nt = TREE_OPERAND(*t, 0);
    change_var_decl (&nt, new_var, var);
    nt2 = TREE_OPERAND(*t, 1);
    change_var_decl (&nt2, new_var, var);

    TREE_OPERAND(*t, 0) = nt;
    TREE_OPERAND(*t, 1) = nt2;
    return;

  case MODIFY_EXPR:
  case PREDECREMENT_EXPR:
  case PREINCREMENT_EXPR:
  case POSTDECREMENT_EXPR:
  case POSTINCREMENT_EXPR:
    /* These write their result. */

    nt = TREE_OPERAND(*t, 0);
    nt2= TREE_OPERAND(*t, 1);
    change_var_decl (&nt, new_var, var);
    change_var_decl (&nt2, new_var, var);
    TREE_OPERAND(*t, 0) = nt;
    TREE_OPERAND(*t, 1) = nt2;
    return;

  case ADDR_EXPR:
    nt = TREE_OPERAND(*t, 0);
    change_var_decl (&nt, new_var, var);
    TREE_OPERAND(*t, 0) = nt;
    return;

  case ARRAY_REF:
  case BIT_FIELD_REF:
    nt  = TREE_OPERAND(*t, 0);
    nt2 = TREE_OPERAND(*t, 1);
    nt3 = TREE_OPERAND(*t, 2);
    change_var_decl (&nt,  new_var, var);
    change_var_decl (&nt2, new_var, var);
    change_var_decl (&nt3, new_var, var);
    TREE_OPERAND(*t, 0) = nt;
    TREE_OPERAND(*t, 1) = nt2;
    TREE_OPERAND(*t, 2) = nt3;
    return;

  case TREE_LIST:
    nt  = TREE_PURPOSE(*t);
    nt2 = TREE_VALUE (*t);
    nt3 = TREE_CHAIN (*t);
    change_var_decl (&nt,  new_var, var);
    change_var_decl (&nt2, new_var, var);
    change_var_decl (&nt3, new_var, var);
    TREE_OPERAND(*t, 0) = nt;
    TREE_OPERAND(*t, 1) = nt2;
    TREE_OPERAND(*t, 2) = nt3;
    return;

  case TREE_VEC:
  {
    int len = TREE_VEC_LENGTH (*t);
    int i;
    for (i = 0; i < len; i++)
    {
      nt = TREE_VEC_ELT (*t, i);
      change_var_decl (&nt, new_var, var);
      TREE_VEC_ELT (*t, i) = nt;
    }
    return;
  }

  case VECTOR_CST:
    nt = TREE_VECTOR_CST_ELTS (*t);
    change_var_decl (&nt, new_var,  var);
    TREE_VECTOR_CST_ELTS (*t) = nt;
    break;

  case COMPLEX_CST:
    nt  = TREE_REALPART (*t);
    nt2 = TREE_IMAGPART (*t);
    change_var_decl (&nt,  new_var, var);
    change_var_decl (&nt2, new_var, var);
    TREE_REALPART (*t) = nt;
    TREE_IMAGPART (*t) = nt2;
    return;

  case CONSTRUCTOR:
  {
    unsigned HOST_WIDE_INT idx;
    constructor_elt *ce;

    for (idx = 0;
	 VEC_iterate(constructor_elt, CONSTRUCTOR_ELTS (*t), idx, ce);
	 idx++)
    {
      change_var_decl (&ce->value, new_var, var);
    }
    return;
  }

  case BIND_EXPR:
  {
#if 0
    tree decl;
    /* bviyer: probably wont need this */
    for (decl = BIND_EXPR_VARS (*t); decl; decl = TREE_CHAIN (decl))
    {
      change_var_decl(&decl, new_var, var);
    }
#endif
    
    change_var_decl (&BIND_EXPR_BODY (*t),new_var, var);
    return;
  }

  case STATEMENT_LIST:
  {
    tree_stmt_iterator i;
    for (i = tsi_start (*t); !tsi_end_p (i); tsi_next (&i))
      change_var_decl (tsi_stmt_ptr (i), new_var, var);
    return;
  }

  case OMP_PARALLEL:
  case OMP_TASK:
  case OMP_FOR:
  case OMP_SINGLE:
  case OMP_SECTION:
  case OMP_SECTIONS:
  case OMP_MASTER:
  case OMP_ORDERED:
  case OMP_CRITICAL:
  case OMP_ATOMIC:
  case OMP_CLAUSE:
    break;

  case TARGET_EXPR:
  {
    nt  = TREE_OPERAND(*t, 0);
    nt2 = TREE_OPERAND(*t, 1);
    nt3 = TREE_OPERAND(*t, 2);
    nt4 = TREE_OPERAND(*t, 3);
    change_var_decl (&nt,  new_var, var);
    change_var_decl (&nt2, new_var, var);
    change_var_decl (&nt3, new_var, var);
    TREE_OPERAND(*t, 0) = nt;
    TREE_OPERAND(*t, 1) = nt2;
    TREE_OPERAND(*t, 2) = nt3;
    if (TREE_OPERAND (*t, 3) != TREE_OPERAND (*t, 1))
    {
      change_var_decl (&nt4, new_var,  var);
      TREE_OPERAND(*t, 3) = nt4;
    }
    return;
  }

  case RETURN_EXPR:
    return;

  case DECL_EXPR:
    return;

  case INTEGER_TYPE:
  case ENUMERAL_TYPE:
  case BOOLEAN_TYPE:
    nt  = TYPE_MIN_VALUE (*t);
    nt2 = TYPE_MAX_VALUE (*t);
    change_var_decl (&nt,  new_var, var);
    change_var_decl (&nt2, new_var, var);
    TYPE_MIN_VALUE(*t) = nt;
    TYPE_MAX_VALUE(*t) = nt2;
 
    return;

  case POINTER_TYPE:
    nt = TREE_TYPE (*t);
    change_var_decl (&nt, new_var, var);
    TREE_TYPE(*t) = nt;
    break;

  case ARRAY_TYPE:
    nt  = TREE_TYPE (*t);
    nt2 = TYPE_DOMAIN(*t);
    change_var_decl (&nt,  new_var, var);
    change_var_decl (&nt2, new_var, var);
    TREE_TYPE(*t) = nt;
    TYPE_DOMAIN (*t) = nt2;
    return;

  case RECORD_TYPE:
    nt = TYPE_FIELDS(*t);
    change_var_decl(&nt, new_var, var);
    TYPE_FIELDS(*t) = nt;
    return;
    
  case METHOD_TYPE:
    nt  = TYPE_ARG_TYPES (*t);
    nt2 = TYPE_METHOD_BASETYPE (*t);
    change_var_decl(&nt,  new_var, var);
    change_var_decl(&nt2, new_var, var);
    TYPE_ARG_TYPES(*t) = nt;
    TYPE_METHOD_BASETYPE(*t) = nt2;

  case AGGR_INIT_EXPR:
  case CALL_EXPR:
  {
    int len = 0;
    int ii = 0;
    if (TREE_CODE(TREE_OPERAND(*t, 0)) == INTEGER_CST)
    {
      len = TREE_INT_CST_LOW(TREE_OPERAND(*t,0));

      for (ii = 0; ii < len; ii++)
      {
	nt = TREE_OPERAND (*t, ii);
	change_var_decl(&nt, new_var, var);
	TREE_OPERAND(*t, ii) = nt;
      }
      nt2 = TREE_TYPE(*t);
      change_var_decl(&nt2, new_var, var);
      TREE_TYPE (*t) = nt2;
    }
    break;
  }

  default:
    if (is_expr)
    {
      int i, len;

      /* Walk over all the sub-trees of this operand.  */
      len = TREE_CODE_LENGTH (code);

      /* Go through the subtrees.  We need to do this in forward order so
	 that the scope of a FOR_EXPR is handled properly.  */
      for (i = 0; i < len; ++i)
      {
	nt = TREE_OPERAND(*t, i);
	change_var_decl (&nt, new_var, var);
	TREE_OPERAND(*t, i) = nt;
      }
    }
    return;
  }

  
}

void
insert_reduction_values (struct reduction_values **reduction_val_head,
			 enum tree_code reduction_op, tree var_list)
{
  struct reduction_values *red_iter = NULL;
  bool found = false;
  if (reduction_val_head == NULL)
  {
    reduction_val_head = (struct reduction_values **)
      xmalloc(sizeof (struct reduction_values *));
    gcc_assert(reduction_val_head != NULL);
  }
  if (*reduction_val_head == NULL)
  {
    *reduction_val_head = (struct reduction_values *)
      xmalloc(sizeof (struct reduction_values));
    gcc_assert(*reduction_val_head != NULL);

    (*reduction_val_head)->reduction_operator = reduction_op;
    (*reduction_val_head)->tree_reduction_var_list  = var_list;
    (*reduction_val_head)->not_reduced = list_length (var_list);
    (*reduction_val_head)->ptr_next = NULL;
    return;
  }
  else
  {
    for (red_iter = *reduction_val_head; red_iter->ptr_next != NULL;
	 red_iter = red_iter->ptr_next)
    {
      if (red_iter->reduction_operator == reduction_op)
      {
	found = true;
	break;
      }
    }

    if (found)
    {
      red_iter->tree_reduction_var_list =
	tree_cons (NULL_TREE, var_list, red_iter->tree_reduction_var_list);
      red_iter->not_reduced += list_length (red_iter->tree_reduction_var_list);
    }
    else
    {
      red_iter->ptr_next = (struct reduction_values *)
	xmalloc(sizeof (struct reduction_values));
      gcc_assert (red_iter->ptr_next != NULL);

      red_iter->ptr_next->reduction_operator = reduction_op;
      red_iter->ptr_next->tree_reduction_var_list = var_list;
      red_iter->ptr_next->not_reduced =
	list_length (red_iter->tree_reduction_var_list);
      red_iter->ptr_next->ptr_next = NULL;
      
    }
    return;
  }
}
  
bool
same_var_in_multiple_lists_p(struct pragma_simd_values *ps_values)
{
  tree vl_tree = NULL_TREE, vl_value = NULL_TREE;
  tree pr_tree = NULL_TREE, pr_value = NULL_TREE;
  tree rd_tree = NULL_TREE, rd_value = NULL_TREE;
  struct reduction_values *rd_list = NULL;

  if (ps_values->reduction_vals != NULL)
  {
    for (rd_list = ps_values->reduction_vals; rd_list != NULL;
	 rd_list = rd_list->ptr_next)
    {
      rd_tree = rd_list->tree_reduction_var_list;
      vl_tree = ps_values->vectorlength;
      pr_tree = ps_values->private_vars;
	
      while ((vl_tree != NULL_TREE) ||
	     (rd_tree != NULL_TREE) ||
	     (pr_tree != NULL_TREE))
      {
	if (vl_tree != NULL_TREE)
	  vl_value = TREE_VALUE(vl_tree);
	else
	  vl_value = NULL_TREE;
    
	if (pr_tree != NULL_TREE)
	  pr_value = TREE_VALUE(pr_tree);
	else
	  pr_value = NULL_TREE;

	if (rd_tree != NULL_TREE)
	  rd_value = TREE_VALUE(rd_tree);
	else
	  rd_value = NULL_TREE;

	if (((vl_value != NULL_TREE || pr_value != NULL_TREE) &&
	     (simple_cst_equal (vl_value, pr_value) == 1)) ||
	    ((vl_value != NULL_TREE || rd_value != NULL_TREE) &&
	     (simple_cst_equal (vl_value, rd_value) == 1)) ||
	    ((pr_value != NULL_TREE || rd_value != NULL_TREE) &&
	     (simple_cst_equal (pr_value, rd_value) == 1)))
	{
	  return true;
	}

	if (vl_tree != NULL_TREE)
	  vl_tree = TREE_CHAIN(vl_tree);

	if (pr_tree != NULL_TREE)
	  pr_tree = TREE_CHAIN(pr_tree);

	if (rd_tree != NULL_TREE)
	  rd_tree = TREE_CHAIN(rd_tree);
      }
    }
  }
  else
  {    
    while ((vl_tree != NULL_TREE) ||
	   (rd_tree != NULL_TREE) ||
	   (pr_tree != NULL_TREE))
    {
      if (vl_tree != NULL_TREE)
	vl_value = TREE_VALUE(vl_tree);
      else
	vl_value = NULL_TREE;
    
      if (pr_tree != NULL_TREE)
	pr_value = TREE_VALUE(pr_tree);
      else
	pr_value = NULL_TREE;

      if (rd_tree != NULL_TREE)
	rd_value = TREE_VALUE(rd_tree);
      else
	rd_value = NULL_TREE;

      /* IF either one is not NULL and the trees are equal, then we
       * say we have found a duplicate */
      if (((vl_value != NULL_TREE || pr_value != NULL_TREE) &&
	   (simple_cst_equal (vl_value, pr_value) == 1)) ||
	  ((vl_value != NULL_TREE || rd_value != NULL_TREE) &&
	   (simple_cst_equal (vl_value, rd_value) == 1)) ||
	  ((pr_value != NULL_TREE || rd_value != NULL_TREE) &&
	   (simple_cst_equal (pr_value, rd_value) == 1)))
      {
	return true;
      }
 
      
      if (vl_tree != NULL_TREE)
	vl_tree = TREE_CHAIN(vl_tree);

      if (pr_tree != NULL_TREE)
	pr_tree = TREE_CHAIN(pr_tree);

      if (rd_tree != NULL_TREE)
	rd_tree = TREE_CHAIN(rd_tree);
    }
  }
  
  return false;
  
}

void
check_off_reduction_var (gimple reduc_stmt, int pragma_simd_index)
{
  struct pragma_simd_values *ps_iter = NULL;
  struct reduction_values *rv_iter = NULL;
  enum tree_code op_code = NOP_EXPR;
  tree def = NULL_TREE;
  /* tree rhs = NULL_TREE; */
  tree ii_iter = NULL_TREE;
  tree ii_value = NULL_TREE;
  
  
  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return;

  if (psv_head == NULL)
    return;

  if (gimple_code (reduc_stmt) != GIMPLE_ASSIGN)
    return;
  else
  {
    def = gimple_assign_lhs (reduc_stmt);
    gcc_assert (def != NULL_TREE);

    if (TREE_CODE(def) == SSA_NAME)
    {
      def = SSA_NAME_VAR(def);
    }

    op_code = gimple_assign_rhs_code(reduc_stmt);
  }


  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->pragma_encountered == true) &&
	(ps_iter->index == pragma_simd_index))
    {
      break;
    }
  }

  gcc_assert (ps_iter != NULL);

  for (rv_iter = ps_iter->reduction_vals; rv_iter != NULL;
       rv_iter = rv_iter->ptr_next)
  {
    if (rv_iter->reduction_operator == op_code)
    {
      for (ii_iter = rv_iter->tree_reduction_var_list;
	   ii_iter != NULL_TREE;
	   ii_iter = TREE_CHAIN(ii_iter))
      {
	ii_value = TREE_VALUE (ii_iter);

	if (simple_cst_equal (ii_value, DECL_NAME(def)))
	{
	  rv_iter->not_reduced--;
	}
      }
    }
  }
  return;
}
      

HOST_WIDE_INT
find_linear_step_size (int pragma_simd_index, tree var)
{
  tree ii_var_iter   = NULL_TREE;
  tree ii_var_value  = NULL_TREE;
  tree ii_step_iter  = NULL_TREE;
  tree ii_step_value = NULL_TREE;
  HOST_WIDE_INT step = 0;
  struct pragma_simd_values *ps_iter = NULL;

  
  if (psv_head == NULL)
    return 0;

  if (pragma_simd_index <= INVALID_PRAGMA_SIMD_SLOT)
    return 0;

  for (ps_iter = psv_head; ps_iter != NULL; ps_iter = ps_iter->ptr_next)
  {
    if ((ps_iter->pragma_encountered == true) &&
	(ps_iter->index == pragma_simd_index))
    {
      ii_var_iter  = ps_iter->linear_vars;
      ii_step_iter = ps_iter->linear_steps;

      while ((ii_var_iter  != NULL_TREE) &&
	     (ii_step_iter != NULL_TREE))
      {
	ii_var_value  = TREE_VALUE (ii_var_iter);
	ii_step_value = TREE_VALUE (ii_step_iter);

	if (simple_cst_equal (ii_var_value, DECL_NAME(var)))
	{
	  step = int_cst_value (ii_step_value);
	  return step;
	}
      }
    }
  }

  return 0;
}
	
	  
  
