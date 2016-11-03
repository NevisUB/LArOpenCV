#ifndef __DEFECTVERTEX_CXX__
#define __DEFECTVERTEX_CXX__

#include "DefectCluster.h"
#include "DefectVertex.h"
#include "PCACandidates.h"

#include "Core/Circle.h"

namespace larocv {

  /// Global larocv::DefectVertexFactory to register ClusterAlgoFactory
  static DefectVertexFactory __global_DefectVertexFactory__;

  void DefectVertex::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const defect_cluster_algo_name = pset.get<std::string>("DefectClusterAlgo","defbreak1");
    _defect_cluster_algo_id = this->ID(defect_cluster_algo_name);
  }

  cv::Rect DefectVertex::edge_aware_box(const cv::Mat& img,geo2d::Vector<float> center,int hwidth,int hheight)
  {
    geo2d::Vector<int> tl(center.x-hwidth,center.y-hheight);

    int width  = 2*hwidth;
    int height = 2*hheight;
    
    if (tl.x < 0) tl.x = 0;
    if (tl.y < 0) tl.y = 0;

    if (tl.x > img.cols-1) tl.x = img.cols-1;
    if (tl.y > img.rows-1) tl.y = img.rows-1;

    if ((tl.x+width ) > (img.cols-1)) tl.x=(img.cols-1 - width);
    if ((tl.y+height) > (img.rows-1)) tl.y=(img.rows-1 - height);
    
    return cv::Rect(tl.x,tl.y,width,height);
  }
  
  void DefectVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       larocv::ImageMeta& meta,
			       larocv::ROI& roi) {
    
    //const auto& pcacandidates_data = AlgoData<PCACandidatesData>(ID()-1);
    //const auto& ctor_lines_v   = pcacandidates_data._ctor_lines_v_v[meta.plane()];

    const auto& defectcluster_data = AlgoData<DefectClusterData>(_defect_cluster_algo_id);
    const auto& atomic_defect_pts_v_v  = defectcluster_data._atomic_defect_pts_v_v_v[meta.plane()];

    //count the number of defects
    int n=0;
    for(const auto& atomic_defect_pts_v : atomic_defect_pts_v_v)
      for(const auto& atomic_defect : atomic_defect_pts_v)
	n++;
    
    LAROCV_DEBUG() << "Plane: " << meta.plane() << std::endl;
    LAROCV_DEBUG() << "See: " << n << " defects" << std::endl;
    
    //circle, and "score"
    std::vector<std::pair<geo2d::Circle<float>,float> > circle_score_p_v;
    circle_score_p_v.resize(n);

    n = -1;
    float radius_size=10;
    
    //for each track cluster
    for(const auto& atomic_defect_pts_v : atomic_defect_pts_v_v) {
      //for each atomic defect in this track cluster
      for(const auto& atomic_defect : atomic_defect_pts_v) {
	n++;
	LAROCV_DEBUG() << "Defect point is " << atomic_defect << std::endl;

	float score=-1.;
	
	LAROCV_DEBUG() << "Radius : " << radius_size << "... current score: " << score << std::endl;
	
	geo2d::Circle<float> defect_circle(atomic_defect,radius_size);
	
	auto small_bbox = edge_aware_box(img,atomic_defect,radius_size,radius_size);
	  
	LAROCV_DEBUG() << "Small bbox calculated: " << small_bbox << std::endl;
	LAROCV_DEBUG() << "Image dimensions : [" << img.rows << "," << img.cols << "]" << std::endl;
	
	auto small_img  = ::cv::Mat(img,small_bbox);
	cv::Mat thresh_small_img;
	cv::threshold(small_img,thresh_small_img,10,1,CV_THRESH_BINARY);
	
	geo2d::VectorArray<int> points;
	findNonZero(thresh_small_img, points);

	float mean_x=0;
	float mean_y=0;
	for(auto& pt : points) {
	  mean_x += pt.x;
	  mean_y += pt.y;
	}

	mean_x/=points.size();
	mean_y/=points.size();

	float std_xy = 0.0;	
	float std_x  = 0.0;
	float std_y  = 0.0;

	for(auto& pt : points) {
	  std_x  += (pt.x - mean_x)*(pt.x - mean_x);
	  std_y  += (pt.y - mean_y)*(pt.y - mean_y);
	  std_xy += (pt.y - mean_y)*(pt.x - mean_x);
	}

	
	float r = std_xy / ( sqrt(std_x) * sqrt(std_y) );

	std_x /= points.size();
	std_y /= points.size();

	if (std_x < 1) r=1.0;
	if (std_y < 1) r=1.0;
	
	r=std::abs(r);
	LAROCV_DEBUG() << "Calculated correlation: " << r << " from " << points.size() << " points inside rectangle\n";
	LAROCV_DEBUG() << "mean x: " << mean_x << std::endl;
	LAROCV_DEBUG() << "mean y: " << mean_y << std::endl;
	LAROCV_DEBUG() << "cov(xy): " << r / points.size() << std::endl;
	LAROCV_DEBUG() << "std_x :  " << std_x << std::endl;
	LAROCV_DEBUG() << "std_y :  " << std_y << std::endl;
	circle_score_p_v[n].first=defect_circle;
	circle_score_p_v[n].second=r;

	// ContourArray_t ctor_v;    
	// std::vector<::cv::Vec4i> cv_hierarchy_v;
	// ctor_v.clear(); cv_hierarchy_v.clear();
	
	// cv::findContours(thresh_small_img,
	// 		 ctor_v,
	// 		 cv_hierarchy_v,
	// 		 CV_RETR_EXTERNAL,
	// 		 CV_CHAIN_APPROX_SIMPLE);
	
	// //get the largest contour
	// Contour_t* ctor;
	// uint s=0;
	// for(auto& ct : ctor_v){
	//   if (ct.size()>s)
	//     { s=ct.size(); ctor=&ct; }
	// }

	// LAROCV_DEBUG() << "Got largest ctor size: " << ctor->size() << std::endl;
	// mean_x=0;
	// mean_y=0;
	// std::cout << "[";
	// for(auto& pt : *ctor) {
	//   mean_x+=pt.x;
	//   mean_y+=pt.y;
	//   std::cout << "[" << pt.x << "," << pt.y << "],";
	// }
	// std::cout << "]" << std::endl;
	
	// mean_x/=ctor->size();
	// mean_y/=ctor->size();

	// std_xy = 0.0;	
	// std_x  = 0.0;
	// std_y  = 0.0;

	// for(auto& pt : *ctor) {
	//   std_x  += (pt.x - mean_x)*(pt.x - mean_x);
	//   std_y  += (pt.y - mean_y)*(pt.y - mean_y);
	//   std_xy += (pt.y - mean_y)*(pt.x - mean_x);
	// }
	
	// r = std_xy / ( sqrt(std_x) * sqrt(std_y) );

	// r = std::abs(r);
	// LAROCV_DEBUG() << "Calculated CONTOUR ncorrelation: " << r << " from " << ctor->size() << " points inside rectangle\n";
	// LAROCV_DEBUG() << "CONTOUR mean x: " << mean_x << std::endl;
	// LAROCV_DEBUG() << "CONTOUR mean y: " << mean_y << std::endl;
	// LAROCV_DEBUG() << "CONTOUR cov(xy): " << r / ctor->size() << std::endl;
	// LAROCV_DEBUG() << "CONTOUR std_x :  " << std_x / ctor->size() << std::endl;
	// LAROCV_DEBUG() << "CONTOUR std_y :  " << std_y / ctor->size() << std::endl;
      }
    }

    auto& data = AlgoData<DefectVertexData>();
    auto& circle_data_v = data._circledata_v_v[meta.plane()];
    circle_data_v.resize(1);
    
    float min_score=10.;
    
    for(const auto& circle_score_p : circle_score_p_v) {
      const auto score = circle_score_p.second;
      LAROCV_DEBUG() << "See score : " << score << "... min_score : " << min_score << std::endl;
      if (score < min_score){
	LAROCV_DEBUG() << "Smaller!" << std::endl;
	min_score=score;
	circle_data_v[0] = circle_score_p.first;
      }
    }
  }
  
  bool DefectVertex::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  { return true; }
}
#endif
