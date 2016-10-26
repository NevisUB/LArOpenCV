#ifndef __CIRCLEVERTEX_CXX__
#define __CIRCLEVERTEX_CXX__

#include "CircleVertex.h"
#include "PCACandidates.h"

#include "Core/Circle.h"

namespace larocv {

  /// Global larocv::CircleVertexFactory to register ClusterAlgoFactory
  static CircleVertexFactory __global_CircleVertexFactory__;

  void CircleVertex::_Configure_(const ::fcllite::PSet &pset)
  {
    _max_radius_size = 15;
  }
  
  void CircleVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       larocv::ImageMeta& meta,
			       larocv::ROI& roi) {
    
    const auto& pcacandidates_data = AlgoData<PCACandidatesData>(ID()-1);
    
    // intersection points, 1 per initial contour.
    const auto& ipoints_v = pcacandidates_data._ipoints_v_v[meta.plane()]; 
    
    auto& circlevertex_data = AlgoData<larocv::CircleVertexData>();
    
    //find the points who's circle of _max_radius_size contains other most number of points.
    if ( ipoints_v.size() < 2 ) return;
			      
    std::vector<unsigned> n_inside_v(ipoints_v.size(),0);

    for(unsigned pidx=0; pidx < ipoints_v.size(); ++pidx) {
	
	auto const& pt       = ipoints_v[pidx];
	auto&       n_inside = n_inside_v[pidx];
	
	geo2d::Circle<float> circle_pt(pt,_max_radius_size);

	for(unsigned oidx=0; oidx < ipoints_v.size(); ++oidx) {

	  if (oidx == pidx) continue;

	  double dist;

	  if ( geo2d::contains(circle_pt,ipoints_v[oidx],dist) ) n_inside+=1;
	  
	}

    }
    
    auto max_itr = std::max_element(n_inside_v.begin(),n_inside_v.end());
    auto max_idx = max_itr - n_inside_v.begin();
    
    //make this circle
    geo2d::Circle<float> circle_pt(ipoints_v[max_idx],_max_radius_size);
    
    //GEO2D_Contour_t inside;
    std::vector<geo2d::Vector<float> > inside;
    
    double dist;
    
    //get the points inside (another loop)
    for(unsigned pidx=0; pidx < ipoints_v.size(); ++pidx) {
      if ( geo2d::contains(circle_pt,ipoints_v[pidx],dist) ) {
	LAROCV_DEBUG() << "Point coordinate: " << ipoints_v[pidx] << " INSIDE CIRCLE" << std::endl;
	inside.emplace_back(ipoints_v[pidx]);
      }else {
	LAROCV_DEBUG() << "Point coordinate: " << ipoints_v[pidx] << " OUTSIDE CIRCLE" << std::endl;
      }
    }
    geo2d::Circle<float> circle_min(inside); 
    
    LAROCV_DEBUG() << "Candidate circle on plane " << meta.plane()
		   << " center @ " << circle_min.center << " radius " << circle_min.radius << std::endl;
    
    circlevertex_data._circledata_v_v[meta.plane()].emplace_back(std::move(circle_min));
    
  }
  
  bool CircleVertex::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {return true;}
}
#endif
