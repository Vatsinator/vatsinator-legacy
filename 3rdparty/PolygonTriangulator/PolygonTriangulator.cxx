#include "PolygonTriangulator.h"

///////////////////////////////////////////////////////////////////////
//                                                                   //
// Class: PolygonTriangulator                                        //
//                                                                   //
// Description: Triangulates any (non-complex) 2D polygon.           //
//                                                                   //
// Author: Thomas Kittelmann (Thomas.Kittelmann@cern.ch), March 2007 //
//                                                                   //
// Notes:                                                            //
//                                                                   //
//     Actual algorithms are adapted from                            //
//     http://www.mema.ucl.ac.be/~wu/Poly2Tri/poly2tri.html          //
//     (see copyright notice below)                                  //
//                                                                   //
//     Main changes performed for the present incarnation are        //
//     interface & namespace changes, indentation, small             //
//     performance tweaks and removal of unused features.            //
//     Most importantly, got rid of static and globals so the class  //
//     can be reliably used more than once per process. Also, the    //
//     output triangles are sorted to preserve "handedness".         //
//                                                                   //
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////// Copyright file from poly2tri webpage follows: /////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Poly2Tri:Fast and Robust Simple Polygon triangulation with/without holes
//                         by Sweep Line Algorithm
//                                Liang, Wu
//         http://www.mema.ucl.ac.be/~wu/Poly2Tri/poly2tri.html
//         Copyright (C) 2003, 2004, 2005, ALL RIGHTS RESERVED.
//
// ---------------------------------------------------------------------
// wu@mema.ucl.ac.be                           wuliang@femagsoft.com
// Centre for Sys. Eng. & App. Mech.           FEMAGSoft S.A.
// Universite Cathalique de Louvain            4, Avenue Albert Einstein
// Batiment Euler, Avenue Georges Lemaitre, 4  B-1348 Louvain-la-Neuve
// B-1348, Louvain-la-Neuve                    Belgium
// Belgium
// ---------------------------------------------------------------------
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHAN-
// TABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// This program may be freely redistributed under the condition that all
// the copyright notices in all source files ( including the copyright
// notice printed when the `-h' switch is selected) are not removed.Both
// the binary and source codes may not be sold or included in any comme-
// rcial products without a license from the corresponding author(s) &
// entities.
//
// 1) Arbitrary precision floating-point arithmetic and fast robust geo-
//    metric predicates (predicates.cc) is copyrighted by
//    Jonathan Shewchuk (http://www.cs.berkeley.edu/~jrs) and you may get
//    the source code from http://www.cs.cmu.edu/~quake/robust.html
//
// 2) The shell script mps2eps is copyrighted by Jon Edvardsson
//    (http://www.ida.liu.se/~pelab/members/index.php4/?12) and you may
//    get the copy from http://www.ida.liu.se/~joned/download/mps2eps/
//
// 3) All other source codes and exmaples files distributed in Poly2Tri
//    are copyrighted by Liang, Wu (http://www.mema.ucl.ac.be/~wu) and
//    FEMAGSoft S.A.
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cmath>
#include <map>
#include <limits>
#include <list>
#include <queue>
#include <set>
#include <iostream>
#include <stack>
#include <cassert>
#include <cstdlib>

namespace internal_poltrig {

  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////// Defs.h ////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////

  //-------------------------------------------------------------------------/
  //Copyright (C) 2003, 2004, 2005, ALL RIGHTS RESERVED.
  //Centre for Sys. Eng. & App. Mech.           FEMAGSoft S.A.
  //Universite Cathalique de Louvain            4, Avenue Albert Einstein
  //Batiment Euler, Avenue Georges Lemaitre, 4  B-1348 Louvain-la-Neuve
  //B-1348, Louvain-la-Neuve                    Belgium
  //Belgium
  //-------------------------------------------------------------------------/
  //Name:         defs.h
  //Purpose:      some definition for mesh generation
  //Author:       Wu Liang
  //Created:      03/2000
  //-------------------------------------------------------------------------/


#define sqr(t)  (t)*(t)

  const double    PI=3.141592653589793238462643383279502884197169399375105820974944592308;
  enum  Type      { UNKNOWN, INPUT, INSERT, START, END, MERGE, SPLIT, REGULAR_UP, REGULAR_DOWN};

  class   Pointbase;
  class   Linebase;

  template <class T, class KeyType>      class    SplayTree;
  typedef std::map<unsigned int, Pointbase*>           PointbaseMap;
  typedef std::map<unsigned int, Linebase*>            LineMap;
  typedef std::priority_queue<Pointbase>               PQueue;
  typedef SplayTree<Linebase*, double>            EdgeBST;
  typedef std::list<unsigned int>                      Monopoly;
  typedef std::list<Monopoly>                          Monopolys;
  typedef std::vector<unsigned int>                    Triangle;
  typedef std::list<Triangle>                          Triangles;
  typedef std::map<unsigned int, std::set<unsigned int> >   AdjEdgeMap;


  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////// predicates.cc /////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////

  /*****************************************************************************/
  /*                                                                           */
  /*  Routines for Arbitrary Precision Floating-point Arithmetic               */
  /*  and Fast Robust Geometric Predicates                                     */
  /*  (predicates.cc)                                                           */
  /*                                                                           */
  /*  May 18, 1996                                                             */
  /*                                                                           */
  /*  Placed in the public domain by                                           */
  /*  Jonathan Richard Shewchuk                                                */
  /*  School of Computer Science                                               */
  /*  Carnegie Mellon University                                               */
  /*  5000 Forbes Avenue                                                       */
  /*  Pittsburgh, Pennsylvania  15213-3891                                     */
  /*  jrs@cs.cmu.edu                                                           */
  /*                                                                           */
  /*  This file contains C implementation of algorithms for exact addition     */
  /*    and multiplication of floating-point numbers, and predicates for       */
  /*    robustly performing the orientation and incircle tests used in         */
  /*    computational geometry.  The algorithms and underlying theory are      */
  /*    described in Jonathan Richard Shewchuk.  "Adaptive Precision Floating- */
  /*    Point Arithmetic and Fast Robust Geometric Predicates."  Technical     */
  /*    Report CMU-CS-96-140, School of Computer Science, Carnegie Mellon      */
  /*    University, Pittsburgh, Pennsylvania, May 1996.  (Submitted to         */
  /*    Discrete & Computational Geometry.)                                    */
  /*                                                                           */
  /*  This file, the paper listed above, and other information are available   */
  /*    from the Web page http://www.cs.cmu.edu/~quake/robust.html .           */
  /*                                                                           */
  /*****************************************************************************/

  /*****************************************************************************/
  /*                                                                           */
  /*  Using this code:                                                         */
  /*                                                                           */
  /*  First, read the short or long version of the paper (from the Web page    */
  /*    above).                                                                */
  /*                                                                           */
  /*  Be sure to call exactinit() once, before calling any of the arithmetic   */
  /*    functions or geometric predicates.  Also be sure to turn on the        */
  /*    optimizer when compiling this file.                                    */
  /*                                                                           */
  /*                                                                           */
  /*  Several geometric predicates are defined.  Their parameters are all      */
  /*    points.  Each point is an array of two or three floating-point         */
  /*    numbers.  The geometric predicates, described in the papers, are       */
  /*                                                                           */
  /*    orient2d(pa, pb, pc)                                                   */
  /*    orient2dfast(pa, pb, pc)                                               */
  /*    orient3d(pa, pb, pc, pd)                                               */
  /*    orient3dfast(pa, pb, pc, pd)                                           */
  /*    incircle(pa, pb, pc, pd)                                               */
  /*    incirclefast(pa, pb, pc, pd)                                           */
  /*    insphere(pa, pb, pc, pd, pe)                                           */
  /*    inspherefast(pa, pb, pc, pd, pe)                                       */
  /*                                                                           */
  /*  Those with suffix "fast" are approximate, non-robust versions.  Those    */
  /*    without the suffix are adaptive precision, robust versions.  There     */
  /*    are also versions with the suffices "exact" and "slow", which are      */
  /*    non-adaptive, exact arithmetic versions, which I use only for timings  */
  /*    in my arithmetic papers.                                               */
  /*                                                                           */
  /*                                                                           */
  /*  An expansion is represented by an array of floating-point numbers,       */
  /*    sorted from smallest to largest magnitude (possibly with interspersed  */
  /*    zeros).  The length of each expansion is stored as a separate integer, */
  /*    and each arithmetic function returns an integer which is the length    */
  /*    of the expansion it created.                                           */
  /*                                                                           */
  /*  Several arithmetic functions are defined.  Their parameters are          */
  /*                                                                           */
  /*    e, f           Input expansions                                        */
  /*    elen, flen     Lengths of input expansions (must be >= 1)              */
  /*    h              Output expansion                                        */
  /*    b              Input scalar                                            */
  /*                                                                           */
  /*  The arithmetic functions are                                             */
  /*                                                                           */
  /*    grow_expansion(elen, e, b, h)                                          */
  /*    grow_expansion_zeroelim(elen, e, b, h)                                 */
  /*    expansion_sum(elen, e, flen, f, h)                                     */
  /*    expansion_sum_zeroelim1(elen, e, flen, f, h)                           */
  /*    expansion_sum_zeroelim2(elen, e, flen, f, h)                           */
  /*    fast_expansion_sum(elen, e, flen, f, h)                                */
  /*    fast_expansion_sum_zeroelim(elen, e, flen, f, h)                       */
  /*    linear_expansion_sum(elen, e, flen, f, h)                              */
  /*    linear_expansion_sum_zeroelim(elen, e, flen, f, h)                     */
  /*    scale_expansion(elen, e, b, h)                                         */
  /*    scale_expansion_zeroelim(elen, e, b, h)                                */
  /*    compress(elen, e, h)                                                   */
  /*                                                                           */
  /*  All of these are described in the long version of the paper; some are    */
  /*    described in the short version.  All return an integer that is the     */
  /*    length of h.  Those with suffix _zeroelim perform zero elimination,    */
  /*    and are recommended over their counterparts.  The procedure            */
  /*    fast_expansion_sum_zeroelim() (or linear_expansion_sum_zeroelim() on   */
  /*    processors that do not use the round-to-even tiebreaking rule) is      */
  /*    recommended over expansion_sum_zeroelim().  Each procedure has a       */
  /*    little note next to it (in the code below) that tells you whether or   */
  /*    not the output expansion may be the same array as one of the input     */
  /*    expansions.                                                            */
  /*                                                                           */
  /*                                                                           */
  /*  If you look around below, you'll also find macros for a bunch of         */
  /*    simple unrolled arithmetic operations, and procedures for printing     */
  /*    expansions (commented out because they don't work with all C           */
  /*    compilers) and for generating rand floating-point numbers whose      */
  /*    significand bits are all rand.  Most of the macros have undocumented */
  /*    requirements that certain of their parameters should not be the same   */
  /*    variable; for safety, better to make sure all the parameters are       */
  /*    distinct variables.  Feel free to send email to jrs@cs.cmu.edu if you  */
  /*    have questions.                                                        */
  /*                                                                           */
  /*****************************************************************************/

