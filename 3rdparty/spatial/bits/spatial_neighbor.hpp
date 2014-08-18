// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_neighbor.hpp
 *  Provides neighbor iterator and all the functions around it.
 */

#ifndef SPATIAL_NEIGHBOR_HPP
#define SPATIAL_NEIGHBOR_HPP

#include <utility> // provides ::std::pair<>
#include "../metric.hpp"
#include "../traits.hpp"
#include "spatial_bidirectional.hpp"
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
     *  \tparam Metric The type of \metric applied to the iterator.
     *  \tparam DistanceType The type used to store the distance value.
     */
    template<typename Ct, typename Metric>
    struct Neighbor_data : container_traits<Ct>::key_compare
    {
      //! Build an unintialized neighbor data object.
      Neighbor_data() { }

      /**
       *  Builds required neighbor data from the given container, metric and
       *  dimension.
       *
       *  \param container The container being iterated.
       *  \param metric The metric to apply during iteration
       *  \param key The key representing the iteration target.
       */
      Neighbor_data
      (const typename container_traits<Ct>::key_compare& container,
       const Metric& metric,
       const typename container_traits<Ct>::key_type& key)
        : container_traits<Ct>::key_compare(container), _target(metric, key) { }

      /**
       *  The target of the iteration; element of the container are iterate
       *  from the closest to the element furthest away from the target.
       */
      Compress<Metric, typename container_traits<Ct>::key_type> _target;

      /**
       *  The last valid computed value of the distance. The value stored is
       *  only valid if the iterator is not past-the-end.
       */
      typename Metric::distance_type _distance;
    };
  } // namespace details

  /**
   *  A spatial iterator for a container \c Ct that goes through the nearest
   *  to the furthest element from a target key, with distances applied
   *  according to a user-defined geometric space that is a model of \metric.
   *
   *  \tparam Ct The container type bound to the iterator.
   *  \tparam DistanceType The type used to represent distances.
   *  \tparam Metric An type that is a model of \metric.
   *
   *  The Metric type is a complex type that must be a model of \metric:
   *
   *  \code
   *  struct Metric
   *  {
   *    typedef DistanceType distance_type;
   *
   *    DistanceType
   *    distance_to_key(dimension_type rank,
   *                    const Key& origin, const Key& key) const;
   *
   *    DistanceType
   *    distance_to_plane(dimension_type rank, dimension_type dim,
   *                      const Key& origin, const Key& key) const;
   *  };
   *  \endcode
   *
   *  The details of the \c Metric type are explained in \metric. The library
   *  provides ready-made models of \c Metric such as \euclidian and \manhattan
   *  that are designed to work only with C++'s built-in arithmetic types. If
   *  more metrics needs to be defined, see the explanation in the \metric
   *  concept.
   */
  template <typename Ct, typename Metric =
            euclidian<typename details::mutate<Ct>::type,
                      double,
                      typename details::with_builtin_difference<Ct>::type> >
  class neighbor_iterator
    : public details::Bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Key comparator type transferred from the container
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! The metric type used by the iterator
    typedef Metric metric_type;

    //! The distance type that is read from metric_type
    typedef typename Metric::distance_type distance_type;

    //! The key type that is used as a target for the nearest neighbor search
    typedef typename container_traits<Ct>::key_type key_type;

    //! Uninitialized iterator.
    neighbor_iterator() { }

    /**
     *  The standard way to build this iterator: specify a metric to apply, an
     *  iterator on a container, and that container.
     *
     *  \param container_ The container to iterate.
     *  \param metric_ The \metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param iter_ An iterator on container.
     */
    neighbor_iterator
    (Ct& container_, const Metric& metric_,
     const typename container_traits<Ct>::key_type& target_,
     const typename container_traits<Ct>::iterator& iter_)
      : Base(container_.rank(), iter_.node,
             modulo(iter_.node, container_.rank())),
        _data(container_.key_comp(), metric_, target_) { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this constructor saves some CPU
     *  cycle, by comparison to the other constructor.
     *
     *  \param container_ The container to iterate.
     *  \param metric_ The metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param node_dim_ The dimension of the node pointed to by iterator.
     *  \param node_ Use the value of node as the start point for the
     *  iteration.
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
     */
    neighbor_iterator
    (Ct& container_, const Metric& metric_,
     const typename container_traits<Ct>::key_type& target_,
     dimension_type node_dim_,
     typename container_traits<Ct>::mode_type::node_ptr node_)
      : Base(container_.rank(), node_, node_dim_),
        _data(container_.key_comp(), metric_, target_) { }

    /**
     *  Build the iterator with a given rank and key compare functor, if the
     *  container is not available.
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
     *  \param rank_ The rank of the container being iterated.
     *  \param key_comp_ The key compare functor associated with the iterator.
     *  \param metric_ The metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param node_dim_ The dimension of the node pointed to by iterator.
     *  \param node_ Use the value of node as the start point for the
     *  iteration.
     */
    neighbor_iterator
    (const typename container_traits<Ct>::rank_type& rank_,
     const typename container_traits<Ct>::key_compare& key_comp_,
     const Metric& metric_,
     const typename container_traits<Ct>::key_type& target_,
     dimension_type node_dim_,
     typename container_traits<Ct>::mode_type::node_ptr node_)
      : Base(rank_, node_, node_dim_), _data(key_comp_, metric_, target_) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator++()
    { return increment_neighbor(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator++(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      increment_neighbor(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator--()
    { return decrement_neighbor(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator--(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      decrement_neighbor(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return static_cast<const key_compare&>(_data); }

    //! Return the metric used by the iterator
    metric_type
    metric() const { return _data._target.base(); }

    //! Read-only accessor to the last valid distance of the iterator
    const distance_type&
    distance() const { return _data._distance; }

    //! Read/write accessor to the last valid distance of the iterator
    distance_type&
    distance() { return _data._distance; }

    //! Read-only accessor to the target of the iterator
    const key_type&
    target_key() const { return _data._target(); }

    //! Read/write accessor to the target of the iterator
    key_type&
    target_key() { return _data._target(); }

  private:
    //! The related data for the iterator.
    details::Neighbor_data<Ct, Metric> _data;
  };

  /**
   *  A spatial iterator for a container \c Ct that goes through the nearest
   *  to the furthest element from a target key, with distances applied
   *  according to a user-defined geometric space of type \c Metric.
   *
   *  The Metric type is a complex type that must be a model of
   *  \metric:
   *
   *  \code
   *  struct Metric
   *  {
   *    typedef my_distance_type distance_type;
   *
   *    distance_type
   *    distance_to_key(dimension_type rank,
   *                    const Key& origin, const Key& key) const;
   *
   *    distance_type
   *    distance_to_plane(dimension_type rank, dimension_type dim,
   *                      const Key& origin, const Key& key) const;
   *  };
   *  \endcode
   *
   *  The details of the \c Metric type are explained in \metric.  Metrics are
   *  generally not defined by the user of the library, given their
   *  complexity. Rather, the user of the library uses ready-made models of
   *  \metric such as \euclidian and \manhattan. If more metrics needs to be
   *  defined, see the explanation in for the \metric concept.
   *
   *  This iterator only returns constant objects.
   *
   *  \tparam Ct The container type bound to the iterator.
   *  \tparam DistanceType The type used to represent distances.
   *  \tparam Metric An type that follow the \metric concept.
   */
  template<typename Ct, typename Metric>
  class neighbor_iterator<const Ct, Metric>
    : public details::Const_bidirectional_iterator
  <typename container_traits<Ct>::mode_type,
   typename container_traits<Ct>::rank_type>
  {
  private:
    typedef typename details::Const_bidirectional_iterator
    <typename container_traits<Ct>::mode_type,
     typename container_traits<Ct>::rank_type> Base;

  public:
    //! Key comparator type transferred from the container
    typedef typename container_traits<Ct>::key_compare key_compare;

    //! The metric type used by the iterator
    typedef Metric metric_type;

    //! The distance type that is read from metric_type
    typedef typename Metric::distance_type distance_type;

    //! The key type that is used as a target for the nearest neighbor search
    typedef typename container_traits<Ct>::key_type key_type;

    //! Uninitialized iterator.
    neighbor_iterator() { }

    /**
     *  The standard way to build this iterator: specify a metric to apply,
     *  an iterator on a container, and that container.
     *
     *  \param container_ The container to iterate.
     *  \param metric_ The metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param iter_ An iterator on \c container.
     */
    neighbor_iterator
    (const Ct& container_, const Metric& metric_,
     const typename container_traits<Ct>::key_type& target_,
     typename container_traits<Ct>::const_iterator iter_)
      : Base(container_.rank(), iter_.node,
             modulo(iter_.node, container_.rank())),
        _data(container_.key_comp(), metric_, target_) { }

    /**
     *  When the information of the dimension for the current node being
     *  pointed to by the iterator is known, this constructor saves some CPU
     *  cycle, by comparison to the other constructor.
     *
     *  \param container_ The container to iterate.
     *  \param metric_ The metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param node_dim_ The dimension of the node pointed to by iterator.
     *  \param node_ Use the value of node as the start point for the
     *  iteration.
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
     */
    neighbor_iterator
    (const Ct& container_, const Metric& metric_,
     const typename container_traits<Ct>::key_type& target_,
     dimension_type node_dim_,
     typename container_traits<Ct>::mode_type::const_node_ptr node_)
      : Base(container_.rank(), node_, node_dim_),
        _data(container_.key_comp(), metric_, target_) { }

    /**
     *  Build the iterator with a given rank and key compare functor, if the
     *  container is not available. It requires the node information to be
     *  known but is a fast constructor.
     *
     *  \param rank_ The rank of the container being iterated.
     *  \param key_comp_ The key compare functor associated with the iterator.
     *  \param metric_ The metric applied during the iteration.
     *  \param target_ The target of the neighbor iteration.
     *  \param node_dim_ The dimension of the node pointed to by iterator.
     *  \param node_ Use the value of node as the start point for the
     *  iteration.
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
     */
    neighbor_iterator
    (const typename container_traits<Ct>::rank_type& rank_,
     const typename container_traits<Ct>::key_compare& key_comp_,
     const Metric& metric_,
     const typename container_traits<Ct>::key_type& target_,
     dimension_type node_dim_,
     typename container_traits<Ct>::mode_type::const_node_ptr node_)
      : Base(rank_, node_, node_dim_), _data(key_comp_, metric_, target_) { }

    //! Covertion of mutable iterator into a constant iterator is permitted.
    neighbor_iterator(const neighbor_iterator<Ct, Metric>& iter)
      : Base(iter.rank(), iter.node, iter.node_dim),
        _data(iter.key_comp(), iter.metric(), iter.target_key()) { }

    //! Increments the iterator and returns the incremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator++()
    { return increment_neighbor(*this); }

    //! Increments the iterator but returns the value of the iterator before
    //! the increment. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator++(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      increment_neighbor(*this);
      return x;
    }

    //! Decrements the iterator and returns the decremented value. Prefer to
    //! use this form in \c for loops.
    neighbor_iterator<Ct, Metric>& operator--()
    { return decrement_neighbor(*this); }

    //! Decrements the iterator but returns the value of the iterator before
    //! the decrement. Prefer to use the other form in \c for loops.
    neighbor_iterator<Ct, Metric> operator--(int)
    {
      neighbor_iterator<Ct, Metric> x(*this);
      decrement_neighbor(*this);
      return x;
    }

    //! Return the key_comparator used by the iterator
    key_compare
    key_comp() const { return static_cast<const key_compare&>(_data); }

    //! Return the metric used by the iterator
    metric_type
    metric() const { return _data._target.base(); }

    //! Read-only accessor to the last valid distance of the iterator
    distance_type
    distance() const { return _data._distance; }

    //! Read/write accessor to the last valid distance of the iterator
    distance_type&
    distance() { return _data._distance; }

    //! Read-only accessor to the target of the iterator
    const key_type&
    target_key() const { return _data._target(); }

    //! Read/write accessor to the target of the iterator
    key_type&
    target_key() { return _data._target(); }

  private:
    //! The related data for the iterator.
    details::Neighbor_data<Ct, Metric> _data;
  };

  /**
   *  Read/write accessor for neighbor iterators that retrieve the valid
   *  calculated distance from the target. The distance read is only relevant if
   *  the iterator does not point past-the-end.
   */
  ///@{
  template <typename Ct, typename Metric>
  inline typename Metric::distance_type
  distance(const neighbor_iterator<Ct, Metric>& iter)
  { return iter.distance(); }

  template <typename Ct, typename Metric>
  inline void
  distance(neighbor_iterator<Ct, Metric>& iter,
           const typename Metric::distance_type& dist)
  { iter.distance() = dist; }
  ///@}

  /**
   *  A quick accessor for neighbor iterators that retrive the key that is the
   *  target for the nearest neighbor iteration.
   */
  template <typename Ct, typename Metric>
  inline const typename container_traits<Ct>::key_type&
  target_key(const neighbor_iterator<Ct, Metric>& iter)
  { return iter.target_key(); }

  /**
   *  This structure defines a pair of neighbor iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \tparam Metric The metric used by the iterator.
   *  \see neighbor_iterator
   */
  template <typename Ct, typename Metric
            = euclidian<typename details::mutate<Ct>::type,
                        double, typename details::with_builtin_difference<Ct>
                        ::type> >
  struct neighbor_iterator_pair
    : std::pair<neighbor_iterator<Ct, Metric>,
                neighbor_iterator<Ct, Metric> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<neighbor_iterator<Ct, Metric>,
                      neighbor_iterator<Ct, Metric> > Base;

    //! Empty constructor.
    neighbor_iterator_pair() { }

    //! Regular constructor that builds a neighbor_iterator_pair out of 2
    //! neighbor_iterators.
    neighbor_iterator_pair(const neighbor_iterator<Ct, Metric>& a,
                           const neighbor_iterator<Ct, Metric>& b)
      : Base(a, b) { }
  };

  /**
   *  This structure defines a pair of constant neighbor iterator.
   *
   *  \tparam Ct The container to which these iterator relate to.
   *  \tparam Metric The metric used by the iterator.
   *  \see neighbor_iterator
   */
  template <typename Ct, typename Metric>
  struct neighbor_iterator_pair<const Ct, Metric>
    : std::pair<neighbor_iterator<const Ct, Metric>,
                neighbor_iterator<const Ct, Metric> >
  {
    /**
     *  A pair of iterators that represents a range (that is: a range of
     *  elements to iterate, and not an orthogonal range).
     */
    typedef std::pair<neighbor_iterator<const Ct, Metric>,
                      neighbor_iterator<const Ct, Metric> > Base;

    //! Empty constructor.
    neighbor_iterator_pair() { }

    //! Regular constructor that builds a neighbor_iterator_pair out of 2
    //! neighbor_iterators.
    neighbor_iterator_pair(const neighbor_iterator<const Ct, Metric>& a,
                           const neighbor_iterator<const Ct, Metric>& b)
      : Base(a, b)
    { }

    //! Convert a mutable neighbor iterator pair into a const neighbor iterator
    //!pair.
    neighbor_iterator_pair(const neighbor_iterator_pair<Ct, Metric>& p)
      : Base(p.first, p.second) { }
  };

  namespace details
  {
    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    increment_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    decrement_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    minimum_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    maximum_neighbor(neighbor_iterator<Container, Metric>& iter);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    lower_bound_neighbor(neighbor_iterator<Container, Metric>& iter,
                         typename Metric::distance_type bound);

    template <typename Container, typename Metric>
    neighbor_iterator<Container, Metric>&
    upper_bound_neighbor(neighbor_iterator<Container, Metric>& iter,
                         typename Metric::distance_type bound);
  } // namespace details

  /**
   *  Build a past-the-end neighbor iterator with a user-defined \metric.
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  ///@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_end(Ct& container, const Metric& metric,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator<Ct, Metric>
      (container, metric, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_end(const Ct& container, const Metric& metric,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator<const Ct, Metric>
      (container, metric, target, container.dimension() - 1,
       container.end().node);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_cend(const Ct& container, const Metric& metric,
                const typename container_traits<Ct>::key_type& target)
  { return neighbor_end(container, metric, target); }
  ///@}

  /**
   *  Build a past-the-end neighbor iterator, assuming an euclidian metric with
   *  distances expressed in double. It requires that the container used was
   *  defined with a built-in key compare functor.
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  ///@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_end(Ct& container,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_end(const Ct& container,
               const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_cend(const Ct& container,
                const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_end
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target);
  }
  ///@}

  /**
   *  Build a \ref neighbor_iterator pointing to the nearest neighbor of \c
   *  target using a user-defined \metric.
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  ///@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_begin(Ct& container, const Metric& metric,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    neighbor_iterator<Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::minimum_neighbor(it);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_begin(const Ct& container, const Metric& metric,
                 const typename container_traits<Ct>::key_type& target)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    neighbor_iterator<const Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::minimum_neighbor(it);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_cbegin(const Ct& container, const Metric& metric,
                  const typename container_traits<Ct>::key_type& target)
  { return neighbor_begin(container, metric, target); }
  ///@}

  /**
   *  Build a \ref neighbor_iterator pointing to the nearest neighbor of \c
   *  target assuming an euclidian metric with distances expressed in double. It
   *  requires that the container used was defined with a built-in key compare
   *  functor.
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  ///@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_begin(Ct& container,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_begin(const Ct& container,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_cbegin(const Ct& container,
                  const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_begin
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target);
  }
  ///@}

  /**
   *  Build a \ref neighbor_iterator pointing to the neighbor closest to
   *  target but for which distance to target is greater or equal to the value
   *  given in \c bound. Uses a user-defined \metric.
   *
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   *  \param bound The minimum distance from the target.
   */
  ///@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_lower_bound(Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       typename Metric::distance_type bound)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::lower_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_lower_bound(const Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       typename Metric::distance_type bound)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<const Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::lower_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_clower_bound(const Ct& container, const Metric& metric,
                        const typename container_traits<Ct>::key_type& target,
                        typename Metric::distance_type bound)
  { return neighbor_lower_bound(container, metric, target, bound); }
  ///@}

  /**
   *  Build a \ref neighbor_iterator pointing to the neighbor closest to
   *  target but for which distance to target is greater or equal to the value
   *  given in \c bound. It assumes an euclidian metric with distances expressed
   *  in double. It also requires that the container used was defined with one
   *  of the built-in key compare functor.
   *
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   *  \param bound The minimum distance from the target.
   */
  ///@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_lower_bound(Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target, bound);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_lower_bound(const Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target, bound);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_clower_bound(const Ct& container,
                        const typename container_traits<Ct>::key_type& target,
                        double bound)
  {
    return neighbor_lower_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target, bound);
  }
  ///@}

  /**
   *  Build a \ref neighbor_iterator pointing to the neighbor closest to
   *  target but for which distance to target is strictly greater than the value
   *  given in \c bound. Uses a user-defined \metric.
   *
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   *  \param bound The minimum distance from the target.
   */
  ///@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator<Ct, Metric>
  neighbor_upper_bound(Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       typename Metric::distance_type bound)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::upper_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_upper_bound(const Ct& container, const Metric& metric,
                       const typename container_traits<Ct>::key_type& target,
                       typename Metric::distance_type bound)
  {
    if (container.empty()) return neighbor_end(container, metric, target);
    except::check_positive_distance(bound);
    neighbor_iterator<const Ct, Metric>
      it(container, metric, target, 0, container.end().node->parent);
    return details::upper_bound_neighbor(it, bound);
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator<const Ct, Metric>
  neighbor_cupper_bound(const Ct& container, const Metric& metric,
                        const typename container_traits<Ct>::key_type& target,
                        typename Metric::distance_type bound)
  { return neighbor_upper_bound(container, metric, target); }
  ///@}

  /**
   *  Build a \ref neighbor_iterator pointing to the neighbor closest to
   *  target but for which distance to target is greater than the value given in
   *  \c bound. It assumes an euclidian metric with distances expressed in
   *  double. It also requires that the container used was defined with one of
   *  the built-in key compare functor.
   *
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   *  \param bound The minimum distance to the target.
   */
  ///@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<Ct> >::type
  neighbor_upper_bound(Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target, bound);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_upper_bound(const Ct& container,
                       const typename container_traits<Ct>::key_type& target,
                       double bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target, bound);
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator<const Ct> >::type
  neighbor_cupper_bound(const Ct& container,
                        const typename container_traits<Ct>::key_type& target,
                        double bound)
  {
    return neighbor_upper_bound
      (container,
       euclidian<Ct, double,
                 typename details::with_builtin_difference<Ct>::type>
         (details::with_builtin_difference<Ct>()(container)),
       target, bound);
  }
  ///@}

  /**
   *  Returns a \ref neighbor_iterator_pair representing the range of values
   *  from the closest to the furthest in the container iterated. Uses a
   *  user-defined \metric.
   *
   *  \param container The container in which a neighbor must be found.
   *  \param metric The metric to use in search of the neighbor.
   *  \param target The target key used in the neighbor search.
   */
  ///@{
  template <typename Ct, typename Metric>
  inline neighbor_iterator_pair<Ct, Metric>
  neighbor_range(Ct& container, const Metric& metric,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator_pair<Ct, Metric>
      (neighbor_begin(container, metric, target),
       neighbor_end(container, metric, target));
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator_pair<const Ct, Metric>
  neighbor_range(const Ct& container, const Metric& metric,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator_pair<const Ct, Metric>
      (neighbor_begin(container, metric, target),
       neighbor_end(container, metric, target));
  }

  template <typename Ct, typename Metric>
  inline neighbor_iterator_pair<const Ct, Metric>
  neighbor_crange(const Ct& container, const Metric& metric,
                  const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator_pair<const Ct, Metric>
      (neighbor_begin(container, metric, target),
       neighbor_end(container, metric, target));
  }
  ///@}

  /**
   *  Returns a \ref neighbor_iterator_pair representing the range of values
   *  from the closest to the furthest in the container iterated. It assumes an
   *  euclidian metric with distances expressed in double. It also requires that
   *  the container used was defined with one of the built-in key compare
   *  functor.
   *
   *  \param container The container in which a neighbor must be found.
   *  \param target The target key used in the neighbor search.
   */
  ///@{
  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator_pair<Ct> >::type
  neighbor_range(Ct& container,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator_pair<Ct>
      (neighbor_begin(container, target), neighbor_end(container, target));
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator_pair<const Ct> >::type
  neighbor_range(const Ct& container,
                 const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator_pair<const Ct>
      (neighbor_begin(container, target), neighbor_end(container, target));
  }

  template <typename Ct>
  inline typename enable_if<details::is_compare_builtin<Ct>,
                            neighbor_iterator_pair<const Ct> >::type
  neighbor_crange(const Ct& container,
                  const typename container_traits<Ct>::key_type& target)
  {
    return neighbor_iterator_pair<const Ct>
      (neighbor_begin(container, target), neighbor_end(container, target));
  }
  ///@}

  namespace details
  {
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    increment_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef typename neighbor_iterator<Container, Metric>::node_ptr node_ptr;
      typename container_traits<Container>::rank_type rank(it.rank());
      typename container_traits<Container>::key_compare cmp(it.key_comp());
      Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(it.node != 0);
      SPATIAL_ASSERT_CHECK(!header(it.node));
      // In this algorithm, we seek to find the next nearest point to
      // origin. Thus assuming that this point exists, its distance to origin is
      // equal or greater than that of the previous nearest point to origin.
      // Since K-d tree are good at preserving locality, it is best to search
      // the next nearest point from the current nearest point, since these 2
      // points could be close to one another in the tree. In order to find the
      // next nearest, we perform in-order transveral, at the left and right
      // simultaneously.
      //
      // right iteration variables start with 'r'
      // left iteration variables start with 'l'
      node_ptr rn = it.node;
      node_ptr ln = it.node;
      dimension_type rn_dim = it.node_dim;
      dimension_type ln_dim = it.node_dim;
      bool rn_break = false;
      bool ln_break = false;
      // existing values used for comparison
      node_ptr curr = it.node;
      node_ptr near_node = 0;
      dimension_type near_dim = 0;
      typename Metric::distance_type near_distance = 0;
      typename Metric::distance_type tmp;
      do
        {
          // One step to the right side...
          if (rn_break) { goto left_side; }
          if (rn->right != 0
              && (!cmp(rn_dim, target_key(it), const_key(rn))
                  || near_node == 0
                  || near_distance >= met.distance_to_plane
                  (rank(), rn_dim, target_key(it), const_key(rn))))
            {
              rn = rn->right;
              rn_dim = incr_dim(rank, rn_dim);
              while(rn->left != 0
                    && (!cmp(rn_dim, const_key(rn), target_key(it))
                        || near_node == 0
                        || near_distance >= met.distance_to_plane
                        (rank(), rn_dim, target_key(it), const_key(rn))))
                {
                  rn = rn->left;
                  rn_dim = incr_dim(rank, rn_dim);
                }
            }
          else
            {
              node_ptr p = rn->parent;
              while (!header(p) && p->right == rn)
                {
                  rn = p;
                  rn_dim = decr_dim(rank, rn_dim);
                  p = rn->parent;
                }
              rn = p;
              rn_dim = decr_dim(rank, rn_dim);
            }
          if (header(rn))
            { if (ln_break) break; rn_break = true; goto left_side; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(rn));
          if (tmp < distance(it) || (tmp == distance(it) && rn < curr))
            { goto left_side; }
          if (near_node == 0 || tmp < near_distance)
            {
              near_node = rn;
              near_dim = rn_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance && rn < near_node)
            {
              near_node = rn;
              near_dim = rn_dim;
            }
        left_side:
          // One step to the left side...
          if (ln_break) { continue; }
          if (ln->left != 0
              && (!cmp(ln_dim, const_key(ln), target_key(it))
                  || near_node == 0
                  || near_distance >= met.distance_to_plane
                  (rank(), ln_dim, target_key(it), const_key(ln))))
            {
              ln = ln->left;
              ln_dim = incr_dim(rank, ln_dim);
              while(ln->right != 0
                    && (!cmp(ln_dim, target_key(it), const_key(ln))
                        || near_node == 0
                        || near_distance >= met.distance_to_plane
                        (rank(), ln_dim, target_key(it), const_key(ln))))
                {
                  ln = ln->right;
                  ln_dim = incr_dim(rank, ln_dim);
                }
            }
          else
            {
              node_ptr p = ln->parent;
              while (!header(p) && p->left == ln)
                {
                  ln = p;
                  ln_dim = decr_dim(rank, ln_dim);
                  p = ln->parent;
                }
              ln = p;
              ln_dim = decr_dim(rank, ln_dim);
            }
          if (header(ln))
            { if (rn_break) break; ln_break = true; continue; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(ln));
          if (tmp < distance(it) || (tmp == distance(it) && ln < curr))
            { continue; }
          if (near_node == 0 || tmp < near_distance)
            {
              near_node = ln;
              near_dim = ln_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance && ln < near_node)
            {
              near_node = ln;
              near_dim = ln_dim;
            }
        }
      while(true);
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0 || distance(it) < near_distance
                           || (distance(it) == near_distance
                               && curr < near_node));
      SPATIAL_ASSERT_CHECK(rn_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(ln_dim == (rank() - 1));
      if (near_node != 0)
        {
          it.node = near_node;
          it.node_dim = near_dim;
          it.distance() = near_distance;
        }
      else
        {
          it.node = rn; // rn points past-the-end
          it.node_dim = rn_dim;
        }
      return it;
    }

    // The next largest key on the neighbor dimension is likely to be found in
    // the children of the current best, so, descend into the children of node
    // first.
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    decrement_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef typename neighbor_iterator<Container, Metric>::node_ptr node_ptr;
      typename container_traits<Container>::rank_type rank(it.rank());
      typename container_traits<Container>::key_compare cmp(it.key_comp());
      Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(it.node != 0);
      // Must come back from an end position for reverse iteration...
      if (header(it.node))
        {
          it.node = it.node->parent;
          it.node_dim = 0; // root is always compared on dimension 0
          return maximum_neighbor(it);
        }
      // As in 'increment', we follow the same convention: we traverse the tree
      // in-order to the left and the right simultaneously.
      //
      // right iteration variables start with 'r'
      // left iteration variables start with 'l'
      node_ptr rn = it.node;
      node_ptr ln = it.node;
      dimension_type rn_dim = it.node_dim;
      dimension_type ln_dim = it.node_dim;
      bool rn_break = false;
      bool ln_break = false;
      // existing values used for comparison
      node_ptr curr = it.node;
      node_ptr near_node = 0;
      dimension_type near_dim = 0;
      typename Metric::distance_type near_distance = 0;
      typename Metric::distance_type tmp;
      do
        {
          // In-order traversal that starts with all nodes before 'curr'
          if (ln_break) { goto right_side; }
          if (ln->left != 0
              && (!cmp(ln_dim, const_key(ln), target_key(it))
                  || distance(it) >= met.distance_to_plane
                  (rank(), ln_dim, target_key(it), const_key(ln))))
            {
              ln = ln->left;
              ln_dim = incr_dim(rank, ln_dim);
              while(ln->right != 0
                    && (!cmp(ln_dim, target_key(it), const_key(ln))
                        || distance(it) >= met.distance_to_plane
                        (rank(), ln_dim, target_key(it), const_key(ln))))
                {
                  ln = ln->right;
                  ln_dim = incr_dim(rank, ln_dim);
                }
            }
          else
            {
              node_ptr p = ln->parent;
              while (!header(p) && p->left == ln)
                {
                  ln = p;
                  ln_dim = decr_dim(rank, ln_dim);
                  p = ln->parent;
                }
              ln = p;
              ln_dim = decr_dim(rank, ln_dim);
            }
          if (header(ln))
            { if (rn_break) break; ln_break = true; goto right_side; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(ln));
          if (tmp > distance(it) || (tmp == distance(it) && ln > curr))
            { goto right_side; }
          if (near_node == 0 || tmp > near_distance)
            {
              near_node = ln;
              near_dim = ln_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance && ln > near_node)
            {
              near_node = ln;
              near_dim = ln_dim;
            }
        right_side:
          // Now, in-order traversal that starts with all nodes after 'curr'
          if (rn_break) { continue; }
          if (rn->right != 0
              && (!cmp(rn_dim, target_key(it), const_key(rn))
                  || distance(it) >= met.distance_to_plane
                  (rank(), rn_dim, target_key(it), const_key(rn))))
            {
              rn = rn->right;
              rn_dim = incr_dim(rank, rn_dim);
              while(rn->left != 0
                    && (!cmp(rn_dim, const_key(rn), target_key(it))
                        || distance(it) >= met.distance_to_plane
                        (rank(), rn_dim, target_key(it), const_key(rn))))
                {
                  rn = rn->left;
                  rn_dim = incr_dim(rank, rn_dim);
                }
            }
          else
            {
              node_ptr p = rn->parent;
              while (!header(p) && p->right == rn)
                {
                  rn = p;
                  rn_dim = decr_dim(rank, rn_dim);
                  p = rn->parent;
                }
              rn = p;
              rn_dim = decr_dim(rank, rn_dim);
            }
          if (header(rn))
            { if(ln_break) break; rn_break = true; continue; }
          tmp = met.distance_to_key(rank(), target_key(it), const_key(rn));
          if (tmp > distance(it) || (tmp == distance(it) && rn > curr))
            { continue; }
          if (near_node == 0 || tmp > near_distance)
            {
              near_node = rn;
              near_dim = rn_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance && rn > near_node)
            {
              near_node = rn;
              near_dim = rn_dim;
            }
        }
      while(true);
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(near_node != curr);
      SPATIAL_ASSERT_CHECK(near_node == 0
                           || distance(it) > near_distance
                           || (distance(it) == near_distance
                               && curr > near_node));
      SPATIAL_ASSERT_CHECK(rn_dim == (rank() - 1));
      SPATIAL_ASSERT_CHECK(ln_dim == (rank() - 1));
      if (near_node != 0)
        {
          it.node = near_node;
          it.node_dim = near_dim;
          it.distance() = near_distance;
        }
      else
        {
          it.node = rn; // rn points past-the-end here
          it.node_dim = rn_dim;
        }
      return it;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate
    // in left-first fashion.
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    minimum_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef typename neighbor_iterator<Container, Metric>::node_ptr node_ptr;
      typename container_traits<Container>::rank_type rank(it.rank());
      typename container_traits<Container>::key_compare cmp(it.key_comp());
      Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      dimension_type node_dim = it.node_dim;
      node_ptr near_node = node;
      node_ptr end = node->parent;
      dimension_type near_dim = node_dim;
      typename Metric::distance_type near_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      typename Metric::distance_type tmp;
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, const_key(node), target_key(it))
                || near_distance
                >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                         const_key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
          if (tmp < near_distance)
            {
              near_node = node;
              near_dim = node_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance
                   // to provide total ordering among the nodes, we use the node
                   // pointer as a fall back comparison mechanism
                   && node < near_node)
            {
              near_node = node;
              near_dim = node_dim;
            }
        }
      // In-order, right, left, then all the way up
      do
        {
          if (node->right != 0
              && (!cmp(node_dim, target_key(it), const_key(node))
                  || near_distance
                  >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                           const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = met.distance_to_key(rank(), target_key(it),
                                        const_key(node));
              if (tmp < near_distance)
                {
                  near_node = node;
                  near_dim = node_dim;
                  near_distance = tmp;
                }
              else if (tmp == near_distance && node < near_node)
                {
                  near_node = node;
                  near_dim = node_dim;
                }
              while(node->left != 0
                    && (!cmp(node_dim, const_key(node), target_key(it))
                        || near_distance
                        >= met.distance_to_plane(rank(), node_dim,
                                                 target_key(it),
                                                 const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = met.distance_to_key(rank(), target_key(it),
                                            const_key(node));
                  if (tmp < near_distance)
                    {
                      near_node = node;
                      near_dim = node_dim;
                      near_distance = tmp;
                    }
                  else if (tmp == near_distance && node < near_node)
                    {
                      near_node = node;
                      near_dim = node_dim;
                    }
                }
            }
          // No more nodes to visit, so go up!
          else
            {
              node_ptr p = node->parent;
              while (p != end && p->right == node)
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                }
              node = p;
              node_dim = decr_dim(rank, node_dim);
            }
        }
      while(node != end);
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(near_node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(near_node != 0);
      it.node = near_node; it.node_dim = near_dim;
      it.distance() = near_distance;
      return it;
    }

    // Find the minimum from node and stop when reaching the parent. Iterate
    // in left-first fashion.
    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    maximum_neighbor(neighbor_iterator<Container, Metric>& it)
    {
      typedef typename neighbor_iterator<Container, Metric>::node_ptr node_ptr;
      typename container_traits<Container>::rank_type rank(it.rank());
      Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      node_ptr end = node->parent;
      dimension_type node_dim = it.node_dim;
      typename Metric::distance_type tmp;
      // Finding the maximum is, for lack of a better algorithm, equivalent to a
      // O(n) search. An alternative has been explored: being able to find if a
      // node is in a cell that is smaller than the current 'far_node' node
      // found.  However, with the data at hand, computing the cell turned out
      // to be more expensive than doing a simple iteration over all nodes in
      // the tree.  Maybe, one day we'll find a better algorithm that also has
      // no impact on the memory footprint of the tree (although I doubt these 2
      // conditions will ever be met. Probably there will be a tradeoff.)
      //
      // Iterate from left most to right most, and stop at node's parent.
      while (node->left != 0)
        { node = node->left; node_dim = incr_dim(rank, node_dim); }
      node_ptr far_node = node;
      dimension_type far_dim = node_dim;
      typename Metric::distance_type far_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      do {
        if (node->right != 0)
          {
            node = node->right; node_dim = incr_dim(rank, node_dim);
            while (node->left != 0)
              { node = node->left; node_dim = incr_dim(rank, node_dim); }
          }
        else
          {
            node_ptr p = node->parent;
            while (p != end && node == p->right)
              {
                node = p; p = node->parent;
                node_dim = decr_dim(rank, node_dim);
              }
            node = p;
            node_dim = decr_dim(rank, node_dim);
          }
        if (node == end) { break; }
        tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
        if (tmp > far_distance
            || (tmp == far_distance && node > far_node))
          {
            far_node = node;
            far_dim = node_dim;
            far_distance = tmp;
          }
      }
      while(true);
      SPATIAL_ASSERT_CHECK(far_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(far_node));
      SPATIAL_ASSERT_CHECK(node != 0);
      SPATIAL_ASSERT_CHECK(far_node != 0);
      it.node = far_node; it.node_dim = far_dim;
      it.distance() = far_distance;
      return it;
    }

    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    lower_bound_neighbor(neighbor_iterator<Container, Metric>& it,
                         typename Metric::distance_type bound)
    {
      typedef typename neighbor_iterator<Container, Metric>::node_ptr node_ptr;
      typename container_traits<Container>::rank_type rank(it.rank());
      typename container_traits<Container>::key_compare cmp(it.key_comp());
      Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      node_ptr end = node->parent;
      node_ptr near_node = 0;
      dimension_type node_dim = it.node_dim;
      dimension_type near_dim = node_dim;
      typename Metric::distance_type tmp;
      typename Metric::distance_type near_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      if (near_distance >= bound) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, const_key(node), target_key(it))
                || near_node == 0 || near_distance
                >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                         const_key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
          if (tmp < bound) { continue; }
          if (near_node == 0 || tmp < near_distance)
            {
              near_node = node;
              near_dim = node_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance
                   // to provide total ordering among the nodes, we use the node
                   // pointer as a fall back comparison mechanism
                   && node < near_node)
            {
              near_node = node;
              near_dim = node_dim;
            }
        }
      // In-order, right, left, then all the way up
      do
        {
          if (node->right != 0
              && (!cmp(node_dim, target_key(it), const_key(node))
                  || near_node == 0 || near_distance
                  >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                           const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = met.distance_to_key(rank(), target_key(it),
                                        const_key(node));
              if (tmp >= bound)
                {
                  if (near_node == 0 || tmp < near_distance)
                    {
                      near_node = node;
                      near_dim = node_dim;
                      near_distance = tmp;
                    }
                  else if (tmp == near_distance && node < near_node)
                    {
                      near_node = node;
                      near_dim = node_dim;
                    }
                }
              while(node->left != 0
                    && (!cmp(node_dim, const_key(node), target_key(it))
                        || near_node == 0
                        || near_distance
                        >= met.distance_to_plane(rank(), node_dim,
                                                 target_key(it),
                                                 const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = met.distance_to_key(rank(), target_key(it),
                                            const_key(node));
                  if (tmp >= bound)
                    {
                      if (near_node == 0 || tmp < near_distance)
                        {
                          near_node = node;
                          near_dim = node_dim;
                          near_distance = tmp;
                        }
                      else if (tmp == near_distance && node < near_node)
                        {
                          near_node = node;
                          near_dim = node_dim;
                        }
                    }
                }
            }
          // No more nodes to visit, so go up!
          else
            {
              node_ptr p = node->parent;
              while (p != end && p->right == node)
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                }
              node = p;
              node_dim = decr_dim(rank, node_dim);
            }
        }
      while(node != end);
      if (near_node == 0)
        { near_node = node; near_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      it.node = near_node; it.node_dim = near_dim;
      it.distance() = near_distance;
      return it;
    }

    template <typename Container, typename Metric>
    inline neighbor_iterator<Container, Metric>&
    upper_bound_neighbor(neighbor_iterator<Container, Metric>& it,
                         typename Metric::distance_type bound)
    {
      typedef typename neighbor_iterator<Container, Metric>::node_ptr node_ptr;
      typename container_traits<Container>::rank_type rank(it.rank());
      typename container_traits<Container>::key_compare cmp(it.key_comp());
      Metric met(it.metric());
      SPATIAL_ASSERT_CHECK(it.node_dim < rank());
      SPATIAL_ASSERT_CHECK(!header(it.node));
      SPATIAL_ASSERT_CHECK(it.node != 0);
      node_ptr node = it.node;
      node_ptr end = node->parent;
      node_ptr near_node = 0;
      dimension_type node_dim = it.node_dim;
      dimension_type near_dim = node_dim;
      typename Metric::distance_type tmp;
      typename Metric::distance_type near_distance
        = met.distance_to_key(rank(), target_key(it), const_key(node));
      if (near_distance > bound) { near_node = node; }
      // Depth traversal starts with left first
      while(node->left != 0
            && (!cmp(node_dim, const_key(node), target_key(it))
                || near_node == 0
                || near_distance
                >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                         const_key(node))))
        {
          node = node->left;
          node_dim = incr_dim(rank, node_dim);
          tmp = met.distance_to_key(rank(), target_key(it), const_key(node));
          if (tmp <= bound) { continue; }
          if (near_node == 0 || tmp < near_distance)
            {
              near_node = node;
              near_dim = node_dim;
              near_distance = tmp;
            }
          else if (tmp == near_distance
                   // to provide total ordering among the nodes, we use the node
                   // pointer as a fall back comparison mechanism
                   && node < near_node)
            {
              near_node = node;
              near_dim = node_dim;
            }
        }
      // In-order, right, left, then all the way up
      do
        {
          if (node->right != 0
              && (!cmp(node_dim, target_key(it), const_key(node))
                  || near_node == 0 || near_distance
                  >= met.distance_to_plane(rank(), node_dim, target_key(it),
                                           const_key(node))))
            {
              node = node->right;
              node_dim = incr_dim(rank, node_dim);
              tmp = met.distance_to_key(rank(), target_key(it),
                                        const_key(node));
              if (tmp > bound)
                {
                  if (near_node == 0 || tmp < near_distance)
                    {
                      near_node = node;
                      near_dim = node_dim;
                      near_distance = tmp;
                    }
                  else if (tmp == near_distance && node < near_node)
                    {
                      near_node = node;
                      near_dim = node_dim;
                    }
                }
              while(node->left != 0
                    && (!cmp(node_dim, const_key(node), target_key(it))
                        || near_node == 0
                        || near_distance
                        >= met.distance_to_plane(rank(), node_dim,
                                                 target_key(it),
                                                 const_key(node))))
                {
                  node = node->left;
                  node_dim = incr_dim(rank, node_dim);
                  tmp = met.distance_to_key(rank(), target_key(it),
                                            const_key(node));
                  if (tmp > bound)
                    {
                      if (near_node == 0 || tmp < near_distance)
                        {
                          near_node = node;
                          near_dim = node_dim;
                          near_distance = tmp;
                        }
                      else if (tmp == near_distance && node < near_node)
                        {
                          near_node = node;
                          near_dim = node_dim;
                        }
                    }
                }
            }
          // No more nodes to visit, so go up!
          else
            {
              node_ptr p = node->parent;
              while (p != end && p->right == node)
                {
                  node = p;
                  node_dim = decr_dim(rank, node_dim);
                  p = node->parent;
                }
              node = p;
              node_dim = decr_dim(rank, node_dim);
            }
        }
      while(node != end);
      if (near_node == 0)
        { near_node = node; near_dim = node_dim; }
      SPATIAL_ASSERT_CHECK(near_dim < rank());
      SPATIAL_ASSERT_CHECK(node != 0);
      it.node = near_node; it.node_dim = near_dim;
      it.distance() = near_distance;
      return it;
    }
  } // namespace details
} // namespace spatial

#endif // SPATIAL_NEIGHBOR_HPP
