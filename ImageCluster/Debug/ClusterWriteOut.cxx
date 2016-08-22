#ifndef __CLUSTERWRITEOUT_CXX__
#define __CLUSTERWRITEOUT_CXX__

#include "ClusterWriteOut.h"

namespace larocv{

  void ClusterWriteOut::_Configure_(const ::fcllite::PSet &pset)
  {}
  

  Cluster2DArray_t ClusterWriteOut::_Process_(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    if (! meta.debug() ) throw larbys("You can not call ClusterWriteOut outside of debug mode");

    std::stringstream ss1, ss2;

    ::larlite::user_info uinfo{};
    ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
    uinfo.store("ID",ss1.str());

    clear_ss(ss1);

    LAROCV_DEBUG((*this)) << "Writing debug information for " << clusters.size() << "\n";
    
    for(size_t i = 0; i < clusters.size(); ++i){

      const auto& cluster = clusters[i];

      ////////////////////////////////////////////
      /////Contour points
      
      ss1  <<  "ClusterID_" << cluster.ClusterID() << "_contour_x";
      ss2  <<  "ClusterID_" << cluster.ClusterID() << "_contour_y";

      for(const auto& point : cluster._contour) {
	double x = meta.XtoTimeTick(point.x);
	double y = meta.YtoWire(point.y);
	
	uinfo.append(ss1.str(),x);
	uinfo.append(ss2.str(),y);
      }

      clear_ss(ss1);
      clear_ss(ss2);

      ////////////////////////////////////////////
      /////Cluster parameters

      //_startPt
      ss1  <<  "ClusterID_" << cluster.ClusterID() <<  "_startPt_x";
      ss2  <<  "ClusterID_" << cluster.ClusterID() <<  "_startPt_y";
      uinfo.store(ss1.str(),meta.XtoTimeTick(cluster._startPt.x));
      uinfo.store(ss2.str(),meta.YtoWire    (cluster._startPt.y));

      clear_ss(ss1);
      clear_ss(ss2);


      //_centerPt
      ss1  <<  "ClusterID_" << cluster.ClusterID() <<  "_centerPt_x";
      ss2  <<  "ClusterID_" << cluster.ClusterID() <<  "_centerPt_y";
      uinfo.store(ss1.str(),meta.XtoTimeTick(cluster._centerPt.x));
      uinfo.store(ss2.str(),meta.YtoWire    (cluster._centerPt.y));

      clear_ss(ss1);
      clear_ss(ss2);

      //_eigenVecFirst
      ss1  <<  "ClusterID_" << cluster.ClusterID() <<  "_eigenVecFirst_x";
      ss2  <<  "ClusterID_" << cluster.ClusterID() <<  "_eigenVecFirst_y";
      uinfo.store(ss1.str(),meta.XtoTimeTick(cluster._eigenVecFirst.x));
      uinfo.store(ss2.str(),meta.YtoWire    (cluster._eigenVecFirst.y));

      clear_ss(ss1);
      clear_ss(ss2);
	
    }

    meta.ev_user()->emplace_back(uinfo);

    uinfo.clear_data();
    ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_roi_bounds";
    uinfo.store("ID",ss1.str());

    clear_ss(ss1);
    
    ss1  << "roi_bounds_x";
    ss2  << "roi_bounds_y";
    
    for ( size_t b = 0; b < roi.roibounds().size(); b++){
      
      auto c = roi.roibounds()[b];
      auto w = c.x ; 
      auto t = c.y ;
      
      uinfo.append(ss1.str(),w);
      uinfo.append(ss2.str(),t);
      
    }

    clear_ss(ss1);
    clear_ss(ss2);
    
    meta.ev_user()->emplace_back(uinfo);

    return clusters;    
  }


  void ClusterWriteOut::clear_ss(::std::stringstream& ss) {
    ss.str(std::string());
    ss.clear();
  }
  
}
#endif