  /* On some machines, the exact arithmetic routines might be defeated by the  */
  /*   use of internal extended precision floating-point registers.  Sometimes */
  /*   this problem can be fixed by defining certain values to be volatile,    */
  /*   thus forcing them to be stored to memory and rounded off.  This isn't   */
  /*   a great solution, though, as it slows the arithmetic down.              */
  /*                                                                           */
  /* To try this out, write "#define INEXACT volatile" below.  Normally,       */
  /*   however, INEXACT should be defined to be nothing.  ("#define INEXACT".) */

#define INEXACT                          /* Nothing */
  /* #define INEXACT volatile */

#define REAL double                      /* float or double */

  /* Which of the following two methods of finding the absolute values is      */
  /*   fastest is compiler-dependent.  A few compilers can inline and optimize */
  /*   the fabs() call; but most will incur the overhead of a function call,   */
  /*   which is disastrously slow.  A faster way on IEEE machines might be to  */
  /*   mask the appropriate bit, but that's difficult to do in C.              */

#define Absolute(a)  ((a) >= 0.0 ? (a) : -(a))
  /* #define Absolute(a)  fabs(a) */

  /* Many of the operations are broken up into two pieces, a main part that    */
  /*   performs an approximate operation, and a "tail" that computes the       */
  /*   roundoff error of that operation.                                       */
  /*                                                                           */
  /* The operations Fast_Two_Sum(), Fast_Two_Diff(), Two_Sum(), Two_Diff(),    */
  /*   Split(), and Two_Product() are all implemented as described in the      */
  /*   reference.  Each of these macros requires certain variables to be       */
  /*   defined in the calling routine.  The variables `bvirt', `c', `abig',    */
  /*   `_i', `_j', `_k', `_l', `_m', and `_n' are declared `INEXACT' because   */
  /*   they store the result of an operation that may incur roundoff error.    */
  /*   The input parameter `x' (or the highest numbered `x_' parameter) must   */
  /*   also be declared `INEXACT'.                                             */

#define Fast_Two_Sum_Tail(a, b, x, y) \
  bvirt = x - a; \
  y = b - bvirt

#define Fast_Two_Sum(a, b, x, y) \
  x = (REAL) (a + b); \
  Fast_Two_Sum_Tail(a, b, x, y)

#define Two_Sum_Tail(a, b, x, y) \
  bvirt = (REAL) (x - a); \
  avirt = x - bvirt; \
  bround = b - bvirt; \
  around = a - avirt; \
  y = around + bround

#define Two_Sum(a, b, x, y) \
  x = (REAL) (a + b); \
  Two_Sum_Tail(a, b, x, y)

#define Two_Diff_Tail(a, b, x, y) \
  bvirt = (REAL) (a - x); \
  avirt = x + bvirt; \
  bround = bvirt - b; \
  around = a - avirt; \
  y = around + bround

#define Two_Diff(a, b, x, y) \
  x = (REAL) (a - b); \
  Two_Diff_Tail(a, b, x, y)


  //  c = (REAL) (splitter * a);

#define Split(a, ahi, alo) \
  c = (REAL) (1.0 * a); \
  abig = (REAL) (c - a); \
  ahi = c - abig; \
  alo = a - ahi

#define Two_Product_Tail(a, b, x, y) \
  Split(a, ahi, alo); \
  Split(b, bhi, blo); \
  err1 = x - (ahi * bhi); \
  err2 = err1 - (alo * bhi); \
  err3 = err2 - (ahi * blo); \
  y = (alo * blo) - err3

#define Two_Product(a, b, x, y) \
  x = (REAL) (a * b); \
  Two_Product_Tail(a, b, x, y)

  /* Macros for summing expansions of various fixed lengths.  These are all    */
  /*   unrolled versions of Expansion_Sum().                                   */

#define Two_One_Diff(a1, a0, b, x2, x1, x0) \
  Two_Diff(a0, b , _i, x0); \
  Two_Sum( a1, _i, x2, x1)

#define Two_Two_Diff(a1, a0, b1, b0, x3, x2, x1, x0) \
  Two_One_Diff(a1, a0, b0, _j, _0, x0); \
  Two_One_Diff(_j, _0, b1, x3, x2, x1)


  //TK: weird globals, never initialised. Just replaced with 1.0 everywhere...
  //   REAL splitter(1);     /* = 2^ceiling(p / 2) + 1.  Used to split floats in half. */
  /* A set of coefficients used to calculate maximum roundoff errors.          */
  //   REAL resulterrbound;
  //    REAL ccwerrboundA, ccwerrboundB, ccwerrboundC;
  //   REAL o3derrboundA, o3derrboundB, o3derrboundC;
  //   REAL iccerrboundA, iccerrboundB, iccerrboundC;
  //   REAL isperrboundA, isperrboundB, isperrboundC;

  /*****************************************************************************/
  /*                                                                           */
  /*  fast_expansion_sum_zeroelim()   Sum two expansions, eliminating zero     */
  /*                                  components from the output expansion.    */
  /*                                                                           */
  /*  Sets h = e + f.  See the long version of my paper for details.           */
  /*                                                                           */
  /*  If round-to-even is used (as with IEEE 754), maintains the strongly      */
  /*  nonoverlapping property.  (That is, if e is strongly nonoverlapping, h   */
  /*  will be also.)  Does NOT maintain the nonoverlapping or nonadjacent      */
  /*  properties.                                                              */
  /*                                                                           */
  /*****************************************************************************/

  int fast_expansion_sum_zeroelim(const int& elen, REAL* e, const int& flen, REAL* f, REAL* h)
    /* h cannot be e or f. */
  {
    REAL Q;
    INEXACT REAL Qnew;
    INEXACT REAL hh;
    INEXACT REAL bvirt;
    REAL avirt, bround, around;
    int eindex, findex, hindex;
    REAL enow, fnow;

    enow = e[0];
    fnow = f[0];
    eindex = findex = 0;
    if ((fnow > enow) == (fnow > -enow)) {
      Q = enow;
      enow = e[++eindex];
    } else {
      Q = fnow;
      fnow = f[++findex];
    }
    hindex = 0;
    if ((eindex < elen) && (findex < flen)) {
      if ((fnow > enow) == (fnow > -enow)) {
	Fast_Two_Sum(enow, Q, Qnew, hh);
	enow = e[++eindex];
      } else {
	Fast_Two_Sum(fnow, Q, Qnew, hh);
	fnow = f[++findex];
      }
      Q = Qnew;
      if (hh != 0.0) {
	h[hindex++] = hh;
      }
      while ((eindex < elen) && (findex < flen)) {
	if ((fnow > enow) == (fnow > -enow)) {
	  Two_Sum(Q, enow, Qnew, hh);
	  enow = e[++eindex];
	} else {
	  Two_Sum(Q, fnow, Qnew, hh);
	  fnow = f[++findex];
	}
	Q = Qnew;
	if (hh != 0.0) {
	  h[hindex++] = hh;
	}
      }
    }
    while (eindex < elen) {
      Two_Sum(Q, enow, Qnew, hh);
      enow = e[++eindex];
      Q = Qnew;
      if (hh != 0.0) {
	h[hindex++] = hh;
      }
    }
    while (findex < flen) {
      Two_Sum(Q, fnow, Qnew, hh);
      fnow = f[++findex];
      Q = Qnew;
      if (hh != 0.0) {
	h[hindex++] = hh;
      }
    }
    if ((Q != 0.0) || (hindex == 0)) {
      h[hindex++] = Q;
    }
    return hindex;
  }


