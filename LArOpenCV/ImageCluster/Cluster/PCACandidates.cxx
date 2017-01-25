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

    _pca_x_d_to_q         = pset.get<float>("PCAXDistance",5.0);
    _nonzero_pixel_thresh = pset.get<int>  ("PixelThreshold",10);

    _per_vertex = pset.get<bool>("BreakPerVertex");

    Register(new data::PCACandidatesData);
  }

  geo2d::Line<float> PCACandidates::calculate_pca(const GEO2D_Contour_t& ctor) {

    cv::Mat ctor_pts(ctor.size(), 2, CV_32FC1); //32 bit precision is fine
	
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<float>(i, 0) = ctor.at(i).x;
      ctor_pts.at<float>(i, 1) = ctor.at(i).y;
    }
	
    cv::PCA pca_ana(ctor_pts, cv::Mat(), CV_PCA_DATA_AS_ROW,0);
      
    geo2d::Line<float> pca_principle(geo2d::Vector<float>(pca_ana.mean.at<float>(0,0),
							  pca_ana.mean.at<float>(0,1)),
				     geo2d::Vector<float>(pca_ana.eigenvectors.at<float>(0,0),
							  pca_ana.eigenvectors.at<float>(0,1)));
    return pca_principle;
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

    // image preparation for later use
    cv::Mat thresh_img;
    cv::threshold(img,thresh_img,_nonzero_pixel_thresh,255,CV_THRESH_BINARY);
    std::vector<cv::Point_<int> > nonzero;
    findNonZero(thresh_img,nonzero);
    // distance threshold for point registration (used later for pca crossing points)
    auto d_to_q_2 = _pca_x_d_to_q*_pca_x_d_to_q;
    
    // if defect algo is not provided, use input clusters
    if(_defect_cluster_algo_id == kINVALID_ALGO_ID) {

      data._input_id = this->ID() - 1;

      //for each input cluster
      for(size_t cidx=0; cidx<clusters.size(); ++cidx) {

	auto const& ctor = clusters[cidx]._contour;
	
	auto pca_principle = calculate_pca(ctor);
	
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
	    auto pca_principle = calculate_pca(atom);
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
	      auto pca_principle = calculate_pca(atom);
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
	  std::vector<geo2d::Vector<float> > points_v;
	  points_v.reserve(line_index_v.size() * line_index_v.size());
	  // loop over line
	  for(size_t i=0; i<line_index_v.size(); ++i) {
	    auto const& line1 = data.line(line_index_v[i]);
	    // N^2 loop over line
	    for(size_t j=i+1; j<line_index_v.size(); ++j) {
	      auto const& line2 = data.line(line_index_v[j]);
	      // estimate crossing point
	      auto ipoint = geo2d::IntersectionPoint(line1,line2);
	      //condition that Xsing point must be near a non-zero pixel value
	      for(const auto& pt : nonzero) {
		if ( geo2d::dist2(geo2d::Vector<float>(pt),ipoint) < d_to_q_2 ) {
		  //it's near nonzero, put it in, continue
		  points_v.emplace_back(std::move(ipoint));
		  break;
		}
	      }
	    }
	  } // end loop over finding xs points
	  bool point_analysis_done = true;
	  data.emplace_points(std::move(points_v), meta.plane(), vtx_data.id());
	}// end loop over ertex
      }// vertex-wise condition for pca/point analysis
    }// defect atomic cluster analysis done

    // if pca crossing point analysis is not done, perform
    if(!point_analysis_done) {
      auto const line_index_v = data.index_plane(meta.plane());
      std::vector<geo2d::Vector<float> > points_v;
      points_v.reserve(line_index_v.size() * line_index_v.size());
      // loop over line
      for(size_t i=0; i<line_index_v.size(); ++i) {
	auto const& line1 = data.line(line_index_v[i]);
	// N^2 loop over line
	for(size_t j=i+1; j<line_index_v.size(); ++j) {
	  auto const& line2 = data.line(line_index_v[j]);
	  // estimate crossing point
	  auto ipoint = geo2d::IntersectionPoint(line1,line2);
	  //condition that Xsing point must be near a non-zero pixel value
	  for(const auto& pt : nonzero) {
	    if ( geo2d::dist2(geo2d::Vector<float>(pt),ipoint) < d_to_q_2 ) {
	      //it's near nonzero, put it in, continue
	      points_v.emplace_back(std::move(ipoint));
	      break;
	    }
	  }
	}
      } // end loop over finding xs points
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
