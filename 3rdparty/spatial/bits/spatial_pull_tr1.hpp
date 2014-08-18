// -*- C++ -*-
//
// Copyright Sylvain Bougerel 2009 - 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file COPYING or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/**
 *  \file   spatial_pull_tr1.hpp
 *  Contains the macro to pull the TR1 headers
 *
 */

#ifndef SPATIAL_PULL_TR1_HPP
#define SPATIAL_PULL_TR1_HPP

#ifdef __GLIBCXX__
#  include <tr1/type_traits>
#else
#  ifdef __IBMCPP__
#    define __IBMCPP_TR1__
#  endif
#  include <type_traits>
#endif

#endif // SPATIAL_PULL_TR1_HPP