  /*****************************************************************************/
  /*                                                                           */
  /*  estimate()   Produce a one-word estimate of an expansion's value.        */
  /*                                                                           */
  /*  See either version of my paper for details.                              */
  /*                                                                           */
  /*****************************************************************************/

  REAL estimate(const int& elen, REAL* e)
  {
    REAL Q;
    int eindex;

    Q = e[0];
    for (eindex = 1; eindex < elen; ++eindex) {
      Q += e[eindex];
    }
    return Q;
  }

  /*****************************************************************************/
  /*                                                                           */
  /*  orient2dfast()   Approximate 2D orientation test.  Nonrobust.            */
  /*  orient2dexact()   Exact 2D orientation test.  Robust.                    */
  /*  orient2dslow()   Another exact 2D orientation test.  Robust.             */
  /*  orient2d()   Adaptive exact 2D orientation test.  Robust.                */
  /*                                                                           */
  /*               Return a positive value if the points pa, pb, and pc occur  */
  /*               in counterclockwise order; a negative value if they occur   */
  /*               in clockwise order; and zero if they are collinear.  The    */
  /*               result is also a rough approximation of twice the signed    */
  /*               area of the triangle defined by the three points.           */
  /*                                                                           */
  /*  Only the first and last routine should be used; the middle two are for   */
  /*  timings.                                                                 */
  /*                                                                           */
  /*  The last three use exact arithmetic to ensure a correct answer.  The     */
  /*  result returned is the determinant of a matrix.  In orient2d() only,     */
  /*  this determinant is computed adaptively, in the sense that exact         */
  /*  arithmetic is used only to the degree it is needed to ensure that the    */
  /*  returned value has the correct sign.  Hence, orient2d() is usually quite */
  /*  fast, but will run more slowly when the input points are collinear or    */
  /*  nearly so.                                                               */
  /*                                                                           */
  /*****************************************************************************/

  REAL orient2dadapt(REAL* pa, REAL* pb, REAL* pc, const REAL& detsum)
  {
    INEXACT REAL acx, acy, bcx, bcy;
    REAL acxtail, acytail, bcxtail, bcytail;
    INEXACT REAL detleft, detright;
    REAL detlefttail, detrighttail;
    REAL det, errbound;
    REAL B[4], C1[8], C2[12], D[16];
    INEXACT REAL B3;
    int C1length, C2length, Dlength;
    REAL u[4];
    INEXACT REAL u3;
    INEXACT REAL s1, t1;
    REAL s0, t0;

    INEXACT REAL bvirt;
    REAL avirt, bround, around;
    INEXACT REAL c;
    INEXACT REAL abig;
    REAL ahi, alo, bhi, blo;
    REAL err1, err2, err3;
    INEXACT REAL _i, _j;
    REAL _0;

    acx = (REAL) (pa[0] - pc[0]);
    bcx = (REAL) (pb[0] - pc[0]);
    acy = (REAL) (pa[1] - pc[1]);
    bcy = (REAL) (pb[1] - pc[1]);

    Two_Product(acx, bcy, detleft, detlefttail);
    Two_Product(acy, bcx, detright, detrighttail);

    Two_Two_Diff(detleft, detlefttail, detright, detrighttail,
		 B3, B[2], B[1], B[0]);
    B[3] = B3;

    det = estimate(4, B);
    //    errbound = ccwerrboundB * detsum;
    errbound = detsum;
    if ((det >= errbound) || (-det >= errbound)) {
      return det;
    }

    Two_Diff_Tail(pa[0], pc[0], acx, acxtail);
    Two_Diff_Tail(pb[0], pc[0], bcx, bcxtail);
    Two_Diff_Tail(pa[1], pc[1], acy, acytail);
    Two_Diff_Tail(pb[1], pc[1], bcy, bcytail);

    if ((acxtail == 0.0) && (acytail == 0.0)
	&& (bcxtail == 0.0) && (bcytail == 0.0)) {
      return det;
    }

    //    errbound = ccwerrboundC * detsum + resulterrbound * Absolute(det);
    errbound = detsum + Absolute(det);
    det += (acx * bcytail + bcy * acxtail)
      - (acy * bcxtail + bcx * acytail);
    if ((det >= errbound) || (-det >= errbound)) {
      return det;
    }

    Two_Product(acxtail, bcy, s1, s0);
    Two_Product(acytail, bcx, t1, t0);
    Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
    u[3] = u3;
    C1length = fast_expansion_sum_zeroelim(4, B, 4, u, C1);

    Two_Product(acx, bcytail, s1, s0);
    Two_Product(acy, bcxtail, t1, t0);
    Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
    u[3] = u3;
    C2length = fast_expansion_sum_zeroelim(C1length, C1, 4, u, C2);

    Two_Product(acxtail, bcytail, s1, s0);
    Two_Product(acytail, bcxtail, t1, t0);
    Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
    u[3] = u3;
    Dlength = fast_expansion_sum_zeroelim(C2length, C2, 4, u, D);

    return(D[Dlength - 1]);
  }

  REAL orient2d(REAL* pa, REAL* pb, REAL* pc)
  {
    REAL detleft, detright, det;
    REAL detsum, errbound;

    detleft = (pa[0] - pc[0]) * (pb[1] - pc[1]);
    detright = (pa[1] - pc[1]) * (pb[0] - pc[0]);
    det = detleft - detright;

    if (detleft > 0.0) {
      if (detright <= 0.0) {
	return det;
      } else {
	detsum = detleft + detright;
      }
    } else if (detleft < 0.0) {
      if (detright >= 0.0) {
	return det;
      } else {
	detsum = -detleft - detright;
      }
    } else {
      return det;
    }

    //    errbound = ccwerrboundA * detsum;
    errbound = detsum;
    if ((det >= errbound) || (-det >= errbound)) {
      return det;
    }

    return orient2dadapt(pa, pb, pc, detsum);
  }


  ////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////// splay.h ///////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////

  //-------------------------------------------------------------------------/
  //Copyright (C) 2003, 2004, 2005, ALL RIGHTS RESERVED.
  //Centre for Sys. Eng. & App. Mech.           FEMAGSoft S.A.
  //Universite Cathalique de Louvain            4, Avenue Albert Einstein
  //Batiment Euler, Avenue Georges Lemaitre, 4  B-1348 Louvain-la-Neuve
  //B-1348, Louvain-la-Neuve                    Belgium
  //Belgium
  //-------------------------------------------------------------------------/
  //
  //Name:         splay.h (self-adjusting balanced binary search tree)
  //Purpose:      declaration and implentation of splay tree class for fast
  //              binary searching
  //Author:       Liang, Wu (wu@mema.ucl.ac.be, wuliang@femagsoft.com)
  //Created:      03/2002
  //Modified:     2003, 2004, 2005
  //-------------------------------------------------------------------------/

  template <class T, class KeyType>
  class SplayTree;

  template <class T, class KeyType>
  class BTreeNode
  {
  public:
    friend class SplayTree<T, KeyType>;
    BTreeNode( ) : _data(), _left( NULL ), _right( NULL ), _visited(false) { }
    BTreeNode( const T & data, BTreeNode *lt, BTreeNode *rt )
      : _data(data),_left( lt ), _right( rt ), _visited(false) { }

    T& data()                     { return _data; }
    BTreeNode* Left()             { return _left; }
    BTreeNode* Right()            { return _right; }
    void SetVisited(const bool& visited) { _visited=visited; }
    bool GetVisited()             { return _visited; }
    KeyType keyValue()            { return _data->keyValue(); }

  private:
    T          _data;
    BTreeNode *_left;
    BTreeNode *_right;
    bool      _visited;

  };


  template <class T, class KeyType>
  class SplayTree
  {
  public:
    explicit SplayTree( ):root(NULL),size(0) { }
    SplayTree( const SplayTree & rhs );
    ~SplayTree( );

    void MakeEmpty( );
    bool IsEmpty( ) const;
    long int Size() { return size; }
    BTreeNode<T, KeyType>* Root() { return root; }

    void Find( const KeyType& keys, BTreeNode<T, KeyType>* & res);
    void FindMin( BTreeNode<T, KeyType>* &min );
    void FindMax( BTreeNode<T, KeyType>* &max );
    // We only use this function for polygon Triangulation to find the direct
    // left edge of an event vertex;
    void FindMaxSmallerThan( const KeyType& keys, BTreeNode<T, KeyType>* &res);

