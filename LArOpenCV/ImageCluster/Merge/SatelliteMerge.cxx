#ifndef __SATELLITEMERGE_CXX__
#define __SATELLITEMERGE_CXX__

#include "SatelliteMerge.h"

namespace larocv{

  /// Global larocv::SatelliteMergeFactory to register AlgoFactory
  static SatelliteMergeFactory __global_SatelliteMergeFactory__;

  void SatelliteMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _area_separation  = pset.get<double> ( "AreaCut"           );
    _min_sat_dist     = pset.get<double> ( "MinSatDistance"    );
    _min_shower_dist  = pset.get<double> ( "MinShowerDistance" );
    _density          = pset.get<double> ( "ContourDensity"    );
  }
  
  Cluster2DArray_t SatelliteMerge::_Process_(const larocv::Cluster2DArray_t& clusters,
					     const ::cv::Mat& img,
					     larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    _secret_initial_sats.clear();
    std::vector<larocv::Contour_t> satellite_v; satellite_v.reserve(clusters.size());
    std::vector<larocv::Contour_t> shower_v;    shower_v.reserve(clusters.size());
    
    for(auto & cluster : clusters) {
      auto area   = ::cv::contourArea(cluster._contour);
      
      if ( area > _area_separation )
	shower_v.push_back(cluster._contour);
      else
	satellite_v.push_back(cluster._contour);

    }

    _secret_initial_sats = satellite_v;
    //Can we group the sattelites? Lets associate them based on closest distance.
    
    std::vector<larocv::Contour_t> sats_v;
    std::vector<larocv::Contour_t> sats_comb_v;

    sats_v = satellite_v;

    std::vector<::cv::Point> c3;
    std::vector<cv::Point> hull; hull.reserve( sats_v.size() );
    
    std::map<size_t,bool> used_shower;
    std::map<size_t,bool> used_sats;
      
    bool still_merging  = true;
    int  n_merged = 0;

    //pair-wise still_merging

    while ( still_merging ) {

      n_merged = 0;
      used_sats.clear();
      sats_comb_v.clear();

      for(unsigned k = 0; k < sats_v.size(); ++k) used_sats[k] = false;
      
      for(unsigned i = 0; i < sats_v.size(); ++i) {
	for(unsigned j = 0; j < sats_v.size(); ++j) {
	  
	  if ( i == j ) continue;
	  if (  used_sats[i] || used_sats[j] ) continue;

	  
	  c3.clear();
	    
	  auto& c1 = sats_v[i]; auto& c2 = sats_v[j];
	
	  if ( _pt_distance(c1,c2) < _min_sat_dist ) {  // combine them
	    
	    _combine_two_contours(c1,c2,c3);
	    
	    //auto c3 = ::cv::Mat(c1) + ::cv::Mat(c2);
	    //convexHull( ::cv::Mat(c3), hull, false );
	    n_merged++;
	  }
	  else
	    continue;
	  
	  used_sats[i] = true; used_sats[j] = true;
	  
	  // sats_comb_v.emplace_back(hull);
	  sats_comb_v.emplace_back(c3);
	  
	}
      }

      for(auto& u : used_sats) {
	if ( u.second ) continue;
	sats_comb_v.emplace_back(sats_v.at(u.first));
      }
      
      std::swap(sats_v,sats_comb_v);
      
      if ( ! n_merged  ) still_merging = false;
    }

    //We have merged, now decide something. Are we close enough to a larger cluster? Does out convex hull overlap
    //a bigger ``shower cluster" ? These are just ideas
    ContourArray_t out_ctors;
    std::map< int, std::vector<int> > showers_to_combine;

    used_shower.clear();
    used_sats  .clear();
    
    for(unsigned i = 0; i < shower_v.size(); ++i) used_shower[i] = false;
    for(unsigned i = 0; i < sats_v.size();   ++i) used_sats  [i] = false;
    
    if (shower_v.size() ) {
	    
      used_sats.clear();
      used_shower.clear();
      
      int dmin = 9e6;
      int s;
      for( int j = 0; j < sats_v.size(); ++j ) {
	auto& sat = sats_v[j];
	dmin = 9e6;
	s    = -1;
	
	for(int i  = 0; i < shower_v.size(); ++i ){
	  auto& sh = shower_v[i];
	  auto d   = _pt_distance(sh,sat);
	  if (d < dmin) { dmin = d; s = i; }
	}

	if (s < 0) { std::cout << "bad s...\n"; throw std::exception(); }
	
	if ( dmin < _min_shower_dist )
	  { showers_to_combine[s].push_back(j); used_shower[s] = true; used_sats[j] = true; }
	
      }
    }

    for(auto& shower : showers_to_combine)
      for(auto& index: shower.second)
	{ _combine_two_contours(shower_v[shower.first],sats_v[index],c3); out_ctors.emplace_back(c3); }
    
    for(auto& ushow : used_shower)
      { if ( ushow.second ) continue; out_ctors.emplace_back( shower_v[ ushow.first ] ); }

    for(auto& usats : used_sats)
      { if ( usats.second ) continue; out_ctors.emplace_back( sats_v[ usats.first] ); }

    Cluster2DArray_t result;
    result.resize(out_ctors.size());

    for(size_t i=0; i<out_ctors.size(); ++i) std::swap(result[i]._contour,out_ctors[i]);

    return result;
  }

  void SatelliteMerge::_combine_two_contours(const larocv::Contour_t& c1, const larocv::Contour_t& c2, larocv::Contour_t& c3) {
    c3.clear();
    c3.reserve( c1.size() + c2.size() );
    c3.insert( c3.end(), c1.begin(), c1.end() );
    c3.insert( c3.end(), c2.begin(), c2.end() );
  }

  
  double SatelliteMerge::_pt_distance(const larocv::Contour_t& c1, const larocv::Contour_t& c2) {
    double min_d = 9e10;
    //returns closes distance between pts on contours
    //I think I overcount right? c1.x - c2.x and c2.x - c1.x whatever...
    for(const auto& pt1 : c1) {
      for(const auto& pt2 : c2) {
	auto d = std::sqrt( (pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y) );
	if ( d < min_d ) min_d = d;
      }
    }

    if ( min_d == 9e10 ) { throw larbys("No minium distance? How..."); }
    
    return min_d;
  }
}
#endif
