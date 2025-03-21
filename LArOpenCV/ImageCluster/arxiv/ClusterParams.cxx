#ifndef __CLUSTERPARAMS_CXX__
#define __CLUSTERPARAMS_CXX__

#include "ClusterParams.h"
namespace larcv {

  void ClusterParams::compare(const ClusterParams& cparm2) {
  
  
    std::cout << "\t====================\n"
      // std::cout << "\t Closeness is: " << std::sqrt( std::pow( this->startx_ - cparm2.startx_,2) +
      // 						   std::pow( this->starty_ - cparm2.starty_,2) ) << "\n"

	      << "\t area: 1:"       <<  this->area_  << " 2:" << cparm2.area_ << "\n"
	      << "\t perimeter: 1:"  <<  this->perimeter_  << " 2:" << cparm2.perimeter_ << "\n"
	      << "\t nhits 1:"       <<  this->nhits_ << " 2:" << cparm2.nhits_ << "\n"
	      << "\t eigen 1: "      <<  this->eval1_ << " 2:" << cparm2.eval1_ << "\n"
	      << "\t eigen 2: "      <<  this->eval2_ << " 2:" << cparm2.eval2_ << "\n"
	      << "\t angle between: " <<  std::acos( (this->eigendirx_*cparm2.eigendirx_ + this->eigendiry_*cparm2.eigendiry_ )/
						     ( std::sqrt(std::pow(this->eigendirx_ ,2) + std::pow(this->eigendiry_ ,2)) *
						       std::sqrt(std::pow(cparm2.eigendirx_,2) + std::pow(cparm2.eigendiry_,2)) ) ) * 180/3.14 << " \n";
	      

    // r1(t) = (x1,y1) + (R1X,R1Y)*t;
    // r2(t) = (x2,y2) + (R2X,R2Y)*t;

    // x1 + R1X*t == x2 + R2X*t;
    // t = (x1 - x2) / (R1X - R2X);
    
    // x_inter = ((x2 - x1) / (R1X - R2X) )* R1X + x1;
    // y_inter = ((x2 - x1) / (R1X - R2X) )* R1Y + y1;
    
    // auto xinter = ( ( cparm2.startx_ - this->startx_) / ( this->eigendirx_ - cparm2.eigendirx_ ) ) * this->eigendirx_ + this->startx_;
    // auto yinter = ( ( cparm2.startx_ - this->startx_) / ( this->eigendirx_ - cparm2.eigendirx_ ) ) * this->eigendiry_ + this->starty_;
    
    // auto d1 = std::sqrt( std::pow(xinter - this->startx_ ,2) + std::pow(yinter - this->starty_ ,2) );
    // auto d2 = std::sqrt( std::pow(xinter - cparm2.startx_,2) + std::pow(yinter - cparm2.starty_,2) );
    
    // std::cout << "\t intersection point is: ("  <<  xinter << "," << yinter << ")\n";
    // std::cout << "\t distance from start 1: " << d1 << " 2: " << d2 << "\n";
    // std::cout << "\t====================\n";

  }
}
#endif
