
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
//     (see copyright notice in .src file)                           //
//                                                                   //
//     Main changes performed for the present incarnation are        //
//     interface & namespace changes, indentation, small             //
//     performance tweaks and removal of unused features.            //
//     Most importantly, got rid of static and globals so the class  //
//     can be reliably used more than once per process. Also, the    //
//     output triangles are sorted to preserve "handedness".         //
//                                                                   //
///////////////////////////////////////////////////////////////////////


#ifndef POLYGONTRIANGULATOR_H
#define POLYGONTRIANGULATOR_H

#include <vector>
#include <list>

class PolygonTriangulator {
public:

  typedef std::vector<unsigned> Triangle;
  typedef std::list<Triangle> Triangles;

  //When constructed it automatically performs the triangulation.
  PolygonTriangulator(const std::vector<double>& polygon_xcoords,
		      const std::vector<double>& polygon_ycoords);


  // Access the result with this
  const Triangles* triangles() const;

  // Output "handedness" (clockwise or anticlockwise order) is the
  // same for all of the triangles as it were for the input points.

  //NB: If triangles().size()==0, something went wrong.

  ~PolygonTriangulator();

private:
  class Polygon;
  Polygon * polygon;
};

#endif
