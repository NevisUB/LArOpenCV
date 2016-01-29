#ifndef __PCATRACKSHOWER_CXX__
#define __PCATRACKSHOWER_CXX__

#include "PCATrackShower.h"

namespace larcv{

  void PCATrackShower::_Configure_(const ::fcllite::PSet &pset)
  {
    _outtree = new TTree("PCA","PCA");
      
    _outtree->Branch("_eval1",&_eval1,"eval1/D");
    _outtree->Branch("_eval2",&_eval2,"eval2/D");
      
  }

  ContourArray_t PCATrackShower::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  {

    
    //http://docs.opencv.org/master/d3/d8d/classcv_1_1PCA.html
    
    //cluster == contour


    std::vector<Point2D> cntr_pt_v; cntr_pt_v.resize(clusters.size());
    std::vector<std::vector<Point2D> > eigen_vecs_v; eigen_vecs_v.resize(clusters.size());
    std::vector<std::vector<double> >  eigen_val_v; eigen_val_v.resize(clusters.size());
    
    
    //for(const auto& cluster : clusters) {
    for(unsigned i = 0; i < clusters.size(); ++i) {

      auto& cluster    = clusters[i];
      auto& cntr_pt    = cntr_pt_v[i];
      auto& eigen_vecs = eigen_vecs_v[i];
      auto& eigen_val  = eigen_val_v[i];
      
      // PCA ana requies MAT object w/ data sitting in rows, 2 columns
      // 1 for each ``feature" or coordinate
      
      ::cv::Mat ctor_pts(cluster.size(), 2, CV_64FC1);

      for (unsigned i = 0; i < ctor_pts.rows; ++i)
	{
	  ctor_pts.at<double>(i, 0) = cluster[i].x;
	  ctor_pts.at<double>(i, 1) = cluster[i].y;
	}
      //Perform PCA analysis
      ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0); // maxComponents = 0 (retain all)

      //variables we might care about

      //Center point
      // ::cv::Point
      cntr_pt = Point2D( pca_ana.mean.at<double>(0,0),
			 pca_ana.mean.at<double>(0,1) );
      

      //Principle directions (vec) and relative lengths (vals)
      eigen_vecs.resize(2);
      eigen_val.resize(2);
      
      for (unsigned i = 0; i < 2; ++i) {
	eigen_vecs[i] = Point2D(pca_ana.eigenvectors.at<double>(i, 0),
				pca_ana.eigenvectors.at<double>(i, 1));
	eigen_val[i]  = pca_ana.eigenvalues.at<double>(0, i);
      }

      _eval1 = eigen_val[0];
      _eval2 = eigen_val[1];

      _outtree->Fill();
      //Do something
      
    }


    std::swap(cntr_pt_v   ,_cntr_pt_v);
    std::swap(eigen_vecs_v,_eigen_vecs_v);
    std::swap(eigen_val_v ,_eigen_val_v);

          
    return clusters;


  }



}
#endif