    void Insert( const T & x );
    void Delete( const KeyType& keys);
    void Delete( const KeyType& keys, BTreeNode<T, KeyType>* &res);
    void DeleteMin(BTreeNode<T, KeyType>* &min);
    void DeleteMax(BTreeNode<T, KeyType>* &max);

    const SplayTree & operator=( const SplayTree & rhs );
    void PreOrder( void(*Visit)(BTreeNode<T,KeyType> *u) )
    { PreOrder(Visit, root); }
    void InOrder( void(*Visit)(BTreeNode<T,KeyType> *u) )
    { InOrder(Visit, root); }

    void InOrder( void(*Visit)(BTreeNode<T,KeyType>*u, double y), double y)
    { InOrder(Visit, root, y); }

    void PostOrder( void(*Visit)(BTreeNode<T,KeyType> *u) )
    { PostOrder(Visit, root); }

    int Height( ) const { return Height(root); }  //height of root
    int Height(BTreeNode<T, KeyType> *t) const;    //Height of subtree t;
    BTreeNode<T, KeyType>* Left(BTreeNode<T, KeyType> *node) { return node->_left; }
    BTreeNode<T, KeyType>* Right(BTreeNode<T, KeyType> *node) { return node->_right; }

  private:
    BTreeNode<T, KeyType> *root;
    long int              size;

    void reclaimMemory( BTreeNode<T, KeyType> * t ) const;
    BTreeNode<T, KeyType> * clone( BTreeNode<T, KeyType> *t ) const;

    //Transverse
    void PreOrder( void(*Visit)(BTreeNode<T, KeyType> *u), BTreeNode<T, KeyType> *t);
    void InOrder( void(*Visit)(BTreeNode<T, KeyType> *u), BTreeNode<T, KeyType> *t);
    void PostOrder( void(*Visit)(BTreeNode<T, KeyType> *u), BTreeNode<T, KeyType> *t);

    void InOrder( void(*Visit)(BTreeNode<T, KeyType>*, double y),
		  BTreeNode<T, KeyType> *t, double y);



    // Tree manipulations
    void rotateWithLeftChild( BTreeNode<T, KeyType> * & k2 ) const;
    void rotateWithRightChild( BTreeNode<T, KeyType> * & k1 ) const;
    void splay( const KeyType& keys, BTreeNode<T, KeyType> * & t ) const;
  };

  //----------------------------------------------------------------------
  //Constructor;
  //----------------------------------------------------------------------
  template <class T, class KeyType>
  SplayTree<T, KeyType>::SplayTree( const SplayTree<T, KeyType> & rhs )
  {
    *this = rhs;
  }

  //-----------------------------------------------------------------------
  //Destructor.
  //-----------------------------------------------------------------------
  template <class T, class KeyType>
  SplayTree<T, KeyType>::~SplayTree( )
  {
    MakeEmpty( );
  }

  //------------------------------------------------------------------------
  //Insert x into the tree.
  //------------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::Insert( const T & x )
  {

    BTreeNode<T, KeyType> *newNode= new BTreeNode<T, KeyType>;
    newNode->_data=x;

    if( root == NULL )
      {
	newNode->_left = newNode->_right = NULL;
	root = newNode; ++size;
      }
    else
      {
	KeyType keys=x->keyValue();
	splay( keys, root );
	KeyType rootk=root->keyValue();
	if( keys < rootk )
	  {
	    newNode->_left = root->_left;
	    newNode->_right = root;
	    root->_left = NULL;
	    root = newNode;
	    ++size;
	  }
        else if( keys > rootk )
	  {

	    newNode->_right = root->_right;
	    newNode->_left = root;
	    root->_right = NULL;
	    root = newNode;
	    ++size;
	  }
        else
	  {
	    delete newNode;//TK: don't leak

	    //slight incresed the keyvalue to avoid duplicated keys
	    x->increaseKeyValue(1.0e-10);
	    Insert(x);

	  }
      }
  }

  //---------------------------------------------------------------------
  //Remove the node with the keys from the tree, and retrieve the result
  //---------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::Delete( const KeyType& keys, BTreeNode<T, KeyType>* &res)
  {
    BTreeNode<T, KeyType> *newTree;

    splay( keys, root );
    if( root->keyValue() != keys ) { res=NULL; return; } // Item not found; do nothing

    res = root;

    if( root->_left == NULL )
      newTree = root->_right;
    else
      {
	// Find the maximum in the _left subtree
	// Splay it to the root; and then attach _right child
	newTree = root->_left;
	splay( keys, newTree );
	newTree->_right = root->_right;
      }

    root = newTree;
    size--;
  }

  //---------------------------------------------------------------------
  //Remove the node with the keys from the tree.
  //---------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::Delete( const KeyType& keys)
  {
    BTreeNode<T, KeyType> *newTree;

    splay( keys, root );
    KeyType rootk=root->keyValue();
    if( rootk != keys ) { return; } // Item not found; do nothing

    if( root->_left == NULL ) newTree = root->_right;
    else
      {
	// Find the maximum in the _left subtree
	// Splay it to the root; and then attach _right child
	newTree = root->_left;
	splay( keys, newTree );
	newTree->_right = root->_right;
      }

    delete root;
    root = newTree;
    size--;
  }



  //---------------------------------------------------------------------
  //Find and Delete the node with min keys from the tree;
  //---------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::DeleteMin(BTreeNode<T, KeyType>* &min)
  {
    if( IsEmpty( ) )  { min=NULL; return; }

    double keys=-1.0e30;
    splay( keys, root );

    min = root;

    BTreeNode<T, KeyType> *newTree;
    if( root->_left == NULL ) newTree = root->_right;
    else
      {
	newTree = root->_left;
	splay( keys, newTree );
	newTree->_right = root->_right;
      }

    size--;
    root = newTree;

  }

  //----------------------------------------------------------------------
  //Find and Delete the node with max keys from the tree;
  //----------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::DeleteMax(BTreeNode<T, KeyType>* &max)
  {
    if( IsEmpty( ) )  { max=NULL; return; }

    double keys=1.0e30;
    splay( keys, root );

    max = root;

    BTreeNode<T, KeyType> *newTree;
    if( root->_left == NULL ) newTree = root->_right;
    else
      {
	newTree = root->_left;
	splay( keys, newTree );
	newTree->_right = root->_right;
      }
    size--;
    root = newTree;
  }


  //----------------------------------------------------------------------
  //Find the smallest item in the tree, but won't delete it from the tree.
  //----------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::FindMin(BTreeNode<T, KeyType>* & min )
  {
    if( IsEmpty( ) )  { min=NULL; return; }
    BTreeNode<T, KeyType> *ptr = root;

    while( ptr->_left != NULL ) ptr = ptr->_left;
    splay( ptr->keyValue(), root );
    min = ptr;
  }

  //----------------------------------------------------------------------
  //Find the largest item in the tree. but won't delete it from the tree.
  //----------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::FindMax(BTreeNode<T, KeyType>* & max)
  {
    if( IsEmpty( ) )   { max=NULL; return; }

    BTreeNode<T, KeyType> *ptr = root;
    while( ptr->_right != NULL ) ptr = ptr->_right;
    splay( ptr->keyValue(), root );
    max =  ptr;
  }

  //--------------------------------------------------------------------
  //Find the node with the keys in the tree.
  //res==NULL if it donesn't exist in the tree;
  //--------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::Find( const KeyType& keys, BTreeNode<T, KeyType>* & res)
  {
    if( IsEmpty( ) ) { res=NULL; return; }
    splay( keys, root );
    if( root->keyValue() != keys ) { res=NULL; return; }
    else res = root;
  }

  //--------------------------------------------------------------------
  //Find the maximum node smaller than or equal to the given key.
  //This function specially designed for polygon Triangulation to
  //find the direct left edge at event vertex;
  //--------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::FindMaxSmallerThan( const KeyType& keys, BTreeNode<T, KeyType>* &res)
  {
    if( IsEmpty( ) ) { res=NULL; return; }
    splay( keys, root );

    if( root->data()->keyValue() < keys) res=root;
    else if(root->_left)
      {
	res=root->_left;
	while(res->_right) res=res->_right;
      }
    else
      {
	assert(false);
      }
  }

  //--------------------------------------------------------------------
  //Make the tree logically empty.
  //--------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::MakeEmpty( )
  {
    BTreeNode<T, KeyType>* ptr;
    while( !IsEmpty( ) )
      {
	DeleteMax(ptr);
	delete ptr;
      }
  }

  //---------------------------------------------------------------------
  //Test if the tree is logically empty.
  //---------------------------------------------------------------------
  template <class T, class KeyType>
  bool SplayTree<T, KeyType>::IsEmpty( ) const
  {
    return root == NULL;
  }

