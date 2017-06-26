#ifndef __MATCHANALYSIS_CXX__
#define __MATCHANALYSIS_CXX__

#include "MatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/VectorAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"

namespace larocv {
  
  static MatchAnalysisFactory __global_MatchAnalysisFactory__;
  
  void MatchAnalysis::_Configure_(const Config_t &pset)
  {
    auto name_combined = pset.get<std::string>("VertexProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    auto name_particle = pset.get<std::string>("ParticleProducer");
    _particle_id=kINVALID_ALGO_ID;
    if (!name_particle.empty()) {
      _particle_id = this->ID(name_particle);
      if (_particle_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _break_contours = pset.get<bool>("BreakContours");

    if(_break_contours)
      _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
    
    _AtomicAnalysis.Configure(pset.get<Config_t>("AtomicAnalysis"));
    _VertexAnalysis.Configure(pset.get<Config_t>("VertexAnalysis"));

    _debug_match_ana = pset.get<bool>("DebugMatchAna",false);
  
    _tree = new TTree("MatchAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid" , &_roid  , "roid/I");
    _tree->Branch("vtxid", &_vtxid , "vtxid/I");
    _tree->Branch("x"    , &_x     , "x/D");
    _tree->Branch("y"    , &_y     , "y/D");
    _tree->Branch("z"    , &_z     , "z/D");

    _tree->Branch("par_pixel_ratio_v",&_par_pixel_ratio_v);
    _tree->Branch("par_valid_end_pt_v",&_par_valid_end_pt_v);
    _tree->Branch("par_end_pt_x_v",&_par_end_pt_x_v);
    _tree->Branch("par_end_pt_y_v",&_par_end_pt_y_v);
    _tree->Branch("par_end_pt_z_v",&_par_end_pt_z_v);
    _tree->Branch("par_n_planes_charge_v",&_par_n_planes_charge_v);
    _tree->Branch("par_3d_PCA_theta_estimate_v",&_par_3d_PCA_theta_estimate_v);
    _tree->Branch("par_3d_PCA_phi_estimate_v",&_par_3d_PCA_phi_estimate_v);
    _tree->Branch("par_3d_segment_theta_estimate_v",&_par_3d_segment_theta_estimate_v);
    _tree->Branch("par_3d_segment_phi_estimate_v",&_par_3d_segment_phi_estimate_v);
    _tree->Branch("vertex_n_planes_charge",&_vertex_n_planes_charge,"vertex_n_planes_charge/I");
    _tree->Branch("par_pca_end_x_v",&_par_pca_end_x_v);
    _tree->Branch("par_pca_end_y_v",&_par_pca_end_y_v);
    _tree->Branch("par_pca_end_z_v",&_par_pca_end_z_v);
    _tree->Branch("pca_end_in_fiducial_v",&_pca_end_in_fiducial_v);
    _tree->Branch("par_pca_end_len_v",&_par_pca_end_len_v);
    _roid = 0;
  }
  
  void MatchAnalysis::Clear() {
    _par_pixel_ratio_v.clear();
    _par_valid_end_pt_v.clear();
    _par_end_pt_x_v.clear();
    _par_end_pt_y_v.clear();
    _par_end_pt_z_v.clear();
    _par_n_planes_charge_v.clear();
    _par_3d_PCA_theta_estimate_v.clear();
    _par_3d_PCA_phi_estimate_v.clear();
    _par_3d_segment_theta_estimate_v.clear();
    _par_3d_segment_phi_estimate_v.clear();
    _vertex_n_planes_charge = kINVALID_INT;
    _par_pca_end_x_v.clear();
    _par_pca_end_y_v.clear();
    _par_pca_end_z_v.clear();
    _pca_end_in_fiducial_v.clear();
    _par_pca_end_len_v.clear();

  }

  void MatchAnalysis::_Process_() {
    
    if(NextEvent()) _roid=0;

    if(_debug_match_ana) ClearMatchAna();
    
    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);

    const auto& meta_v = MetaArray();
    for(const auto& meta : meta_v)
      _VertexAnalysis.ResetPlaneInfo(meta);
    
    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();

    _vtxid = -1;
    LAROCV_DEBUG() << "Got " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;

      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;

      Clear();

      size_t npar = par_id_v.size();
      
      _par_pixel_ratio_v.resize(npar);
      _par_valid_end_pt_v.resize(npar);
      _par_end_pt_x_v.resize(npar);
      _par_end_pt_y_v.resize(npar);
      _par_end_pt_z_v.resize(npar);
      _par_n_planes_charge_v.resize(npar);
      _par_3d_PCA_theta_estimate_v.resize(npar);
      _par_3d_PCA_phi_estimate_v.resize(npar);
      _par_3d_segment_theta_estimate_v.resize(npar);
      _par_3d_segment_phi_estimate_v.resize(npar);
      _par_pca_end_x_v.resize(npar);
      _par_pca_end_y_v.resize(npar);
      _par_pca_end_z_v.resize(npar);
      _par_pca_end_len_v.resize(npar);
      
      LAROCV_DEBUG() << "Got " << par_id_v.size() << " particles" << std::endl;
      LAROCV_DEBUG() << " & algo data particle vector sz " << particle_v.size() << std::endl;
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];

	LAROCV_DEBUG() << "At particle id " << par_idx << " @ " << par_id << std::endl;
	
	
	//
	// do something with this particle
	//
	auto& par_pixel_ratio = _par_pixel_ratio_v[par_idx];
	auto& par_valid_end_pt = _par_valid_end_pt_v[par_idx];
	auto& par_end_pt_x = _par_end_pt_x_v[par_idx];
	auto& par_end_pt_y = _par_end_pt_y_v[par_idx];
	auto& par_end_pt_z = _par_end_pt_z_v[par_idx];
	auto& par_n_planes_charge = _par_n_planes_charge_v[par_idx];
	auto& par_3d_segment_theta_estimate = _par_3d_segment_theta_estimate_v[par_idx];
	auto& par_3d_segment_phi_estimate = _par_3d_segment_phi_estimate_v[par_idx];
	auto& par_3d_PCA_theta_estimate = _par_3d_PCA_theta_estimate_v[par_idx];
	auto& par_3d_PCA_phi_estimate = _par_3d_PCA_phi_estimate_v[par_idx];
	auto& par_pca_end_x = _par_pca_end_x_v[par_idx];
	auto& par_pca_end_y = _par_pca_end_y_v[par_idx];
	auto& par_pca_end_z = _par_pca_end_z_v[par_idx];
	auto& pca_end_in_fiducial = _pca_end_in_fiducial_v[par_idx];
	auto& par_pca_end_len = _par_pca_end_len_v[par_idx];
	
	//
	// compute the number of planes this particle is on
	//
	par_n_planes_charge = 0;
	
	//
	// compute the pixel ratio
	// for 2 particles ratio small/large
	// for (small/large + middle/large) / 2
	//
	std::array<double,3> pixel_arr;
	for(auto& v : pixel_arr) v=kINVALID_DOUBLE;
	
	//
	// determine the end point of track clusters
	//
	std::array<data::TrackClusterCompound,3> cluscomp_arr;
	std::array<const data::TrackClusterCompound*,3> cluscomp_ptr_arr;
	for(auto& v : cluscomp_ptr_arr) v=nullptr;

	//
	// particle cluster loop
	//
	for(size_t plane=0; plane<3; ++plane) {
	LAROCV_DEBUG() << "@ plane " << plane <<std::endl;

	  const auto& pcluster = par._par_v[plane];
	  const auto& ctor = pcluster._ctor;

	  if(ctor.empty()) continue;
	  
	  par_n_planes_charge += 1;
	  
	  //
	  // compute the pixel ratio
	  //
	  pixel_arr[plane] = CountNonZero(adc_img_v.at(plane),ctor,0);
	  
	  //
	  // determine the end point of track clusters
	  //
	  auto& cluscomp = cluscomp_arr[plane]; 
	  cluscomp_ptr_arr[plane] = &cluscomp_arr[plane];
	  
	  if (_break_contours) {
	    LAROCV_DEBUG() << "Chose to break ctor sz " << ctor.size() << std::endl;
	    cluscomp = _DefectBreaker.BreakContour(ctor);
	  } else {
	    LAROCV_DEBUG() << "Chose _not_ to break ctor sz " << ctor.size() << std::endl;
	    data::AtomicContour atomic_ctor(ctor,0);
	    cluscomp.emplace_back(std::move(atomic_ctor));
	  }
	  
	  LAROCV_DEBUG() << "Broke 1 contour sz " << ctor.size()
			 << " into " << cluscomp.size()
			 << " atomics" << std::endl;

	  auto vtx2d = vtx3d.cvtx2d_v.at(plane);

	  LAROCV_DEBUG() << " @ 2d vtx " << vtx2d.center << std::endl;
	  auto ordered_atom_id_v = _AtomicAnalysis.OrderAtoms(cluscomp,vtx2d.center);
	  cluscomp.set_atomic_order(ordered_atom_id_v);
	  
	  auto atom_edges_v = _AtomicAnalysis.AtomsEdge(cluscomp, vtx2d.center, ordered_atom_id_v);
	  
	  for (size_t atom_id=0; atom_id<cluscomp.size(); ++atom_id) {
 	    auto& atomic = cluscomp.at(atom_id);
	    auto& start_end = atom_edges_v.at(atom_id);
	    LAROCV_DEBUG() << "@ atom " << atom_id
			   << " (start & end) (" << start_end.first << " & " << start_end.second << ")" << std::endl;
	    atomic.add_edge(start_end.first);
	    atomic.add_edge(start_end.second);
	    if (atomic.id() == ordered_atom_id_v.back()) {
	      //_AtomicAnalysis.RefineAtomicEndPoint(adc_img_v[plane],atomic);
	      cluscomp.set_end_pt(atomic.edges()[1]);
	    }
	  } // end this atomic
	} // end this plane cluster

	//
	// compute the pixel ratio 
	//
	std::sort(pixel_arr.begin(),pixel_arr.end());

	auto largest = pixel_arr.back();

	if (largest == kINVALID_DOUBLE) 
	  par_pixel_ratio = pixel_arr[0] / pixel_arr[1];
	else
	  par_pixel_ratio = (pixel_arr[0] + pixel_arr[1]) / (2 * largest);

	//
	// determine the end point of track clusters
	//
	data::Vertex3D end3d;
	auto edge_found = _VertexAnalysis.MatchEdge(cluscomp_ptr_arr,end3d);
	par_valid_end_pt = edge_found;
	
	if(edge_found) {
	  par_end_pt_x = end3d.x;
	  par_end_pt_y = end3d.y;
	  par_end_pt_z = end3d.z;
	}

	//
	// determine the 3D angle for this particle in 2 ways
	//
	par_3d_segment_theta_estimate = kINVALID_DOUBLE;
	par_3d_segment_phi_estimate   = kINVALID_DOUBLE;
	
	// using the segment
	if (edge_found) {
	  auto segment_angle = Angle3D(vtx3d,end3d);
	  par_3d_segment_theta_estimate = segment_angle.first;
	  par_3d_segment_phi_estimate   = segment_angle.second;
	}

	par_3d_PCA_theta_estimate = kINVALID_DOUBLE;
	par_3d_PCA_phi_estimate   = kINVALID_DOUBLE;
	
	// using the PCA
	auto space_pts_v = SpacePointsEstimate(par,thresh_img_v);
	
	auto pca_angle = Angle3D(space_pts_v,vtx3d);
	par_3d_PCA_theta_estimate = pca_angle.first;
	par_3d_PCA_phi_estimate   = pca_angle.second;

	//
	// Estimate the 3D end point
	//
	auto end_pt_3d = EndPoint3D(space_pts_v,
				    pca_angle.first,pca_angle.second,
				    vtx3d);
	
	auto start_end_dist = Distance3D(end_pt_3d,vtx3d);
	par_pca_end_x = end_pt_3d[0];
	par_pca_end_y = end_pt_3d[1];
	par_pca_end_z = end_pt_3d[2];
	par_pca_end_len = start_end_dist;

	data::Vertex3D end_pca;
	end_pca.x = end_pt_3d[0];
	end_pca.y = end_pt_3d[1];
	end_pca.z = end_pt_3d[2];
	
	pca_end_in_fiducial = _VertexAnalysis.CheckFiducial(end_pca);
	
	LAROCV_DEBUG() << "End particle " << par_idx << " @ " << par_id  << std::endl;
      } // end this particle
      
      //
      // determine n planes charge @ vertex
      //
      _vertex_n_planes_charge = 0;
      for(size_t plane=0; plane<3; ++plane) {
	const auto& pt2d = vtx3d.vtx2d_v[plane];
	geo2d::Circle<float> circle(pt2d.pt,6);
	auto npx = CountNonZero(MaskImage(thresh_img_v[plane],circle,0,false));
	if(npx) _vertex_n_planes_charge += 1;
      }

      if(_debug_match_ana) StoreMatchAna();
      _tree->Fill();
    } // end this vertex
    
