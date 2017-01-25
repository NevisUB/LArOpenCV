#ifndef __PCACANDIDATES_CXX__
#define __PCACANDIDATES_CXX__

#include "PCACandidates.h"
#include "Geo2D/Core/Line.h"
#include "DefectCluster.h"

namespace larocv {

  /// Global larocv::PCACandidatesFactory to register AlgoFactory
  static PCACandidatesFactory __global_PCACandidatesFactory__;

  void PCACandidates::_Configure_(const Config_t &pset)
  {

    _PCACrossing.Configure(pset);
    
    // default is empty string which signals not to use defect cluster algo data
    auto const defect_cluster_algo_name = pset.get<std::string>("DefectClusterAlgo","");
    if(defect_cluster_algo_name.empty())
      _defect_cluster_algo_id = kINVALID_ALGO_ID;
    else {
      _defect_cluster_algo_id = this->ID(defect_cluster_algo_name);
      if(_defect_cluster_algo_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Invalid defect cluster algorithm name given: "
			  << defect_cluster_algo_name
			  << std::endl;
	throw larbys();
      }
      LAROCV_NORMAL() << "PCACandidates " << this->Name()
		      << " will compute PCA for input atomics "
		      << std::endl;

    }
    if(_defect_cluster_algo_id == kINVALID_ALGO_ID)
      LAROCV_NORMAL() << "PCACandidates " << this->Name()
		      << " will compute PCA for input clusters (not atomics) "
		      << std::endl;


    _per_vertex = pset.get<bool>("BreakPerVertex");

    Register(new data::PCACandidatesData);
  }

  void PCACandidates::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
    if(this->ID() == 0) throw larbys("PCACandidates should not be run first!");
    LAROCV_DEBUG() << "processing plane " << meta.plane() << std::endl;

    // this data
    auto& data = AlgoData<data::PCACandidatesData>(0);

    // boolean for algorithm logic
    bool point_analysis_done = false;

    // if defect algo is not provided, use input clusters
    if(_defect_cluster_algo_id == kINVALID_ALGO_ID) {

      data._input_id = this->ID() - 1;

      //for each input cluster
      for(size_t cidx=0; cidx<clusters.size(); ++cidx) {

	auto const& ctor = clusters[cidx]._contour;
	
	auto pca_principle = CalcPCA(ctor);
	
	data.move(std::move(pca_principle),cidx,cidx,meta.plane());
      }
      
    }else{

      // use defect data to analyze pca for atoms.
      // for defect cluster, there are two type of atoms:
      // 0) those organized by vertex
      // 1) those not
      // user's boolean flag tell us which type of atoms to analyze
      
      data._input_id = _defect_cluster_algo_id;

      auto const& defect_data = AlgoData<larocv::data::DefectClusterData>(_defect_cluster_algo_id,0);
      
      if(!_per_vertex && defect_data._raw_cluster_vv.size() > meta.plane()) {
	// analyze atomic clusters that are not organized by vertex

	// obtain this plane's clusters
	auto const& plane_clusters = defect_data._raw_cluster_vv[meta.plane()];

	// loop over clusters
	for(auto const& compound : plane_clusters.get_cluster()) {
	  // loop over atoms
	  for(auto const& atom : compound.get_atoms()) {
	    // estimate pca
	    auto pca_principle = CalcPCA(atom);
	    // register this pca w/ id
	    data.move(std::move(pca_principle),
		      atom.id(), compound.id(), meta.plane());
	  }
	}
      }

      else{
	// analyze atomic clusters that are organized by vertex
	// loop over vertex
	for(auto const& vtx_data : defect_data.get_vertex_clusters()) {
	  // loop over clusters
	  for(auto const& compound : vtx_data.get_cluster(meta.plane())) {
	    // loop over atoms
	    for(auto const& atom : compound.get_atoms()) {
	      // estimate pca
	      auto pca_principle = CalcPCA(atom);
	      // register this pca w/ id
	      data.move(std::move(pca_principle),
			atom.id(), compound.id(), meta.plane(), vtx_data.id());
	    }
	  }// end loop over clusters to make PCA lines

	  //
	  // crossing point analysis should be done here for per-vtx case
	  // because there are "multiple planes" in general (i.e. different vtx)
	  //
	  // get a list of line index for this vtx & plane
	  auto const line_index_v = data.index_plane(meta.plane(),vtx_data.id());
	  std::vector<geo2d::Line<float> > line_v;
	  line_v.reserve(line_index_v.size());
	  
	  for(size_t line_id:line_index_v) line_v.emplace_back(data.line(line_id));

	  auto points_v = _PCACrossing.ComputeIntersections(line_v,img);
	  
	  // end loop over finding xs points
	  bool point_analysis_done = true;
	  data.emplace_points(std::move(points_v), meta.plane(), vtx_data.id());
	}// end loop over ertex
      }// vertex-wise condition for pca/point analysis
    }// defect atomic cluster analysis done

    // if pca crossing point analysis is not done, perform
    if(!point_analysis_done) {
      auto const line_index_v = data.index_plane(meta.plane());
      std::vector<geo2d::Line<float> > line_v;
      line_v.reserve(line_index_v.size());
      
      for(size_t line_id:line_index_v) line_v.emplace_back(data.line(line_id));

      auto points_v = _PCACrossing.ComputeIntersections(line_v,img);
      
      // end loop over finding xs points
      bool point_analysis_done = true;
      data.emplace_points(std::move(points_v), meta.plane());
    }
  }

  bool PCACandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    if(_defect_cluster_algo_id != kINVALID_ALGO_ID) {
      
      const auto& def_data = AlgoData<data::DefectClusterData>(_defect_cluster_algo_id,0);

      uint n_defects=0;

      if(!_per_vertex) {

	for(uint plane=0;plane<3;++plane)
	
	  n_defects += def_data._raw_cluster_vv.at(plane).num_defects();

      } else {

	for(auto const& vtx_cluster : def_data.get_vertex_clusters()) {

	  for(size_t plane=0; plane < vtx_cluster.num_planes(); ++plane) {

	    for(auto const& part_compound : vtx_cluster.get_cluster(plane)) {

	      n_defects += part_compound.get_defects().size();
	      
	    }
	  }
	}

      }
      if ( !n_defects && !_per_vertex) {
	LAROCV_DEBUG() << "PostProcess found NO defect points in any plane" << std::endl;
	return false;
      }
    }
    return true;
  }
}
#endif
