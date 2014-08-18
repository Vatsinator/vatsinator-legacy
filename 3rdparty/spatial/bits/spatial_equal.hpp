// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_equal.hpp
 *  Contains the definition of the equal iterators. These iterators
 *  walk through all items in the container that are equal to a key given in
 *  parameter of the iterator.
 */

#ifndef SPATIAL_EQUAL_HPP
#define SPATIAL_EQUAL_HPP

#include "spatial_bidirectional.hpp"
#include "../traits.hpp"
#include "spatial_rank.hpp"
#include "spatial_except.hpp"
#include "spatial_compress.hpp"

namespace spatial
{
  /**
   *  This type provides an iterator to iterate through all elements of a
   *  container that match a given key, passed as a parameter to the
   *  constructor. The given key is called the model.
   *
   *  \tparam Container The container upon which these iterator relate to.
   *  \headerfile equal_iterator.hpp
   */
  template <typename Container>
  class equal_iterator
    : public details::Bidirectional_iterator
      <typename container_traits<Container>::mode_type,
       typename container_traits<Container>::rank_type>
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Container>::mode_type,
     typename container_traits<Container>::rank_type> Base;

  public:
    //! The type used to store the model key to be looked up in the container.
    typedef typename container_traits<Container>::key_type key_type;

    //! The comparison functor used to compare keys.
    typedef typename container_traits<Container>::key_compare key_compare;

    //! Uninitialized iterator.
    equal_iterator() { }

