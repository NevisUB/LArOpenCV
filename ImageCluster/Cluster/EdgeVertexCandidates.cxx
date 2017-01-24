#ifndef __EDGEVERTEXCANDIDATES_CXX__
#define __EDGEVERTEXCANDIDATES_CXX__

#include "EdgeVertexCandidates.h"
#include "Core/Line.h"
//#include "AlgoData/PCACandidatesData.h"
#include "AlgoData/dQdXProfilerData.h"
#include "AlgoData/LinearTrackClusterData.h"
#include "AlgoFunction/ImagePatchAnalysis.h"
#include "AlgoFunction/Contour2DAnalysis.h"
#include "AlgoFunction/SpectrumAnalysis.h"

namespace larocv {

  /// Global larocv::EdgeVertexCandidatesFactory to register AlgoFactory
  static EdgeVertexCandidatesFactory __global_EdgeVertexCandidatesFactory__;

  void EdgeVertexCandidates::_Configure_(const ::fcllite::PSet &pset)
  {
    LAROCV_DEBUG() << "processing" << std::endl;

    auto const lintrack_algo_name = pset.get<std::string>("LinearTrackAlgoName","");
    
    if(lintrack_algo_name.empty()) {
      LAROCV_CRITICAL() << "LinearTrackAlgoName not specified" << std::endl; 
      throw larbys();
    } else {
      _lintrack_algo_id = this->ID(lintrack_algo_name);
      if(_lintrack_algo_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Invalid LinearTrack algorithm name given: "
			  << lintrack_algo_name
			  << std::endl;
	throw larbys();
      }
      LAROCV_NORMAL() << "EdgeVertexCandidates " << this->Name()
		      << " will use lintrack algorithm " << lintrack_algo_name
		      << std::endl;
      
    }
    
    auto const dqdx_algo_name = pset.get<std::string>("dQdXAlgoName","");

    if(dqdx_algo_name.empty()) {
      LAROCV_CRITICAL() << "dQdXAlgoName not specified" << std::endl; 
      throw larbys();
    } else {
      _dqdx_algo_id = this->ID(dqdx_algo_name);
      if(_dqdx_algo_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Invalid dqdx algorithm name given: "
			  << dqdx_algo_name
			  << std::endl;
	throw larbys();
      }
      LAROCV_NORMAL() << "EdgeVertexCandidates " << this->Name()
		      << " will use dQdX algorithm " << dqdx_algo_name
		      << std::endl;
      
    }
    
    Register(new data::EdgeVertexCandidatesData);
  }


