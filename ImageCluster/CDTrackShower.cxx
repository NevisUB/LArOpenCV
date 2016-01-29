#ifndef __CDTRACKSHOWER_CXX__
#define __CDTRACKSHOWER_CXX__

#include "CDTrackShower.h"

namespace larcv{

  void CDTrackShower::_Configure_(const ::fcllite::PSet &pset)
  {
    _outtree = new TTree("CD","CD");
    _outtree->Branch("_defects_dists" , "std::vector<double>" , &_defects_dists);
  }
  
  ContourArray_t CDTrackShower::_Process_(const larcv::ContourArray_t& clusters,
					  const ::cv::Mat& img,
					  larcv::ImageMeta& meta)
  {
    
    std::vector<std::vector<int> > hullpts_v; hullpts_v.resize(clusters.size());
    std::vector<std::vector<::cv::Vec4i> >    defects_v; defects_v.resize(clusters.size());
      

    for (unsigned i = 0; i < clusters.size(); ++i ) {
      auto& cluster = clusters [i];
      auto& hullpts = hullpts_v[i];
      auto& defects = defects_v[i];
	
      ::cv::convexHull(cluster,hullpts);
      hullpts.push_back(hullpts.at(0)); // odd that this is needed to close up the hull
      ::cv::convexityDefects(cluster,hullpts,defects);

      _defects_dists.clear();
      
      for( auto & d : defects ) 
	_defects_dists.push_back(  ( (float) d[3] ) / 256.0 );

      _outtree->Fill();
    }

    std::swap(hullpts_v,_hullpts_v);
    std::swap(defects_v,_defects_v);

    
    
    return clusters;
  }



}
#endif
