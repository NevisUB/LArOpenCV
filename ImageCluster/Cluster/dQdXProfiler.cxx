#ifndef __DQDXPROFILER_CXX__
#define __DQDXPROFILER_CXX__

#include "Core/VectorArray.h"
#include "Core/Geo2D.h"
#include "dQdXProfiler.h"
#include "AlgoData/VertexClusterData.h"
#include "AlgoData/LinearTrackClusterData.h"

//#include <typeinfo>

namespace larocv {

  /// Global larocv::dQdXProfilerFactory to register AlgoFactory
  static dQdXProfilerFactory __global_dQdXProfilerFactory__;

  void dQdXProfiler::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const pca_algo_name = pset.get<std::string>("PCACandidatesName");
    _pca_algo_id = this->ID( pca_algo_name );

    auto const vertextrack_algo_name = pset.get<std::string>("VertexTrackClusterName","");
    _vtxtrack_algo_id = kINVALID_ALGO_ID;
    if(!vertextrack_algo_name.empty()) {
      LAROCV_INFO() << "Using vertex track cluster algo: " << vertextrack_algo_name << std::endl;
      _vtxtrack_algo_id = this->ID( vertextrack_algo_name );
    }else{
      LAROCV_INFO() << "Not using vertex track cluster algo..." << std::endl;
    }

    auto const lineartrack_algo_name = pset.get<std::string>("LinearTrackClusterName","");
    _lintrack_algo_id = kINVALID_ALGO_ID;
    if(!lineartrack_algo_name.empty()) {
      LAROCV_INFO() << "Using linear track cluster algo: " << lineartrack_algo_name << std::endl;
      _lintrack_algo_id = this->ID(lineartrack_algo_name);
    }else{
      LAROCV_INFO() << "Not using linear track cluster algo..." << std::endl;
    }
    
    _pi_threshold   = 10;
    _dx_resolution  = 1.;