  //----------------------------------------------------------------------
  //copy overload operator.
  //----------------------------------------------------------------------
  template <class T, class KeyType>
  const SplayTree<T, KeyType> & SplayTree<T, KeyType>::operator=( const SplayTree<T, KeyType> & rhs )
  {
    if( this != &rhs )
      {
	MakeEmpty( );
	root = clone( rhs.root );
      }

    return *this;
  }

  //-----------------------------------------------------------------------
  //Internal method to perform a top-down splay.
  //x is the key of target node to splay around.
  //t is the root of the subtree to splay.
  //-----------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::splay( const KeyType& keys, BTreeNode<T, KeyType> * & t ) const
  {
    BTreeNode<T, KeyType> *_leftTreeMax, *_rightTreeMin;
    //    static BTreeNode<T, KeyType> header;
    BTreeNode<T, KeyType> header;//TK: Removed static keyword. Rather a bit slower than thread problems...

    header._left = header._right = NULL;
    _leftTreeMax = _rightTreeMin = &header;

    for( ; ; )
      {
	KeyType rkey=t->keyValue();
	if( keys < rkey )
	  {
	    if(t->_left == NULL) break;
	    if( keys < t->_left->keyValue() ) rotateWithLeftChild( t );
	    if( t->_left == NULL ) break;

	    // Link Right
	    _rightTreeMin->_left = t;
	    _rightTreeMin = t;
	    t = t->_left;
	  }
	else if( keys > rkey )
	  {
	    if( t->_right == NULL ) break;
	    if( keys > t->_right->keyValue() ) rotateWithRightChild( t );
	    if( t->_right == NULL ) break;

	    // Link Left
	    _leftTreeMax->_right = t;
	    _leftTreeMax = t;
	    t = t->_right;
	  }
	else  break;
      }

    _leftTreeMax->_right = t->_left;
    _rightTreeMin->_left = t->_right;
    t->_left = header._right;
    t->_right = header._left;

  }

  //--------------------------------------------------------------------
  //Rotate binary tree node with _left child.
  //--------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::rotateWithLeftChild( BTreeNode<T, KeyType> * & k2 ) const
  {
    BTreeNode<T, KeyType> *k1 = k2->_left;
    k2->_left = k1->_right;
    k1->_right = k2;
    k2 = k1;
  }

  //---------------------------------------------------------------------
  //Rotate binary tree node with _right child.
  //---------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::rotateWithRightChild( BTreeNode<T, KeyType> * & k1 ) const
  {
    BTreeNode<T, KeyType> *k2 = k1->_right;
    k1->_right = k2->_left;
    k2->_left = k1;
    k1 = k2;
  }

  //----------------------------------------------------------------------
  //Internal method to reclaim internal nodes in subtree t.
  //WARNING: This is prone to running out of stack space.
  //----------------------------------------------------------------------
  template <class T, class KeyType>
  void SplayTree<T, KeyType>::reclaimMemory( BTreeNode<T, KeyType> * t ) const
  {
    if( t != t->_left )
      {
	reclaimMemory( t->_left );
	reclaimMemory( t->_right );
	delete t;
      }
  }

  //-----------------------------------------------------------------------
  //Internal method to clone subtree.
  //WARNING: This is prone to running out of stack space.
  //-----------------------------------------------------------------------
  template <class T, class KeyType>
  BTreeNode<T, KeyType> * SplayTree<T, KeyType>::clone( BTreeNode<T, KeyType> * t ) const
  {
    if( t == t->_left )  // Cannot test against NULLNode!!!
      return NULL;
    else
      return new BTreeNode<T, KeyType>( t->_data, clone( t->_left ), clone( t->_right ) );
  }

  //-----------------------------------------------------------------------
  //Tranverse the tree by pre-order method;
  //-----------------------------------------------------------------------
  template<class T, class KeyType>
  void SplayTree<T, KeyType>::PreOrder( void(*Visit)(BTreeNode<T, KeyType> *u), BTreeNode<T, KeyType> *t)
  {
    if(t!=NULL)
      {
	Visit(t);
	PreOrder(Visit,t->_left);
	PreOrder(Visit,t->_right);
      }

  }

  //-----------------------------------------------------------------------
  //Tranverse the tree by in-order method;
  //-----------------------------------------------------------------------
  template<class T, class KeyType>
  void SplayTree<T, KeyType>::InOrder( void(*Visit)(BTreeNode<T, KeyType> *u), BTreeNode<T, KeyType> *t)
  {
    if(t!=NULL)
      {
        InOrder(Visit,t->_left);
        Visit(t);
        InOrder(Visit,t->_right);
      }
  }


  //-----------------------------------------------------------------------
  //Tranverse the tree by in-order method;
  //-----------------------------------------------------------------------
  template<class T, class KeyType>
  void SplayTree<T, KeyType>::InOrder( void(*Visit)(BTreeNode<T, KeyType>*u, double y)
				       , BTreeNode<T, KeyType> *t, double y)
  {
    if(t!=NULL)
      {
        InOrder(Visit,t->_left, y);
        Visit(t, y);
        InOrder(Visit,t->_right, y);
      }
  }



  //-----------------------------------------------------------------------
  //Tranverse the tree by post-order method;
  //-----------------------------------------------------------------------
  template<class T, class KeyType>
  void SplayTree<T, KeyType>::PostOrder( void(*Visit)(BTreeNode<T, KeyType> *u), BTreeNode<T, KeyType> *t)
  {
    if(t!=NULL)
      {
        PostOrder(Visit,t->_left);
        PostOrder(Visit,t->_right);
        Visit(t);
      }
  }

  //-----------------------------------------------------------------------
  //return the height of subtree
  //-----------------------------------------------------------------------
  template<class T, class KeyType>
  int SplayTree<T, KeyType>::Height(BTreeNode<T, KeyType> *subtree) const
  {
    if(subtree==NULL) return 0;
    int lh=Height(subtree->_left);
    int rh=Height(subtree->_right);

    return (lh>rh)?(++lh):(++rh);
  }


  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////// geometry.h //////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////

  //-------------------------------------------------------------------------/
  //Copyright (C) 2003, 2004, 2005, ALL RIGHTS RESERVED.
  //Centre for Sys. Eng. & App. Mech.           FEMAGSoft S.A.
  //Universite Cathalique de Louvain            4, Avenue Albert Einstein
  //Batiment Euler, Avenue Georges Lemaitre, 4  B-1348 Louvain-la-Neuve
  //B-1348, Louvain-la-Neuve                    Belgium
  //Belgium
  //-------------------------------------------------------------------------/
  //
  //Name:         geometry.h (all geometry premitives related polygon triang-
  //              ulation by sweep line algorithm)
  //Author:       Liang, Wu (wu@mema.ucl.ac.be, wuliang@femagsoft.com)
  //Created:      03/2001
  //Modified:     10/2005. Modified and simplified only for polygon triangul-
  //              ation purpose.
  //-------------------------------------------------------------------------/


  //base class for points;
  class Pointbase
  {
  public:
    //constructors and destructor
    Pointbase() :id(0),x(0),y(0),type(UNKNOWN),left(true) {}
    Pointbase(const Pointbase& pb);

    Pointbase(const double& xx, const double& yy)
      :id(0), x(xx), y(yy), type(UNKNOWN), left(true) { }

    Pointbase(const int& idd, const double& xx, const double& yy)
      :id(idd), x(xx), y(yy), type(UNKNOWN),left(true) { }

    Pointbase(const double& xx, const double& yy, const Type& ttype)
      :id(0), x(xx), y(yy), type(ttype),left(true) { }

    Pointbase(const int& idd, const double& xx, const double& yy, const Type& ttype)
      :id(idd),x(xx), y(yy), type(ttype),left(true) { }

    //operator overloading
    friend  bool operator==(const Pointbase&, const Pointbase&);
    friend  bool operator>(const Pointbase&, const Pointbase&);
    friend  bool operator<(const Pointbase&, const Pointbase&);
    friend  bool operator!=(const Pointbase&, const Pointbase&);

    //public data
    unsigned int    id;              //id of point;
    double          x, y;            //coordinates;
    Type            type;            //type of points;
    bool            left;            //left chain or not;

  };


  //base class for polygon boundary
  //Linebase class is a directed line segment with start/end point
  class Linebase
  {
  public:
    //constructors and destructor
    Linebase();
    Linebase(Pointbase* ep1, Pointbase* ep2, const Type& type,long int & l_id);
    Linebase(const Linebase& line);
    ~Linebase() {};

    unsigned int id() const { return _id; }

    //two end points
    Pointbase*   endPoint(const int& i) const { return _endp[i]; }
    Type         type() const { return _type; }
    double       keyValue() const { return _key; }
    void         setKeyValue(const double& y);
    //slightly increased the key to avoid duplicated key for searching tree.
    void         increaseKeyValue(const double& diff) { _key+=diff; }
    //reverse a directed line segment; reversable only for inserted diagonals
    void         reverse();

