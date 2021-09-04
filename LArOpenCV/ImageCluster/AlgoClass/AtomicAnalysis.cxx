#ifndef ATOMICANALYSIS_CXX
#define ATOMICANALYSIS_CXX

#include "AtomicAnalysis.h"
#include "Geo2D/Core/Geo2D.h"
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/core/core.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
#endif
#include "LArOpenCV/Core/larbys.h"

namespace larocv {
  AtomicAnalysis::AtomicAnalysis():_logger(nullptr)
  {
    _pi_threshold   = 10;
    _dx_resolution  = 1.;
    
    _atomic_region_pad  = 5.;
    _atomic_contour_pad = 3.;
    
    _logger = &(logger::get("DBLogger"));
  }
  
  void AtomicAnalysis::Configure(const Config_t &pset) {
    int log_level = pset.get<int>("Verbosity",0);
    _logger->set((msg::Level_t)log_level);

    _pi_threshold   = pset.get<float>("PiThreshold",10);
    _dx_resolution  = pset.get<float>("dXResolution",1.);
    
    _atomic_region_pad  = pset.get<float>("AtomicRegionPad",5.);
    _atomic_contour_pad = pset.get<float>("AtomicContourPad",3.);
    
  }
  
  double
  AtomicAnalysis::DistanceAtom2Point(const data::AtomicContour& atom,
				     const geo2d::Vector<float>& point) const
  {
    geo2d::Vector<float> pt;
    double point_dist = 0;
    double closest_dist = 1e10;
    for(auto const& contour_pt : atom) {
      pt.x = contour_pt.x;
      pt.y = contour_pt.y;
      point_dist = geo2d::dist2(pt,point);
      if(point_dist < closest_dist) closest_dist = point_dist;
    }
    return sqrt(closest_dist);
  }
  
  
  std::vector<size_t>
  AtomicAnalysis::OrderAtoms(const data::TrackClusterCompound& cluster,
			     const geo2d::Vector<float>& start_) const
  {
    const auto& vtx2d = start_; 
    LAROCV_DEBUG() << "called for ClusterCompound ID " << cluster.ID()
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
      atom_dist2vtx = DistanceAtom2Point(atom, vtx2d);
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
  
  std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
  AtomicAnalysis::AtomsEdge(const data::TrackClusterCompound& cluster,
			   const geo2d::Vector<float>& start_,
			   const std::vector<size_t> atom_order_v) const
  {
    const auto& vtx2d = start_;
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
		     << " atom ID " << atom_index
		     << " of sz " << atom.size() << std::endl;
      // loop over points to find the end
      double max_dist = -1.0*kINVALID_DOUBLE;
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
      
      if(this->logger().level() <= msg::kINFO) {
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
  
  
  geo2d::Vector<float>
  AtomicAnalysis::ChooseStartPoint(data::TrackClusterCompound& cluscomp) const {
    
    geo2d::Vector<float> start(kINVALID_FLOAT,kINVALID_FLOAT);
    
    if (cluscomp.size() > 1) {
      
      const data::AtomicContour* start_atomic = nullptr;
      
      size_t ass_defect_id = kINVALID_SIZE;
      
      //return the first atomic you see with 1 defect
      for(const auto& atomic_contour : cluscomp) {
	const auto& ass_defects_id_v = atomic_contour.associated_defects();
	
	//find the first contour that has a single defect
	if (ass_defects_id_v.size() == 1) {
	  start_atomic  = &atomic_contour;
	  ass_defect_id = ass_defects_id_v[0];
	  break;
	}
	
      }
      
      if ( !start_atomic)
	throw larbys("Could not choose starting atomic based on ass defects");
      
      const auto& pt_defect = cluscomp.get_defect(ass_defect_id);
      
      float max_dist = 0;
      //look for the farthest points away on the contour
      
      for(const auto& pt : *start_atomic) {
	geo2d::Vector<float> pt_f(pt.x,pt.y);
	auto dist = geo2d::dist(pt_f,pt_defect._pt_defect);
	if (dist > max_dist) {
	  max_dist = dist;
	  start = pt_f;
	}
      }
      
    }  else { // single cluster
      
      //Find the point farthest lowest in Y (wire)
      float maxy = kINVALID_FLOAT;
      for(const auto& pt : cluscomp.at(0))  {
	if (pt.y < maxy) {
	  maxy = pt.y;
	  start.x = (float) pt.x;
	  start.y = (float) pt.y;
	}
      }
      
      if (maxy == kINVALID_FLOAT)
	throw larbys("No maxy found");
    }
    
    
    if ( start.x == kINVALID_SIZE)
      throw larbys("Could not choose starting edge");
    
    return start;
  }
  std::vector<float>
  AtomicAnalysis::AtomdQdX(const cv::Mat& img,
			   const data::AtomicContour& atom,
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

      if (bin >= dqdx.size()) {
	LAROCV_WARNING() << "Skipping bin " << bin << std::endl;
	continue;
      }
      
      dqdx.at(bin) += q;

    }

    return dqdx;
  }

  void AtomicAnalysis::RefineAtomicEndPoint(const cv::Mat& img, data::AtomicContour& atom) const {
    cv::Mat thresh_img;
    ::cv::threshold(img, thresh_img, _pi_threshold, 1, cv::THRESH_BINARY);
    std::vector<geo2d::Vector<int> > nonzero_pts;
    cv::findNonZero(thresh_img, nonzero_pts);
    double min_dist=1e20;
    double dist;
    auto& end_pt = atom.edges()[1];
    geo2d::Vector<float> particle_end_pt = end_pt;
    for(auto const& pt : nonzero_pts) {
      if(cv::pointPolygonTest(atom,pt,false)<0)
	continue;
      dist = pow((pt.x - end_pt.x),2) + pow((pt.y - end_pt.y),2);
      if(dist < min_dist) {
	particle_end_pt.x = pt.x;
	particle_end_pt.y = pt.y;
	min_dist = dist;
      }
    }
    end_pt = particle_end_pt;
  }

}
#endif
  
