#ifndef ATOMICANALYSIS_CXX
#define ATOMICANALYSIS_CXX

#include "AtomicAnalysis.h"
#include "Geo2D/Core/Geo2D.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  void AtomicAnalysis::Configure(const Config_t &pset) {
    int log_level = pset.get<int>("Verbosity",0);
    _logger->set((larocv::msg::Level_t)log_level);
  }
  
  void AtomicAnalysis::AssociateDefects(const larocv::data::ClusterCompound& cluscomp,
				       const larocv::data::AtomicContour& parent,
				       const larocv::data::ContourDefect& defect,
				       larocv::data::AtomicContour& child1,
				       larocv::data::AtomicContour& child2)
  {
    
    if(this->logger().level() <= ::larocv::msg::kDEBUG) {
      std::stringstream ss;
      ss << "Parent defect points:";
      for(auto const& defect_id : parent.associated_defects())
	ss << " ID " << defect_id << " @ " << cluscomp.get_defect(defect_id)._pt_defect;
      ss << " ... new defect ID " << defect.id() << " @ " << defect._pt_defect;
      LAROCV_DEBUG() << std::string(ss.str()) << std::endl;
    }
    // Three cases to handle
    // case 0) parent is the "original" (i.e. no associated defect)
    // case 1) parent is an "edge" (i.e. 1 associated defect)
    // case 2) parent is "non-edge" (i.e. 2 associated defects)
    
    // case 0) simply associate both children to the defect
    if(parent.associated_defects().empty()) {
      LAROCV_DEBUG() << "Case 0: defect " << defect.id() << " is associated with both children" << std::endl;
      child1.associate(defect.id());
      child2.associate(defect.id());
      return;
    }
    
    // case 1 and 2 both require to compute the "closest defects from history" to each child
    
    // Look for the closest defect in the parent (parent) for child 1
    double closest_defect1_dist1 = 1e20;
    double closest_defect1_dist2 = 1e20;
    size_t closest_defect1_id1 = larocv::kINVALID_SIZE;
    size_t closest_defect1_id2 = larocv::kINVALID_SIZE;
    for(auto const& defect_id : parent.associated_defects()) {
      auto const& defect_pt = cluscomp.get_defect(defect_id)._pt_defect;
      double dist = pointPolygonTest(child1,defect_pt,true) * -1.;
      if(dist <  closest_defect1_dist1) {
	// update 2nd closest defect
	closest_defect1_dist2 = closest_defect1_dist1;
	closest_defect1_id2   = closest_defect1_id1;
	closest_defect1_dist1 = dist;
	closest_defect1_id1 = defect_id;
      }else if(dist < closest_defect1_dist2) {
	closest_defect1_dist2 = dist;
	closest_defect1_id2   = defect_id;
      }
    }
    
    LAROCV_DEBUG() << "Child1 atom size " << child1.size()
		   << " closest defect " << closest_defect1_id1
		   << " @ " << cluscomp.get_defect(closest_defect1_id1)._pt_defect
		   << " dist = " << closest_defect1_dist1 << std::endl;
    
    // Look for the closest defect in the parent (parent) for child 2
    double closest_defect2_dist1 = 1e20;
    double closest_defect2_dist2 = 1e20;
    size_t closest_defect2_id1 = larocv::kINVALID_SIZE;
    size_t closest_defect2_id2 = larocv::kINVALID_SIZE;
    for(auto const& defect_id : parent.associated_defects()) {
      auto const& defect_pt = cluscomp.get_defect(defect_id)._pt_defect;
      double dist = pointPolygonTest(child2,defect_pt,true) * -1.;
      if(dist < closest_defect2_dist1) {
	// update 2nd closest defect
	closest_defect2_dist2 = closest_defect2_dist1;
	closest_defect2_id2   = closest_defect2_id1;
	closest_defect2_dist1 = dist;
	closest_defect2_id1   = defect_id;
      }else if(dist < closest_defect2_dist2) {
	closest_defect2_dist2 = dist;
	closest_defect2_id2   = defect_id;
      }
    }
    LAROCV_DEBUG() << "Child2 atom size " << child2.size()
		   << " closest defect " << closest_defect2_id1
		   << " @ " << cluscomp.get_defect(closest_defect2_id1)._pt_defect
		   << " dist = " << closest_defect2_dist1 << std::endl;
    
    // case 1) parent is an "edge"
    // In this case one of children must inherit "edge" feature.
    // The edge child should have a larger distance to the "closest" defect in the history (history = 1 defect anyway...)
    if(parent.associated_defects().size() == 1) {
      if(closest_defect1_dist1 > closest_defect2_dist1) {
	child1.associate(defect.id());
	child2.associate(defect.id());
	child2.associate(closest_defect2_id1);
	LAROCV_DEBUG() << "Case 1: new edge @ defect " << defect.id()
		       << " and non-edge defect " << closest_defect2_id1
		       << std::endl;
      }else{
	child1.associate(defect.id());
	child1.associate(closest_defect1_id1);
	child2.associate(defect.id());
	LAROCV_DEBUG() << "Case 1: new edge @ defect " << defect.id()
		       << " and non-edge defect " << closest_defect1_id1
		       << std::endl;
      }
      return;
    }
    
    // case 2) parent is "non-edge"
    child1.associate(defect.id());
    child1.associate(closest_defect1_id1);
    
    child2.associate(defect.id());
    if(closest_defect1_id1 == closest_defect2_id1) {
      child2.associate(closest_defect2_id2);
      LAROCV_DEBUG() << "Case 2: defect pairs (" << defect.id() << "," << closest_defect1_id1 << ")"
		     << " and (" << defect.id() << "," << closest_defect2_id2 << ")" << std::endl;
    }
    else {
      child2.associate(closest_defect2_id1);
      LAROCV_DEBUG() << "Case 2: defect pairs (" << defect.id() << "," << closest_defect1_id1 << ")"
		     << " and (" << defect.id() << "," << closest_defect2_id1 << ")" << std::endl;
    }    
    return;
  }
  
  
  double
  AtomicAnalysis::DistanceAtom2Point(const larocv::data::AtomicContour& atom,
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
  AtomicAnalysis::OrderAtoms(const larocv::data::ClusterCompound& cluster,
			     const geo2d::Vector<float>& start_) const
  {
    const auto& vtx2d = start_; 
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
    size_t closest_atom_id = larocv::kINVALID_SIZE;
    size_t start_atom_id = larocv::kINVALID_SIZE;
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
      size_t next_atom_id = larocv::kINVALID_SIZE;
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
	  if(next_atom_id != larocv::kINVALID_SIZE) {
	    LAROCV_CRITICAL() << "Detected a logic inconsistency in atom/defect association (duplicated association)" << std::endl;
	    throw larocv::larbys();
	  }
	  next_atom_id = ass_atom_id;
	  LAROCV_DEBUG() << "    ... found next atom ID " << next_atom_id << std::endl;
	  
	  // Note one can just "break" the loop here to save time.
	  // In this attempt we won't break to explicitly check the association's validity.
	}
	
      } // end looping over defects for this atom
      
      // make sure next atom is found
      if(next_atom_id == larocv::kINVALID_SIZE) {
	LAROCV_CRITICAL() << "Detected a logic inconsistency in atom/defect association (next atom not found)" << std::endl;
	throw larocv::larbys();
      }
      used_atom_id_v[next_atom_id] = true;
      result_v.push_back(next_atom_id);
      last_atom_id = next_atom_id;
    } // end looping over to order atoms
    
    return result_v;
  }
  
  std::vector<std::pair<geo2d::Vector<float>,geo2d::Vector<float> > >
  AtomicAnalysis::AtomsEdge(const larocv::data::ClusterCompound& cluster,
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
      throw larocv::larbys();
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
  
  
  geo2d::Vector<float>
  AtomicAnalysis::ChooseStartPoint(larocv::data::ClusterCompound& cluscomp) {
    
    geo2d::Vector<float> start(larocv::kINVALID_FLOAT,larocv::kINVALID_FLOAT);
    
    if (cluscomp.size() > 1) {
      
      const larocv::data::AtomicContour* start_atomic = nullptr;
      
      size_t ass_defect_id = larocv::kINVALID_SIZE;
      
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
	throw larocv::larbys("Could not choose starting atomic based on ass defects");
      
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
      float maxy = larocv::kINVALID_FLOAT;
      for(const auto& pt : cluscomp.at(0))  {
	if (pt.y < maxy) {
	  maxy = pt.y;
	  start.x = (float) pt.x;
	  start.y = (float) pt.y;
	}
      }
      
      if (maxy == larocv::kINVALID_FLOAT)
	throw larocv::larbys("No maxy found");
    }
    
    
    if ( start.x == larocv::kINVALID_SIZE)
      throw larocv::larbys("Could not choose starting edge");
    
    return start;
  }
  

}
#endif
  