    //set and return helper of a directed line segment;
    void         setHelper(const unsigned int& i) { _helper=i; }
    unsigned int helper() { return _helper; }

  protected:
    unsigned int _id;           //id of a line segment;
    Pointbase*   _endp[2];      //two end points;

    Type         _type;         //type of a line segement, input/insert
    double       _key;          //key of a line segment for splay tree searching
    unsigned int _helper;       //helper of a line segemnt
  };

  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////// geometry.cc //////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////

  //-------------------------------------------------------------------------/
  //Copyright (C) 2003, 2004, 2005, ALL RIGHTS RESERVED.
  //Centre for Sys. Eng. & App. Mech.           FEMAGSoft S.A.
  //Universite Cathalique de Louvain            4, Avenue Albert Einstein
  //Batiment Euler, Avenue Georges Lemaitre, 4  B-1348 Louvain-la-Neuve
  //B-1348, Louvain-la-Neuve                    Belgium
  //Belgium
  //-------------------------------------------------------------------------/
  //
  //Name:         geometry.cc (all geometry premitive implementations related
  //              to polygon triangulation by sweep line algorithm)
  //Author:       Liang, Wu (wu@mema.ucl.ac.be, wuliang@femagsoft.com)
  //Created:      03/2001
  //Modified:     10/2005. Modified and simplified only for polygon triangul-
  //              ation purpose.
  //-------------------------------------------------------------------------/


  //Jonathan schewchuk's exact arithmetic code, see predicates.cc for detais;
  extern double orient2d(double* pa, double* pb, double* pc);

  //----------------------------------------------------------------------------
  //square of the distance of two points;
  //----------------------------------------------------------------------------
  double dist_sqr(const Pointbase& sp, const Pointbase& ep)
  {
    return sqr(sp.x-ep.x)+sqr(sp.y-ep.y);
  }

  //----------------------------------------------------------------------------
  //square of the distance of two points;
  //----------------------------------------------------------------------------
  double dist_sqr(double *pa, double *pb)
  {
    return sqr(pa[0]-pb[0])+sqr(pa[1]-pb[1]);
  }

  void UpdateKey(BTreeNode<Linebase*,double>* node, double y)
  {
    node->data()->setKeyValue(y);
  }

  //----------------------------------------------------------------------------
  //copy constructor
  //----------------------------------------------------------------------------
  Pointbase::Pointbase(const Pointbase& pb)
  {
    this->id=pb.id;
    this->x=pb.x;
    this->y=pb.y;
    this->type=pb.type;
    this->left=pb.left;
  }

  //----------------------------------------------------------------------------
  //operator ( ==, >, < and != ) overloading for pointbase class
  //----------------------------------------------------------------------------
  bool operator==(const Pointbase& pa, const Pointbase& pb)
  {
    return (pa.x==pb.x && pa.y==pb.y);
  }

  //----------------------------------------------------------------------------
  bool operator>(const Pointbase& pa, const Pointbase& pb)
  {
    return( (pa.y > pb.y) || ( (pa.y==pb.y) && (pa.x < pb.x)) );
  }

  //----------------------------------------------------------------------------
  bool operator<(const Pointbase& pa, const Pointbase& pb)
  {
    return( (pa.y < pb.y) || ( (pa.y==pb.y) && (pa.x > pb.x)) );
  }

  //----------------------------------------------------------------------------
  bool operator!=(const Pointbase& pa, const Pointbase& pb)
  {
    return !(pa.x==pb.x && pa.y==pb.y);
  }

  //----------------------------------------------------------------------------
  //Linebase construct
  //----------------------------------------------------------------------------
  Linebase::Linebase():_type(UNKNOWN), _key(0), _helper(0)
  {
    for(int i=0; i<2; ++i) _endp[i]=0;
    _id=0;
  }

  //-----------------------------------------------------------------------------
  //Linebase construct
  //-----------------------------------------------------------------------------
  Linebase::Linebase(Pointbase* sp, Pointbase* ep, const Type& type, long int & l_id):_type(type), _key(0), _helper(0)
  {
    _endp[0]=sp;
    _endp[1]=ep;
    _id=++l_id;
  }

  //----------------------------------------------------------------------------
  //copy constructor
  //----------------------------------------------------------------------------
  Linebase::Linebase(const Linebase& line)
  {
    this->_id=line._id;
    this->_endp[0]=line._endp[0];
    this->_endp[1]=line._endp[1];
    this->_key=line._key;
    this->_type=line._type;
    this->_helper=line._helper;
  }


  //----------------------------------------------------------------------------
  //reverse a directed line segment, reverseable only for insert diagonals
  //----------------------------------------------------------------------------
  void Linebase::reverse()
  {
    assert(_type==INSERT);
    Pointbase* tmp=_endp[0];
    _endp[0]=_endp[1];
    _endp[1]=tmp;
  }

  void Linebase::setKeyValue(const double& y)
  {
    if( _endp[1]->y==_endp[0]->y )
      _key=_endp[0]->x < _endp[1]->x ? _endp[0]->x:_endp[1]->x;
    else    _key=( y - _endp[0]->y ) * ( _endp[1]->x - _endp[0]->x ) / (_endp[1]->y - _endp[0]->y ) + _endp[0]->x;
  }

}//end namespace internal_poltrig

class PolygonTriangulator::Polygon
{
public:
  //constructor and destructor
  Polygon(const std::vector<double>& x,const std::vector<double>& y);
  ~Polygon();

  // main member function for polygon triangulation;
  void         partition2Monotone();
  void         searchMonotones();
  void         triangulation();

  //return all triangles
  const Triangles*    triangles() { return &_triangles; }

  internal_poltrig::PointbaseMap& points()    { return _points; }
  internal_poltrig::LineMap&      edges()     { return _edges; }

  //private member functions.
private:
  long int l_id;//previous a global, but that gives mem. crashes. Must be reinitted for every polygon.

  void set_contour (const std::vector<double>& x,const std::vector<double>& y);
  void         initializate();

  //prev or next point/edge id for a given ith point/edge;
  unsigned int prev(const unsigned int& i);
  unsigned int next(const unsigned int& i);

  //handle event vertext according to vertex type;
  void         handleStartVertex(const unsigned int& );
  void         handleEndVertex(const unsigned int& );
  void         handleSplitVertex(const unsigned int& );
  void         handleMergeVertex(const unsigned int& );
  void         handleRegularVertexUp(const unsigned int& );
  void         handleRegularVertexDown(const unsigned int& );

  //add diagonal between two vertices;
  void         addDiagonal(const unsigned int&  i, const unsigned int&  j);


  //angle ABC for three given points, for monotone polygon searching purpose;
  double       angleCosb(double *A, double *B, double *C);
  //find the next edge, for monotone polygon searching purpose;
  unsigned int selectNextEdge(internal_poltrig::Linebase* edge);

  //triangulate a monotone polygon piece;
  void         triangulateMonotone(internal_poltrig::Monopoly& mpoly);

  //private data memebers
  internal_poltrig::PQueue      _qpoints;                            //priority queue for event points
  internal_poltrig::EdgeBST     _edgebst;                            //edge binary searching tree (splaytree)
  internal_poltrig::Monopolys   _mpolys;                             //all monotone polygon piece list;
  Triangles   _triangles;                          //all triangle list;

  //data for monotone piece searching purpose;
  internal_poltrig::AdjEdgeMap  _startAdjEdgeMap;                    //all edges starting from given points (map)
  internal_poltrig::LineMap     _diagonals;                          //added diagonals to partition polygon to
  //monotont pieces, not all diagonals of

  void init_vertices_and_lines();
  unsigned int            _ncontours;   //number of contours
  std::vector<unsigned int>    _nVertices;   //
  internal_poltrig::PointbaseMap            _points;      //all vertices
  internal_poltrig::LineMap                 _edges;       //all edges
  double                  _xmin,_xmax, _ymin,_ymax; //boundary box for polygon

};


void PolygonTriangulator::Polygon::init_vertices_and_lines()
{
  int sid,eid;
  int num=0;

  internal_poltrig::Type type;

  for(unsigned j=0; j<_ncontours; ++j)
    {
      for(unsigned i=1; i<=_nVertices[j]; ++i)//fixme: 0-based?
	{
          sid=num+i;
          eid=(i==_nVertices[j])?num+1:num+i+1;
	  type = internal_poltrig::INPUT;
          internal_poltrig::Linebase* line=new internal_poltrig::Linebase(_points[sid], _points[eid], type,l_id);
	  _edges[l_id]=line;
        }
      num+=_nVertices[j];
    }

  int sum=0;
  for(unsigned int i=0; i<_ncontours; ++i)
    {
      sum+= _nVertices[i];
      _nVertices[i]=sum;
    }

}


