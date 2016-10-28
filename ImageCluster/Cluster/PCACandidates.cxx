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
    const auto& atomic_ctor_ass_v  = defectcluster_data._atomic_ctor_ass_v_v[meta.plane()];

    //this data
    auto& data = AlgoData<larocv::PCACandidatesData>();
    data._input_id = this->ID() - 1;

    //pca lines, indexed by atomic ID
    auto& ctor_lines_v = data._ctor_lines_v_v[meta.plane()];

    //same size as input clusters (atomic clusters...)
    ctor_lines_v.resize(clusters.size());

    // contour lines indexed by original cluster ID
    auto& ctor_lines_v_v = data._ctor_lines_v_v_v[meta.plane()];
    ctor_lines_v_v.resize(n_original_clusters);
    
    //atomic indicies, indexed by original cluster ID
    auto& atomic_id_v_v  = data._atomic_id_v_v_v[meta.plane()];
    atomic_id_v_v.resize(n_original_clusters);

    //for each original cluster
    for(unsigned orig_idx=0; orig_idx < n_original_clusters; ++orig_idx) {

      std::vector<geo2d::Line<float> > pca_lines_v;
      std::vector<size_t> atomic_id_v;

      //for each atomic
      for(unsigned atomic_idx=0; atomic_idx < atomic_ctor_ass_v.size(); ++atomic_idx) {

	//is this atomic associated to the original cluster
	if (orig_idx != atomic_ctor_ass_v.at(atomic_idx) ) continue;

	//yes, get this contour at atomic index
	auto const& ctor = clusters[atomic_idx]._contour;
	
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



	//copy the pca into atomic index-ed vector
	ctor_lines_v[atomic_idx] = pca_principle;

	//move it into pca vector
	pca_lines_v.emplace_back(std::move(pca_principle));

	//store this atomic index
	atomic_id_v.push_back(atomic_idx);
	
      }

      ctor_lines_v_v[orig_idx] = pca_lines_v;
      atomic_id_v_v[orig_idx]  = atomic_id_v;

      LAROCV_DEBUG() << "Found " << ctor_lines_v.size() << " PCA lines for original index " << orig_idx << std::endl;
    }

    LAROCV_DEBUG() << "Total crossing PCA lines found: " << ctor_lines_v.size() << std::endl;
    
    std::vector<geo2d::Vector<float> > ipoints_v;
    ipoints_v.reserve(ctor_lines_v.size() * ctor_lines_v.size());
    
    for(unsigned i=0;i<ctor_lines_v.size();++i) { 
      for(unsigned j=i+1;j<ctor_lines_v.size();++j) {
	auto ipoint = geo2d::IntersectionPoint(ctor_lines_v[i],ctor_lines_v[j]);
	ipoints_v.emplace_back(std::move(ipoint));
      }
    }

    data._ipoints_v_v[meta.plane()] = ipoints_v;
  }

  bool PCACandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    const auto& dfect_data = AlgoData<DefectClusterData>(ID()-1);
    uint n_defects=0;

    for(uint plane=0;plane<3;++plane) {
    auto atomic_defect_v_v = dfect_data._atomic_defect_v_v_v[plane];
    for (auto& atomic_defect_v : atomic_defect_v_v)
      n_defects+=atomic_defect_v.size();
    }
    
    if ( !n_defects ) {
      LAROCV_DEBUG() << "PostProcess found NO defect points in any plane" << std::endl;
      return false;
    }
	
    return true;
  }
  
  
}
#endif
