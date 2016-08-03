//by vic

#ifndef VERTEX2D_H
#define VERTEX2D_H

#include "Cluster2D.h"

namespace larocv{
  
  class Vertex2D : public std::pair<const Cluster2D*,const Cluster2D*> {
  
  public:
    
    Vertex2D(){}

    Vertex2D(double d1,
	     double d2,
	     Point2D vtx,
	     int i1,
	     int i2) :
      _d1(d1),
      _d2(d2),
      _vtx(vtx),
      _i1(i1),
      _i2(i2)
    {}
    
    ~Vertex2D(){}
    
    double _d1; // distance from vertex to c1
    double _d2; // distance from vertex to c2

    int _i1; //index of cluster 1 in clusters
    int _i2; //index of cluster 2 in clusters
    
    Point2D _vtx; //vertex point in unknown coordinate system
    
  };
}
#endif
/** @} */ // end of doxygen group 

