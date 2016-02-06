#ifndef __CLUSTERPARAMS_H__
#define __CLUSTERPARAMS_H__

#include <iostream>
#include <cmath>
#include <vector>
#include <utility>

#include "Core/ImageMeta.h"
#include "Core/laropencv_base.h"


namespace larcv {
  
  class ClusterParams {

  public:

    ClusterParams() {}
    
    ClusterParams(int    in_index,
		  double trunk_length,
		  double trunk_cov,
		  double shower_len,
		  double eval1,
		  double eval2,
		  double area,
		  double perimeter,
		  double startx,
		  double starty,
		  double eigendirx,
		  double eigendiry,
		  int nhits,
		  std::vector<std::pair<int,int> > hits,
		  std::vector<double> line,
		  std::vector<int> charge,
		  std::vector<::cv::Rect> rect,
		  std::vector<std::vector<double> > llines,
		  std::vector<double> meanline) :
      
      in_index_(in_index),
      trunk_length_(trunk_length),
      trunk_cov_(trunk_cov),
      shower_len_(shower_len),
      eval1_(eval1),
      eval2_(eval2),
      area_(area),
      perimeter_(perimeter),
      startx_(startx),
      starty_(starty),
      eigendirx_(eigendirx),
      eigendiry_(eigendiry),
      nhits_(nhits),
      hits_(hits),
      line_(line),
      charge_(charge),
      rect_(rect),
      llines_(llines),
      meanline_(meanline)
      
    {}      

    ~ClusterParams(){}

    void compare(const ClusterParams& cparm2);
    
    int    in_index_;
    double trunk_length_;
    double trunk_cov_;
    double shower_len_;
    double eval1_;
    double eval2_;
    double area_;
    double perimeter_;
    double startx_;
    double starty_;
    double eigendirx_;
    double eigendiry_;

    int    nhits_;
    std::vector<std::pair<int,int> > hits_;
    std::vector<double> line_;
    std::vector<int> charge_;
    std::vector<::cv::Rect> rect_;
    std::vector<std::vector<double> > llines_;
    std::vector<double> meanline_;
  };
}
#endif
