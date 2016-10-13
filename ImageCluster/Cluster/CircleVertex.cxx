#ifndef __CIRCLEVERTEX_CXX__
#define __CIRCLEVERTEX_CXX__

#include "CircleVertex.h"
#include "Core/Line.h"
#include "DefectClusterData.h"

namespace larocv {


  void CircleVertex::_Configure_(const ::fcllite::PSet &pset)
  {

  }
  
  larocv::Cluster2DArray_t CircleVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
						   const ::cv::Mat& img,
						   larocv::ImageMeta& meta,
						   larocv::ROI& roi)
  {

    //get the defect cluster data atomic associations for broken clusters => original clusters
    auto& defectcluster_data = AlgoData<larocv::DefectClusterData>();
    const auto& atomic_ctor_ass_v = defectcluster_data._atomic_ctor_ass_v_v[meta.plane()];

    //find the intersection points of the PCAs per clusters
    //it's current stored in unfortunately bloated Cluster2D object
    //as eigenVecFirst, let just do the PCA here


    std::vector<geo2d::Line<float> > ctor_lines_v;
    ctor_lines_v.reserve(clusters.size());
    
    for(const auto& cluster : clusters) {
      auto& ctor = cluster._contour;

      ::cv::Mat ctor_pts(ctor.size(), 2, CV_32FC1); //32 bit precision is fine
    
      for (unsigned i = 0; i < ctor_pts.rows; ++i) {
	ctor_pts.at<float>(i, 0) = ctor[i].x;
	ctor_pts.at<float>(i, 1) = ctor[i].y;
      }
      
      cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);
      
      geo2d::Line<float> pca_principle(geo2d::Vector<float>(pca_ana.mean.at<float>(0,0),
							    pca_ana.mean.at<float>(0,1)),
				       geo2d::Vector<float>(pca_ana.eigenvectors.at<float>(0,0),
							    pca_ana.eigenvectors.at<float>(0,1)));
      
      ctor_lines_v.emplace_back(std::move(pca_principle));
    }
    
    std::vector<geo2d::Vector<float> > ipoints_v;
    ipoints_v.reserve(ctor_lines_v.size() * (ctor_lines_v.size() + 1) / 2); //n(n+1)/2 right?
    
    for(unsigned i=0;i<ctor_lines_v.size();++i) { 
      for(unsigned j=i;j<ctor_lines_v.size();++j) {
	//compute their intersection
	auto ipoint = geo2d::IntersectionPoint(ctor_lines_v[i],ctor_lines_v[j]);
	ipoints_v.emplace_back(std::move(ipoint));
      }
    }
    
    auto& circlevertex_data = AlgoData<larocv::CircleVertexData>();
    circlevertex_data.set_data(ctor_lines_v,ipoints_v,meta.plane());
    
    return clusters;
  }
  
  
  
}
#endif
