#ifndef __CLUSTERPARAMS_H__
#define __CLUSTERPARAMS_H__

#include <iostream>
#include <cmath>
#include <vector>
#include <utility>

#include "Core/ImageMeta.h"
#include "Core/laropencv_base.h"

#include "PCABox.h"

namespace larcv {
  
  class ClusterParams {

  public:

    ClusterParams() {}
    
    ClusterParams(int    in_index,
		  double eval1,
		  double eval2,
		  double area,
		  double perimeter,
		  double eigendirx,
		  double eigendiry,
		  int nhits,
		  std::vector<std::pair<int,int> > hits,
		  std::vector<double> line,
		  std::vector<int> charge,
		  std::vector<PCABox> boxes,
		  std::map<int,std::vector<int> > combined,
		  int chosen) :
      
      in_index_(in_index),
      eval1_(eval1),
      eval2_(eval2),
      area_(area),
      perimeter_(perimeter),
      eigendirx_(eigendirx),
      eigendiry_(eigendiry),
      nhits_(nhits),
      hits_(hits),
      line_(line),
      charge_(charge),
      boxes_(boxes),
      combined_(combined),
      chosen_(chosen)
    {}      

    ~ClusterParams(){}

    void compare(const ClusterParams& cparm2);
    
    int    in_index_;
    double eval1_;
    double eval2_;
    double area_;
    double perimeter_;
    double eigendirx_;
    double eigendiry_;

    int chosen_;
    
    int nhits_;
    std::vector<std::pair<int,int> > hits_;
    std::vector<double> line_;
    std::vector<int> charge_;

    std::vector<PCABox> boxes_;
    std::map<int,std::vector<int> > combined_;
  };
}
#endif
