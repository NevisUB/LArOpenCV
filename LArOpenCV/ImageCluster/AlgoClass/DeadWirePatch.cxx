#ifndef DEADWIREPATCH_CXX
#define DEADWIREPATCH_CXX

#include "DeadWirePatch.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "Geo2D/Core/spoon.h"
#include "Geo2D/Core/Geo2D.h"

#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#endif
#endif
#include <set>
#include <cassert>

namespace larocv {


  void DeadWirePatch::Configure(const Config_t& pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));

    _bandaid = pset.get<bool>("Bandaid");
    _bondage = pset.get<bool>("Bondage");
    
    if (_bandaid && _bondage) throw larbys("Can't specify bandaid & bondage");
  }

  cv::Mat DeadWirePatch::Patch(const cv::Mat&img,
			       const cv::Mat&dead_ch_img) {

    if (_bandaid) return WireBandaid(img,dead_ch_img);
    if (_bondage) return WireBondage(img,dead_ch_img);
    throw larbys("Requested to patch, but how?");
  }
  
  cv::Mat DeadWirePatch::MakeDeadImage(const cv::Mat& img) {
    auto dead_img = Threshold(img,1.0,255);
    cv::bitwise_not(dead_img,dead_img);
    return dead_img;
  }
  
  void DeadWirePatch::FindWireEdges (const GEO2D_Contour_t& ctor,
				     geo2d::Vector<float>& edge_low,
				     geo2d::Vector<float>& edge_high){
    
    int wire_low  = kINVALID_INT;
    int wire_high = -1 * kINVALID_INT;
    
    for (const auto& pt : ctor){
      if ( pt.y <= wire_low  )	wire_low  = pt.y;
      if ( pt.y >= wire_high )  wire_high = pt.y;
    }
    
    float sum_low_x = 0 , sum_high_x = 0;
    float ctr_low_x = 0 , ctr_high_x = 0;

    for (const auto& pt : ctor){
      if (pt.y == wire_low  ) {
	sum_low_x += pt.x; 
	ctr_low_x += 1;
      }
      if (pt.y == wire_high ) {
	sum_high_x += pt.x; 
	ctr_high_x += 1;
      }
    }

    edge_low.y  = wire_low;
    edge_low.x  = sum_low_x / ctr_low_x;
    
    edge_high.y = wire_high;
    edge_high.x = sum_high_x / ctr_high_x;
  }

  void
  DeadWirePatch::FindWireEdges (const cv::Mat& img,
				geo2d::Vector<float>& edge_low,
				geo2d::Vector<float>& edge_high){
  
    int wire_low  = kINVALID_INT;
    int wire_high = -1 * kINVALID_INT;
    
    auto pts_v = FindNonZero(img);
    
    for(const auto& pt : pts_v) {
      if ( pt.y <= wire_low  )	wire_low  = pt.y;
      if ( pt.y >= wire_high )  wire_high = pt.y;      
    }
    
    float sum_low_x = 0 , sum_high_x = 0;
    int   ctr_low_x = 0 , ctr_high_x = 0;
    
    for (auto pt : pts_v){
      if (pt.y == wire_low  ) {
	sum_low_x += pt.x; 
	ctr_low_x += 1;
      }
      if (pt.y == wire_high ) {
	sum_high_x += pt.x; 
	ctr_high_x += 1;
      }
    }

    edge_low.y  = wire_low;
    edge_low.x  = sum_low_x / ctr_low_x;
    
    edge_high.y = wire_high;
    edge_high.x = sum_high_x / ctr_high_x;
    
  }

  cv::Mat DeadWirePatch::WireBandaid(const cv::Mat& img,
				     const cv::Mat& dead_ch_img) {
    
    //////////////Rui Below
    // Get Dead Contours
    auto res_img = img.clone();
    auto dead_img = MakeDeadImage(dead_ch_img);
    GEO2D_ContourArray_t dead_ctors;
    dead_ctors = FindContours(dead_img);
      
    // Note: 
    // pt.y here is the wire #
      
    std::vector<std::pair<int, int>> dead_regions;
    dead_regions.clear();
    //dead_regions.resize(dead_ctors.size());
      
    for(auto each : dead_ctors){
      std::pair<int, int> dead_wires;
      int y1 = kINVALID_INT;
      int y2 = -1 * kINVALID_INT;
	
      auto pts = FindNonZero(MaskImage(dead_img, each, 0, false));
	
      for (auto pt : pts){
	if ( pt.y < y1 ) y1 = pt.y;
	if ( pt.y > y2 ) y2 = pt.y;
      }
		
      dead_wires = y2 > y1 ? std::make_pair(y1, y2) : std::make_pair(y2, y1);
      dead_regions.push_back(dead_wires);
    }
      
    // Get Super Contours 
    GEO2D_ContourArray_t ctor_v;
    _SuperClusterer.CreateSuperCluster(res_img,ctor_v);

    // Store wire edges of ctors for checking if touches dead regions
    std::vector<std::pair<geo2d::Vector<float>, geo2d::Vector<float>>> wire_edges_v;
    wire_edges_v.clear();
      
    for (size_t idx = 0; idx< ctor_v.size(); ++idx){
      geo2d::Vector<float> pt_low; 
      geo2d::Vector<float> pt_high;
	
      //if (ctor_v[idx].size() < 20) continue;
      FindWireEdges(MaskImage(res_img, ctor_v[idx], 0, false), pt_low, pt_high);
      //FindWireEdges(ctor_v[idx], pt_low, pt_high);
	
      auto wire_edges = std::make_pair(pt_low, pt_high);
      wire_edges_v.push_back(wire_edges);
    }

    GEO2D_ContourArray_t test_ctor_v;
    test_ctor_v.clear();
    std::vector<int> used_ctor_idx;
    used_ctor_idx.clear();
      
    for (size_t wire_edge_idx1 = 0; wire_edge_idx1 < wire_edges_v.size(); ++wire_edge_idx1){
      if (ctor_v[wire_edge_idx1].size() < 20) continue;
      auto ctor1_low  = wire_edges_v[wire_edge_idx1].first;
      auto ctor1_high = wire_edges_v[wire_edge_idx1].second;
	
      for (auto this_dead_region : dead_regions){
	auto this_dead_low  = this_dead_region.first;
	auto this_dead_high = this_dead_region.second;

	if ((ctor1_low.y - this_dead_high == 1)) {
	  for (size_t wire_edge_idx2 = 0; wire_edge_idx2 < wire_edges_v.size(); ++wire_edge_idx2){
	    if (ctor_v[wire_edge_idx2].size() < 20) continue;
	    if(wire_edge_idx1 == wire_edge_idx2 ) continue;
	      
	    auto ctor2_low  = wire_edges_v[wire_edge_idx2].first;
	    auto ctor2_high = wire_edges_v[wire_edge_idx2].second;
	    if (this_dead_low - ctor2_high.y == 1 ) {
	      used_ctor_idx.push_back(wire_edge_idx1);
	      used_ctor_idx.push_back(wire_edge_idx2);
		
	      auto ls = geo2d::LineSegment<float>(ctor1_low, ctor2_high);
		
	      GEO2D_Contour_t ls_ctor;
	      ls_ctor.clear();
		
	      auto counts_1 = CountNonZero(MaskImage(res_img,ctor_v[wire_edge_idx1],0, false));
	      auto counts_2 = CountNonZero(MaskImage(res_img,ctor_v[wire_edge_idx2],0, false));
		
	      auto Qsum_1 = SumNonZero(MaskImage(res_img,ctor_v[wire_edge_idx1],0, false));
	      auto Qsum_2 = SumNonZero(MaskImage(res_img,ctor_v[wire_edge_idx2],0, false));

	      auto filling = (Qsum_1+Qsum_2) / (counts_1 + counts_2);
		
	      for (size_t ls_idx = ls.pt2.y; ls_idx <= ls.pt1.y ; ++ls_idx){
		  
		geo2d::Vector<int> pt(ls.x(ls_idx), ls_idx);
		ls_ctor.push_back(pt);
		  
		res_img.at<uchar>(pt.y, pt.x) = filling;
		    
		// { 
		//   //if(this->logger().level() == ::larocv::msg::kDEBUG) {
		//   std::stringstream ss0;
		//   ss0 << "img_tmp_ls_added_"<<plane<<".png";
		//   cv::imwrite(std::string(ss0.str()).c_str(), img);
		// }

	      }

	      GEO2D_ContourArray_t tmp;
	      tmp.resize(3);
	      tmp[0] = ctor_v[wire_edge_idx1];
	      tmp[1] = ctor_v[wire_edge_idx2];
	      tmp[2] = ls_ctor;

	      // { 
	      // 	//if(this->logger().level() == ::larocv::msg::kDEBUG) {
	      // 	auto img = dead_img;
	      // 	auto img_rest = MaskImage(img, tmp, 0, false);
	      // 	std::stringstream ss0;
	      // 	ss0 << "img_tmp_dead_"<<plane<<".png";
	      // 	cv::imwrite(std::string(ss0.str()).c_str(), img_rest);
	      // }
	      // { 
	      // 	//if(this->logger().level() == ::larocv::msg::kDEBUG) {
	      // 	//auto img = dead_img;
	      // 	auto img = img_v[img_idx];
	      // 	auto img_rest = MaskImage(img, tmp, 0, false);
	      // 	std::stringstream ss0;
	      // 	ss0 << "img_tmp_"<<plane<<".png";
	      // 	cv::imwrite(std::string(ss0.str()).c_str(), img_rest);
	      // }
	      //auto merge_tmp = MergeByMask(tmp[0], tmp[1], img);
	      //auto test_ctor = Merge(merge_tmp, tmp[2]);
	      //test_ctor_v.push_back(test_ctor);
	    }
	  }
	} 
      }
    }
    ////////////Rui above
    return res_img;
  }
 

  std::vector<std::pair<int,int> > 
  DeadWirePatch::GenDeadRows(const cv::Mat& dead_ch_img) {

    //get the row ranges for dead blocks
    
    std::vector<std::pair<int,int> > dead_row_v;
    int dstart = kINVALID_INT;
    int dend   = kINVALID_INT;
    int prev_dead = 0;
    for(int drow = 0; drow < dead_ch_img.rows; ++drow) {
      int dead = (int)dead_ch_img.at<uchar>(drow,0);
      if(!dead) {
	if (dend != kINVALID_INT) {
	  dead_row_v.emplace_back(dstart,dend);
	  dstart = dend = kINVALID_INT;
	}
	continue;
      }

      if (dstart == kINVALID_INT) dstart = drow;
      dend = drow;
    }
    
    if (dend != kINVALID_INT)
      dead_row_v.emplace_back(dstart,dend);

    for(auto& dead_row : dead_row_v) {
      if (dead_row.first)   
	dead_row.first -= 1;

      if (dead_row.second<(dead_ch_img.rows-1))  
	dead_row.second += 1;
    }

    return dead_row_v;

  }

  std::vector<DeadWireChunk>
  DeadWirePatch::ScanEdgePixels(const cv::Mat& img,
				const std::vector<std::pair<int,int> >& dead_wire_v) {


    std::vector<DeadWireChunk> edge_dwc_v;
    edge_dwc_v.resize(dead_wire_v.size());

    for(int col=0; col < img.cols; ++col) {
      for(size_t did = 0; did < dead_wire_v.size(); ++did) {
	auto& edge_dwc = edge_dwc_v[did];
	
	uint upper = (uint)img.at<uchar>(dead_wire_v[did].first,col);
	uint lower = (uint)img.at<uchar>(dead_wire_v[did].second,col);

	if(upper) edge_dwc.upper_pt_v.emplace_back(col,dead_wire_v[did].first);
	if(lower) edge_dwc.lower_pt_v.emplace_back(col,dead_wire_v[did].second);
      }
    }
    
    return edge_dwc_v;
  }

  cv::Mat DeadWirePatch::WireBondage(const cv::Mat& img,
				     const cv::Mat& dead_ch_img) {
    
    auto res_img = img.clone();
    auto timg = Threshold(img,10,255);

    auto tmp_ctor_v = FindContours(timg);
    GEO2D_ContourArray_t ctor_v;
    ctor_v.reserve(tmp_ctor_v.size());

    for(auto& ctor : tmp_ctor_v) {
      if (larocv::ContourArea(ctor) < 20) continue;
      ctor_v.emplace_back(std::move(ctor));
    }

    auto dead_img    = MakeDeadImage(dead_ch_img);
    auto dead_wire_v = GenDeadRows(dead_img);

    for(auto& dead_wire : dead_wire_v) 
      LAROCV_DEBUG() << "[" << dead_wire.first << "," << dead_wire.second << "]" << std::endl;

    auto edge_dwc_v  = ScanEdgePixels(timg,dead_wire_v);
    

    for(size_t dwc_id=0; dwc_id < edge_dwc_v.size(); ++dwc_id) {
      LAROCV_DEBUG() << "@dwc_id=" << dwc_id << std::endl;
      auto& edge_dwc = edge_dwc_v[dwc_id];
      
      
      std::vector<DeadEdgePatch> upper_patch_v;
      std::vector<DeadEdgePatch> lower_patch_v;
      
      int prev_x = kINVALID_INT;
      for(size_t uid=0; uid < edge_dwc.upper_pt_v.size(); ++uid) {

	auto& pt = edge_dwc.upper_pt_v[uid];
	LAROCV_DEBUG() << "@uid=" << uid << " pt=" << pt << std::endl;
	
	auto id = larocv::FindContainingContour(ctor_v,pt);
	if (id == kINVALID_SIZE) continue;
	if (prev_x != (int)pt.x) {
	  DeadEdgePatch dep;
	  upper_patch_v.emplace_back(std::move(dep));
	}
	prev_x = (int)pt.x;
	upper_patch_v.back().super_id = id;
	upper_patch_v.back().pt_v.emplace_back(std::move(pt));
      }
      
      prev_x = kINVALID_INT;
      for(size_t lid=0; lid < edge_dwc.lower_pt_v.size(); ++lid) {
	auto& pt = edge_dwc.lower_pt_v[lid];
	auto id = larocv::FindContainingContour(ctor_v,pt);
	if (id == kINVALID_SIZE) continue;
	if (prev_x != (int)pt.x) {
	  DeadEdgePatch dep;
	  lower_patch_v.emplace_back(std::move(dep));
	}
	prev_x = (int)pt.x;
	lower_patch_v.back().super_id = id;
	lower_patch_v.back().pt_v.emplace_back(std::move(pt));
      }
      
      std::vector<DeadEdgePatch> upper_v;
      std::vector<DeadEdgePatch> lower_v;

      for(size_t uid =0; uid < upper_patch_v.size(); ++uid) {
	if (upper_patch_v[uid].pt_v.empty()) continue;
	auto& patch = upper_patch_v[uid];
	patch.init();
	auto ctor_img = MaskImage(img,ctor_v[patch.super_id],0,false);
	ctor_img = MaskImage(ctor_img,patch.circle,0,false);
	try {
	  patch.local_pca = CalcPCA(ctor_img);
	} catch(const larocv::larbys& err) { continue; }

	int px = (int)timg.at<uchar>((int)patch.circle.center.y, 
				     (int)patch.circle.center.x);
	
	if (!px) continue;
	upper_v.emplace_back(std::move(patch));
      }

      
      for(size_t lid =0; lid < lower_patch_v.size(); ++lid) {
	if (lower_patch_v[lid].pt_v.empty()) continue;
	auto& patch = lower_patch_v[lid];
	patch.init();
	auto ctor_img = MaskImage(img,ctor_v[patch.super_id],0,false);
	ctor_img = MaskImage(ctor_img,patch.circle,0,false);
	try {
	  patch.local_pca = CalcPCA(ctor_img);
	} catch(const larocv::larbys& err) { continue; }
	int px = (int)timg.at<uchar>((int)patch.circle.center.y, 
				     (int)patch.circle.center.x);
	
	if (!px) continue;
	lower_v.emplace_back(std::move(patch));
      }
      
      
      //
      // Connect upper to lower
      //

      float patch_width = dead_wire_v[dwc_id].second - dead_wire_v[dwc_id].first;
      
      std::vector<bool> used_v(lower_v.size(),false);

      for(size_t uid=0; uid<upper_v.size(); ++uid) {
	LAROCV_DEBUG() << "@uid=" << uid << std::endl;

	const auto& upper = upper_v[uid];
	for(size_t lid=0; lid<lower_v.size(); ++lid) {
	  LAROCV_DEBUG() << "@lid=" << lid << std::endl;
	  if (used_v[lid]) continue;

	  const auto& lower = lower_v[lid];

	  auto uangle = geo2d::angle(upper.local_pca);
	  auto langle = geo2d::angle(lower.local_pca);

	  auto dtheta_pca = std::abs(uangle - langle);
	  LAROCV_DEBUG() << "dtheta_pca="<<dtheta_pca << std::endl;
	  if (dtheta_pca > 15) continue;

	  geo2d::LineSegment<float> ls(upper.circle.center,lower.circle.center);

	  auto ulangle = geo2d::angle(ls);

	  auto dtheta_upca = std::abs(uangle - ulangle);
	  auto dtheta_lpca = std::abs(langle - ulangle);

	  LAROCV_DEBUG() << "dtheta_upca="<<dtheta_upca << std::endl;
	  LAROCV_DEBUG() << "dtheta_lpca="<<dtheta_lpca << std::endl;
	  LAROCV_DEBUG() << "now.." << std::endl;
	  dtheta_upca = dtheta_upca > 90 ? std::abs(dtheta_upca - 180) : dtheta_upca;
	  dtheta_lpca = dtheta_lpca > 90 ? std::abs(dtheta_lpca - 180) : dtheta_lpca;

	  LAROCV_DEBUG() << "dtheta_upca="<<dtheta_upca << std::endl;
	  LAROCV_DEBUG() << "dtheta_lpca="<<dtheta_lpca << std::endl;
	  if (dtheta_upca > 15) continue;
	  if (dtheta_lpca > 15) continue;

	  used_v[lid] = true;
	  //
	  // Connect them
	  //
	  LAROCV_DEBUG() << "connect @patch_width=" << patch_width << std::endl;
	  cv::line(res_img,
		   cv::Point((int)(ls.pt1.x + 0.5),(int)(ls.pt1.y+0.5)),
		   cv::Point((int)(ls.pt2.x + 0.5),(int)(ls.pt2.y+0.5)),
		   cv::Scalar(255));
		   
	  
	} // end lower
      } // end upper
      LAROCV_DEBUG() << "end this dwc" << std::endl;
    } // end this block

    
    return res_img;
  }
 
}
#endif
