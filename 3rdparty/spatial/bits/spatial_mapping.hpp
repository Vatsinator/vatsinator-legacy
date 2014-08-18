// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_mapping.hpp
 *  Contains the definition of the \mapping_iterator.
 */

#ifndef SPATIAL_MAPPING_HPP
#define SPATIAL_MAPPING_HPP

#include <utility> // provides ::std::pair<> and ::std::make_pair()
#include "../traits.hpp"
#include "spatial_bidirectional.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"

namespace spatial
{
  namespace details
  {
    /**
     *  Extra information needed by the iterator to perform its work. This
     *  information is copied to each iterator from a given container.
     *
     *  Although it may be possible to modify this information directly from
     *  it's members, it may be unwise to do so, as it could invalidate the
     *  iterator and cause the program to behave unexpectedly. If any of this
     *  information needs to be modified, it is probably recommended to create a
     *  new iterator altogether.
     *
     *  \tparam Ct The container to which these iterator relate to.
     */
    template <typename Ct>
    struct Mapping_data : container_traits<Ct>::key_compare
    {
      //! Build an uninitialized mapping data object.
      Mapping_data() { }

      /**
       *  Builds required mapping data from the given container, dimension and
       *  mapping dimension.
       *
       *  \param c The container being iterated.
       *  \param m The mapping dimension used in the iteration.
       */
      Mapping_data
      (const Ct& c, dimension_type m)
        : container_traits<Ct>::key_compare(c.key_comp()), mapping_dim(m)
      { }

      /**
       *  Builds required mapping data from the given key comparison functor,
       *  dimension and mapping dimension.
       *
       *  \param c The container being iterated.
       *  \param m The mapping dimension used in the iteration.
       */
      Mapping_data
      (const typename container_traits<Ct>::key_compare& c, dimension_type m)
        : container_traits<Ct>::key_compare(c), mapping_dim(m)
      { }

      /**
       *  The current dimension of iteration.
       *
       *  You can modify this key if you suddenly want the iterator to change
       *  dimension of iteration. However this field must always satisfy:
       *  \f[
       *  mapping_dim() < Rank()
       *  \f]
       *  Rank being the template rank provider for the iterator.
       *
       *  \attention If you modify this value directly, no safety check will be
       *  performed.
       */
      dimension_type mapping_dim;
    };
  }

  /**
   *  This iterator walks through all items in the container in order from the
   *  lowest to the highest value along a particular dimension. The \c key_comp
   *  comparator of the container is used for comparision.
   *
   *  In effect, that makes any container of the library behave as a \c std::set
   *  or \c std::map. Through this iterator, a spatial container with 3
   *  dimensions can provide the same features as 3 \c std::set(s) or \c
   *  std::map(s) containing the same elements and ordered on each of these
   *  dimensions. Beware that iteration through the tree is very efficient when
   *  the dimension \kdtree is very small by comparison to the number of
   *  objects, but pretty inefficient otherwise, by comparison to a \c std::set.
   */
  template<typename Ct>
  class mapping_iterator
    : public details::Bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Uninitialized iterator.
    mapping_iterator() { }

