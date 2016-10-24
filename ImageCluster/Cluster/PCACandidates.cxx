#ifndef __PCACANDIDATES_CXX__
#define __PCACANDIDATES_CXX__

#include "PCACandidates.h"
#include "Core/Line.h"
#include "DefectClusterData.h"

namespace larocv {

  /// Global larocv::PCACandidatesFactory to register AlgoFactory
  static PCACandidatesFactory __global_PCACandidatesFactory__;

  void PCACandidates::_Configure_(const ::fcllite::PSet &pset)
  {

  }
  
  void PCACandidates::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
    if(this->ID() == 0) throw larbys("PCACandidates should not be run first!");
    
    //get the defect cluster data atomic associations for broken clusters => original clusters
    const auto& defectcluster_data = AlgoData<DefectClusterData>(ID()-1);
    unsigned n_original_clusters   = defectcluster_data._n_original_clusters_v[meta.plane()];
    const auto& atomic_ctor_ass_v  = defectcluster_data._atomic_ctor_ass_v_v[meta.plane()];
    
    auto& circlevertex_data = AlgoData<larocv::PCACandidatesData>();
    circlevertex_data._input_id = this->ID() - 1;
    //find the intersection points of the PCAs per clusters
    //it's current stored in unfortunately bloated Cluster2D object
    //as eigenVecFirst, let just do the PCA here

    for(unsigned orig_idx=0; orig_idx < n_original_clusters; ++orig_idx) {

      std::vector<geo2d::Line<float> > ctor_lines_v;
      std::vector<size_t> atomic_id_v;

      for(unsigned atomic_idx=0; atomic_idx < atomic_ctor_ass_v.size(); ++atomic_idx){

	if (orig_idx != atomic_ctor_ass_v[atomic_idx] ) continue;

	auto const& ctor = clusters[atomic_idx]._contour;
	
	cv::Mat ctor_pts(ctor.size(), 2, CV_32FC1); //32 bit precision is fine
	
	for (unsigned i = 0; i < ctor_pts.rows; ++i) {
	  ctor_pts.at<float>(i, 0) = ctor[i].x;
	  ctor_pts.at<float>(i, 1) = ctor[i].y;
	}
	
	cv::PCA pca_ana(ctor_pts, cv::Mat(), CV_PCA_DATA_AS_ROW,0);
      
	geo2d::Line<float> pca_principle(geo2d::Vector<float>(pca_ana.mean.at<float>(0,0),
							      pca_ana.mean.at<float>(0,1)),
					 geo2d::Vector<float>(pca_ana.eigenvectors.at<float>(0,0),
							      pca_ana.eigenvectors.at<float>(0,1)));

	ctor_lines_v.emplace_back(std::move(pca_principle));
	atomic_id_v.push_back(atomic_idx);

      }
      
      std::vector<geo2d::Vector<float> > ipoints_v;
      ipoints_v.reserve(ctor_lines_v.size() * (ctor_lines_v.size() + 1) / 2); //n(n+1)/2 right?

      for(unsigned i=0;i<ctor_lines_v.size();++i) { 
	for(unsigned j=i+1;j<ctor_lines_v.size();++j) {
	  auto ipoint = geo2d::IntersectionPoint(ctor_lines_v[i],ctor_lines_v[j]);
	  ipoints_v.emplace_back(std::move(ipoint));
	}
      }

      for ( auto & ctor_line : ctor_lines_v) {
	LAROCV_DEBUG() << "meta.plane() : " << meta.plane() <<  " orig_index : " <<  orig_idx << " pt: " <<  ctor_line.pt << " dir : " << ctor_line.dir << std::endl;
      }
      
      //copy into algodata
      circlevertex_data.set_data(ctor_lines_v,ipoints_v,atomic_id_v,meta.plane());
    }
  }

  void PCACandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {}
  
  
}
#endif
