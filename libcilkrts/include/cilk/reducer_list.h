/*
 * Copyright (C) 2009-2011 
 * Intel Corporation
 * 
 * This file is part of the Intel Cilk Plus Library.  This library is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 * 
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

/*
 * reducer_list.h
 *
 * Purpose: Reducer hyperobject to accumulate a list of elements.
 *
 * Classes: reducer_list_append<Type, Allocator>
 *          reducer_list_prepend<Type, Allocator>
 *
 * Description:
 * ============
 * This component provides reducer-type hyperobject representations that allow
 * either prepending or appending values to an STL list.  By replacing the
 * variable with the hyperobject defined in this component, the data race is
 * eliminated.
 *
 * Usage Example:
 * ==============
 * Assume we wish to traverse an array of objects, performing an operation on
 * each object and accumulating the result of the operation into an STL list
 * variable.
 *..
 *  int compute(const X& v);
 *
 *  int test()
 *  {
 *      const std::size_t ARRAY_SIZE = 1000000;
 *      extern X myArray[ARRAY_SIZE];
 *      // ...
 *
 *      std::list<int> result;
 *      for (std::size_t i = 0; i < ARRAY_SIZE; ++i)
 *      {
 *          result.push_back(compute(myArray[i]));
 *      }
 *
 *      std::cout << "The result is: ";
 *      for (std::list<int>::iterator i = result.begin(); i != result.end(); ++i)
 *      {
 *          std::cout << *i << " " << std::endl;
 *      }
 *
 *      return 0;
 *  }
 *..
 * Changing the 'for' to a 'cilk_for' will cause the loop to run in parallel,
 * but doing so will create a data race on the 'result' list.
 * The race is solved by changing 'result' to a 'reducer_list_append' hyperobject:
 *..
 *  int compute(const X& v);
 *
 *  int test()
 *  {
 *      const std::size_t ARRAY_SIZE = 1000000;
 *      extern X myArray[ARRAY_SIZE];
 *      // ...
 *
 *      cilk::hyperobject<cilk::reducer_list_append<int> > result;
 *      cilk_for (std::size_t i = 0; i < ARRAY_SIZE; ++i)
 *      {
 *          result.push_back(compute(myArray[i]));
 *      }
 *
 *      std::cout << "The result is: ";
 *      const std::list &r = result.get_value();
 *      for (std::list<int>::iterator i = r.begin(); i != r.end(); ++i)
 *      {
 *          std::cout << *i << " " << std::endl;
 *      }
 *
 *      return 0;
 *  }
 *..
 *
 * Operations provided:
 * ====================
 *
 * 'reducer_list_prepend' and 'reducer_list_append' support accumulation of an
 * ordered list of items.  Lists accumulated in Cilk++ strands will be merged
 * to maintain the order of the lists - the order will be the same as if the
 * application was run on a single core.
 *..
 * The the current value of the reducer can be gotten and set using the
 * 'get_value', 'get_reference', and 'set_value' methods.  As with most
 * reducers, these methods produce deterministic results only if called before
 * the first spawn after creating a 'hyperobject' or when all strands spawned
 * since creating the 'hyperobject' have been synced.
 */

#ifndef REDUCER_LIST_H_INCLUDED
#define REDUCER_LIST_H_INCLUDED

#include <cilk/reducer.h>
#include <list>

namespace cilk
{

/**
 * @brief Reducer hyperobject to accumulate a list of elements where elements are
 * added to the end of the list.
 */
template<class _Ty,
         class _Ax = std::allocator<_Ty> >
class reducer_list_append
{
public:
    /// std::list reducer_list_prepend is based on
    typedef std::list<_Ty, _Ax> list_type;
    /// Type of elements in a reducer_list_prepend
    typedef _Ty list_value_type;
    /// Type of elements in a reducer_list_prepend
    typedef _Ty basic_value_type;

public:
    /// Definition of data view, operation, and identity for reducer_list_append
    struct Monoid: monoid_base<std::list<_Ty, _Ax> >
    {
        static void reduce (std::list<_Ty, _Ax> *left,
                            std::list<_Ty, _Ax> *right);
    };

private:
    reducer<Monoid> imp_;

public:

    // Default Constructor - Construct a reducer with an empty list
    reducer_list_append();

    // Construct a reducer with an initial list
    reducer_list_append(const std::list<_Ty, _Ax> &initial_value);

