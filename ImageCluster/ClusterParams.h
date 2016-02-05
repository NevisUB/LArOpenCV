#ifndef __CLUSTERPARAMS_H__
#define __CLUSTERPARAMS_H__

namespace larcv {
  
  class ClusterParams {

  public:

    ClusterParams() {}
    
    ClusterParams(int    in_index,
		  double trunk_length,
		  double trunk_cov,
		  double eval1,
		  double eval2,
		  double area,
		  double perimeter,
		  double startx,
		  double starty,
		  double eigendirx,
		  double eigendiry) :
      
      in_index_(in_index),
      trunk_length_(trunk_length),
      trunk_cov_(trunk_cov),
      eval1_(eval1),
      eval2_(eval2),
      area_(area),
      perimeter_(perimeter),
      startx_(startx),
      starty_(starty),
      eigendirx_(eigendirx),
      eigendiry_(eigendiry)
    {}      

    ~ClusterParams(){}
    
    int    in_index_;
    double trunk_length_;
    double trunk_cov_;
    double eval1_;
    double eval2_;
    double area_;
    double perimeter_;
    double startx_;
    double starty_;
    double eigendirx_;
    double eigendiry_;
    
  };
}
#endif
