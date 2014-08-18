// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   mapping_iterator.hpp
 *  Provides mapping iterator and all the functions around it.
 */

#ifndef SPATIAL_MAPPING_ITERATOR_HPP
#define SPATIAL_MAPPING_ITERATOR_HPP

#include <utility> // std::pair
#include "bits/spatial_mapping.hpp"

namespace spatial
{
  /**
   *  This structure defines a pair of mutable mapping iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair
    : std::pair<mapping_iterator<Ct>, mapping_iterator<Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<mapping_iterator<Ct>, mapping_iterator<Ct> > Base;

    //! Empty constructor.
    mapping_iterator_pair() { }

    //! Regular constructor that builds a mapping_iterator_pair out of 2
    //! mapping_iterators.
    mapping_iterator_pair(const mapping_iterator<Ct>& a,
                          const mapping_iterator<Ct>& b) : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant mapping iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \see mapping_iterator
   */
  template <typename Ct>
  struct mapping_iterator_pair<const Ct>
    : std::pair<mapping_iterator<const Ct>, mapping_iterator<const Ct> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<mapping_iterator<const Ct>, mapping_iterator<const Ct> >
    Base;

    //! Empty constructor.
    mapping_iterator_pair() { }

    //! Regular constructor that builds a mapping_iterator_pair out of 2
    //! mapping_iterators.
    mapping_iterator_pair(const mapping_iterator<const Ct>& a,
                          const mapping_iterator<const Ct>& b) : Base(a, b)
    { }

