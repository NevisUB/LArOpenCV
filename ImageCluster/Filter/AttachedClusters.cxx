#ifndef __ATTACHEDCLUSTERS_CXX__
#define __ATTACHEDCLUSTERS_CXX__

#include "AttachedClusters.h"

namespace larocv{

  void AttachedClusters::_Configure_(const ::fcllite::PSet &pset)
  {
    _maxDefectSize  = pset.get<int> ( "MaxDefectSize" );
  }

  Cluster2DArray_t AttachedClusters::_Process_(const larocv::Cluster2DArray_t& clusters,
					       const ::cv::Mat& img,
					       larocv::ImageMeta& meta)
  { 
    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );
    
    
    std::vector<std::vector<int> > hullpts_v; hullpts_v.resize(clusters.size());
    std::vector<std::vector<::cv::Vec4i> >    defects_v; defects_v.resize(clusters.size());

    
    for (unsigned i = 0; i < clusters.size(); ++i ) {

      auto& cluster = clusters [i];
      auto& hullpts = hullpts_v[i];
      auto& defects = defects_v[i];
      
      ::cv::convexHull(cluster._contour, hullpts);
      hullpts.push_back(hullpts.at(0)); // odd that this is needed to close up the hull

      ::cv::convexityDefects(cluster._contour,hullpts,defects);

      if ( ! defects.size() ) continue;
      
      std::vector<double> defects_d; defects_d.resize( defects.size() );

      for( int j = 0; j < defects.size(); ++j )

	defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 );
    

      auto max_defect_itr = std::max_element( std::begin(defects_d), std::end(defects_d) );
      auto max_defect     =  * ( max_defect_itr );
      auto ndefects       =  defects_d.size();

      std::cout << "\t>> saw_max_defect " << max_defect << " for total of ndefects: " << ndefects << "\n";

      if ( max_defect > _maxDefectSize )
	continue;
      
      oclusters.emplace_back( cluster );
    }
    
    return oclusters;
  }

}
#endif