    // Return a const reference to the current list
    const std::list<_Ty, _Ax> &get_value() const;

    // Return a reference to the current list
    std::list<_Ty, _Ax> &get_reference();
    std::list<_Ty, _Ax> const &get_reference() const;

    // Replace the list's contents with the given list
    void set_value(const list_type &value);

    // Add an element to the end of the list
    void push_back(const _Ty element);


private:
    // Not copyable
    reducer_list_append(const reducer_list_append&);
    reducer_list_append& operator=(const reducer_list_append&);

};  // class reducer_list_append

/////////////////////////////////////////////////////////////////////////////
// Implementation of inline and template functions
/////////////////////////////////////////////////////////////////////////////

// ------------------------------------------
// template class reducer_list_append::Monoid
// ------------------------------------------

/**
 * Appends list from "right" reducer_list onto the end of the "left".
 * When done, the "right" reducer_list is empty.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @param left reducer_list to be reduced into
 * @param right reducer_list to be reduced from
 */
template<class _Ty, class _Ax>
void
reducer_list_append<_Ty, _Ax>::Monoid::reduce(std::list<_Ty, _Ax> *left,
                                              std::list<_Ty, _Ax> *right)
{
    left->splice(left->end(), *right);
}

/**
 * Default constructor - creates an empty list
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 */
template<class _Ty, class _Ax>
reducer_list_append<_Ty, _Ax>::reducer_list_append() :
    imp_()
{
}

/**
 * Construct a reducer_list_append based on a list
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @param initial_value - [in] Inital list
 */
template<class _Ty, class _Ax>
reducer_list_append<_Ty, _Ax>::reducer_list_append(const std::list<_Ty, _Ax> &initial_value) :
    imp_(std::list<_Ty, _Ax>(initial_value))
{
}

/**
 * Allows read-only access to the list - same as get_reference()
 *
 * @warning If this method is called before the parallel calculation is
 * complete, the list returned by this method will be a partial result.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @returns A const reference to the list that is the current contents of this view.
 */
template<class _Ty, class _Ax>
const std::list<_Ty, _Ax> &reducer_list_append<_Ty, _Ax>::get_value() const
{
    return imp_.view();
}

/**
 * Allows mutable access to list
 *
 * @warning If this method is called before the parallel calculation is
 * complete, the list returned by this method will be a partial result.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @returns A reference to the list that is the current contents of this view.
 */
template<class _Ty, class _Ax>
std::list<_Ty, _Ax> &reducer_list_append<_Ty, _Ax>::get_reference()
{
    return imp_.view();
}

/**
 * Allows read-only access to list
 *
 * @warning If this method is called before the parallel calculation is
 * complete, the list returned by this method will be a partial result.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @returns A const reference to the list that is the current contents of this view
 */
template<class _Ty, class _Ax>
const std::list<_Ty, _Ax> &reducer_list_append<_Ty, _Ax>::get_reference() const
{
    return imp_.view();
}

/**
 * Replace the list's contents
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @param value - The list to replace the current contents of this view
 */
template<class _Ty, class _Ax>
void reducer_list_append<_Ty, _Ax>::set_value(const list_type &value)
{
    // Clean out any value in our list
    imp_.view().clear();

    // If the new list is empty, we're done
    if (value.empty())
        return;

    // Copy each element into our list
    imp_.view() = value;
}

/**
 * Adds an element to the end of the list
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @param element - The element to be added to the end of the list
 */
template<class _Ty, class _Ax>
void reducer_list_append<_Ty, _Ax>::push_back(const _Ty element)
{
    imp_.view().push_back(element);
}

/**
 * @brief Reducer hyperobject to accumulate a list of elements where elements are
 * added to the beginning of the list.
 */
template<class _Ty,
         class _Ax = std::allocator<_Ty> >
class reducer_list_prepend
{
public:
    /// std::list reducer_list_prepend is based on
    typedef std::list<_Ty, _Ax> list_type;
    /// Type of elements in a reducer_list_prepend
    typedef _Ty list_value_type;
    /// Type of elements in a reducer_list_prepend
    typedef _Ty basic_value_type;

public:
    /// @brief Definition of data view, operation, and identity for reducer_list_prepend
    struct Monoid: monoid_base<std::list<_Ty, _Ax> >
    {
        static void reduce (std::list<_Ty, _Ax> *left,
                            std::list<_Ty, _Ax> *right);
    };

private:
    reducer<Monoid> imp_;

public:

