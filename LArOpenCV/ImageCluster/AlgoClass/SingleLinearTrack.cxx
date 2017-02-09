#ifndef __SINGLELINEARTRACK_CXX__
#define __SINGLELINEARTRACK_CXX__

#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Geo2D.h"
#include "SingleLinearTrack.h"
#include "Geo2D/Core/spoon.h"
#include <map>
#include <array>
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  
  void SingleLinearTrack::Configure(const Config_t &pset)
  {
    _min_size_track_ctor = pset.get<size_t>("TrackContourSizeMin");
    _min_length_track_ctor = pset.get<float>("TrackContourLengthMin");
    
    _pi_threshold = 10;
    _seed_plane_v = pset.get<std::vector<size_t> >("SeedPlane");
    
    _edges_from_mean = pset.get<bool>("EdgesFromMean",true);
    
    for(auto const& plane : _seed_plane_v) {
      if(plane >= _seed_plane_v.size()) {
	LAROCV_CRITICAL() << "Seed plane " << plane << " exceeds # planes " << _seed_plane_v.size() << std::endl;
	throw larbys();
      }
    }
    _minimum_neighbor_distance = 5.0;
    _min_compat_dist = 5;
    _xplane_tick_resolution = 3;
    _num_planes = 3;
  }
  
  void SingleLinearTrack::EdgesFromMeanValue(const GEO2D_Contour_t& ctor,
					      geo2d::Vector<float>& edge1,
					      geo2d::Vector<float>& edge2) const
  {
    // cheap trick assuming this is a linear, linear track cluster
    geo2d::Vector<float> mean_pt, ctor_pt;
    mean_pt.x = mean_pt.y = 0.;
    for(auto const& pt : ctor) { mean_pt.x += pt.x; mean_pt.y += pt.y; }
    mean_pt.x /= (double)(ctor.size());
    mean_pt.y /= (double)(ctor.size());
    // find the furthest point from the mean (x,y)
    double dist_max=0;
    double dist;
    for(auto const& pt : ctor) {
      ctor_pt.x = pt.x;
      ctor_pt.y = pt.y;
      dist = geo2d::dist(mean_pt,ctor_pt);
      if(dist > dist_max) {
	edge1 = pt;
	dist_max = dist;
      }
    }
    // find the furthest point from edge1
    dist_max=0;
    for(auto const& pt : ctor) {
      ctor_pt.x = pt.x;
      ctor_pt.y = pt.y;
      dist = geo2d::dist(edge1,ctor_pt);
      if(dist > dist_max) {
	edge2 = pt;
	dist_max = dist;
      }
    }
    // set edge1 time < edge2 time
    if(edge2.x < edge1.x) std::swap(edge1,edge2);
  }

  
  void SingleLinearTrack::EdgesFromPCAProjection(const cv::Mat& img,
						  const GEO2D_Contour_t& ctor,
						  geo2d::Vector<float>& edge1,
						  geo2d::Vector<float>& edge2) const
  {
    //but instead I could fit PCA, project all points on this line, then find edges
    
    // make a contour from points inside contour
    GEO2D_Contour_t nonzero_pts, inside_pts;
    // crop image so we don't have to loop over all points
    cv::Rect brect = cv::boundingRect( cv::Mat(ctor) );
    cv::Mat crop_img = img(brect);

    // get nonzero pixels
    findNonZero(crop_img, nonzero_pts);
    inside_pts.reserve(nonzero_pts.size());

    // check nonzero pixels inside contour or on edge with 1 pixel tolerance;
    for(auto& nz_pt : nonzero_pts) {
      nz_pt += brect.tl();
      auto dist = cv::pointPolygonTest(ctor, nz_pt, true);
      if(dist>=0) inside_pts.emplace_back(std::move(nz_pt)); //1px tolerance
    }
    
    if (inside_pts.size() < 2) {
      edge1 = inside_pts[0];
      edge2 = inside_pts[1];
      return;
    }
    
    //calc PCA line
    geo2d::Line<float> pca_line;
    bool valid_pca = true;
    try {
      pca_line = CalcPCA(inside_pts);
    } catch(geo2d::spoon) {
      //pca line gave infinite slope
      valid_pca=false;
    }

    std::vector<geo2d::Vector<float> > proj_pts;
    proj_pts.reserve(inside_pts.size());
    
    //for each X point, find corresponding projected Y point
    for(const auto& pt : inside_pts) {
      geo2d::Vector<float> proj_pt = pt;
      
      if (valid_pca)
	{ proj_pt.x = pt.x; proj_pt.y = pca_line.y(pt.x); }
    
      proj_pts.emplace_back(std::move(proj_pt));
    }

    geo2d::Vector<float> edge1_p,edge2_p;

    //find the two points farthest away from each other
    float max_dist=-1.0;
    for(uint pt_id1 = 0; pt_id1 < proj_pts.size(); ++pt_id1) {
      for(uint pt_id2 = pt_id1+1; pt_id2 < proj_pts.size(); ++pt_id2) {
	auto dist = geo2d::dist(proj_pts[pt_id1],proj_pts[pt_id2]);
	if (dist > max_dist) {
	  max_dist = dist;
	  edge1_p = proj_pts[pt_id1];
	  edge2_p = proj_pts[pt_id2];
	}
      }
    }
    
    //find the closest non zero point from contour to the projected point
    //this is so the edge point is an non-zero pixel point
    
    float min_dist_e1(9e6),min_dist_e2(9e6);
    
    for(auto const& pt : inside_pts) {

      auto dist_e1 = geo2d::dist(edge1_p,geo2d::Vector<float>(pt));
      auto dist_e2 = geo2d::dist(edge2_p,geo2d::Vector<float>(pt));
      
      if(dist_e1 < min_dist_e1) {
	min_dist_e1=dist_e1;
	edge1=pt;
      }

      if(dist_e2 < min_dist_e2) {
	min_dist_e2=dist_e2;
	edge2=pt;
      }
      
    }
    // set edge1 time < edge2 time
    if(edge2.x < edge1.x) std::swap(edge1,edge2);    
  }
  
  
  void SingleLinearTrack::FindEdges(const cv::Mat& img,
				     const GEO2D_Contour_t& ctor,
				     geo2d::Vector<float>& edge1,
				     geo2d::Vector<float>& edge2) const
  {
    if (_edges_from_mean)
      EdgesFromMeanValue(ctor,edge1,edge2);
    else
      EdgesFromPCAProjection(img,ctor,edge1,edge2);      
  }
  
  std::vector<larocv::data::LinearTrack2D>
  SingleLinearTrack::FindLinearTrack2D(const size_t plane, const cv::Mat& img) const
  {
    // NOTE: this is just MIP contour finding from HIPCluster.cxx
    // NOTE: replace this with getting HIPCluster algo data
    
    // Prepare image for analysis per vertex
    ::cv::Mat thresh_img;
    //::cv::threshold(thresh_img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
    
    //Dilate
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(2,2));
    ::cv::dilate(img,thresh_img,kernel,::cv::Point(-1,-1),1);
    
    //Blur
    ::cv::blur(thresh_img,thresh_img,::cv::Size(2,2));

    ::cv::threshold(thresh_img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
    
    GEO2D_ContourArray_t parent_ctor_v;
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(thresh_img, parent_ctor_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    LAROCV_INFO() << "Plane " << plane << " found " << parent_ctor_v.size() << " LinearTrack2D candidates..." << std::endl;
    std::vector<larocv::data::LinearTrack2D> result_v;
    for(auto const& ctor : parent_ctor_v) {
      bool used=false;
      geo2d::Vector<float> edge1, edge2;
      LAROCV_DEBUG() << "Searching edges" << std::endl;
      FindEdges(thresh_img,ctor,edge1,edge2);
      if(ctor.size() < _min_size_track_ctor && geo2d::dist(edge1,edge2) < _min_length_track_ctor) { // ||
	LAROCV_DEBUG() << "Ignoring too-small contour @ size = " << ctor.size()
		       << " and length " << geo2d::dist(edge1,edge2) << std::endl;
	continue;
      }
      LAROCV_DEBUG() << "Found edge1 " << edge1 << "... edge2: " << edge2 << std::endl;
      // check if this contour is a part of clustered interaction
      if ( plane < _registered_vtx2d_vv.size() ) {
	for(auto const& vtx2d : _registered_vtx2d_vv.at(plane)) {
	  auto dist = ::cv::pointPolygonTest(ctor, vtx2d, true);
	  if(dist>=0) { used = true; break; }
	  dist *= -1.;
	  if(dist < _minimum_neighbor_distance) { used = true; break; }
	}
      }
      // if(used) {
      // 	LAROCV_DEBUG() << "Ignoring contour too-close to an alredy-found vertex" << std::endl;
      // 	continue;
      // }

      // Found a linear track candidate ... compute edge points
      larocv::data::LinearTrack2D strack;
      strack.ctor  = ctor;
      strack.edge1 = edge1;
      strack.edge2 = edge2;
      LAROCV_DEBUG() << "Found LinearTrack2D size = " << ctor.size()
		     << " ... edge1 @ " << strack.edge1
		     << " ... edge2 @ " << strack.edge2
		     << std::endl;
      result_v.push_back(strack);
    }
    return result_v;
  }

  void SingleLinearTrack::RegisterVertex2D(const size_t plane,
					    const std::vector<geo2d::Vector<float> >& vtx2d_v)
  {
    _registered_vtx2d_vv.resize(plane+1);
    _registered_vtx2d_vv[plane] = vtx2d_v;
  }

  std::vector<data::LinearTrack3D>
  SingleLinearTrack::FindLinearTrack(const std::vector<const cv::Mat>& img_v)
  {

    // First find linear track candidates
    std::vector<std::vector<larocv::data::LinearTrack2D> > strack_vv(img_v.size());
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      strack_vv[plane] = FindLinearTrack2D(plane,img_v[plane]);
      LAROCV_INFO() << "Plane " << plane << " found " << strack_vv[plane].size() << " LinearTrack2D..." << std::endl;
    }

    if(_seed_plane_v.size() != strack_vv.size()) {
      LAROCV_CRITICAL() << "# seed plane != # LineartTrack2D collection size!" << std::endl;
      throw larbys();
    }

    std::vector<std::vector<bool> > used_vv;
    for(auto const& strack_v : strack_vv) {
      std::vector<bool> used_v(strack_v.size(),false);
      used_vv.push_back(used_v);
    }

    std::vector<std::vector<std::vector<std::array<size_t,3> > > > many_plane_cand_vvv;    
    std::vector<std::vector<std::vector<std::array<size_t,3> > > > two_plane_cand_vvv;
    many_plane_cand_vvv.resize(_num_planes);
    two_plane_cand_vvv.resize(_num_planes);
    for(auto& many_plane_cand_vv : many_plane_cand_vvv) many_plane_cand_vv.resize(_num_planes);
    for(auto& two_plane_cand_vv : two_plane_cand_vvv) two_plane_cand_vv.resize(_num_planes);
    
    // loop over all possible plane combinations on which we loop over all possible cluster combinations
    for(size_t seed1_idx=0; seed1_idx<_seed_plane_v.size(); ++seed1_idx) {

      auto const& seed1_plane = _seed_plane_v[seed1_idx];
      auto const& strack1_v   = strack_vv[seed1_plane];
      auto&       used1_v     = used_vv[seed1_plane];
      
      for(size_t seed2_idx=seed1_idx+1; seed2_idx<_seed_plane_v.size(); ++seed2_idx) {

	auto const& seed2_plane = _seed_plane_v[seed2_idx];
	auto const& strack2_v   = strack_vv[seed2_plane];
	auto&       used2_v     = used_vv[seed2_plane];

	LAROCV_INFO() << "Inspecting LinearTrack using plane " << seed1_plane << " and " << seed2_plane << std::endl;

	std::multimap<double,std::array<size_t,3> > score_map;
	
	// loop over possible combinations of strack1 and strack2
	for(size_t strack1_idx=0; strack1_idx<strack1_v.size(); ++strack1_idx) {

	  if(used1_v[strack1_idx]) continue;
	  auto const& strack1 = strack1_v[strack1_idx];
	  
	  for(size_t strack2_idx=0; strack2_idx<strack2_v.size(); ++strack2_idx) {

	    if(used2_v[strack2_idx]) continue;
	    auto const& strack2 = strack2_v[strack2_idx];

	    // Find possible combinations
	    larocv::data::Vertex3D cand11, cand22, cand12, cand21;
	    bool cand11_ok, cand22_ok, cand12_ok, cand21_ok;
	    cand11_ok = _geo.YZPoint(strack1.edge1, seed1_plane, strack2.edge1, seed2_plane, cand11);
	    cand12_ok = _geo.YZPoint(strack1.edge1, seed1_plane, strack2.edge2, seed2_plane, cand12);
	    cand21_ok = _geo.YZPoint(strack1.edge2, seed1_plane, strack2.edge1, seed2_plane, cand21);
	    cand22_ok = _geo.YZPoint(strack1.edge2, seed1_plane, strack2.edge2, seed2_plane, cand22);

	    if(!cand11_ok && !cand12_ok && !cand21_ok && !cand22_ok) continue;

	    LAROCV_DEBUG() << "LinearTrack2D pair " << strack1_idx << " and " << strack2_idx
			   << " have compatible pattern: ("
			   << (cand11_ok ? 1 : 0) << ","
			   << (cand12_ok ? 1 : 0) << ","
			   << (cand21_ok ? 1 : 0) << ","
			   << (cand22_ok ? 1 : 0) << ")"
			   << std::endl;

	    // Loop over other planes and find possible combination strack
	    std::array<double,4> sum_min_dist_v;
	    std::array<size_t,4> num_good_plane_v;
	    for(size_t cand_idx=0; cand_idx<4; ++cand_idx) {
	      sum_min_dist_v[cand_idx] = 1e9;
	      num_good_plane_v[cand_idx] = 0;
	    }
	    std::vector<std::array<size_t,4> > strack_idx_vv;
	    for(size_t plane=0; plane<_num_planes; ++plane) {
	      
	      if(plane == seed1_plane || plane == seed2_plane) continue;

	      // Loop over linear track on this plane, find the best representative strack
	      auto const& strack_v = strack_vv[plane];
	      std::array<double,4> min_dist_v;
	      std::array<size_t,4> strack_idx_v;
	      for(size_t cand_idx=0; cand_idx<4; ++cand_idx) {
		min_dist_v[cand_idx] = 1e9;
		strack_idx_v[cand_idx] = kINVALID_SIZE;
	      }
	      double dist=0;
	      for(size_t strack_idx=0; strack_idx < strack_v.size(); ++strack_idx) {
		if(used_vv[plane][strack_idx]) continue;
		auto const& strack = strack_v[strack_idx];
		// Find closest edge point
		if(cand11_ok) {
		  auto const& edge11 = cand11.vtx2d_v[plane].pt;
		  dist = geo2d::dist(edge11, strack.edge1);
		  if(dist < min_dist_v[0] && dist < _min_compat_dist) { min_dist_v[0] = dist; strack_idx_v[0] = strack_idx; }
		  dist = geo2d::dist(edge11, strack.edge2);
		  if(dist < min_dist_v[0] && dist < _min_compat_dist) { min_dist_v[0] = dist; strack_idx_v[0] = strack_idx; }
		}
		if(cand12_ok) {
		  auto const& edge12 = cand12.vtx2d_v[plane].pt;
		  dist = geo2d::dist(edge12, strack.edge1);
		  if(dist < min_dist_v[1] && dist < _min_compat_dist) { min_dist_v[1] = dist; strack_idx_v[1] = strack_idx; }
		  dist = geo2d::dist(edge12, strack.edge2);
		  if(dist < min_dist_v[1] && dist < _min_compat_dist) { min_dist_v[1] = dist; strack_idx_v[1] = strack_idx; }
		}
		if(cand21_ok) {
		  auto const& edge21 = cand21.vtx2d_v[plane].pt;
		  dist = geo2d::dist(edge21, strack.edge1);
		  if(dist < min_dist_v[2] && dist < _min_compat_dist) { min_dist_v[2] = dist; strack_idx_v[2] = strack_idx; }
		  dist = geo2d::dist(edge21, strack.edge2);
		  if(dist < min_dist_v[2] && dist < _min_compat_dist) { min_dist_v[2] = dist; strack_idx_v[2] = strack_idx; }
		}
		if(cand22_ok) {
		  auto const& edge22 = cand22.vtx2d_v[plane].pt;
		  dist = geo2d::dist(edge22, strack.edge1);
		  if(dist < min_dist_v[3] && dist < _min_compat_dist) { min_dist_v[3] = dist; strack_idx_v[3] = strack_idx; }
		  dist = geo2d::dist(edge22, strack.edge2);
		  if(dist < min_dist_v[3] && dist < _min_compat_dist) { min_dist_v[3] = dist; strack_idx_v[3] = strack_idx; }
		}
	      }
	      for(size_t cand_idx=0; cand_idx<4; ++cand_idx) {

		if(strack_idx_v[cand_idx] == kINVALID_SIZE) continue;

		if(sum_min_dist_v[cand_idx]== 1e9)
		  sum_min_dist_v[cand_idx]  = min_dist_v[cand_idx];
		else
		  sum_min_dist_v[cand_idx] += min_dist_v[cand_idx];
		
		num_good_plane_v[cand_idx] += 1;
	      }
	      strack_idx_vv.push_back(strack_idx_v);
	    }
	    // Now let's make a decision on 4 candidate cases
	    size_t best_cand_idx = kINVALID_SIZE;
	    double min_dist=1e9;
	    for(size_t cand_idx=0; cand_idx<4; ++cand_idx) {
	      if(num_good_plane_v[cand_idx] == 0) continue;
	      double dist = sum_min_dist_v[cand_idx] / num_good_plane_v[cand_idx];
	      if(dist < min_dist) {
		best_cand_idx = cand_idx;
		min_dist = dist;
	      }
	    }
	    // register w/ score, strack0_idx, strack1_idx, 00 or 01 info
	    std::array<size_t,3> val;
	    val[0] = strack1_idx;
	    val[1] = strack2_idx;
	    val[2] = best_cand_idx;
	    if(best_cand_idx == kINVALID_SIZE) {
	      if(cand11_ok && cand22_ok) best_cand_idx = 0;
	      else if(cand12_ok && cand21_ok) best_cand_idx = 1;
	      else if(cand11_ok) best_cand_idx = 0;
	      //if(cand11_ok) best_cand_idx = 0;
	      else if(cand22_ok) best_cand_idx = 3;
	      else if(cand12_ok) best_cand_idx = 1;
	      else if(cand21_ok) best_cand_idx = 2;
	      val[2] = best_cand_idx;
	      LAROCV_DEBUG() << "Possible 2-plane match ... chosen candidate: " << best_cand_idx
			     << std::endl;
	      two_plane_cand_vvv[seed1_plane][seed2_plane].push_back(val);
	    }else{
	      LAROCV_DEBUG() << "Possible many-plane match ... chosen candidate: " << best_cand_idx
			     << std::endl;
	      score_map.emplace(min_dist,val);
	    }
	  } // end looping over strack2
	} // end looping over strack1
	
	// Pick candidate pairs based on scores
	LAROCV_DEBUG() << "Looping over many-plane match score map..." << std::endl;
	for(auto const& dist_cand_pair : score_map) {
	  auto const& strack1_idx = dist_cand_pair.second[0];
	  auto const& strack2_idx = dist_cand_pair.second[1];
	  LAROCV_DEBUG() << "Score " << dist_cand_pair.first
			 << " ... plane (" << seed1_plane << "," << seed2_plane << ")"
			 << " tracks (" << strack1_idx << "," << strack2_idx << ")"
			 << std::endl;
	  if(used1_v[strack1_idx]) continue;
	  if(used2_v[strack2_idx]) continue;
	  LAROCV_INFO() << "Many-plane match"
			<< " ... plane (" << seed1_plane << "," << seed2_plane << ")"
			<< " tracks (" << strack1_idx << "," << strack2_idx << ")"
			<< std::endl;
	  many_plane_cand_vvv[seed1_plane][seed2_plane].push_back(dist_cand_pair.second);
	  used1_v[strack1_idx] = true;
	  used2_v[strack2_idx] = true;
	}
      } // end looping over 2nd plane of two plane pairs
    } // end looping over 1st plane of two plane pairs

    std::vector<data::LinearTrack3D> res_v;

    // now we have two and many plane candidates ... register
    for(size_t plane1=0; plane1<_num_planes; ++plane1) {
      for(size_t plane2=0; plane2<_num_planes; ++plane2) {
	if(plane1 == plane2) continue;
	auto const& cand_info_v = many_plane_cand_vvv[plane1][plane2];
	LAROCV_DEBUG() << "Checking many-plane match on plane ("
		       << plane1 << "," << plane2 << ") ... "
		       << cand_info_v.size() << " vertex candidates" << std::endl;
	
	for(auto const& cand_info : cand_info_v) {

	  larocv::data::LinearTrack3D res;
	  res.edge1.vtx2d_v.resize(_num_planes);
	  res.edge2.vtx2d_v.resize(_num_planes);
	  std::vector<larocv::data::LinearTrack2D> res_strack_v(_num_planes);
	  auto const& strack1 = strack_vv[plane1][cand_info[0]];
	  auto const& strack2 = strack_vv[plane2][cand_info[1]];
	  res_strack_v[plane1] = strack1;
	  res_strack_v[plane2] = strack2;
	  // re-generate YZ point
	  auto const& cand_type = cand_info[2];
	  bool edge2_ok = false;
	  if (cand_type == 0) {
	    _geo.YZPoint(strack1.edge1, plane1, strack2.edge1, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge2, plane1, strack2.edge2, plane2, res.edge2);
	  }
	  else if (cand_type == 1) {
	    _geo.YZPoint(strack1.edge1, plane1, strack2.edge2, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge2, plane1, strack2.edge1, plane2, res.edge2);
	    std::swap(res_strack_v[plane2].edge1,res_strack_v[plane2].edge2);
	  }
	  else if (cand_type == 2) {
	    _geo.YZPoint(strack1.edge2, plane1, strack2.edge1, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge1, plane1, strack2.edge2, plane2, res.edge2);
	    std::swap(res_strack_v[plane1].edge1,res_strack_v[plane1].edge2);	    
	  }else {
	    _geo.YZPoint(strack1.edge2, plane1, strack2.edge2, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge1, plane1, strack2.edge1, plane2, res.edge2);
	    std::swap(res_strack_v[plane1].edge1,res_strack_v[plane1].edge2);
	    std::swap(res_strack_v[plane2].edge1,res_strack_v[plane2].edge2);
	  }
	  LAROCV_INFO() << "Solid many-plane match ..."
			<< " plane " << plane1 << " @ " << res.edge1.vtx2d_v[plane1].pt
			<< " + plane " << plane2 << " @ " << res.edge1.vtx2d_v[plane2].pt
			<< " ... " << (edge2_ok ? "with another edge" : "without another edge")
			<< std::endl;

	  // Loop over other planes and find possible combination for edge1
	  for(size_t plane=0; plane<_num_planes; ++plane) {
	    auto const& edge2d = res.edge1.vtx2d_v.at(plane).pt;
	    double min_dist = 1e9;
	    double dist;
	    size_t best_strack_id = kINVALID_SIZE;
	    bool   swap_edges = false;
	    auto const& strack_v = strack_vv[plane];
	    for(size_t strack_id=0; strack_id<strack_v.size(); ++strack_id) {
	      if(used_vv[plane][strack_id]) continue;
	      dist = geo2d::dist(edge2d, strack_v[strack_id].edge1);
	      if(dist < min_dist && dist < _min_compat_dist) { min_dist = dist; best_strack_id = strack_id; swap_edges = false; }
	      dist = geo2d::dist(edge2d, strack_v[strack_id].edge2);
	      if(dist < min_dist && dist < _min_compat_dist) { min_dist = dist; best_strack_id = strack_id; swap_edges = true; }
	    }
	    
	    if(best_strack_id == kINVALID_SIZE) continue;

	    res_strack_v[plane] = strack_v[best_strack_id];
	    if(swap_edges)
	      std::swap(res_strack_v[plane].edge1,res_strack_v[plane].edge2);
	  }
	  res.move(std::move(res_strack_v));
	  res_v.emplace_back(std::move(res));
	}
      }
    }

    // now we have two and many plane candidates ... register
    for(size_t seed1_idx=0; seed1_idx<_seed_plane_v.size(); ++seed1_idx) {
      for(size_t seed2_idx=seed1_idx+1; seed2_idx<_seed_plane_v.size(); ++seed2_idx) {
	auto const& plane1 = _seed_plane_v[seed1_idx];
	auto const& plane2 = _seed_plane_v[seed2_idx];
	auto const& cand_info_v = two_plane_cand_vvv[plane1][plane2];
	for(auto const& cand_info : cand_info_v) {
	  if(used_vv[plane1][cand_info[0]]) continue;
	  if(used_vv[plane2][cand_info[1]]) continue;
	  larocv::data::LinearTrack3D res;
	  res.edge1.vtx2d_v.resize(_num_planes);
	  res.edge2.vtx2d_v.resize(_num_planes);
	  std::vector<larocv::data::LinearTrack2D> res_strack_v(_num_planes);
	  auto const& strack1 = strack_vv[plane1][cand_info[0]];
	  auto const& strack2 = strack_vv[plane2][cand_info[1]];
	  res_strack_v[plane1] = strack1;
	  res_strack_v[plane2] = strack2;
	  // re-generate YZ point
	  auto const& cand_type = cand_info[2];
	  bool edge2_ok = false;
	  if (cand_type == 0) {
	    _geo.YZPoint(strack1.edge1, plane1, strack2.edge1, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge2, plane1, strack2.edge2, plane2, res.edge2);
	  }
	  else if (cand_type == 1) {
	    _geo.YZPoint(strack1.edge1, plane1, strack2.edge2, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge2, plane1, strack2.edge1, plane2, res.edge2);
	    std::swap(res_strack_v[plane2].edge1,res_strack_v[plane2].edge2);
	  }
	  else if (cand_type == 2) {
	    _geo.YZPoint(strack1.edge2, plane1, strack2.edge1, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge1, plane1, strack2.edge2, plane2, res.edge2);
	    std::swap(res_strack_v[plane1].edge1,res_strack_v[plane1].edge2);	    
	  }else {
	    _geo.YZPoint(strack1.edge2, plane1, strack2.edge2, plane2, res.edge1);
	    edge2_ok = _geo.YZPoint(strack1.edge1, plane1, strack2.edge1, plane2, res.edge2);
	    std::swap(res_strack_v[plane1].edge1,res_strack_v[plane1].edge2);
	    std::swap(res_strack_v[plane2].edge1,res_strack_v[plane2].edge2);
	  }

	  LAROCV_INFO() << "Solid many-plane match ..."
			<< " plane " << plane1 << " @ " << res.edge1.vtx2d_v[plane1].pt
			<< " + plane " << plane2 << " @ " << res.edge1.vtx2d_v[plane2].pt
			<< " ... " << (edge2_ok ? "with another edge" : "without another edge")
			<< std::endl;

	  // Loop over other planes and find possible combination for edge1
	  for(size_t plane=0; plane<_num_planes; ++plane) {
	    auto const& edge2d = res.edge1.vtx2d_v.at(plane).pt;
	    double min_dist = 1e9;
	    double dist;
	    size_t best_strack_id = kINVALID_SIZE;
	    bool   swap_edges = false;
	    auto const& strack_v = strack_vv[plane];
	    for(size_t strack_id=0; strack_id<strack_v.size(); ++strack_id) {
	      if(used_vv[plane][strack_id]) continue;
	      dist = geo2d::dist(edge2d, strack_v[strack_id].edge1);
	      if(dist < min_dist && dist < _min_compat_dist) { min_dist = dist; best_strack_id = strack_id; swap_edges = false; }
	      dist = geo2d::dist(edge2d, strack_v[strack_id].edge2);
	      if(dist < min_dist && dist < _min_compat_dist) { min_dist = dist; best_strack_id = strack_id; swap_edges = true; }
	    }
	    
	    if(best_strack_id == kINVALID_SIZE) continue;

	    res_strack_v[plane] = strack_v[best_strack_id];
	    if(swap_edges)
	      std::swap(res_strack_v[plane].edge1,res_strack_v[plane].edge2);
	  }
	  res.move(std::move(res_strack_v));
	  res_v.emplace_back(std::move(res));
	}
      }
    }
    return res_v;
  }

}
#endif