    _roid += 1;
  }


  std::pair<float,float> MatchAnalysis::Angle3D(const data::Vertex3D& vtx1,
						const data::Vertex3D& vtx2) {
      
    LAROCV_DEBUG() << "Angle 3D from 2 vertex" << std::endl;
    auto res_dist = Distance(vtx1,vtx2);
    auto res_vtx  = Difference(vtx1,vtx2);
      
    if (res_dist == 0) throw larbys("Vertex1 and Vertex2 cannot be the same");
      
    auto cos = res_vtx.z / res_dist;
    //auto tan = res_vtx.y / res_vtx.x;
      
    auto arccos = std::acos(cos);
    auto arctan = std::atan2(res_vtx.y,res_vtx.x);
      
    LAROCV_DEBUG() << "rad: theta="<<arccos<<" phi="<<arctan<<std::endl;
    LAROCV_DEBUG() << "deg: theta="<<arccos*180/3.14<<" phi="<<arctan*180/3.14<<std::endl;
    return std::make_pair(arccos,arctan);
  }  


  std::vector<data::Vertex3D> MatchAnalysis::SpacePointsEstimate(const data::Particle& particle,
								 const std::vector<cv::Mat>& img_v) {
    // get the two largest particles clusters

    std::array<float,3> area_v;

    for(size_t plane=0; plane<particle._par_v.size(); ++plane) {
      const auto& pcluster = particle._par_v[plane];
      if (pcluster._ctor.empty()) {
	area_v[plane] = kINVALID_FLOAT;
	continue;
      }
      area_v[plane] = ContourArea(pcluster._ctor);
    }
    
    float plane0_sz, plane1_sz;
    plane0_sz = plane1_sz = -1.0*kINVALID_FLOAT;
    
    size_t plane0, plane1;
    plane0 = plane1 = kINVALID_SIZE;
    
    // get the largest
    for(size_t plane=0; plane<particle._par_v.size(); ++plane) {
      if (area_v[plane] == kINVALID_FLOAT) continue;
      if (area_v[plane] > plane0_sz) {
	plane0 = plane;
	plane0_sz = area_v[plane];
      }
    }
    
    // get the second largest
    for(size_t plane=0; plane<particle._par_v.size(); ++plane) {
      if (plane==plane0) continue;
      if (area_v[plane] == kINVALID_FLOAT) continue;
      if (area_v[plane] > plane1_sz) {
	plane1 = plane;
	plane1_sz = area_v[plane];
      }
    }
    
    assert(plane0_sz != -1.0*kINVALID_FLOAT);
    assert(plane1_sz != -1.0*kINVALID_FLOAT);
	   
    assert(plane0 != kINVALID_SIZE);
    assert(plane1 != kINVALID_SIZE);

    assert(plane0_sz >= plane1_sz);
    assert(plane0 != plane1);
    assert(plane0 < particle._par_v.size());
    assert(plane1 < particle._par_v.size());

    const auto& ctor0 = particle._par_v.at(plane0)._ctor;
    const auto& ctor1 = particle._par_v.at(plane1)._ctor;

    // get the list of points inside
    auto pxpts0_v = FindNonZero(MaskImage(img_v.at(plane0),ctor0,0,false));
    auto pxpts1_v = FindNonZero(MaskImage(img_v.at(plane1),ctor1,0,false));

    // make 3D point and store in cv::Mat for PCA
    const auto& geo = _VertexAnalysis.Geo();
    std::vector<bool> used_v(pxpts1_v.size(),false);
    
    std::vector<data::Vertex3D> vtx3d_v;
    vtx3d_v.reserve(pxpts1_v.size());
      
    for(size_t pxid0=0; pxid0 < pxpts0_v.size(); ++pxid0) {
      for(size_t pxid1=0; pxid1 < pxpts1_v.size(); ++pxid1) {
	if (used_v[pxid1]) continue;
	data::Vertex3D res;
	if (!geo.YZPoint(pxpts0_v.at(pxid0),plane0,
			 pxpts1_v.at(pxid1),plane1,
			 res)) continue;
	
	vtx3d_v.emplace_back(std::move(res));
	used_v.at(pxid1) = true;
      }
    }

    return vtx3d_v;
  }

  std::pair<float,float> MatchAnalysis::Angle3D(const data::Particle& particle,
						const std::vector<cv::Mat>& img_v,
						const data::Vertex3D& start3d) {

    auto space_pts_v = SpacePointsEstimate(particle,img_v);
    return Angle3D(space_pts_v,start3d);
  }
  
  std::pair<float,float> MatchAnalysis::Angle3D(const std::vector<data::Vertex3D>& vtx3d_v,
						const data::Vertex3D& start3d) {

    
    cv::Mat vertex_mat(vtx3d_v.size(), 3, CV_32FC1);
    
    for(size_t vtxid=0; vtxid<vtx3d_v.size(); ++vtxid) {
      vertex_mat.at<float>(vtxid, 0) = vtx3d_v.at(vtxid).x;
      vertex_mat.at<float>(vtxid, 1) = vtx3d_v.at(vtxid).y;
      vertex_mat.at<float>(vtxid, 2) = vtx3d_v.at(vtxid).z;
    }

    LAROCV_DEBUG() << "Calculating PCA for " << vertex_mat.rows << " 3D points" << std::endl;
    cv::PCA pca_ana(vertex_mat, cv::Mat(), CV_PCA_DATA_AS_ROW, 0);

    std::array<float,3> mean_v;
    std::array<float,3> eigen_v;

    for(size_t plane=0; plane<3; ++plane) {
      mean_v[plane] = pca_ana.mean.at<float>(0,plane);
      eigen_v[plane]= pca_ana.eigenvectors.at<float>(0,plane);
    }

    auto eigen_len = std::sqrt( eigen_v[0] * eigen_v[0] +
				eigen_v[1] * eigen_v[1] +
				eigen_v[2] * eigen_v[2] );
    
    
    LAROCV_DEBUG() << "PCA @ ("<<mean_v[0]<<","<<mean_v[1]<<","<<mean_v[2]
		   <<") dir: ("<<eigen_v[0]<<","<<eigen_v[1]<<","<<eigen_v[2]<<")"<<std::endl;
    

    // check if incoming start point is valid
    if (start3d.x != kINVALID_DOUBLE) {
      // determine if we should flip the eigen direction based on the 3D mean position
      assert(start3d.x != kINVALID_DOUBLE);
      assert(start3d.y != kINVALID_DOUBLE);
      assert(start3d.z != kINVALID_DOUBLE);

      std::array<float,3> mean_dir_v;
      mean_dir_v[0] = mean_v[0] - start3d.x;
      mean_dir_v[1] = mean_v[1] - start3d.y;
      mean_dir_v[2] = mean_v[2] - start3d.z;
      auto mean_dir_len = std::sqrt( mean_dir_v[0] * mean_dir_v[0] +
				     mean_dir_v[1] * mean_dir_v[1] +
				     mean_dir_v[2] * mean_dir_v[2] );
      mean_dir_v[0] /= mean_dir_len;
      mean_dir_v[1] /= mean_dir_len;
      mean_dir_v[2] /= mean_dir_len;

      //
      // implement direction handling, negative dot product, flip the sign on eigen
      //

      auto dot_product = mean_dir_v[0] * eigen_v[0] + mean_dir_v[1] * eigen_v[1] + mean_dir_v[2] * eigen_v[2];
      if (dot_product < 0) {
	eigen_v[0] *= -1;
	eigen_v[1] *= -1;
	eigen_v[2] *= -1;
      }
    }
      
    
    auto cos = eigen_v[2] / eigen_len;
    //auto tan = eigen_v[1] / eigen_v[0];

    auto arccos = std::acos(cos);
    auto arctan = std::atan2(eigen_v[1],eigen_v[0]);

    LAROCV_DEBUG() << "rad: theta="<<arccos<<" phi="<<arctan<<std::endl;
    LAROCV_DEBUG() << "deg: theta="<<arccos*180.0/3.14<<" phi="<<arctan*180.0/3.14<<std::endl;
    return std::make_pair(arccos,arctan);
  }


  std::array<float,3> MatchAnalysis::EndPoint3D(const std::vector<data::Vertex3D>& space_pts_v,
						const float theta, const float phi,
						const data::Vertex3D& start_pt) {
    auto pca_dir   = AsVector(theta,phi);
    auto vertex_pt = AsVector(start_pt.x, start_pt.y, start_pt.z);
    auto pca_pt    = Sum(vertex_pt,pca_dir);
    
    std::vector<std::array<float,3> > projected_v;
    projected_v.reserve(space_pts_v.size());
    
    for(const auto& vtx3d : space_pts_v) {
      auto pt = AsVector(vtx3d.x,vtx3d.y,vtx3d.z);
      projected_v.emplace_back(ClosestPoint(vertex_pt,pca_pt,pt));
    }

    // get the projected point that is farthest away
    float max_dist = -1.0*kINVALID_FLOAT;
    const std::array<float,3>* far_ptr = nullptr;
    for(const auto& pt : projected_v) {
      auto distance = Distance(pt,vertex_pt); 
      if ( distance > max_dist) {
	max_dist = distance;
	far_ptr = &pt;
      }
    }
    
    // project all points onto the PCA line
    if (far_ptr == nullptr) {
      LAROCV_WARNING() << "End point 3D could not be estimated" << std::endl;
      return {{kINVALID_FLOAT,kINVALID_FLOAT,kINVALID_FLOAT}};
    }

    return *far_ptr;
  }


  float MatchAnalysis::Distance3D(const std::array<float,3>& pt1,
				  const data::Vertex3D& vtx) {
    auto pt2 = AsVector(vtx.x,vtx.y,vtx.z);
    return Distance(pt1,pt2);
  }
  
  float MatchAnalysis::Distance3D(const data::Vertex3D& vtx,
				  const std::array<float,3>& pt1) {
    return Distance3D(pt1,vtx);
  }
  
  
  void MatchAnalysis::StoreMatchAna() {
    MatchAna match_ana;
    
    match_ana.par_pixel_ratio_v               = _par_pixel_ratio_v; 
    match_ana.par_valid_end_pt_v              = _par_valid_end_pt_v;
    match_ana.par_end_pt_x_v                  = _par_end_pt_x_v; 
    match_ana.par_end_pt_y_v                  = _par_end_pt_y_v; 
    match_ana.par_end_pt_z_v                  = _par_end_pt_z_v; 
    match_ana.par_n_planes_charge_v           = _par_n_planes_charge_v; 
    match_ana.par_3d_PCA_theta_estimate_v     = _par_3d_PCA_theta_estimate_v;
    match_ana.par_3d_PCA_phi_estimate_v       = _par_3d_PCA_phi_estimate_v; 
    match_ana.par_3d_segment_theta_estimate_v = _par_3d_segment_theta_estimate_v;
    match_ana.par_3d_segment_phi_estimate_v   = _par_3d_segment_phi_estimate_v; 
    match_ana.vertex_n_planes_charge          = _vertex_n_planes_charge;
    match_ana.par_pca_end_x_v                   = _par_pca_end_x_v;
    match_ana.par_pca_end_y_v                   = _par_pca_end_y_v;
    match_ana.par_pca_end_z_v                   = _par_pca_end_z_v;
    match_ana.par_pca_end_len_v                 = _par_pca_end_len_v;
      
    _match_ana_v.emplace_back(std::move(match_ana));
  }

    
  
}
#endif