    /**
     *  Build an equal iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the dimension
     *  for the node pointed to by \c iter is not known. The dimension of the
     *  node will be recomputed from the given iterator by iterating through all
     *  parents until the header node has been reached. This iteration is
     *  bounded by \Olog when the container is perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param model_ The key to look for.
     *  \param iter An iterator on the type Ct.
     */
    equal_iterator(Container& container, const key_type& model_,
                   typename container_traits<Container>::iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _model(container.key_comp(), model_) { }

    /**
     *  Build an equal iterator from the node and current dimension of a
     *  container's element.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster than the other,
     *  since the dimension does not have to be calculated. Note however that
     *  the calculation of the dimension in the other iterator takes slightly
     *  longer than \Olog in general, and so it is not likely to affect the
     *  performance of your application in any major way.
     *
     *  \param container The container being iterated.
     *  \param model_ The key to look for.
     *  \param ptr An iterator on the type Ct.
     *  \param dim The node's dimension for the node pointed to by node.
     *  \param container The container being iterated.
     */
    equal_iterator
    (Container& container, const key_type& model_, dimension_type dim,
     typename container_traits<Container>::mode_type::node_ptr ptr)
      : Base(container.rank(), ptr, dim), _model(container.key_comp(), model_)
    { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<Container>& operator++()
    { return increment_equal(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    equal_iterator<Container> operator++(int)
    {
      equal_iterator<Container> x(*this);
      increment_equal(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<Container>& operator--()
    { return decrement_equal(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    equal_iterator<Container> operator--(int)
    {
      equal_iterator<Container> x(*this);
      decrement_equal(*this);
      return x;
    }

    //! Return the value of the model key used to find equal keys in the
    //! container.
    key_type model() const { return _model(); }

    //! Return the functor used to compare keys in this iterator.
    key_compare key_comp() const { return _model.base(); }

  private:
    //! The model key used to find equal keys in the container.
    details::Compress<key_compare, key_type> _model;
  };

  /**
   *  This type provides an iterator to iterate through all elements of a
   *  container that match a given key, passed as a parameter to the
   *  constructor. The given key is called the model.
   *
   *  The values returned by this iterator will not be mutable.
   *
   *  \tparam Ct The container upon which these iterator relate to.
   */
  template <typename Container>
  class equal_iterator<const Container>
    : public details::Const_bidirectional_iterator
      <typename container_traits<Container>::mode_type,
       typename container_traits<Container>::rank_type>
  {
  private:
    typedef details::Const_bidirectional_iterator
    <typename container_traits<Container>::mode_type,
     typename container_traits<Container>::rank_type> Base;

  public:
    //! The type used to store the model key to be looked up in the container.
    typedef typename container_traits<Container>::key_type key_type;

    //! The comparison functor used to compare keys.
    typedef typename container_traits<Container>::key_compare key_compare;

    //! Uninitialized iterator.
    equal_iterator() { }

    /**
     *  Build an equal iterator from a container's iterator type.
     *
     *  This constructor should be used in the general case where the dimension
     *  for the node pointed to by \c iter is not known. The dimension of the
     *  node will be recomputed from the given iterator by iterating through all
     *  parents until the header node has been reached. This iteration is
     *  bounded by \Olog when the container is perfectly balanced.
     *
     *  \param container The container being iterated.
     *  \param model_ The key to look for.
     *  \param iter An iterator from the container.
     */
    equal_iterator(const Container& container, const key_type& model_,
                   typename container_traits<Container>::const_iterator iter)
      : Base(container.rank(), iter.node, modulo(iter.node, container.rank())),
        _model(container.key_comp(), model_) { }

    /**
     *  Build an equal iterator from the node and current dimension of a
     *  container's element.
     *
     *  This constructor should be used only when the dimension of the node
     *  pointed to by iter is known. If in doubt, use the other
     *  constructor. This constructor perform slightly faster thanskuuuu THE OTHER,
     *  since the dimension does not have to be calculated.
     *
     *  \param container The container being iterated.
     *  \param model_ The key to look for.
     *  \param dim The dimension associated with \c ptr when checking the
     *  invariant in \c container.
     *  \param ptr A pointer to a node belonging to \c container.
     */
    equal_iterator
    (const Container& container, const key_type& model_, dimension_type dim,
     typename container_traits<Container>::mode_type::const_node_ptr ptr)
      : Base(container.rank(), ptr, dim), _model(container.key_comp(), model_)
    { }

    //! Convertion of an iterator into a const_iterator is permitted.
    equal_iterator(const equal_iterator<Container>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _model(iter.key_comp(), iter.model()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<const Container>& operator++()
    { return increment_equal(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    equal_iterator<const Container> operator++(int)
    {
      equal_iterator<const Container> x(*this);
      increment_equal(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    equal_iterator<const Container>& operator--()
    { return decrement_equal(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    equal_iterator<const Container> operator--(int)
    {
      equal_iterator<const Container> x(*this);
      decrement_equal(*this);
      return x;
    }

    //! Return the value of the model key used to find equal keys in the
    //! container.
    key_type model() const { return _model(); }

    //! Return the functor used to compare keys in this iterator.
    key_compare key_comp() const { return _model.base(); }

  private:
    //! The model key used to find equal keys in the container.
    details::Compress<key_compare, key_type> _model;
  };

  namespace details
  {
    /**
     *  Returns the next matching iterator, whose value is equal to the model.
     *
     *  \param iter A valid region iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to iterate.
     *  \return  An iterator pointing the next matching value.
     */
    template <typename Container>
    equal_iterator<Container>&
    increment_equal(equal_iterator<Container>& iter);

    /**
     *  Returns the previous matching iterator, whose value is equal to the
     *  model.
     *
     *  \param iter A valid region iterator or a past-the-end iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to iterate.
     *  \return  An iterator pointing the previous value.
     *
     *  If \c iter is a past-the-end iterator (pointing to a header node), the
     *  function will return the last iterator whose value is equal to the model.
     */
    template <typename Container>
    equal_iterator<Container>&
    decrement_equal(equal_iterator<Container>& iter);

    /**
     *  In the children of the node pointed to by \c iter, find the first
     *  matching iterator whose value is equal to the model. If no match can be
     *  found, returns past-the-end.
     *
     *  \param iter A valid region iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to look up.
     *  \return  An iterator pointing the first value, or past-the-end.
     */
    template <typename Container>
    equal_iterator<Container>&
    minimum_equal(equal_iterator<Container>& iter);

    /**
     *  In the children of the node pointed to by \c iter, find the last
     *  matching iterator in the region delimited by \c Predicate, using
     *  in-order transversal. If no match can be found, returns past-the-end.
     *
     *  \param iter A valid region iterator.
     *  \tparam Predicate  The type of predicate for the orthogonal query.
     *  \tparam Ct The type of container to look up.
     *  \return  An iterator pointing the last value, or past-the-end.
     */
    template <typename Container>
    equal_iterator<Container>&
    maximum_equal(equal_iterator<Container>& iter);

  } // namespace details

  template <typename Container>
  inline equal_iterator<Container>
  equal_end(Container& container,
            const typename equal_iterator<Container>::key_type& model)
  {
    return equal_iterator<Container>
      (container, model, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_end(const Container& container,
            const typename equal_iterator<Container>::key_type& model)
  {
    return equal_iterator<const Container>
      (container, model, container.dimension() - 1,
       container.end().node); // At header, dim = rank - 1
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_cend(const Container& container,
             const typename equal_iterator<Container>::key_type& model)
  { return equal_end(container, model); }

  /**
   *  Find the first element in \c container that compares equally to \c model,
   *  using \c container's \c key_compare comparator.
   *
   *  \tparam Container The container type being iterated.
   *  \param container The container being iterated.
   *  \param model A model to find matches among other keys stored in the
   *  container.
   */
  ///@{
  template <typename Container>
  inline equal_iterator<Container>
  equal_begin(Container& container,
              const typename equal_iterator<Container>::key_type& model)
  {
    if (container.empty()) return equal_end(container, model);
    equal_iterator<Container>
      it(container, model, 0, container.end().node->parent); // At root, dim = 0
    return details::minimum_equal(it);
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_begin(const Container& container,
              const typename equal_iterator<Container>::key_type& model)
  {
    if (container.empty()) return equal_end(container, model);
    equal_iterator<const Container>
      it(container, model, 0, container.end().node->parent); // At root, dim = 0
    return details::minimum_equal(it);
  }

  template <typename Container>
  inline equal_iterator<const Container>
  equal_cbegin(const Container& container,
               const typename equal_iterator<Container>::key_type& model)
  { return equal_begin(container, model); }
  ///@}

  namespace details
  {
    /**
     *  Return a boolean indicating whether all of \c x's coordinates are
     *  equal to \c y's coordinates.
     *
     *  The key \c x and \c y are tested across all dimesions using the
     *  comparator \c cmp provided by a container.
     *  \tparam Rank Either \static_rank or \dynamic_rank.
     *  \tparam Key A key type defined in the container as the \c Compare.
     *  \tparam Compare A \trivial_compare type defined in the same
     *  container as \c Key.
     *  \param rank The magnitude of the rank.
     *  \param cmp The comparator used to find equality between the \c x and \c
     *  y coordinates.
     *  \param x The key \c x.
     *  \param y The key \c y.
     */
    template<typename Rank, typename Compare, typename Key>
    inline bool
    is_equal(const Rank& rank, const Compare& cmp,
             const Key& x, const Key& y)
    {
      for (dimension_type i = 0; i < rank(); ++i)
        { if (cmp(i, x, y) || cmp(i, y, x)) { return false; } }
      return true;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    increment_equal(equal_iterator<Container>& iter, relaxed_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      do
        {
          if (iter.node->right != 0
              && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && !cmp(iter.node_dim, const_key(iter.node), iter.model()))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && !is_equal(rank, cmp, iter.model(), const_key(iter.node)));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    increment_equal(equal_iterator<Container>& iter, strict_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      do
        {
          if (iter.node->right != 0
              && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0 // optimization below
                     && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && !is_equal(rank, cmp, iter.model(), const_key(iter.node)));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    increment_equal(equal_iterator<Container>& iter)
    {
      return increment_equal
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    template <typename Container>
    inline equal_iterator<Container>&
    decrement_equal(equal_iterator<Container>& iter, relaxed_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_equal(iter);
        }
      do
        {
          if (iter.node->left != 0
              && !cmp(iter.node_dim, const_key(iter.node), iter.model()))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && !is_equal(rank, cmp, iter.model(), const_key(iter.node)));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    decrement_equal(equal_iterator<Container>& iter, strict_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      if (header(iter.node))
        {
          iter.node = iter.node->parent;
          iter.node_dim = 0; // root is always compared on dimension 0
          return maximum_equal(iter);
        }
      do
        {
          if (iter.node->left != 0 // optimization below
              && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (!header(p) && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (!header(iter.node)
             && !is_equal(rank, cmp, iter.model(), const_key(iter.node)));
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    decrement_equal(equal_iterator<Container>& iter)
    {
      return decrement_equal
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    template <typename Container>
    inline equal_iterator<Container>&
    minimum_equal(equal_iterator<Container>& iter, relaxed_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      typename equal_iterator<Container>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while(iter.node->right != 0
            && cmp(iter.node_dim, const_key(iter.node), iter.model()))
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while(iter.node->left != 0
            && !cmp(iter.node_dim, const_key(iter.node), iter.model()))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (is_equal(rank, cmp, iter.model(), const_key(iter.node))) { break; }
          if (iter.node->right != 0
              && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0
                     && !cmp(iter.node_dim, const_key(iter.node), iter.model()))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    minimum_equal(equal_iterator<Container>& iter, strict_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      typename equal_iterator<Container>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while(iter.node->right != 0
            && cmp(iter.node_dim, const_key(iter.node), iter.model()))
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while (iter.node->left != 0 // optimization below
             && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (is_equal(rank, cmp, iter.model(), const_key(iter.node))) { break; }
          if (iter.node->right != 0
              && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
            {
              iter.node = iter.node->right;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->left != 0 // optimization below
                     && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
                {
                  iter.node = iter.node->left;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->right)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    minimum_equal(equal_iterator<Container>& iter)
    {
      return minimum_equal
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

    template <typename Container>
    inline equal_iterator<Container>&
    maximum_equal(equal_iterator<Container>& iter, relaxed_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      typename equal_iterator<Container>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while (iter.node->left != 0
             && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while (iter.node->right != 0
             && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (is_equal(rank, cmp, iter.model(), const_key(iter.node))) { break; }
          if (iter.node->left != 0
              && !cmp(iter.node_dim, const_key(iter.node), iter.model()))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    maximum_equal(equal_iterator<Container>& iter, strict_invariant_tag)
    {
      const typename container_traits<Container>::rank_type rank(iter.rank());
      const typename equal_iterator<Container>::key_compare cmp(iter.key_comp());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(iter.node));
      typename equal_iterator<Container>::node_ptr end
        = iter.node->parent;
      // Quick positioning according to in-order transversal.
      while (iter.node->left != 0
             && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
        {
          iter.node = iter.node->left;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      while (iter.node->right != 0
             && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
        {
          iter.node = iter.node->right;
          iter.node_dim = incr_dim(rank, iter.node_dim);
        }
      // Start algorithm.
      do
        {
          if (is_equal(rank, cmp, iter.model(), const_key(iter.node))) { break; }
          if (iter.node->left != 0 // optimization below
              && cmp(iter.node_dim, iter.model(), const_key(iter.node)))
            {
              iter.node = iter.node->left;
              iter.node_dim = incr_dim(rank, iter.node_dim);
              while (iter.node->right != 0
                     && !cmp(iter.node_dim, iter.model(), const_key(iter.node)))
                {
                  iter.node = iter.node->right;
                  iter.node_dim = incr_dim(rank, iter.node_dim);
                }
            }
          else
            {
              typename equal_iterator<Container>::node_ptr p
                = iter.node->parent;
              while (p != end && iter.node == p->left)
                {
                  iter.node = p;
                  iter.node_dim = decr_dim(rank, iter.node_dim);
                  p = iter.node->parent;
                }
              iter.node = p;
              iter.node_dim = decr_dim(rank, iter.node_dim);
            }
        }
      while (iter.node != end);
      SPATIAL_ASSERT_CHECK(iter.node_dim < rank());
      SPATIAL_ASSERT_CHECK(iter.node != 0);
      return iter;
    }

    template <typename Container>
    inline equal_iterator<Container>&
    maximum_equal(equal_iterator<Container>& iter)
    {
      return maximum_equal
        (iter, typename container_traits<Container>::mode_type
         ::invariant_category());
    }

  } // namespace details
} // namespace spatial

#endif // SPATIAL_EQUAL_HPP