    _atomic_region_pad  = 5.;
    _atomic_contour_pad = 3.;
  }
  
  void dQdXProfiler::_Process_(const larocv::Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       larocv::ImageMeta& meta,
			       larocv::ROI& roi)
  {
    // need implementation for "clusters" per plane processing in future
    return;
  }

  double dQdXProfiler::DistanceAtom2Vertex(const data::AtomicContour& atom, const geo2d::Vector<float>& vtx2d) const
  {
    geo2d::Vector<float> pt;
    double point_dist = 0;
    double closest_dist = 1e10;
    for(auto const& contour_pt : atom) {
      pt.x = contour_pt.x;
      pt.y = contour_pt.y;
      point_dist = geo2d::dist2(pt,vtx2d);
      if(point_dist < closest_dist) closest_dist = point_dist;
    }
    return sqrt(closest_dist);
  }

  double dQdXProfiler::DistanceAtom2Vertex(const data::AtomicContour& atom, const data::CircleVertex& vtx2d) const
  {
    return DistanceAtom2Vertex(atom,vtx2d.center);
  }

  std::vector<size_t> dQdXProfiler::OrderAtoms(const data::ClusterCompound& cluster, const geo2d::Vector<float>& vtx2d) const
  {
    LAROCV_DEBUG() << "called for ClusterCompound ID " << cluster.id()
		   << " ... 2D vtx @ " << vtx2d << std::endl;
    // goal: order atoms in the cluster from start to the end of a trajectory
    // 0) find the atom closest to 2d vtx
    // 1) follow the association chain with defects to order the rest

    // Retrieve atoms
    auto const& atoms   = cluster.get_atoms();
    // Prepare result
    std::vector<size_t> result_v;
    result_v.reserve(atoms.size());

    // If no atoms, just return empty vector
    if(atoms.empty()) return result_v;
    result_v.resize(1);
    
    // Loop over atoms to find the closest to the 2D vertex on this plane
    double closest_atom_dist2vtx = 1e9;
    double start_atom_dist2vtx = 1e9;
    size_t closest_atom_id = kINVALID_SIZE;
    size_t start_atom_id = kINVALID_SIZE;
    double atom_dist2vtx = 0;
    LAROCV_DEBUG() << "Looping over " << atoms.size() << " atoms to find the closest one to the vtx" << std::endl;
    for(auto const& atom : atoms) {
      atom_dist2vtx = DistanceAtom2Vertex(atom, vtx2d);
      LAROCV_DEBUG() << "Atom " << atom.id()
		     << " distance = " << atom_dist2vtx
		     << " # defect = " << atom.associated_defects().size()
		     << std::endl;
      if(atom_dist2vtx < closest_atom_dist2vtx) {
	closest_atom_dist2vtx = atom_dist2vtx;
	closest_atom_id = atom.id();
      }
      if(atom.associated_defects().size() < 2) {
	if(atom_dist2vtx < start_atom_dist2vtx) {
	  start_atom_dist2vtx = atom_dist2vtx;
	  start_atom_id = atom.id();
	}
      }
    }
    LAROCV_DEBUG() << "Closest atom ID " << closest_atom_id << " distance " << closest_atom_dist2vtx << std::endl;
    LAROCV_DEBUG() << "Start   atom ID " << start_atom_id << " distance " << start_atom_dist2vtx << std::endl;

    result_v[0] = start_atom_id;
    
    // Do book keeping of used atom IDs
    size_t last_atom_id = result_v[0];
    std::vector<size_t> used_atom_id_v(atoms.size(),false);
    used_atom_id_v[last_atom_id] = true;

    // Loop over the rest of atoms to construct the chain
    while(result_v.size() < atoms.size()) {

      // retrieve last atom
      auto const& last_atom = cluster.get_atom(last_atom_id);
      // retrieve defects' id array
      auto const& defect_id_s = last_atom.associated_defects();

      LAROCV_DEBUG() << "Searching next atom (current ID=" << last_atom.id()
		     << ") asociated # defects = " << defect_id_s.size() << std::endl;

      // loop over associated defects, find the next associated atom
      size_t next_atom_id = kINVALID_SIZE;
      for(auto const& defect_id : defect_id_s) {
	LAROCV_DEBUG() << "  associated defect ID " << defect_id << std::endl;
	// retrieve a defect
	auto const& defect = cluster.get_defect(defect_id);
	// loop over associated atoms' id array
	for(auto const& ass_atom_id : defect.associated_atoms()) {
	  LAROCV_DEBUG() << "    association defect " << defect_id << " => atom " << ass_atom_id << std::endl;
	  // if found in "used atom id list", ignore
	  if(used_atom_id_v[ass_atom_id]) continue;
	  // else this atom is our target. make sure there's no duplication
	  if(next_atom_id != kINVALID_SIZE) {
	    LAROCV_CRITICAL() << "Detected a logic inconsistency in atom/defect association (duplicated association)" << std::endl;
	    throw larbys();
	  }
	  next_atom_id = ass_atom_id;
	  LAROCV_DEBUG() << "    ... found next atom ID " << next_atom_id << std::endl;

	  // Note one can just "break" the loop here to save time.
	  // In this attempt we won't break to explicitly check the association's validity.
	}

      } // end looping over defects for this atom

      // make sure next atom is found
      if(next_atom_id == kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Detected a logic inconsistency in atom/defect association (next atom not found)" << std::endl;
	throw larbys();
      }
      used_atom_id_v[next_atom_id] = true;
      result_v.push_back(next_atom_id);
      last_atom_id = next_atom_id;
    } // end looping over to order atoms

    return result_v;
  }

  std::vector<size_t> dQdXProfiler::OrderAtoms(const data::ClusterCompound& cluster, const data::CircleVertex& vtx2d) const
  {
    return OrderAtoms(cluster,vtx2d.center);
  }

  std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
  dQdXProfiler::AtomsEdge(const data::ClusterCompound& cluster,
			  const geo2d::Vector<float>& vtx2d,
			  const std::vector<size_t> atom_order_v) const
  {
    // goal: find the start/end of each atom
    // 0) loop over ordered atoms starting from the closest to the vertex
    // 1) for each atom compute the end by comparing w.r.t. the end of the last atom (1st one compare w/ vtx)
    // 2) for each atom compute the start by comparing its own end
    if(atom_order_v.size() != cluster.get_atoms().size()) {
      LAROCV_CRITICAL() << "Atom's ordering index array length != # atoms!" << std::endl;
      throw larbys();
    }

    // prepare result container
    std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > > result_v;
    result_v.resize(atom_order_v.size());
    // loop over atoms
    auto const& atoms = cluster.get_atoms();
    geo2d::Vector<float> pt, start, end, last_end;
    last_end = vtx2d;
    for(size_t order_index=0; order_index < atom_order_v.size(); ++order_index) {
      auto const& atom_index = atom_order_v[order_index];
      auto const& atom = atoms.at(atom_index);
      LAROCV_DEBUG() << "Inspecting order " << order_index
		     << " atom ID " << atom_index << std::endl;
      // loop over points to find the end
      double max_dist = 0;
      for(auto const& ctor_pt : atom) {
	pt.x = ctor_pt.x;
	pt.y = ctor_pt.y;
	auto dist = geo2d::dist(pt,last_end);
	if(dist > max_dist) {
	  max_dist = dist;
	  end = pt;
	  LAROCV_DEBUG() << "New end point candidate @ " << pt << " dist = " << max_dist << std::endl;
	}
      }
      // loop over points to find the start
      max_dist = 0;
      for(auto const& ctor_pt : atom) {
	pt.x = ctor_pt.x;
	pt.y = ctor_pt.y;
	auto dist = geo2d::dist(pt,end);
	if(dist > max_dist) {
	  max_dist = dist;
	  start = pt;
	  LAROCV_DEBUG() << "New start point candidate @ " << pt << " dist = " << max_dist << std::endl;
	}
      }
      // record start/end + update last_end
      last_end = end;
      result_v[atom_index].first  = start;
      result_v[atom_index].second = end;

      if(this->logger().level() <= larocv::msg::kINFO) {
	std::stringstream ss;
	ss << "Atom ID " << atom.id() << " order index " << order_index
	   << " start @ " << start
	   << " end @ " << end
	   << " # defects " << atom.associated_defects().size();
	for(auto const& defect_id : atom.associated_defects())
	  ss << " ... defect " << defect_id << " @ " << cluster.get_defect(defect_id)._pt_defect;
	LAROCV_INFO() << std::string(ss.str()) << std::endl;
      }
    }
    return result_v;
  }

  std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
  dQdXProfiler::AtomsEdge(const data::ClusterCompound& cluster,
			  const data::CircleVertex& vtx2d,
			  const std::vector<size_t> atom_order_v) const
  {
    return AtomsEdge(cluster,vtx2d.center,atom_order_v);
  }
  
  bool dQdXProfiler::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "processing" << std::endl;
    auto const& pca_data = AlgoData<data::PCACandidatesData>  (_pca_algo_id);
    auto const& def_data = AlgoData<data::DefectClusterData>  (pca_data._input_id);

    if ( _lintrack_algo_id != kINVALID_ALGO_ID  && _vtxtrack_algo_id != kINVALID_ALGO_ID)
      { LAROCV_CRITICAL() << "Specify only vertextrack or lineartrack algo name" << std::endl; throw larbys(); }

    if ( _lintrack_algo_id == kINVALID_ALGO_ID  && _vtxtrack_algo_id == kINVALID_ALGO_ID)
      { LAROCV_CRITICAL() << "Specify only vertextrack or lineartrack algo name" << std::endl; throw larbys(); }

    auto& data = AlgoData<data::dQdXProfilerData>();
        
    if( _vtxtrack_algo_id != kINVALID_ALGO_ID ) {
      LAROCV_DEBUG() << "VertexTrackAlgoID: " << _vtxtrack_algo_id << std::endl;
      auto const& vtx_data = AlgoData<data::VertexClusterArray> (_vtxtrack_algo_id);

      // Loop over vertex
      for(auto const& vtx_clusters : vtx_data._vtx_cluster_v) {

	// Retrieve vertex ID and 3D vertex
	auto const  vtx_id  = vtx_clusters.id();
	auto const& vtx3d   = vtx_clusters.get_vertex();

	// Retrieve particle compound (cluster + atoms) information for this vertex
	auto const& vtx_compound_array = def_data.get_vertex_cluster(vtx_id);

	// Construct result container that stores clusters' dqdx per vertex candidate
	data::ParticledQdXArray vtx_dqdx;

	// Loop over planes
	for(size_t plane_id=0; plane_id<vtx_clusters.num_planes(); ++plane_id) {

	  LAROCV_INFO() << "Inspecting dQ/dX for vertex ID " << vtx3d.id()
			<< " plane " << plane_id
			<< " with " << vtx_compound_array.get_cluster(plane_id).size() << " particles..."
			<< std::endl;

	  // Retrieve 2D vertex on this plane
	  auto const vtx2d = vtx_clusters.get_circle_vertex(plane_id);

	  // Retrieve this plane's image
	  if(plane_id >= img_v.size()) {
	    LAROCV_CRITICAL() << "Vertex stored have a cluster on plane " << plane_id
			      << " for which no image is found!" << std::endl;
	    throw larbys();
	  }
	  cv::Mat img = img_v[plane_id];
	  //::cv::threshold(img, thresh_img, 10,255,CV_THRESH_BINARY);
	
	  // Loop over particles to create dQ/dX per particle on this plane
	  for(auto const& particle : vtx_compound_array.get_cluster(plane_id)) {

	    // Goal: construct dQ/dX per particle and store in ParticledQdX object
	    // 0) order atoms from vertex to the end of trajectory
	    // 1) construct dq/dx per particle by looping over ordered atoms

	    // result container
	    data::ParticledQdX part_dqdx;
	    
	    // order atoms
	    auto const ordered_atom_id_v = OrderAtoms(particle,vtx2d);
	  
	    // get start/end
	    auto atom_edges_v = AtomsEdge(particle, vtx2d, ordered_atom_id_v);
	    
	    // loop atoms (from last one)
	    for(auto const& atom_id : ordered_atom_id_v) {
	      // retrieve atom
	      auto const& atom = particle.get_atom(atom_id);
	      // retrieve PCA
	      auto const  pca_id = pca_data.index_atom(atom_id, particle.id(), plane_id, vtx_id);
	      auto const& pca = pca_data.line(pca_id);
	      // retrieve start/end
	      auto& start_end = atom_edges_v[atom_id];
	      // construct dq/dx
	      std::vector<float> atom_dqdx;
	      try { 
		atom_dqdx = AtomdQdX(img, atom, pca, start_end.first, start_end.second);
	      } catch (const larbys& err) {
		LAROCV_NORMAL() << "AtomdQdX could not be discerned" << std::endl;
		atom_dqdx = {};
	      }
	      // last atom's end point can be improved by the nearest non-zero pixel search
	      if(atom_id == ordered_atom_id_v.back()) {
		cv::Mat thresh_img;
		::cv::threshold(img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
		std::vector<geo2d::Vector<int> > nonzero_pts;
		cv::findNonZero(thresh_img, nonzero_pts);
		double min_dist=1e20;
		double dist;
		geo2d::Vector<float> particle_end_pt = start_end.second;
		for(auto const& pt : nonzero_pts) {
		  if(cv::pointPolygonTest(atom,pt,false)<0)
		    continue;
		  dist = pow((pt.x - start_end.second.x),2) + pow((pt.y - start_end.second.y),2);
		  if(dist < min_dist) {
		    particle_end_pt.x = pt.x;
		    particle_end_pt.y = pt.y;
		    min_dist = dist;
		  }
		}
		start_end.second = particle_end_pt;
	      }
	      // append
	      part_dqdx.push_back(atom_id, start_end.first, start_end.second, atom_dqdx);
	    }// end loop over atoms to create dq/dx

	    // register to vtx-wise dqdx collection
	    vtx_dqdx.move(plane_id, particle.id(), std::move(part_dqdx));

	  }// end loop over particles for one plane

	}// end loop over planes for one vertex

	// retiger vtx-wise dqdx to algo data
	data.move(vtx_id, std::move(vtx_dqdx));

      } // end loop over vertecies in this event

      // done
      return true;
    }
    else if( _lintrack_algo_id != kINVALID_ALGO_ID ) {

      LAROCV_DEBUG() << "LinearTrackAlgoID: " << _lintrack_algo_id << std::endl;
      
      auto const& lintrack_data = AlgoData<data::LinearTrackArray> (_lintrack_algo_id);
      
      // Loop over vertex
      for(auto const& trk_cluster : lintrack_data.get_clusters()) {

	// Retrieve track ID and 3D vertex
	auto const  trk_id  = trk_cluster.id();

	// Retrieve particle compound (cluster + atoms) information for this vertex
	auto const& trk_compound_array = def_data.get_vertex_cluster(trk_id);

	// Construct result container that stores clusters' dqdx per vertex candidate
	data::ParticledQdXArray vtx_dqdx;

	// Loop over planes
	for(size_t plane_id=0; plane_id < 3; ++plane_id) {

	  //catch the case of no good edge point in this view
	  try {
	    LAROCV_INFO() << "Inspecting dQ/dX for edge " << " n/a "
			  << " plane " << plane_id
			  << " with " << trk_compound_array.get_cluster(plane_id).size() << " particles..."
			  << std::endl;
	    if (!trk_compound_array.get_cluster(plane_id).size())
	      { LAROCV_INFO() << "No track compound array for plane id : " << plane_id << std::endl; continue; }
	  }
	  catch (const larbys& err) {
	    // There is no contour here
	    LAROCV_INFO() << "No track compound array for plane id : " << plane_id << std::endl;
	    continue;
	  }

	  const auto& trk_cluster2d = trk_cluster.get_cluster(plane_id);

	  //just choose the first edge
	  
	  geo2d::Vector<float> vtx2d = trk_cluster2d.edge2.x > (float) 1e6 ? trk_cluster2d.edge1 : trk_cluster2d.edge2;
	  if ( vtx2d.x > (float) 1.e6 ) throw larbys(" Both edge1 and edge2 are fucked? ");
	  
	  // Retrieve this plane's image
	  if(plane_id >= img_v.size()) {
	    LAROCV_CRITICAL() << "Vertex stored have a cluster on plane " << plane_id
			      << " for which no image is found!" << std::endl;
	    throw larbys();
	  }
	  cv::Mat img = img_v[plane_id];
	  //::cv::threshold(img, thresh_img, 10,255,CV_THRESH_BINARY);
	
	  // Loop over particles to create dQ/dX per particle on this plane
	  for(auto const& particle : trk_compound_array.get_cluster(plane_id)) {

	    // Goal: construct dQ/dX per particle and store in ParticledQdX object
	    // 0) order atoms from vertex to the end of trajectory
	    // 1) construct dq/dx per particle by looping over ordered atoms

	    // result container
	    data::ParticledQdX part_dqdx;

	    // order atoms
	    auto const ordered_atom_id_v = OrderAtoms(particle,vtx2d);
	  
	    // get start/end
	    auto atom_edges_v = AtomsEdge(particle, vtx2d, ordered_atom_id_v);

	    // loop atoms (from last one)
	    for(auto const& atom_id : ordered_atom_id_v) {
	      // retrieve atom
	      auto const& atom = particle.get_atom(atom_id);
	      // retrieve PCA
	      auto const  pca_id = pca_data.index_atom(atom_id, particle.id(), plane_id, trk_id);
	      auto const& pca = pca_data.line(pca_id);
	      // retrieve start/end
	      auto& start_end = atom_edges_v[atom_id];
	      // construct dq/dx
	      std::vector<float> atom_dqdx;
	      try { 
		atom_dqdx = AtomdQdX(img, atom, pca, start_end.first, start_end.second);
	      } catch (const larbys& err) {
		LAROCV_NORMAL() << "AtomdQdX could not be discerned" << std::endl;
		atom_dqdx = {};
	      }
	      // last atom's end point can be improved by the nearest non-zero pixel search
	      if(atom_id == ordered_atom_id_v.back()) {
		cv::Mat thresh_img;
		::cv::threshold(img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
		std::vector<geo2d::Vector<int> > nonzero_pts;
		cv::findNonZero(thresh_img, nonzero_pts);
		double min_dist=1e20;
		double dist;
		geo2d::Vector<float> particle_end_pt = start_end.second;
		for(auto const& pt : nonzero_pts) {
		  if(cv::pointPolygonTest(atom,pt,false)<0)
		    continue;
		  dist = pow((pt.x - start_end.second.x),2) + pow((pt.y - start_end.second.y),2);
		  if(dist < min_dist) {
		    particle_end_pt.x = pt.x;
		    particle_end_pt.y = pt.y;
		    min_dist = dist;
		  }
		}
		start_end.second = particle_end_pt;
	      }
	      // append
	      part_dqdx.push_back(atom_id, start_end.first, start_end.second, atom_dqdx);
	    }// end loop over atoms to create dq/dx

	    // register to trk-wise dqdx collection
	    vtx_dqdx.move(plane_id, particle.id(), std::move(part_dqdx));

	  }// end loop over particles for one plane

	}// end loop over planes for one vertex

	// retiger trk-wise dqdx to algo data
	data.move(trk_id, std::move(vtx_dqdx));

      } // end loop over vertecies in this event

      // done
      return true;
       
    } else {
      LAROCV_CRITICAL() << "Impossible condition reached!" << std::endl;
      throw larbys();
    }

    return false;
  }

  std::vector<float> dQdXProfiler::AtomdQdX(const cv::Mat& img, const data::AtomicContour& atom,
					    const geo2d::Line<float>& pca,
					    const geo2d::Vector<float>& start,
					    const geo2d::Vector<float>& end) const
  {
    // goal: construct dq/dx for a given atom along pca line
    // 0) identify the set of pixels for this atom
    // 1) project the set of pixels along pca
    // 2) construct binned dq/dx spectrum

    // Define a bounding box in which we will work based on contour + user defined pixel inflation
    auto const rect_rotated = cv::boundingRect( cv::Mat(atom) );
    auto tl = rect_rotated.tl();
    auto br = rect_rotated.br();

    // inflate the region by user defined pixel inflation on each side

    tl.x -= _atomic_region_pad;
    tl.y -= _atomic_region_pad;
    br.x += _atomic_region_pad;
    br.y += _atomic_region_pad;

    // Enforce image boundary condition
    if(tl.x < 0) tl.x = 0;
    if(tl.y < 0) tl.y = 0;
    if(br.x >= img.cols) br.x = img.cols-1;
    if(br.y >= img.rows) br.y = img.rows-1;

    auto width  = br.x-tl.x;
    auto height = br.y-tl.y;

    if (width  < 0) throw larbys("width < 0");
    if (height < 0) throw larbys("height < 0");
    
    // Crop the image
    auto const bbox = cv::Rect(tl.x, tl.y, width, height);

    LAROCV_DEBUG() << "Calculating dQ/dX for atom " << atom.id()
		   << " within the cropped image " << bbox << std::endl;
    
    auto const small_img = cv::Mat(img, bbox);
    
    // List points that we care
    std::vector<geo2d::Vector<int> > pts_in_bbox;
    cv::findNonZero(small_img, pts_in_bbox);
    LAROCV_DEBUG() << "Found " << pts_in_bbox.size() << " non zero points" << std::endl;
    // Cropped image is rectangular and some points in the image has nothing to do with an atom
    // Collect only points that matter to this atom by using user defined distance cut to a contour
    // Also correct its coordinate system w.r.t. bbox origin
    std::vector<geo2d::Vector<float> > pts;
    pts.reserve(pts_in_bbox.size());
    double dist_threshold = _atomic_contour_pad;
    for(auto const& pt : pts_in_bbox) {
      double dist = cv::pointPolygonTest(atom, pt, true);
      if(dist > dist_threshold) continue;
      geo2d::Vector<float> pt_float;
      pt_float.x = pt.x + tl.x;
      pt_float.y = pt.y + tl.y;
      pts.emplace_back(std::move(pt_float));
    }
    LAROCV_DEBUG() << "Found " << pts.size() << " non zero points including neighbors" << std::endl;
    if(pts.empty())
      return std::vector<float>();
    
    // compute a point projection on pca line
    // 0) loop over points, for each point find the closest point of appraoch, A, on the PCA line
    // 1) compute position of A w.r.t. pt1 that defines pca line (1D scalar)
    // 2) record two edge points to figure out ordering in inter-atom perspective

    size_t minpt_index, maxpt_index;
    minpt_index = maxpt_index = kINVALID_SIZE;
    double minpt_dist =  1e10;
    double maxpt_dist = -1e10;
    std::vector<float> pts_1dcoord;
    pts_1dcoord.resize(pts.size(),kINVALID_FLOAT);
    geo2d::Vector<float> scratch_pt1, scratch_pt2;
    
    for(size_t pt_index = 0; pt_index < pts.size(); ++pt_index) {

      auto const& pt = pts[pt_index];

      // find the projection point on a line + distance
      auto dist = geo2d::ClosestPoint(pca, pt, scratch_pt1, scratch_pt2);

      // find the distance from a reference point on line
      //dist = geo2d::dist(pca.pt,scratch_pt1);
      scratch_pt1 -= pca.pt;
      if(pca.dir.x > pca.dir.y) { dist = scratch_pt1.x / pca.dir.x; }
      else{ dist = scratch_pt1.y / pca.dir.y; }
      
      pts_1dcoord[pt_index] = dist;

      // record edge points
      if(dist < minpt_dist) {
	minpt_index = pt_index;
	minpt_dist  = dist;
      }
      if(dist > maxpt_dist) {
	maxpt_index = pt_index;
	maxpt_dist  = dist;
      }
    }

    LAROCV_DEBUG() << " Min pt index " << minpt_index 
		   << " Max pt index " << maxpt_index << std::endl;
    LAROCV_DEBUG() << " Min pt @ " << pts[minpt_index]
		   << " Max pt @ " << pts[maxpt_index] << std::endl;
    LAROCV_DEBUG() << " Start  @ " << start << std::endl;
    // Figure out the edge that should correspond to the start
    if(geo2d::dist(pts[minpt_index],start) < geo2d::dist(pts[maxpt_index],start)) {
      LAROCV_DEBUG() << " Start edge is Min pt @ " << pts[minpt_index] << std::endl;
      // minpt is closer to start ... shift 1D coordinate location accordingly
      for(auto& dist : pts_1dcoord)
	dist = fabs(dist - minpt_dist);
    }else{
      LAROCV_DEBUG() << " Start edge is Max pt @ " << pts[maxpt_index] << std::endl;
      // maxpt is closer to start ... shift 1D coordinate location accordingly
      for(auto& dist : pts_1dcoord)
	dist = fabs(dist - maxpt_dist);
    }

    // Create dqdx distribution
    size_t num_bins = (maxpt_dist - minpt_dist) / _dx_resolution + 1;
    std::vector<float> dqdx(num_bins,0.);
    // Loop over point coordinates to fill dqdx bins
    for(size_t pt_index=0; pt_index<pts.size(); ++pt_index) {

      auto const& pt = pts[pt_index];
      
      float q = (float)(img.at<unsigned char>((size_t)(pt.y),(size_t)(pt.x)));

      size_t bin = (pts_1dcoord[pt_index] / _dx_resolution);

      dqdx[bin] += q;

    }

    return dqdx;
  }  
  
}
#endif