    // Default Constructor - Construct a reducer with an empty list
    reducer_list_prepend();

    // Construct a reducer with an initial list
    reducer_list_prepend(const std::list<_Ty, _Ax> &initial_value);

    // Return a const reference to the current list
    const std::list<_Ty, _Ax> &get_value() const;

    // Return a reference to the current list
    std::list<_Ty, _Ax> &get_reference();
    std::list<_Ty, _Ax> const &get_reference() const;

    // Replace the list's contents with the given list
    void set_value(const list_type &value);

    // Add an element to the beginning of the list
    void push_front(const _Ty element);


private:
    // Not copyable
    reducer_list_prepend(const reducer_list_prepend&);
    reducer_list_prepend& operator=(const reducer_list_prepend&);

};  // class reducer_list_prepend

/////////////////////////////////////////////////////////////////////////////
// Implementation of inline and template functions
/////////////////////////////////////////////////////////////////////////////

// ------------------------------------
// template class reducer_list_prepend::Monoid
// ------------------------------------

/**
 * Appends list from "right" reducer_list onto the end of the "left".
 * When done, the "right" reducer_list is empty.
 */
template<class _Ty, class _Ax>
void
reducer_list_prepend<_Ty, _Ax>::Monoid::reduce(std::list<_Ty, _Ax> *left,
                                               std::list<_Ty, _Ax> *right)
{
    left->splice(left->begin(), *right);
}

/**
 * Default constructor - creates an empty list
 */
template<class _Ty, class _Ax>
reducer_list_prepend<_Ty, _Ax>::reducer_list_prepend() :
    imp_(std::list<_Ty, _Ax>())
{
}

/**
 * Construct a reducer_list_prepend based on a list.
 *
 * @param initial_value List used to initialize the reducer_list_prepend
 */
template<class _Ty, class _Ax>
reducer_list_prepend<_Ty, _Ax>::reducer_list_prepend(const std::list<_Ty, _Ax> &initial_value) :
    imp_(std::list<_Ty, _Ax>(initial_value))
{
}

/**
 * Allows read-only access to the list - same as get_reference()
 *
 * @warning If this method is called before the parallel calculation is
 * complete, the list returned by this method will be a partial result.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @returns A const reference to the list that is the current contents of this view.
 */
template<class _Ty, class _Ax>
const std::list<_Ty, _Ax> &reducer_list_prepend<_Ty, _Ax>::get_value() const
{
    return imp_.view();
}

/**
 * Allows mutable access to the list
 *
 * @warning If this method is called before the parallel calculation is
 * complete, the list returned by this method will be a partial result.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @returns A mutable reference to the list that is the current contents of this view.
 */
template<class _Ty, class _Ax>
std::list<_Ty, _Ax> &reducer_list_prepend<_Ty, _Ax>::get_reference()
{
    return imp_.view();
}

/**
 * Allows read-only access to the list
 *
 * @warning If this method is called before the parallel calculation is
 * complete, the list returned by this method will be a partial result.
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @returns A read-only reference to the list that is the current contents of this view.
 */
template<class _Ty, class _Ax>
const std::list<_Ty, _Ax> &reducer_list_prepend<_Ty, _Ax>::get_reference() const
{
    return imp_.view();
}

/**
 * Replace the list's contents
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @param value - The list to replace the current contents of this view
 */
template<class _Ty, class _Ax>
void reducer_list_prepend<_Ty, _Ax>::set_value(const list_type &value)
{
    // Clean out any value in our list
    imp_.view().clear();

    // If the new list is empty, we're done
    if (value.empty())
        return;

    // Copy each element into our list
    imp_.view() = value;
}

/**
 * Add an element to the beginning of the list
 *
 * @tparam _Ty - Type of the list elements
 * @tparam _Ax - Allocator object used to define the storage allocation
 * model.  If not specified, the allocator class template for _Ty is used.
 * @param element Element to be added to the beginning of the list
 */
template<class _Ty, class _Ax>
void reducer_list_prepend<_Ty, _Ax>::push_front(const _Ty element)
{
    imp_.view().push_front(element);
}

}	// namespace cilk

#endif //  REDUCER_LIST_H_INCLUDED