    /**
     *  The standard way to build this iterator: specify a mapping dimension, an
     *  iterator on a container, and that container.
     *
     *  \param container   The container to iterate.
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *  \param iter        Use the value of \c iter as the start point for the
     *                     iteration.
     */
    mapping_iterator(Ct& container, dimension_type mapping_dim,
                     typename container_traits<Ct>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref spatial::details::modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *  \param dim The dimension of the node pointed to by iterator.
     *  \param ptr Use the value of node as the start point for the
     *             iteration.
     */
    mapping_iterator(Ct& container, dimension_type mapping_dim,
                     dimension_type dim,
                     typename container_traits<Ct>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<Ct>& operator++()
    { return increment_mapping(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    mapping_iterator<Ct> operator++(int)
    {
      mapping_iterator<Ct> x(*this);
      increment_mapping(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<Ct>& operator--()
    { return decrement_mapping(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    mapping_iterator<Ct> operator--(int)
    {
      mapping_iterator<Ct> x(*this);
      decrement_mapping(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return static_cast<const key_compare&>(_data); }

    /**
     *  Accessor to the mapping dimension used by the iterator.
     *
     *  No check is performed on this accessor if a new mapping dimension is
     *  given. If you need to check that the mapping dimension given does not
     *  exceed the rank use the function \ref spatial::mapping_dimension()
     *  instead:
     *
     *  \code
     *  // Create a mapping iterator that works on dimension 0
     *  mapping_iterator<my_container> iter (begin_mapping(container, 0));
     *  // Reset the mapping iterator to work on dimension 2
     *  mapping_dimension(iter, 2);
     *  // This will throw if the container used has a rank lower than 3.
     *  \endcode
     */
    ///@{
    dimension_type
    mapping_dimension() const { return _data.mapping_dim; }
    dimension_type&
    mapping_dimension() { return _data.mapping_dim; }
    ///@}

  private:
    //! The related data for the iterator.
    details::Mapping_data<Ct> _data;
  };

  /**
   *  This iterator walks through all items in the container in order from the
   *  lowest to the highest value along a particular dimension. The \c key_comp
   *  comparator of the container is used for comparision.
   *
   *  In effect, that makes any container of the library behave as a \c std::set
   *  or \c std::map. Through this iterator, a spatial container with 3
   *  dimensions can provide the same features as 3 \c std::set(s) or \c
   *  std::map(s) containing the same elements and ordered on each of these
   *  dimensions. Beware that iteration through the tree is very efficient when
   *  the dimension \kdtree is very small by comparison to the number of
   *  objects, but pretty inefficient otherwise, by comparison to a \c std::set.
   *
   *  Object deferenced by this iterator are always constant.
   */
  template<typename Ct>
  class mapping_iterator<const Ct>
    : public details::Const_bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Alias for the key_compare type used by the iterator.
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! Build an uninitialized iterator.
    mapping_iterator() { }

    /**
     *  The standard way to build this iterator: specify a mapping dimension,
     *  an iterator on a container, and that container.
     *
     *  \param container The container to iterate.
     *  \param mapping_dim The dimension used to order all nodes during the
     *  iteration.
     *  \param iter Use the value of \c iter as the start point for the
     *  iteration.
     */
    mapping_iterator(const Ct& container, dimension_type mapping_dim,
                     typename container_traits<Ct>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this function saves some CPU
     *  cycle, by comparison to the other.
     *
     *  In order to iterate through nodes in the \kdtree built in the
     *  container, the algorithm must know at each node which dimension is
     *  used to partition the space. Some algorithms will provide this
     *  dimension, such as the function \ref spatial::details::modulo().
     *
     *  \attention Specifying the incorrect dimension value for the node will
     *  result in unknown behavior. It is recommended that you do not use this
     *  constructor if you are not sure about this dimension, and use the
     *  other constructors instead.
     *
     *  \param container The container to iterate.
     *
     *  \param mapping_dim The dimension used to order all nodes during the
     *                     iteration.
     *
     *  \param dim The dimension of the node pointed to by iterator.
     *
     *  \param ptr Use the value of \c node as the start point for the
     *             iteration.
     */
    mapping_iterator
    (const Ct& container, dimension_type mapping_dim, dimension_type dim,
     typename container_traits<Ct>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _data(container, mapping_dim)
    { except::check_dimension(container.dimension(), mapping_dim); }

    //! Convertion of mutable iterator into a constant iterator is permitted.
    mapping_iterator(const mapping_iterator<Ct>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _data(iter.key_comp(), iter.mapping_dimension()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<const Ct>& operator++()
    { return increment_mapping(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    mapping_iterator<const Ct> operator++(int)
    {
      mapping_iterator<const Ct> x(*this);
      increment_mapping(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    mapping_iterator<const Ct>& operator--()
    { return decrement_mapping(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    mapping_iterator<const Ct> operator--(int)
    {
      mapping_iterator<const Ct> x(*this);
      decrement_mapping(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return static_cast<const key_compare&>(_data); }

    /**
     *  Accessor to the mapping dimension used by the iterator.
     *
     *  No check is performed on this accessor if a new mapping dimension is
     *  given. If you need to check that the mapping dimension given does not
     *  exceed the rank use the function \ref spatial::mapping_dimension()
     *  instead:
     *
     *  \code
     *  // Create a mapping iterator that works on dimension 0
     *  mapping_iterator<my_container> iter (begin_mapping(container, 0));
     *  // Reset the mapping iterator to work on dimension 2
     *  mapping_dimension(iter, 2);
     *  // This will throw if the container used has a rank lower than 3.
     *  \endcode
     */
    ///@{
    dimension_type
    mapping_dimension() const { return _data.mapping_dim; }
    dimension_type&
    mapping_dimension() { return _data.mapping_dim; }
    ///@}

  private:
    //! The related data for the iterator.
    details::Mapping_data<Ct> _data;
  };

  /**
   *  Return the mapping dimension of the iterator.
   *  \param it the iterator where the mapping dimension is retreived.
   */
  template <typename Container>
  inline dimension_type
  mapping_dimension(const mapping_iterator<Container>& it)
  { return it.mapping_dimension(); }

  /**
   *  Sets the mapping dimension of the iterator.
   *  \param it The iterator where the mapping dimension is set.
   *  \param mapping_dim The new mapping dimension to use.
   */
  template <typename Container>
  inline void
  mapping_dimension(mapping_iterator<Container>& it,
                    dimension_type mapping_dim)
  {
    except::check_dimension(it.dimension(), mapping_dim);
    it.mapping_dimension() = mapping_dim;
  }

  namespace details
  {
    /**
     *  Move the pointer given in parameter to the next element in the ordered
     *  iteration of values along the mapping dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. You should use the
     *  overload \c operator++ on the \mapping_iterator instead. This function
     *  does not perform any sanity checks on the iterator given in parameter.
     *
     *  Since Container is based on \kdtree and \kdtree exhibit good locality of
     *  reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Onlognk in
     *  practice.
     *
     *  \fractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the pointer given in parameter to the previous element in the
     *  ordered iteration of values along the mapping dimension.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. You should use the
     *  overload \c operator-- on the \mapping_iterator instead. This function
     *  does not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter The reference iterator that points to the current node.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  along \c iter's \c mapping_dim, and among the children of the node
     *  pointed to by \c iter.
     *
     *  Since Container is based on \kdtree and \kdtree exhibit good locality of
     *  reference (for arranging values in space, not for values location in
     *  memory), the function will run with time complexity close to \Onlognk in
     *  practice.
     *
     *  \fractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    decrement_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the minimum value along the
     *  iterator's mapping dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  spatial::mapping_begin(). This function does not perform any sanity
     *  checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  smallest coordinate along \c iter's \c mapping_dim, and among the
     *  children of the node pointed to by \c iter.
     *
     *  \fractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    minimum_mapping(mapping_iterator<Container>& iter);

    /**
     *  Move the iterator given in parameter to the maximum value along the
     *  iterator's mapping dimension but only in the sub-tree composed of the
     *  node pointed to by the iterator and its children.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  spatial::mapping_end().  This function does not perform any sanity
     *  checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \return The iterator given in parameter is moved to the value with the
     *  largest coordinate along \c iter's \c mapping_dim, among the children of
     *  the node pointed to by \c iter.
     *
     *  \fractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter);
  } // namespace details

  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_end(Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<Container>
      (container, mapping_dim, container.dimension() - 1,
       container.end().node); // At header (dim = rank - 1)
  }

  ///@{
  /**
   *  Finds the past-the-end position in \c container for this constant
   *  iterator.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the past-the-end position in the
   *  container.
   *
   *  \consttime
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_end(const Container& container, dimension_type mapping_dim)
  {
    except::check_dimension(container.dimension(), mapping_dim);
    return mapping_iterator<const Container>
      (container, mapping_dim, container.dimension() - 1,
       container.end().node); // At header (dim = rank - 1)
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cend(const Container& container, dimension_type mapping_dim)
  { return mapping_end(container, mapping_dim); }
  ///@}

  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c mapping_dim.
   *
   *  \attention This iterator impose constness constraints on its \c value_type
   *  if the container's is a set and not a map. Iterators on sets prevent
   *  modification of the \c value_type because modifying the key may result in
   *  invalidation of the tree. If the container is a map, only the \c
   *  mapped_type can be modified (the \c second element).
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_begin(Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<Container> it(container, mapping_dim, 0,
                                   container.end().node->parent);
    return details::minimum_mapping(it);
  }

  ///@{
  /**
   *  Finds the value in \c container for which its key has the smallest
   *  coordinate over the dimension \c mapping_dim.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_begin(const Container& container, dimension_type mapping_dim)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<const Container>
      it(container, mapping_dim, 0, container.end().node->parent);
    return details::minimum_mapping(it);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cbegin(const Container& container, dimension_type mapping_dim)
  { return mapping_begin(container, mapping_dim); }
  ///@}

  namespace details
  {
    /**
     *  Return true if \c x coordinate is less than \c y coordinate over
     *  dimension \c node_dim, given \c compare. If both coordinate are equal,
     *  then return true if address of \c x is less than address of \c y.
     *
     *  This operator always discriminate \c x and \c y unless they are
     *  precisely the same object.
     *  \tparam Key The key object to use in the comparison.
     *  \tparam Compare The comparison object that is a model of
     *  \trivial_compare.
     */
    template <typename Key, typename Compare>
    inline bool
    less_by_ref(const Compare& compare, dimension_type node_dim,
                const Key& x, const Key& y)
    {
      return (compare(node_dim, x, y) || (&x < &y && !compare(node_dim, y, x)));
    }

    //! Specialization for iterators pointing to node using the relaxed
    //! invariant.
    template <typename Container>
    inline mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter,
                      relaxed_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      node_ptr best = 0; // not null when best has been found
      dimension_type best_dim = iter.node_dim;
      node_ptr r_node = iter.node;
      dimension_type r_node_dim = iter.node_dim;
      node_ptr l_node = iter.node;
      dimension_type l_node_dim = iter.node_dim;
      bool l_break = false;
      bool r_break = false;
      do
        {
          if (!l_break) // One step left
            {
              if (l_node->left != 0
                  && (l_node_dim != iter.mapping_dimension()
                      || !cmp(l_node_dim, const_key(l_node),
                              const_key(iter.node))))
                {
                  l_node = l_node->left;
                  l_node_dim = incr_dim(rank, l_node_dim);
                  while (l_node->right != 0
                         && (l_node_dim != iter.mapping_dimension()
                             || best == 0 || !cmp(l_node_dim, const_key(best),
                                                  const_key(l_node))))
                    {
                      l_node = l_node->right;
                      l_node_dim = incr_dim(rank, l_node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(iter.node), const_key(l_node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(l_node), const_key(best))))
                    { best = l_node; best_dim = l_node_dim; }
                }
              else
                {
                  node_ptr p = l_node->parent;
                  while (!header(p) && p->left == l_node)
                    {
                      l_node = p;
                      l_node_dim = decr_dim(rank, l_node_dim);
                      p = p->parent;
                    }
                  l_node = p;
                  l_node_dim = decr_dim(rank, l_node_dim);
                  if (!header(l_node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(l_node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(l_node),
                                             const_key(best))))
                        { best = l_node; best_dim = l_node_dim; }
                    }
                  else
                    { if (r_break) break; l_break = true; }
                }
            }
          if (!r_break) // One step right
            {
              if (r_node->right != 0
                  && (r_node_dim != iter.mapping_dimension()
                      || best == 0 || !cmp(r_node_dim, const_key(best),
                                           const_key(r_node))))
                {
                  r_node = r_node->right;
                  r_node_dim = incr_dim(rank, r_node_dim);
                  while (r_node->left != 0
                         && (r_node_dim != iter.mapping_dimension()
                             || !cmp(r_node_dim, const_key(r_node),
                                     const_key(iter.node))))
                    {
                      r_node = r_node->left;
                      r_node_dim = incr_dim(rank, r_node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(iter.node), const_key(r_node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(r_node), const_key(best))))
                    { best = r_node; best_dim = r_node_dim; }
                }
              else
                {
                  node_ptr p = r_node->parent;
                  while (!header(p) && p->right == r_node)
                    {
                      r_node = p;
                      r_node_dim = decr_dim(rank, r_node_dim);
                      p = p->parent;
                    }
                  r_node = p;
                  r_node_dim = decr_dim(rank, r_node_dim);
                  if (!header(r_node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(r_node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(r_node),
                                             const_key(best))))
                        { best = r_node; best_dim = r_node_dim; }
                    }
                  else
                    { if (l_break) break; r_break = true; }
                }
            }
        }
      while (true);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = l_node; iter.node_dim = l_node_dim; }
      SPATIAL_ASSERT_CHECK(header(r_node));
      SPATIAL_ASSERT_CHECK(header(l_node));
      SPATIAL_ASSERT_CHECK(r_node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(l_node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointing to node using the strict
    //! invariant.
    template <typename Container>
    inline mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter,
                      strict_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.node_dim;
      dimension_type best_dim = iter.node_dim;
      if (node->left != 0 && node_dim != iter.mapping_dimension()) // optimize
        {
          do
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->left != 0
                 && (node_dim != iter.mapping_dimension() // optimize
                     || cmp(node_dim, const_key(iter.node),
                            const_key(node))));
        }
      node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.mapping_dimension(), const_key(iter.node),
                      const_key(node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->right != 0
              && (node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(node_dim, const_key(best), const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              while (node->left != 0
                     && (node_dim != iter.mapping_dimension()  // optimize
                         || cmp(node_dim,
                                const_key(iter.node), const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(iter.node), const_key(node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(node), const_key(best))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              node_ptr p = node->parent;
              // go upward as long as we don't hit header or ceiling
              while (!header(p)
                     && ((p != ceiling)
                         ? (p->right == node)
                         // either sibling is visited or there is one child
                         : (sibling_visited
                            || (p->right == node && p->left == 0)
                            || (p->left == node && p->right == 0))))
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                  if (node == ceiling)
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(node),
                                             const_key(best))))
                        { best = node; best_dim = node_dim; }
                      sibling_visited = false;
                      ceiling = p;
                    }
                }
              if (!header(p) && p == ceiling)
                {
                  if (p->right == node) { node = p->left; }
                  else { node = p->right; }
                  sibling_visited = true;
                  // go to full left in unvisited sibling
                  while (node->left != 0
                         && (node_dim != iter.mapping_dimension() // optimize
                             || cmp(node_dim,
                                    const_key(iter.node), const_key(node))))
                    {
                      node = node->left;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(iter.node), const_key(node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(node), const_key(best))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(iter.node), const_key(node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(node),
                                             const_key(best))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(header(node));
      SPATIAL_ASSERT_CHECK(node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    increment_mapping(mapping_iterator<Container>& iter)
    {
      return increment_mapping
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter,
                    relaxed_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          if (iter.node->left != 0
              && (iter.node_dim != iter.mapping_dimension()
                  || !cmp(iter.mapping_dimension(),
                          const_key(iter.node), const_key(best))))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(best), const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->left == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && less_by_ref(cmp, iter.mapping_dimension(),
                                 const_key(best), const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    template<typename Container>
    inline mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter,
                    strict_invariant_tag)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      node_ptr end = iter.node->parent;
      while (iter.node->right != 0)
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          if (iter.node->left != 0
              && iter.node_dim != iter.mapping_dimension()) // optimize
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0)
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(best), const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->left == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && less_by_ref(cmp, iter.mapping_dimension(),
                                 const_key(best), const_key(iter.node)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      iter.node = best; iter.node_dim = best_dim;
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      return iter;
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    maximum_mapping(mapping_iterator<Container>& iter)
    {
      return maximum_mapping
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // The next largest key on the mapping dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container>
    inline mapping_iterator<Container>&
    decrement_mapping(mapping_iterator<Container>& iter)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_mapping(iter);
        }
      node_ptr best = 0; // not null when best has been found
      node_ptr node = iter.node;
      dimension_type node_dim = iter.node_dim;
      dimension_type best_dim = iter.node_dim;
      if (node->right != 0)
        {
          do
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
            }
          while (node->right != 0
                 && (node_dim != iter.mapping_dimension()
                     || !cmp(node_dim,
                             const_key(iter.node), const_key(node))));
        }
      node_ptr ceiling = node->parent; // the upper limit of unvisited nodes
      bool sibling_visited = false;    // at ceiling, sibling node is visited
      if (less_by_ref(cmp, iter.mapping_dimension(),
                      const_key(node), const_key(iter.node)))
        { best = node; best_dim = node_dim; }
      do
        {
          if (node->left != 0
              && (node_dim != iter.mapping_dimension()
                  || best == 0
                  || !cmp(node_dim, const_key(node), const_key(best))))
            {
              node = node->left;
              node_dim = incr_dim(rank, node_dim);
              while (node->right != 0
                     && (node_dim != iter.mapping_dimension()
                         || !cmp(node_dim,
                                 const_key(iter.node), const_key(node))))
                {
                  node = node->right;
                  node_dim = incr_dim(rank, node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(node), const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(best), const_key(node))))
                { best = node; best_dim = node_dim; }
            }
          else
            {
              node_ptr p = node->parent;
              // go upward as long as we don't hit header or ceiling
              while (!header(p)
                     && ((p != ceiling)
                         ? (p->left == node)
                         // either sibling is visited or there is one child
                         : (sibling_visited
                            || (p->right == node && p->left == 0)
                            || (p->left == node && p->right == 0))))
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                  if (node == ceiling)
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(node), const_key(iter.node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(best),
                                             const_key(node))))
                        { best = node; best_dim = node_dim; }
                      sibling_visited = false;
                      ceiling = p;
                    }
                }
              if (!header(p) && p == ceiling)
                {
                  if (p->right == node) { node = p->left; }
                  else { node = p->right; }
                  sibling_visited = true;
                  // go to full right in unvisited sibling
                  while (node->right != 0
                         && (node_dim != iter.mapping_dimension()
                             || !cmp(node_dim, const_key(iter.node),
                                     const_key(node))))
                    {
                      node = node->right;
                      node_dim = incr_dim(rank, node_dim);
                    }
                  if (less_by_ref(cmp, iter.mapping_dimension(),
                                  const_key(node), const_key(iter.node))
                      && (best == 0
                          || less_by_ref(cmp, iter.mapping_dimension(),
                                         const_key(best), const_key(node))))
                    { best = node; best_dim = node_dim; }
                }
              else
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  if (!header(node))
                    {
                      if (less_by_ref(cmp, iter.mapping_dimension(),
                                      const_key(node), const_key(iter.node))
                          && (best == 0
                              || less_by_ref(cmp, iter.mapping_dimension(),
                                             const_key(best),
                                             const_key(node))))
                        { best = node; best_dim = node_dim; }
                    }
                }
            }
        }
      while (!header(node));
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      else { iter.node = node; iter.node_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(header(node));
      SPATIAL_ASSERT_CHECK(node_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate in
    // left-first fashion.
    template <typename Container>
    inline mapping_iterator<Container>&
    minimum_mapping(mapping_iterator<Container>& iter)
    {
      typedef typename mapping_iterator<Container>::node_ptr node_ptr;
      const typename container_traits<Container>::rank_type
        rank(iter.rank());
      const typename container_traits<Container>::key_compare
        cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.mapping_dimension() < rank());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      node_ptr end = iter.node->parent;
      while (iter.node->left != 0)
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      node_ptr best = iter.node;
      dimension_type best_dim = iter.node_dim;
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension()
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0)
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (less_by_ref(cmp, iter.mapping_dimension(),
                              const_key(iter.node), const_key(best)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
          else
            {
              node_ptr p = iter.node->parent;
              while (p != end && p->right == iter.node)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
              if (iter.node != end
                  && less_by_ref(cmp, iter.mapping_dimension(),
                                 const_key(iter.node), const_key(best)))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(best_dim < rank());
      SPATIAL_ASSERT_CHECK(best != 0);
      SPATIAL_ASSERT_CHECK(!header(best));
      iter.node = best; iter.node_dim = best_dim;
      return iter;
    }
  } // namespace details
} // namespace spatial

#endif // SPATIAL_MAPPING_HPP