//----------------------------------------------------------------------------
//get input
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::set_contour(const std::vector<double>& x,const std::vector<double>& y)
{
  assert(x.size()==y.size());

  _nVertices.push_back( x.size() );
  unsigned int i = _points.size()+1/*1*/;//fixme: get rid of the +1 ?
  double xx,yy;
  internal_poltrig::Type type;
  for (unsigned int j = 0; j < _nVertices[_ncontours]; ++j, ++i)
    {
      xx=x.at(j);
      yy=y.at(j);
      type=internal_poltrig::INPUT;
      internal_poltrig::Pointbase* point=new internal_poltrig::Pointbase(i,xx,yy,type);
      if(xx > _xmax ) _xmax=xx;
      if(xx < _xmin ) _xmin=xx;
      if(yy > _ymax ) _ymax=yy;
      if(yy < _ymin ) _ymin=yy;
      _points[i]=point;
    }

  ++_ncontours;

}




//----------------------------------------------------------------------------
//polygon class constructor
//----------------------------------------------------------------------------
PolygonTriangulator::Polygon::Polygon(const std::vector<double>& x,const std::vector<double>& y)
{
  l_id = 0;
  _ncontours=0;

  _xmin = _ymin = std::numeric_limits<double>::infinity();
  _xmax = _ymax = - std::numeric_limits<double>::infinity();


  set_contour(x,y);
  init_vertices_and_lines();
  initializate();
}

//----------------------------------------------------------------------------
//polygon destructor
//----------------------------------------------------------------------------
PolygonTriangulator::Polygon::~Polygon()
{
  //clear all dynamic allocated memory
  internal_poltrig::PointbaseMap::iterator itp=_points.begin();
  for(; itp!=_points.end(); ++itp)
    {
      delete itp->second;
    }

  internal_poltrig::LineMap::iterator itl=_edges.begin();
  for(; itl!=_edges.end(); ++itl)
    {
      delete itl->second;
    }

}

//----------------------------------------------------------------------------
//return the previous point (or edge) id for a given ith point (or edge);
//----------------------------------------------------------------------------
unsigned int PolygonTriangulator::Polygon::prev(const unsigned int&  i)
{
  unsigned int j(0),prevLoop(0),currentLoop(0);

  while ( i > _nVertices[currentLoop] )
    {
      prevLoop=currentLoop;
      ++currentLoop;
    }

  if( i==1 || (i==_nVertices[prevLoop]+1) ) j=_nVertices[currentLoop];
  else if( i <= _nVertices[currentLoop] ) j=i-1;

  return j;
}

//----------------------------------------------------------------------------
//return the next point (or edge) id for a given ith point (or edge);
//----------------------------------------------------------------------------
unsigned int PolygonTriangulator::Polygon::next(const unsigned int&  i)
{
  unsigned int j(0),prevLoop(0),currentLoop(0);

  while ( i > _nVertices[currentLoop] )
    {
      prevLoop=currentLoop;
      ++currentLoop;
    }

  if( i < _nVertices[currentLoop] ) j=i+1;
  else if ( i==_nVertices[currentLoop] )
    {
      if( currentLoop==0) j=1;
      else j=_nVertices[prevLoop]+1;
    }

  return j;
}

//----------------------------------------------------------------------------
//polygon initialization;
//to find types of all polygon vertices;
//create a priority queue for all vertices;
//construct an edge set for each vertex (the set holds all edges starting from
//the vertex, only for loop searching purpose).
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::initializate()
{
  internal_poltrig::PointbaseMap::iterator it=_points.begin();
  for(; it!=_points.end(); ++it)
    {
      int id=it->first;
      int idp=prev(id);
      int idn=next(id);
      internal_poltrig::Pointbase p=*_points[id], pnext=*_points[idn], pprev=*_points[idp];

      if( p > pnext && pprev > p )
	_points[id]->type=internal_poltrig::REGULAR_DOWN;
      else if (p > pprev && pnext > p)
	_points[id]->type=internal_poltrig::REGULAR_UP;
      else
	{
	  double pa[2], pb[2], pc[2];

	  pa[0]=_points[idp]->x;
	  pa[1]=_points[idp]->y;

	  pb[0]=_points[id]->x;
	  pb[1]=_points[id]->y;

	  pc[0]=_points[idn]->x;
	  pc[1]=_points[idn]->y;

	  double area=internal_poltrig::orient2d(pa,pb,pc);

	  if( pprev > p && pnext > p ) _points[id]->type=(area >0) ? internal_poltrig::END: internal_poltrig::MERGE ;
	  if( pprev < p && pnext < p ) _points[id]->type=(area >0) ? internal_poltrig::START : internal_poltrig::SPLIT;

	}

      _qpoints.push(*(it->second));

      _startAdjEdgeMap[id].insert(id);

    }
}

//----------------------------------------------------------------------------
//Add a diagonal from point id i to j
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::addDiagonal(const unsigned int&  i, const unsigned int&  j)
{
  internal_poltrig::Type type=internal_poltrig::INSERT;
  internal_poltrig::Linebase* diag=new internal_poltrig::Linebase(_points[i], _points[j], type,l_id);
  _edges[diag->id()]=diag;

  _startAdjEdgeMap[i].insert(diag->id());
  _startAdjEdgeMap[j].insert(diag->id());

  _diagonals[diag->id()]=diag;

}

//----------------------------------------------------------------------------
//Handle start vertex
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::handleStartVertex(const unsigned int&  i)
{
  double y=_points[i]->y;
  _edgebst.InOrder(internal_poltrig::UpdateKey, y);

  _edges[i]->setHelper(i);
  _edges[i]->setKeyValue(y);
  _edgebst.Insert(_edges[i]);

}

//----------------------------------------------------------------------------
//Handle end vertex
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::handleEndVertex(const unsigned int&  i)
{
  double y=_points[i]->y;
  _edgebst.InOrder(internal_poltrig::UpdateKey, y);

  unsigned int previ=prev(i);
  internal_poltrig::Linebase* edge=_edges[previ];
  unsigned int helper=_edges[previ]->helper();


  if(_points[helper]->type==internal_poltrig::MERGE) addDiagonal(i, helper);
  _edgebst.Delete(edge->keyValue());

}

//----------------------------------------------------------------------------
//Handle split vertex
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::handleSplitVertex(const unsigned int&  i)
{
  double x=_points[i]->x, y=_points[i]->y;
  _edgebst.InOrder(internal_poltrig::UpdateKey, y);

  internal_poltrig::BTreeNode<internal_poltrig::Linebase*, double>*  leftnode;
  _edgebst.FindMaxSmallerThan(x, leftnode);
  internal_poltrig::Linebase* leftedge=leftnode->data();

  unsigned int helper=leftedge->helper();
  addDiagonal(i, helper);

  leftedge->setHelper(i);
  _edges[i]->setHelper(i);
  _edges[i]->setKeyValue(y);
  _edgebst.Insert(_edges[i]);
}

//----------------------------------------------------------------------------
//Handle merge vertex
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::handleMergeVertex(const unsigned int&  i)
{
  double x=_points[i]->x, y=_points[i]->y;
  _edgebst.InOrder(internal_poltrig::UpdateKey, y);

  unsigned int previ=prev(i);
  unsigned int helper=_edges[previ]->helper();
  if (_points[helper]->type==internal_poltrig::MERGE) addDiagonal(i, helper);
  _edgebst.Delete(_edges[previ]->keyValue());

  internal_poltrig::BTreeNode<internal_poltrig::Linebase*, double>*  leftnode;
  _edgebst.FindMaxSmallerThan(x, leftnode);
  internal_poltrig::Linebase* leftedge=leftnode->data();

  helper=leftedge->helper();
  if(_points[helper]->type==internal_poltrig::MERGE) addDiagonal(i, helper);

  leftedge->setHelper(i);
}

//----------------------------------------------------------------------------
//Handle regular down vertex
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::handleRegularVertexDown(const unsigned int&  i)
{
  double y=_points[i]->y;
  _edgebst.InOrder(internal_poltrig::UpdateKey, y);

  unsigned int previ=prev(i);
  unsigned int helper=_edges[previ]->helper();
  if(_points[helper]->type==internal_poltrig::MERGE) addDiagonal(i, helper);

  _edgebst.Delete(_edges[previ]->keyValue());
  _edges[i]->setHelper(i);
  _edges[i]->setKeyValue(y);
  _edgebst.Insert(_edges[i]);

}

//----------------------------------------------------------------------------
////Handle regular up vertex
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::handleRegularVertexUp(const unsigned int&  i)
{
  double x=_points[i]->x, y=_points[i]->y;
  _edgebst.InOrder(internal_poltrig::UpdateKey, y);

  internal_poltrig::BTreeNode<internal_poltrig::Linebase*, double>*  leftnode;
  _edgebst.FindMaxSmallerThan(x, leftnode);

  internal_poltrig::Linebase* leftedge=leftnode->data();

  unsigned int helper=leftedge->helper();
  if(_points[helper]->type==internal_poltrig::MERGE) addDiagonal(i, helper);
  leftedge->setHelper(i);

}

