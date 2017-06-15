#ifndef __VTXALIGNEDMERGE_CXX__
#define __VTXALIGNEDMERGE_CXX__

#include "VtxAlignedMerge.h"

namespace larocv{

  void VtxAlignedMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _debug = pset.get<bool>("Debug");
    _max_angle_diff_merge  = pset.get<double>("MaxAngleDiffMerge"  );
    _min_gammagamma_oangle = pset.get<double>("MinGammaGammaOAngle");
    _merge_till_converge   = pset.get<bool>  ("MergeTillConverge"  );
  }

  Cluster2DArray_t VtxAlignedMerge::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    bool merge = true;

    Cluster2DArray_t tmp_clusters = clusters;
    Cluster2DArray_t out_clusters;
    
    while (merge) {

      out_clusters.clear();
      
      // pair links cluster index to pair of ( NHits, angle )
      // vector has 3 elements, one per plane
      // object will hold cluster angle for cluster with largest number of hits per plane
      std::pair< size_t, std::pair<size_t, double> > gamma_00(std::make_pair( 0, std::make_pair( 0, 0.) ) );
      std::pair< size_t, std::pair<size_t, double> > gamma_01(std::make_pair( 0, std::make_pair( 0, 0.) ) );
      
      // map linking cluster index and angle
      std::map<size_t, double> clus_angle_map;
      
      //get the vertex
      auto pi0st = roi.roivtx_in_image(meta);
      
      // sort clusters by size
      // map contains nhits -> index
      std::map<int,int> clus_size_map;
      for (size_t idx = 0; idx < tmp_clusters.size(); idx++) {
	auto const& cluster = tmp_clusters.at(idx);
	clus_size_map[cluster._numHits()] = idx;
      }
      
      std::map<int,int>::reverse_iterator it;
      for (it = clus_size_map.rbegin(); it != clus_size_map.rend(); it++) {
	
	auto idx = it->second;
	
	auto const& cluster = tmp_clusters.at(idx);
	
	Point2D clus_dir(0.,0.);
	
	auto clus_hits = cluster._insideHits;
	
	for (auto& hit : cluster._insideHits){
	  clus_dir.x += hit.x - pi0st.x;
	  clus_dir.y += hit.y - pi0st.y;
	}
	
	double angle = atan2(clus_dir.y,clus_dir.x) * 180. / 3.14;
	int    plane = cluster.PlaneID();
	int    nhits = cluster._numHits();
	
	clus_angle_map[idx] = angle;
	
	if (_debug){
	  std::cout << "New cluster @ Plane " << plane
		    << "\t Nhits = " << nhits
		    << "\t PCA dir = " << angle << std::endl;
	  std::cout << "Current gamma00 angle : " << gamma_00.second.second << "\t gamma01 angle : " << gamma_01.second.second << std::endl;
	}
	
	if ( nhits > gamma_00.second.first ) {
	  if (_debug) std::cout << "\t updating gamma00" << std::endl;
	  gamma_00.second.first  = nhits; // update # of hits for largest cluster in plane
	  gamma_00.second.second = angle; // update direction from PCA for largest cluster in plane
	  gamma_00.first         = idx;   // update index
	}
	
	if ( ( nhits > gamma_01.second.first ) &&
	     ( fabs(angle-gamma_00.second.second ) > _min_gammagamma_oangle) &&
	     ( gamma_00.second.second != 0 ) ) {
	  if (_debug) std::cout << "\t updating gamma01" << std::endl;
	  gamma_01.second.first  = nhits; // update # of hits for largest cluster in plane
	  gamma_01.second.second = angle; // update direction from PCA for largest cluster in plane
	  gamma_01.first         = idx;   // update index
	}
	
	if (_debug) std::cout << std::endl << std::endl;
	
      }// for all input clusters
      
      if (_debug)
	std::cout << " gamma00 has " << gamma_00.second.first << " hits"
		  << " and " << gamma_00.second.second << " angle" << std::endl;
      if (_debug)
	std::cout << " gamma01 has " << gamma_01.second.first << " hits"
		  << " and " << gamma_01.second.second << " angle" << std::endl;
      
      if (gamma_00.second.second == 0) return tmp_clusters;
      
      // "big" cluster 00
      auto gammacluster_00 = tmp_clusters.at( gamma_00.first );
      auto gammaangle_00   = gamma_00.second.second;
      
      // "big" cluster 01
      auto gammacluster_01 = tmp_clusters.at( gamma_01.first );
      auto gammaangle_01   = gamma_01.second.second;
      
      // loop through indices. which to merge?
      // keep track in a vector
      std::vector<size_t> indices_to_merge_00;
      std::vector<size_t> indices_to_merge_01;
      
      for (auto const& clus : clus_angle_map) {
	
	auto clus_idx   = clus.first;
	auto clus_angle = clus.second;
	auto cluster    = tmp_clusters.at(clus_idx);
	
	// don't merge cluster with itself!
	if ( (clus.first == gamma_00.first) || (clus.first == gamma_01.first) ) continue;
	
	// find cluster with respect to which angle is smaller
	double angle_small = fabs(clus_angle - gammaangle_00);
	double angle_large = fabs(clus_angle - gammaangle_01);
	int merge_with = 0; // which gamma to merge with? [0 -> 00, 1 -> 01]
	// flip if we got it wrong
	if (angle_small > angle_large){
	  angle_small = fabs(clus_angle - gammaangle_01);
	  angle_large = fabs(clus_angle - gammaangle_00);
	  merge_with = 1;
	}
	
	if (_debug)
	  std::cout << "new cluster with " << cluster._numHits()
		    << " hits and angle " << clus_angle << std::endl
		    << "\t small : " << angle_small << "\t angle large : " << angle_large
		    << std::endl;
	
	// NOTE
	// below IF statements are not correct
	// when gammaangle_01 is undefined the angle_large requirement may cause
	// merging not to happen when it really should
	
	if (merge_with == 0){
	  if ( (angle_small < 12) and (gammaangle_00 != 0) and (angle_large > 15) ){
	    indices_to_merge_00.push_back( clus_idx );
	    if (_debug) std::cout << "MERGE WITH 00" << std::endl;
	  }
	}
	
	if (merge_with == 1){
	  if ( (angle_small < 12) and (gammaangle_01 != 0) and (angle_large > 15) ){
	    indices_to_merge_01.push_back( clus_idx );
	    if (_debug) std::cout << "MERGE WITH 01" << std::endl;
	  }
	}
	
      }// for all clusters
      
      if (_debug) {
	std::cout << "identified " << indices_to_merge_00.size() << " clusters to merge with gamma00" << std::endl;
	std::cout << "identified " << indices_to_merge_01.size() << " clusters to merge with gamma01" << std::endl;
      }
      
      // make contour to include points from both clusters
      std::vector<::cv::Point> overall_ctor_00;
      std::vector<::cv::Point> overall_ctor_01;
      
      // add it's hits to new contour
      for (auto& pt : gammacluster_00._insideHits) overall_ctor_00.emplace_back(pt);
      for (auto& pt : gammacluster_01._insideHits) overall_ctor_01.emplace_back(pt);
      
      // loop throguh secondary clusters to be merged
      for (auto& merge_idx : indices_to_merge_00) {
	auto cluster = tmp_clusters.at(merge_idx);
	// add points from new cluster
	for (auto& pt : cluster._insideHits) overall_ctor_00.emplace_back(pt);
      }// for all clusters to be merged
      
      // loop throguh secondary clusters to be merged
      for (auto& merge_idx : indices_to_merge_01) {
	auto cluster = tmp_clusters.at(merge_idx);
	// add points from new cluster
	for (auto& pt : cluster._insideHits) overall_ctor_01.emplace_back(pt);
      }// for all clusters to be merged
      
      //make a new hull, give it some space
      if (overall_ctor_00.size() != 0) {
	std::vector<int> hullpts_00;
	hullpts_00.reserve(overall_ctor_00.size());
	//calculate the hull
	if (_debug) std::cout << "convex hull calculation ...";
	::cv::convexHull(overall_ctor_00, hullpts_00);
	if (_debug) std::cout << "\t done." << std::endl;
	//make a new cluster
	Cluster2D outcluster_00;
	//with a contour of fixed size...
	outcluster_00._contour.resize(hullpts_00.size());
	//fill the contour with hull points!
	for(unsigned u=0;u<hullpts_00.size();++u) 
	outcluster_00._contour[u] = overall_ctor_00[ hullpts_00[u] ];
	outcluster_00.copy_params(gammacluster_00);
	out_clusters.emplace_back(outcluster_00);
      }

      //make a new hull, give it some space
      if (overall_ctor_01.size() != 0) {
	std::vector<int> hullpts_01;
	hullpts_01.reserve(overall_ctor_01.size());
	//calculate the hull
	if (_debug) std::cout << "convex hull calculation ...";
	::cv::convexHull(overall_ctor_01, hullpts_01);
	if (_debug) std::cout << "\t done." << std::endl;
	//make a new cluster
	Cluster2D outcluster_01;
	//with a contour of fixed size...
	outcluster_01._contour.resize(hullpts_01.size());
	//fill the contour with hull points!
	for(unsigned u=0;u<hullpts_01.size();++u) 
	  outcluster_01._contour[u] = overall_ctor_01[ hullpts_01[u] ];
	outcluster_01.copy_params(gammacluster_01);
	out_clusters.emplace_back(outcluster_01);
      }
	
      for (size_t i=0; i < tmp_clusters.size(); i++) {
	
	if ( (i == gamma_00.first) || (i == gamma_01.first) )
	  continue;
	
	bool used = false;
	for (auto idx : indices_to_merge_00) {
	  if (idx == i) {
	    used = true;
	    break;
	  }
	}
	for (auto idx : indices_to_merge_01) {
	  if (idx == i) {
	    used = true;
	    break;
	  }
	}
	
	if (used == false)
	  out_clusters.emplace_back( tmp_clusters.at(i) );
      }// for all input clusters

      // if do not merge till converge then we are done
      if (_merge_till_converge == false)
	merge = false;

      // if we haven't merged anything no point in trying again...
      // ...we are done. quit.
      if (tmp_clusters.size() == out_clusters.size())
	merge = false;

      tmp_clusters = out_clusters;
      
    }// while merge is true

    if (_debug) std::cout << "IN : " << clusters.size() << "\t OUT : " << out_clusters.size() << std::endl;
    
    return out_clusters;
  }
}
#endif