    //! Convert a mutable mapping iterator pair into a const mapping iterator
    //!pair.
    mapping_iterator_pair(const mapping_iterator_pair<Ct>& p)
      : Base(p.first, p.second) { }
  };

  namespace details
  {
    /**
     *  Move the iterator given in parameter to the value with the smallest
     *  coordinate greater or equal to \c bound along the mapping dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The lower bound to the iterator position.
     *  \return An iterator pointing to the value with the smallest coordinate
     *  greater or equal to \c bound along \c iter's \c mapping_dim, or to the
     *  parent of the value pointed to.
     *
     *  \fractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    lower_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);

    /**
     *  Move the iterator given in parameter to the value with the largest
     *  coordinate strictly lower than \c bound along the mapping dimension of
     *  \c iter, but only in the sub-tree composed of the node pointed to by the
     *  iterator and its children. If no such value exists, then move the
     *  iterator to the parent of the value currently pointed to.
     *
     *  \attention This function is meant to be used by other algorithms in the
     *  library, but not by the end users of the library. If you feel that you
     *  must use this function, maybe you were actually looking for \ref
     *  mapping_begin(). In any case, use it cautiously, as this function does
     *  not perform any sanity checks on the iterator given in parameter.
     *
     *  \tparam Container The type of container to iterate.
     *  \param iter An iterator that points to the root node of the search.
     *  \param bound The upper bound to the iterator position.
     *  \return \c iter moved to the value with the largest coordinate strictly
     *  less than \c bound along \c iter's \c mapping_dim, or to the
     *  parent of the value pointed to.
     *
     *  \fractime
     */
    template <typename Container>
    mapping_iterator<Container>&
    upper_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound);
  } // namespace details

  /**
   *  Returns a pair of iterator on the first and the last value in the range
   *  that can be iterated. This function is convenient to use with
   *  \c std::tie, and is equivalent to calling \ref mapping_begin() and \ref
   *  mapping_end() on both iterators.
   *
   *  To combine it with \c std::tie, use it this way:
   *  \code
   *  mapping<pointset<2, int> >::iterator it, end;
   *  std::tie(it, end) = mapping_range(0, my_points);
   *  for(; it != end; ++it)
   *  {
   *    // ...
   *  }
   *  \endcode
   *  Notice that an \c iterator type is declared, and not an \c iterator_pair
   *  type.
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
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator_pair<Container>
  mapping_range(Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<Container>
          (mapping_begin(container, mapping_dim),
           mapping_end(container, mapping_dim));
  }

  ///@{
  /**
   *  Returns a pair of constant iterator on the first and the last value in the
   *  range that can be iterated. This function is convenient to use with
   *  \c std::tie, and is equivalent to calling \ref mapping_begin() and \ref
   *  mapping_end() on both iterators.
   *
   *  To combine it with \c std::tie, use it this way:
   *  \code
   *  mapping<pointset<2, int> >::iterator it, end;
   *  std::tie(it, end) = mapping_range(0, my_points);
   *  for(; it != end; ++it)
   *  {
   *    // ...
   *  }
   *  \endcode
   *  Notice that an \c iterator type is declared, and not an \c iterator_pair
   *  type.
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
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator_pair<const Container>
  mapping_range(const Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<const Container>
      (mapping_begin(container, mapping_dim),
       mapping_end(container, mapping_dim));
  }

  template <typename Container>
  inline mapping_iterator_pair<const Container>
  mapping_crange(const Container& container, dimension_type mapping_dim)
  {
    return mapping_iterator_pair<const Container>
      (mapping_begin(container, mapping_dim),
       mapping_end(container, mapping_dim));
  }
  ///@}

  /**
   *  Finds the value with the smallest coordinate along the mapping dimension
   *  that is greater or equal to \c bound, and return an iterator pointing to
   *  this value.
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
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_lower_bound(Container& container, dimension_type mapping_dim,
                      const typename container_traits<Container>::key_type&
                      bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<Container> it
      (container, mapping_dim, 0, container.end().node->parent);
    return details::lower_bound_mapping(it, bound);
  }

  ///@{
  /**
   *  Finds the value with the smallest coordinate along the mapping dimension
   *  that is greater or equal to \c bound, and return a constant iterator to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_lower_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<const Container> it
      (container, mapping_dim, 0, container.end().node->parent);
    return details::lower_bound_mapping(it, bound);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_clower_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  { return mapping_lower_bound(container, mapping_dim, bound); }
  ///@}

  /**
   *  Finds the value with the largest coordinate along the mapping dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
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
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator<Container>
  mapping_upper_bound
  (Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<Container> it // At root (dim = 0)
      (container, mapping_dim, 0, container.end().node->parent);
    return details::upper_bound_mapping(it, bound);
  }

  ///@{
  /**
   *  Finds the value with the largest coordinate along the mapping dimension
   *  that is stricly less than \c bound, and return an iterator pointing to
   *  this value.
   *
   *  \tparam Container The type of container to iterate.
   *  \param mapping_dim The dimension that is the reference for the iteration:
   *  all iterated values will be ordered along this dimension, from smallest to
   *  largest.
   *  \param bound The lowest bound to the iterator position.
   *  \param container The container to iterate.
   *  \throw invalid_dimension If the dimension specified is larger than the
   *  dimension from the rank of the container.
   *  \return An iterator pointing to the value with the smallest coordinate
   *  greater or equal to \c bound along \c mapping_dim.
   *
   *  \fractime
   */
  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_upper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  {
    if (container.empty()) return mapping_end(container, mapping_dim);
    except::check_dimension(container.dimension(), mapping_dim);
    mapping_iterator<const Container> it  // At root (dim = 0)
      (container, mapping_dim, 0, container.end().node->parent);
    return details::upper_bound_mapping(it, bound);
  }

  template <typename Container>
  inline mapping_iterator<const Container>
  mapping_cupper_bound
  (const Container& container, dimension_type mapping_dim,
   const typename container_traits<Container>::key_type& bound)
  { return mapping_upper_bound(container, mapping_dim, bound); }
  ///@}

  namespace details
  {
    //! Specialization for iterators pointed to node using the relaxed
    //! invariant.
    template<typename Container>
    inline mapping_iterator<Container>&
    lower_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension()
                 || !cmp(iter.node_dim, const_key(iter.node), bound)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.node_dim, const_key(best),
                          const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && (iter.node_dim != iter.mapping_dimension()
                         || !cmp(iter.node_dim, const_key(iter.node), bound)))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
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
                  && !cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    //! Specialization for iterators pointed to node using the strict
    //! invariant.
    template<typename Container>
    inline mapping_iterator<Container>&
    lower_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension() // optimize
                 || cmp(iter.node_dim, bound, const_key(iter.node))))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0 // optimize
                     && (iter.node_dim != iter.mapping_dimension()
                         || cmp(iter.node_dim, bound,
                                const_key(iter.node))))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (!cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
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
                  && !cmp(iter.mapping_dimension(), const_key(iter.node), bound)
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    lower_bound_mapping(mapping_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return lower_bound_mapping
        (iter, bound,
         typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline mapping_iterator<Container>&
    upper_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension()
                 || !cmp(iter.node_dim, const_key(iter.node), bound)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (cmp(iter.mapping_dimension(), bound, const_key(iter.node)))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && (iter.node_dim != iter.mapping_dimension()
                         || !cmp(iter.node_dim, const_key(iter.node), bound)))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (cmp(iter.mapping_dimension(), bound, const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
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
                  && cmp(iter.mapping_dimension(), bound, const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    // Walk tree nodes in right-first fashion, bouncing off values that are
    // higher than key.
    template <typename Container>
    inline mapping_iterator<Container>&
    upper_bound_mapping
    (mapping_iterator<Container>& iter,
     const typename container_traits<Container>::key_type& bound,
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
      node_ptr best = 0;
      dimension_type best_dim = 0;
      while (iter.node->left != 0
             && (iter.node_dim != iter.mapping_dimension()
                 // Optimization for strict invariant
                 || cmp(iter.node_dim, bound, const_key(iter.node))))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      if (cmp(iter.mapping_dimension(), bound, const_key(iter.node)))
        { best = iter.node; best_dim = iter.node_dim; }
      do
        {
          if (iter.node->right != 0
              && (iter.node_dim != iter.mapping_dimension() || best == 0
                  || !cmp(iter.mapping_dimension(),
                          const_key(best), const_key(iter.node))))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && (iter.node_dim != iter.mapping_dimension()
                         // Optimization for strict invariant
                         || cmp(iter.node_dim, bound, const_key(iter.node))))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
              if (cmp(iter.mapping_dimension(), bound, const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
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
                  && cmp(iter.mapping_dimension(), bound, const_key(iter.node))
                  && (best == 0
                      || less_by_ref(cmp, iter.mapping_dimension(),
                                     const_key(iter.node), const_key(best))))
                { best = iter.node; best_dim = iter.node_dim; }
            }
        }
      while (iter.node != end);
      if (best != 0) { iter.node = best; iter.node_dim = best_dim; }
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline mapping_iterator<Container>&
    upper_bound_mapping(mapping_iterator<Container>& iter,
                        const typename container_traits<Container>::key_type&
                        bound)
    {
      return upper_bound_mapping
        (iter, bound,
         typename container_traits<Container>::mode_type::invariant_category());
    }
  } // namespace details
}

#endif // SPATIAL_MAPPING_ITERATOR_HPP
