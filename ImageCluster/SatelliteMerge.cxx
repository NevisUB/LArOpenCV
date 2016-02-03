#ifndef __SATELLITEMERGE_CXX__
#define __SATELLITEMERGE_CXX__

#include "SatelliteMerge.h"

namespace larcv{

  void SatelliteMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _area_separation  = pset.get<double> ( "AreaCut"           );
    _min_sat_dist     = pset.get<double> ( "MinSatDistance"    );
    _min_shower_dist  = pset.get<double> ( "MinShowerDistance" );
    _density          = pset.get<double> ( "ContourDensity"    );
  }

  ContourArray_t SatelliteMerge::_Process_(const larcv::ContourArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  {
    
    _secret_initial_sats.clear();
    std::vector<larcv::Contour_t> satellite_v; satellite_v.reserve(clusters.size());
    std::vector<larcv::Contour_t> shower_v;    shower_v.reserve(clusters.size());
    
    for(auto & cv_contour : clusters) {
      auto area   = ::cv::contourArea(cv_contour);
      if ( area > _area_separation )
	shower_v.push_back(cv_contour);
      else
	satellite_v.push_back(cv_contour);

      _secret_initial_sats.push_back(cv_contour);
    }

    
    //Can we group the sattelites? Lets associate them based on closest distance.
    
    std::vector<larcv::Contour_t> sats_v;
    std::vector<larcv::Contour_t> sats_comb_v;

    sats_v = satellite_v;

    std::vector<::cv::Point> c3;
    std::vector<cv::Point> hull; hull.reserve( sats_v.size() );
    
    std::map<size_t,bool> used;

    bool merging  = true;
    int  n_merged = 0;


    
    while ( merging ) {

      n_merged = 0;
      used.clear();
      sats_comb_v.clear();
      for(unsigned k = 0; k < sats_v.size(); ++k) used[k] = false;
      
      for(unsigned i = 0; i < sats_v.size(); ++i) {
	for(unsigned j = 0; j < sats_v.size(); ++j) {
	
	  if ( i == j ) continue;
	  if (  used[i] || used[j] ) continue;
	
	  hull.clear();
	  c3.clear();
	    
	  auto& c1 = sats_v[i]; auto& c2 = sats_v[j];
	
	  if ( _pt_distance(c1,c2) < _min_sat_dist ) {  // combine them

	    _combine_two_contours(c1,c2,c3);
	    //auto c3 = ::cv::Mat(c1) + ::cv::Mat(c2);
	    // convexHull( ::cv::Mat(c3), hull, false );
	    n_merged++;
	  }
	  else
	    continue;
	  
	  used[i] = true;
	  used[j] = true;

	  // sats_comb_v.emplace_back(hull);
	  sats_comb_v.emplace_back(c3);
	  
	}
      }
      
      for(auto& u : used) {
	if ( u.second ) continue;
	sats_comb_v.emplace_back(sats_v.at(u.first));
      }
      
      std::swap(sats_v,sats_comb_v);
      
      if ( !n_merged  ) merging = false;
    }

    return sats_v;

    
    //We have merged, now decide something. Are we close enough to a larger cluster? Does out convex hull overlap
    //a bigger ``shower cluster" ? These are just ideas
    
    ContourArray_t out_ctors;

    auto tmp_showers = shower_v;
    auto tmp_sats    = sats_v;

    ContourArray_t tmp_showers_; tmp_showers_.reserve(shower_v.size());
    ContourArray_t tmp_sats_;    tmp_sats_   .reserve(shower_v.size());
    
    int dmin = 9e10;

    merging  = false;
    
    while ( merging ) {

      tmp_showers_.clear();
      tmp_sats_   .clear();
      n_merged = 0;
      
      for(auto& sats  : tmp_sats) {
	// get the closest ``shower" -- this is more subtle I think than I realize

	dmin = 9e10;
	Contour_t* shower;

	for(auto& sh : tmp_showers) {
	  auto d = _pt_distance(sh,sats);
	  if (d < dmin) { dmin = d; shower = &sh; }
	}
      
	if ( dmin < _min_shower_dist ) {
	  _combine_two_contours(sats,*shower,c3);
	  tmp_showers_.emplace_back(c3);
	  n_merged++;
	}
	else
	  tmp_sats_.emplace_back(sats);
      }

      std::swap(tmp_showers_,tmp_showers);
      std::swap(tmp_sats_   ,tmp_sats   );
      
      if (!n_merged) merging = false;
      
    }
    
    for(auto& shower : tmp_showers) out_ctors.emplace_back( shower);
    for(auto& sat    : tmp_sats)    out_ctors.emplace_back( sat   );
    
    return out_ctors;
  }

  void SatelliteMerge::_combine_two_contours(const larcv::Contour_t& c1, const larcv::Contour_t& c2, larcv::Contour_t& c3) {
    c3.clear();
    c3.reserve( c1.size() + c2.size() );
    c3.insert( c3.end(), c1.begin(), c1.end() );
    c3.insert( c3.end(), c2.begin(), c2.end() );
  }
  
  double SatelliteMerge::_pt_distance(const larcv::Contour_t& c1, const larcv::Contour_t& c2) {
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