//----------------------------------------------------------------------------
//partition polygon to monotone polygon pieces
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::partition2Monotone()
{

  if(_qpoints.top().type!=internal_poltrig::START)
    {
      std::cout<<"Please check your input polygon:\n1)orientations?\n2)duplicated points?\n";
      exit(1);
    }

  while(!_qpoints.empty())
    {
      internal_poltrig::Pointbase vertex=_qpoints.top();
      _qpoints.pop();
      unsigned int id=vertex.id;

      switch(vertex.type)
	{
	case internal_poltrig::START:        handleStartVertex(id);       break;
	case internal_poltrig::END:          handleEndVertex(id);         break;
	case internal_poltrig::MERGE:        handleMergeVertex(id);       break;
	case internal_poltrig::SPLIT:        handleSplitVertex(id);       break;
	case internal_poltrig::REGULAR_UP:   handleRegularVertexUp(id);   break;
	case internal_poltrig::REGULAR_DOWN: handleRegularVertexDown(id); break;
	default:
	  std::cout<<"No duplicated points please!\n";
	  exit(1); break;
	}
    }
}


//----------------------------------------------------------------------------
//two Auxiliary functions to find monotone polygon pieces
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//calculate angle B for A, B, C three given points
//----------------------------------------------------------------------------
double PolygonTriangulator::Polygon::angleCosb(double *pa, double *pb, double *pc)
{
  double dxab = pa[0] - pb[0];
  double dyab = pa[1] - pb[1];

  double dxcb = pc[0] - pb[0];
  double dycb = pc[1] - pb[1];

  double dxab2 = dxab * dxab;
  double dyab2 = dyab * dyab;
  double dxcb2 = dxcb * dxcb;
  double dycb2 = dycb * dycb;
  double ab = dxab2 + dyab2;
  double cb = dxcb2 + dycb2;

  double cosb = dxab * dxcb + dyab * dycb;
  double denom = sqrt( ab * cb);

  cosb/=denom;

  return cosb;
}

//----------------------------------------------------------------------------
//for any given edge, find the next edge we should choose when searching for
//monotone polygon pieces;
//----------------------------------------------------------------------------
unsigned int PolygonTriangulator::Polygon::selectNextEdge(internal_poltrig::Linebase* edge)
{

  unsigned int eid= edge->endPoint(1)->id;
  std::set<unsigned int> edges=_startAdjEdgeMap[eid];
  assert(!edges.empty());

  unsigned int nexte=0;
  if( edges.size() == 1 )  nexte=*(edges.begin());
  else if( edges.size() > 1 )
    {
      unsigned int nexte_ccw(0), nexte_cw(0);
      double max=-2.0,min=2.0;


      std::set<unsigned int>::iterator it=edges.begin();
      for(; it!=edges.end(); ++it)
	{
	  if(*it==edge->id()) continue;
	  double A[2], B[2], C[2];
	  A[0]=edge->endPoint(0)->x;        A[1]=edge->endPoint(0)->y;
	  B[0]=edge->endPoint(1)->x;        B[1]=edge->endPoint(1)->y;

	  if(edge->endPoint(1)!=_edges[*it]->endPoint(0)) _edges[*it]->reverse();
	  C[0]=_edges[*it]->endPoint(1)->x; C[1]=_edges[*it]->endPoint(1)->y;

	  double area=internal_poltrig::orient2d(A, B, C);
	  double cosb=angleCosb(A, B, C);

	  if( area > 0 && max < cosb ) { nexte_ccw=*it; max=cosb; }
	  else if( min > cosb ) { nexte_cw=*it; min=cosb; }
	}

      nexte = (nexte_ccw!=0) ? nexte_ccw : nexte_cw;
    }

  return nexte;
}

//----------------------------------------------------------------------------
//searching all monotone pieces;
//----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::searchMonotones()
{
  int loop=0;

  internal_poltrig::LineMap edges=_edges;

  while( edges.size() > _diagonals.size() )
    {
      ++loop;
      internal_poltrig::Monopoly poly;
      internal_poltrig::LineMap::iterator it=edges.begin();
      internal_poltrig::Pointbase* startp=it->second->endPoint(0);
      internal_poltrig::Pointbase* endp=0;
      internal_poltrig::Linebase*  next=it->second;

      poly.push_back(startp->id);

      for(;;)
	{
	  endp=next->endPoint(1);
	  if(next->type()!=internal_poltrig::INSERT)
	    {
	      edges.erase(next->id());
	      _startAdjEdgeMap[next->endPoint(0)->id].erase(next->id());
	    }
	  if(endp==startp) break;
	  poly.push_back(endp->id);

	  unsigned int nexte=selectNextEdge(next);

	  if(nexte==0)
	    {
	      std::cout<<"Please check your input polygon:\n";
	      std::cout<<"1)orientations?\n2)with duplicated points?\n3)is a simple one?\n";
	      exit(1);
	    }
	  //assert( nexte > 0);
	  next=edges[nexte];
	  if(next->endPoint(0) !=endp ) next->reverse();
	}

      _mpolys.push_back(poly);
    }
}


//----------------------------------------------------------------------------
//triangulate a monotone polygon;
//----------------------------------------------------------------------------
void  PolygonTriangulator::Polygon::triangulateMonotone(internal_poltrig::Monopoly& mpoly)
{
  internal_poltrig::PQueue qvertex;
  internal_poltrig::Monopoly::iterator it=mpoly.begin(), itnext;
  for(; itnext=it, it!=mpoly.end(); ++it)
    {
      ++itnext;
      if(itnext==mpoly.end()) itnext=mpoly.begin();
      internal_poltrig::Pointbase point=*_points[*it], pointnext=*_points[*itnext];
      point.left=(point > pointnext)? true:false;
      qvertex.push(point);
    }

  std::stack<internal_poltrig::Pointbase> spoint;
  for(int i=0; i<2; ++i) { spoint.push(qvertex.top()); qvertex.pop(); }

  while ( qvertex.size() > 1 )
    {
      internal_poltrig::Pointbase topQueuePoint=qvertex.top();
      internal_poltrig::Pointbase topStackPoint=spoint.top();

      if(topQueuePoint.left!=topStackPoint.left)
	{
	  while ( spoint.size()  > 1 )
	    {
	      internal_poltrig::Pointbase p1=spoint.top();
	      spoint.pop();
	      internal_poltrig::Pointbase p2=spoint.top();
	      Triangle v(3);
	      v[0]=topQueuePoint.id;
	      v[1]=p1.id;
	      v[2]=p2.id;
	      sort(v.begin(),v.end());//TK
	      _triangles.push_back(v);

	    }
	  spoint.pop();
	  spoint.push(topStackPoint);
	  spoint.push(topQueuePoint);
	}
      else
	{
	  while( spoint.size() > 1 )
	    {
	      internal_poltrig::Pointbase stack1Point=spoint.top();
	      spoint.pop();
	      internal_poltrig::Pointbase stack2Point=spoint.top();
	      spoint.push(stack1Point);
	      double pa[2], pb[2], pc[2];
	      pa[0]=topQueuePoint.x; pa[1]=topQueuePoint.y;
	      pb[0]=stack2Point.x;   pb[1]=stack2Point.y;
	      pc[0]=stack1Point.x;   pc[1]=stack1Point.y;

	      double area=internal_poltrig::orient2d(pa,pb,pc);
	      bool   left=stack1Point.left;
	      if( (area > 0 && left) || (area < 0 && !left ) )
	    	{
		  Triangle v(3);
		  v[0]=topQueuePoint.id;
		  v[1]=stack2Point.id;
		  v[2]=stack1Point.id;
		  sort(v.begin(),v.end());//TK
		  _triangles.push_back(v);
		  spoint.pop();
	    	} else break;
	    }

	  spoint.push(topQueuePoint);

	}

      qvertex.pop();

    }

  internal_poltrig::Pointbase lastQueuePoint=qvertex.top();
  while( spoint.size() !=1 )
    {
      internal_poltrig::Pointbase topPoint=spoint.top();
      spoint.pop();
      internal_poltrig::Pointbase top2Point=spoint.top();

      Triangle v(3);
      v[0]=lastQueuePoint.id;
      v[1]=topPoint.id;
      v[2]=top2Point.id;
      sort(v.begin(),v.end());//TK
      _triangles.push_back(v);
    }
}

//----------------------------------------------------------------------------
//main triangulation function;
////----------------------------------------------------------------------------
void PolygonTriangulator::Polygon::triangulation()
{
  partition2Monotone();
  searchMonotones();
  internal_poltrig::Monopolys::iterator it=_mpolys.begin();
  for(; it!=_mpolys.end(); ++it)
    triangulateMonotone(*it);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

PolygonTriangulator::PolygonTriangulator(const std::vector<double>& polygon_xcoords,
					 const std::vector<double>& polygon_ycoords)
  : polygon(new Polygon(polygon_xcoords,polygon_ycoords))
{
  polygon->triangulation();
}

PolygonTriangulator::~PolygonTriangulator() { delete polygon; }

const PolygonTriangulator::Triangles* PolygonTriangulator::triangles() const
{
  return polygon->triangles();
}