  void EdgeVertexCandidates::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
    LAROCV_DEBUG() << "processing" << std::endl;
  }
  
  bool EdgeVertexCandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "processing" << std::endl;

    //const auto& pca_data    = AlgoData<data::PCACandidatesData>(_dqdx_algo_id-1);
    const auto& dqdx_data   = AlgoData<data::dQdXProfilerData>(_dqdx_algo_id,0);
    const auto& lintrk_data = AlgoData<data::LinearTrackArray>(_lintrack_algo_id,0);
    auto& edge_data   = AlgoData<data::EdgeVertexCandidatesData>(0);

    LAROCV_DEBUG() << "LinearTrackClusters: " << lintrk_data.get_clusters().size() << std::endl;
    LAROCV_DEBUG() << "VertexTrackClusters: " << dqdx_data.get_vertex_clusters().size() << std::endl;
    
    // for each linear track cluster
    for(const auto & trk_cluster : lintrk_data.get_clusters()) {

      const auto trk_id = trk_cluster.id();
      const auto& vtx_dqdx = dqdx_data.get_vertex_cluster(trk_id);
      
      LAROCV_DEBUG() << "Got vertex dqdx from track cluster id  " << trk_cluster.id() << std::endl;

      std::vector<geo2d::Vector<float> > edge_pt_v(3,geo2d::Vector<float>(kINVALID_FLOAT,kINVALID_FLOAT));
      std::vector<geo2d::Vector<float> > side_pt_v(3,geo2d::Vector<float>(kINVALID_FLOAT,kINVALID_FLOAT));


      larocv::data::EdgeVertexCandidateArray edge_vtx_candidate_array;
      
      for(uint plane_id=0; plane_id < 3; ++plane_id) {
	
	const auto& img = img_v[plane_id];
	
	auto& edge_pt = edge_pt_v[plane_id];
	//auto& side_pt = side_pt_v[plane_id];
	
	LAROCV_DEBUG() << "Plane id " << plane_id << std::endl;
	auto npar_clusters = vtx_dqdx.get_cluster(plane_id).size();

	if ( ! npar_clusters ) continue;
	if ( npar_clusters > 1 ) throw larbys("More than one particle cluster found?");

	const auto& part_dqdx = vtx_dqdx.get_cluster(plane_id).front();
	  
	//for (const auto& part_dqdx : vtx_dqdx.get_cluster(plane_id)) {
	  
	const auto& dqdx_v = part_dqdx.dqdx();
	LAROCV_DEBUG() << "Got dqdx length " << dqdx_v.size() << std::endl;

	//calculate the baseline over all atomics
	size_t pre  = 2;
	size_t post = 2;
	auto rmean_v  = RollingMean(dqdx_v,pre,post);
	auto rsigma_v = RollingSigma(dqdx_v,pre,post);

	auto base_rmean  = BinnedMaxOccurrence(rmean_v,100);
	auto base_rsigma = BinnedMaxOccurrence(rsigma_v,100);

	LAROCV_DEBUG() << "Mean of dqdx : " << Mean(dqdx_v) << std::endl;
	LAROCV_DEBUG() << "Sig  of dqdx : " << Sigma(dqdx_v) << std::endl;
	  
	LAROCV_DEBUG() << "Binned Mean of dqdx : " << base_rmean << std::endl;
	LAROCV_DEBUG() << "Binned Sig  of dqdx : " << base_rsigma << std::endl;

	//Get the global max of rolling mean, hope there is no duplicates
	auto max_itr = std::max_element(rmean_v.begin(),rmean_v.end());
	size_t max_index = std::distance(rmean_v.begin(),max_itr);
	auto   max_value = *max_itr;
	LAROCV_DEBUG() << "Found global max of rolled mean at position " << max_index << " with value " << max_value << std::endl;

	// What if our spectrum is noisy?
	float sigma_base = 5.0;
	if ( (base_rmean / base_rsigma)  < sigma_base ) 
	  LAROCV_INFO() << "Found a noisy spectrum! Careful!" << std::endl;
	  
	// NOTE: Handle case global max is smaller than f0*sigma

	float sigma_thresh = 5.0;
	float rsigma_bound = sigma_thresh * base_rsigma;
	  
	//Scan left and right, if we hit the edge (+/- pre/post) OR or go below sigma_thresh sigma, stop

	// counting index, left index, right index,
	size_t idx  = kINVALID_SIZE;
	size_t ridx = kINVALID_SIZE;
	size_t lidx = kINVALID_SIZE;

	// left edge hit, right edge hit
	bool ledge = false;
	bool redge = false;
	  
	// go right
	LAROCV_DEBUG() << "Checking right edge..." << std::endl;
	for(idx = max_index+1; idx < rmean_v.size() - post; ++idx) {
	  LAROCV_DEBUG() << "( " << rmean_v.at(idx) << "," << rsigma_bound << ")" << std::endl;
	  if ( rmean_v.at(idx) < base_rmean + rsigma_bound ) {
	    ridx = idx;
	    LAROCV_DEBUG() << "ridx set" << std::endl;
	    break;
	  }
	}

	if (ridx == kINVALID_SIZE) redge = true;

	// go left
	LAROCV_DEBUG() << "Checking left edge..." << std::endl;
	for(idx = max_index-1; idx > pre; --idx) {
	  LAROCV_DEBUG() << "( " << rmean_v.at(idx) << "," << rsigma_bound << ")" << std::endl;
	  if ( rmean_v.at(idx) < base_rmean + rsigma_bound ) {
	    lidx = idx;
	    LAROCV_DEBUG() << "lidx set" << std::endl;
	    break;
	  }
	}
	  
	if (lidx == kINVALID_SIZE) ledge = true;

	// both edges found
	if ( ledge && redge ) {
	  LAROCV_CRITICAL() << "Left and right edges found in scan" << std::endl;
	  continue;
	  //throw larbys();
	}

	// both edges NOT found
	if ( !ledge && !redge) {
	  LAROCV_INFO() << "Neither left or right edges found in scan" << std::endl;

	  // size_t d_left  = lidx - pre;
	  // size_t d_right = rmean_v.size() - post - ridx;

	  size_t d_left  = max_index - pre;
	  size_t d_right = rmean_v.size() - post - max_index;

	  //Assigned the edge closest to ridx or lidx;
	  if (d_left > d_right) redge = true;
	  if (d_left < d_right) ledge = true;

	  if ( !ledge && !redge ) throw larbys("Neither left or right edge could be determined, they are the same num?");
	    
	}
	
	LAROCV_DEBUG() << "ledge " << ledge << "... redge " << redge << std::endl;
	LAROCV_DEBUG() << "lidx  " << lidx  << "... ridx  " << ridx << std::endl;

	// set the edge point to the start point if its on the left edge
	// or the end point if its on the right edge
	edge_pt = ledge ? part_dqdx.start_pt() : part_dqdx.end_pt();

	LAROCV_DEBUG() << "Setting edge point " << edge_pt << std::endl;
	
	// Determine where along PCA the candidate vertex location to set side_pt
	size_t d_left  = rmean_v.size() - lidx;
	size_t d_right = ridx;
	size_t d_idx   = redge ? d_left : d_right; // take the shorter one
	
	//should be the index farthest from the chosen edge (lidx or ridx)
	idx = ledge ? ridx : lidx;
	
	size_t atom_index;
	
	LAROCV_DEBUG() << "idx " << idx << std::endl;

	size_t num_atoms = part_dqdx.atom_start_index_array().size();

	  for (atom_index=0; atom_index < num_atoms; ++atom_index) {
	    auto& atom_start_index = part_dqdx.atom_start_index_array()[atom_index];
	    LAROCV_DEBUG() << "Comparing (" << idx << "," << atom_start_index << ")" << std::endl;
	    if (atom_index == num_atoms-1) break;
	    if (idx >= atom_start_index) continue;
	    break;
	  }

	LAROCV_DEBUG() << "Got atom start index: " << atom_index << std::endl;

	//const auto  atom_id       = part_dqdx.atom_id_array()[atom_index];
	const auto& atom_start_pt = part_dqdx.atom_start_pt_array()[atom_index];
	
	LAROCV_DEBUG() << "Got dx resolution: " << part_dqdx.dx_resolution() << std::endl;
	LAROCV_DEBUG() << "Got atom start pt: " << atom_start_pt << std::endl;

	float radius = d_idx;
	// radius *= (float) part_dqdx.dx_resolution();

	LAROCV_DEBUG() << "Got radius: " << radius << std::endl;
	
	geo2d::Circle<float> start_pt_c(edge_pt,radius);
	
	auto qpts_v = QPointOnCircle(img,start_pt_c,10);
	
	LAROCV_DEBUG() << "Got " << qpts_v.size() << " number of qpts" << std::endl;

	larocv::data::EdgeVertexCandidate edge_vtx_c;

	edge_vtx_c.rmean_dqdx_v = rmean_v;
	edge_vtx_c.rsigma_dqdx_v = rsigma_v;

	edge_vtx_c.max_index = max_index;
	edge_vtx_c.max_value = max_value;

	edge_vtx_c.ledge = ledge;
	edge_vtx_c.redge = redge;
	
	edge_vtx_c.ridx = ridx;
	edge_vtx_c.lidx = lidx;
	edge_vtx_c.sidx = idx;

	size_t eidx = ledge ? 0 : rmean_v.size();
	edge_vtx_c.eidx = eidx;
	
	edge_vtx_c.edge_pt   = edge_pt;
	edge_vtx_c.side_pt_v = qpts_v;
	edge_vtx_c.circle    = start_pt_c;

	edge_vtx_c.base_rmean  = base_rmean;
	edge_vtx_c.base_rsigma = base_rsigma;
	
	edge_vtx_candidate_array.insert(plane_id,edge_vtx_c);
	
      } // end 3 planes

      // lintrk_data.move(..., ...)
      edge_data.insert(trk_id,edge_vtx_candidate_array);
    }
    return true;
  }
}
  

#endif

// std::cout << "[";
// for(const auto& s : rmean_v) std::cout << s << ",";
// std::cout << "]" << std::endl;
// std::cout << std::endl;


// trk_id is always 0, right??
// LAROCV_DEBUG() << "This atom id:  " << atom_id << std::endl;
// LAROCV_DEBUG() << "This plane id: " << plane_id << std::endl;
// size_t particle_id = 0; // only one particle associated
// LAROCV_DEBUG() << "This particle id: " << particle_id << std::endl;
// LAROCV_DEBUG() << "This track id: " << trk_id << std::endl;
	
// auto const  pca_id = pca_data.index_atom(atom_id, particle_id, plane_id, trk_id);
// auto const& pca    = pca_data.line(pca_id);   

// LAROCV_DEBUG() << "Got line: pt: " << pca.pt << " dir: " << pca.dir << std::endl;
