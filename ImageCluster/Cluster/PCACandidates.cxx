#ifndef __PCACANDIDATES_CXX__
#define __PCACANDIDATES_CXX__

#include "PCACandidates.h"
#include "Core/Line.h"
#include "DefectCluster.h"

namespace larocv {

  /// Global larocv::PCACandidatesFactory to register AlgoFactory
  static PCACandidatesFactory __global_PCACandidatesFactory__;

  void PCACandidates::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const defect_cluster_algo_name = pset.get<std::string>("DefectClusterAlgo","defbreak1");
    _defect_cluster_algo_id = this->ID(defect_cluster_algo_name);

    _pca_x_d_to_q         = pset.get<float>("PCAXDistance",5.0);
    _nonzero_pixel_thresh = pset.get<int>  ("PixelThreshold",10);
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
    
    //get the defect cluster data
    //atomic associations for atomic clusters => original clusters
    const auto& defectcluster_data = AlgoData<DefectClusterData>(_defect_cluster_algo_id);

    //original clusters (track clusters, MIP/HIP clusters for example)
    unsigned n_original_clusters   = defectcluster_data._n_original_clusters_v[meta.plane()];

    //association between atomic cluster and original cluster
    const auto& atomic_ctor_ass_v_v  = defectcluster_data._atomic_ctor_ass_v_v_v[meta.plane()];

    //this data
    auto& data = AlgoData<larocv::PCACandidatesData>();
    data._input_id = this->ID() - 1;

    //pca lines, indexed by atomic ID
    auto& ctor_lines_v = data._ctor_lines_v_v[meta.plane()];
    //same size as input clusters (atomic clusters...)
    ctor_lines_v.resize(clusters.size());

    //for each original cluster
    for(unsigned c_idx=0; c_idx < n_original_clusters; ++c_idx) {

      LAROCV_DEBUG() << "Input clust c_idx : " << c_idx << std::endl;
      
      std::vector<size_t> atomic_id_v;
      
      //for each atomic
      for(const auto atomic_idx : atomic_ctor_ass_v_v.at(c_idx)) {

	//yes, get this contour at atomic index
	auto const& ctor = clusters[atomic_idx]._contour;

	geo2d::Line<float> pca_principle = calculate_pca(ctor);

	//copy the pca into atomic index-ed vector
	ctor_lines_v[atomic_idx] = pca_principle;
	
      }

      LAROCV_DEBUG() << "Found " << ctor_lines_v.size() << " PCA lines for original index " << c_idx << std::endl;
    }

    LAROCV_DEBUG() << "Total crossing PCA lines found: " << ctor_lines_v.size() << std::endl;
    
    std::vector<geo2d::Vector<float> > ipoints_v;
    ipoints_v.reserve(ctor_lines_v.size() * ctor_lines_v.size());

    cv::Mat thresh_img;
    cv::threshold(img,thresh_img,_nonzero_pixel_thresh,255,CV_THRESH_BINARY);
    std::vector<cv::Point_<int> > nonzero;
    findNonZero(thresh_img,nonzero);

    auto d_to_q_2 = _pca_x_d_to_q*_pca_x_d_to_q;
    
    for(unsigned i=0;i<ctor_lines_v.size();++i) { 
      for(unsigned j=i+1;j<ctor_lines_v.size();++j) {
	auto ipoint = geo2d::IntersectionPoint(ctor_lines_v[i],ctor_lines_v[j]);

	//condition that Xsing point must be near a non-zero pixel value
	for(const auto& pt : nonzero) {
	  if ( geo2d::dist2(geo2d::Vector<float>(pt),ipoint) < d_to_q_2 ) {
	    //it's near nonzero, put it in, continue
	    ipoints_v.emplace_back(std::move(ipoint));
	    break;
	  }
	}
	
      }
    }

    data._atomic_ctor_ass_v_v_v[meta.plane()] = defectcluster_data._atomic_ctor_ass_v_v_v[meta.plane()];
    data._ipoints_v_v[meta.plane()] = ipoints_v;

  }

  bool PCACandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    const auto& dfect_data = AlgoData<DefectClusterData>(ID()-1);
    uint n_defects=0;

    for(uint plane=0;plane<3;++plane) {
    auto atomic_defect_pts_v_v = dfect_data._atomic_defect_pts_v_v_v[plane];
    for (auto& atomic_defect_pts_v : atomic_defect_pts_v_v)
      n_defects+=atomic_defect_pts_v.size();
    }
    
    if ( !n_defects ) {
      LAROCV_DEBUG() << "PostProcess found NO defect points in any plane" << std::endl;
      return false;
    }
	
    return true;
  }
  
  
}
#endif