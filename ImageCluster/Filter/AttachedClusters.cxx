#ifndef __ATTACHEDCLUSTERS_CXX__
#define __ATTACHEDCLUSTERS_CXX__

#include "AttachedClusters.h"

namespace larocv{

  void AttachedClusters::_Configure_(const ::fcllite::PSet &pset)
  {
    _max_defect_size         = pset.get<int>  ( "MaxDefectSize" );
    _hull_contour_area_ratio = pset.get<float>( "HullContourAreaRatio" );
  }

  Cluster2DArray_t AttachedClusters::_Process_(const larocv::Cluster2DArray_t& clusters,
					       const ::cv::Mat& img,
					       larocv::ImageMeta& meta, larocv::ROI& roi)
  { 

    Cluster2DArray_t oclusters;
    oclusters.reserve( clusters.size() );
    
    std::vector<std::vector<int> > hullpts_v;
    hullpts_v.resize(clusters.size());

    std::vector<std::vector<::cv::Vec4i> > defects_v;
    defects_v.resize(clusters.size());

    //start -- debug
    std::stringstream ss1,ss2,ss3,ss4;
    ::larlite::user_info uinfo{};

    ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
    uinfo.store("ID",ss1.str());

    ss1.str(std::string());
    ss1.clear();
    //end -- debug
    
    for (unsigned i = 0; i < clusters.size(); ++i ) {

      auto& cluster = clusters [i];
      auto& hullpts = hullpts_v[i];
      auto& defects = defects_v[i];
      
      ::cv::convexHull(cluster._contour, hullpts);
      hullpts.push_back(hullpts.at(0)); // odd that this is needed to close up the hull

      ::cv::convexityDefects(cluster._contour,hullpts,defects);

      // no defects found
      if ( ! defects.size() ) continue;
      
      std::vector<double> defects_d;
      defects_d.resize( defects.size() );

      for( int j = 0; j < defects.size(); ++j )
	defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 ); //why divide by 256?
    
      auto max_defect_itr = std::max_element( std::begin(defects_d), std::end(defects_d) );
      auto max_defect     =  *(max_defect_itr);
      auto ndefects       =  defects_d.size();

      std::vector<::cv::Point> hullcontour;
      hullcontour.resize(hullpts.size()-1);

      for(unsigned u=0;u<hullcontour.size();++u) 
	hullcontour[u] = cluster._contour[ hullpts[u] ];

      double contour_area = cluster._area;
      double hull_area    = ::cv::contourArea(hullcontour);

      LAROCV_DEBUG() << "\t>> Saw max_defect " << max_defect << " for total of ndefects: " << ndefects << " for cluster " << i << "\n";
      
      if ( meta.debug() ) {

	for( unsigned k = 0; k < hullpts.size(); ++k ) {

	  auto idx=hullpts[k];
	  ss1 << "ClusterID_" << cluster.ClusterID() << "_hull_x";
	  ss2 << "ClusterID_" << cluster.ClusterID() << "_hull_y";

	  LAROCV_DEBUG() <<"(" << cluster._contour[idx].x << "," << cluster._contour[idx].y << ") : ("
				<< meta.XtoTimeTick(cluster._contour[idx].x) << "," << meta.YtoWire(cluster._contour[idx].y) << ")\n";

	  double x = meta.XtoTimeTick(cluster._contour[idx].x);
	  double y = meta.YtoWire    (cluster._contour[idx].y);

	  uinfo.append(ss1.str(),x);
	  uinfo.append(ss2.str(),y);

	  ss1.str(std::string());
	  ss1.clear();

	  ss2.str(std::string());
	  ss2.clear();
	  
	}
      }
      
      if ( max_defect > _max_defect_size )
	continue;
      
      double area_ratio = hull_area > 0 ? contour_area / hull_area : 0.0;

      if ( area_ratio > 1.0 ) throw larbys("How can the convex hull be larger in area than the contour?");

      if ( area_ratio < _hull_contour_area_ratio)
	continue;
      
      
      oclusters.emplace_back(cluster);
    }
    
    
    if ( meta.debug() )
      meta.ev_user()->emplace_back(uinfo);
        
    return oclusters;
  }

}
#endif
