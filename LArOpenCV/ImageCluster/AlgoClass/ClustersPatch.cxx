#ifndef CLUSTERSPATCH_CXX
#define CLUSTERSPATCH_CXX

#include "ClustersPatch.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "Geo2D/Core/spoon.h"
#include "Geo2D/Core/Geo2D.h"

namespace larocv {


  void ClustersPatch::Configure(const Config_t& pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    
    _ctor_pixels_minimum = pset.get<int>("ContourPixelsMinimum");
    
  }
  
  cv::Mat ClustersPatch::AnglePatch(const cv::Mat img){
    
    auto res_img = img.clone();
    
    res_img = Threshold(res_img, 10.0, 255);
    
    if(this->logger().level() == ::larocv::msg::kDEBUG) {
      std::stringstream ss0;
      ss0 << "raw_img"<<".png";
      cv::imwrite(std::string(ss0.str()).c_str(), img);
    }
    
    GEO2D_ContourArray_t raw_img_ctors;
    raw_img_ctors = FindContours(res_img);
    
    LAROCV_DEBUG()<<"Found raw contours "<<raw_img_ctors.size()<<std::endl;  

    GEO2D_ContourArray_t img_ctors;
    img_ctors.clear();
    for(auto ctor : raw_img_ctors) {
      if (ctor.size()> _ctor_pixels_minimum) img_ctors.push_back(ctor);
    }
    
    
    LAROCV_DEBUG()<<"   Found contours "<<img_ctors.size()<<std::endl;  
    
    if( !img_ctors.size() ) return res_img;
    
    std::vector<bool> used_ctor_v;
    used_ctor_v.clear();
    used_ctor_v.resize(img_ctors.size(), false);
    
    for(size_t ctor_idx_this = 0; ctor_idx_this < img_ctors.size(); ++ctor_idx_this){

      if (used_ctor_v[ctor_idx_this]) continue;
      
      used_ctor_v[ctor_idx_this] = true;
      
      LAROCV_DEBUG()<<"ctor_this id "<<ctor_idx_this<<std::endl;

      geo2d::Vector<float> edge1, edge2;
      
      double angle_this = ClusterEdgesAngle(res_img, img_ctors[ctor_idx_this], edge1, edge2);
      
      LAROCV_DEBUG()<<"angle_this "<<angle_this<<std::endl;
      
      for(size_t ctor_idx_that = 0; ctor_idx_that < img_ctors.size(); ++ctor_idx_that){
	
	if (used_ctor_v[ctor_idx_that]) continue;


	geo2d::Vector<float> edge3, edge4;
	double angle_that = ClusterEdgesAngle(res_img, img_ctors[ctor_idx_that], edge3, edge4);
	
	if (std::abs(angle_this - angle_that) < 5){
	  used_ctor_v[ctor_idx_that] = true;	  
	  LAROCV_DEBUG()<<"found 1 "<<std::endl;
	  LAROCV_DEBUG()<<"ctor that id "<<ctor_idx_that<<std::endl;
	  LAROCV_DEBUG()<<"angle_that "<<angle_that<<std::endl;
	  
	  if (edge2.y < edge3.y)
	    res_img = _DeadWirePatch.FillImageGap(img, edge2, edge3, 
						  img_ctors, ctor_idx_this, ctor_idx_that);
	  
	  if (edge4.y < edge1.y)
	    res_img = _DeadWirePatch.FillImageGap(img, edge4, edge1, 
						  img_ctors, ctor_idx_this, ctor_idx_that);
	  
	}
      }
    }
    return img; 
  }

  double ClustersPatch::ClusterEdgesAngle(const cv::Mat img, 
					  const GEO2D_Contour_t ctor,
					  geo2d::Vector<float>& edge1, 
					  geo2d::Vector<float>& edge2
					  ){
    
    double res;
    
    cv::Mat masked_img = MaskImage(img, ctor,0, false);
    
    //Find edge point for angle calculation
    
    FindEdges(masked_img, edge1, edge2);

    LAROCV_DEBUG()<<"edge1.x "<<edge1.x<<" edge1.y "<<edge1.y<<std::endl;
    LAROCV_DEBUG()<<"edge2.x "<<edge2.x<<" edge2.y "<<edge2.y<<std::endl;      
    double angle1;
    double angle2;
    
    _CalcAngle.AngleWithCircleResolution(angle1, img, ctor, edge1);
    _CalcAngle.AngleWithCircleResolution(angle2, img, ctor, edge2);  

    res = (angle1+angle2-180)/2. ;    
    return res;
  }
}
#endif
