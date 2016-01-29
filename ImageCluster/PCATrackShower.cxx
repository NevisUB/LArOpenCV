#ifndef __PCATRACKSHOWER_CXX__
#define __PCATRACKSHOWER_CXX__

#include "PCATrackShower.h"

namespace larcv{

  void PCATrackShower::_Configure_(const ::fcllite::PSet &pset)
  {

    

  }

  ContourArray_t PCATrackShower::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  {

    //http://docs.opencv.org/master/d3/d8d/classcv_1_1PCA.html
    
    //cluster == contour

    for(const auto& cluster : clusters) {

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
      Point2D cntr_pt( pca_ana.mean.at<double>(0,0),
		       pca_ana.mean.at<double>(0,1) );
      

      //Principle directions (vec) and relative lengths (vals)
      std::vector<Point2D> eigen_vecs(2);
      std::vector<double>  eigen_val (2);
      for (unsigned i = 0; i < 2; ++i) {
	eigen_vecs[i] = Point2D(pca_ana.eigenvectors.at<double>(i, 0),
				pca_ana.eigenvectors.at<double>(i, 1));
	eigen_val[i]  = pca_ana.eigenvalues.at<double>(0, i);
      }

      //Do something
      
    }

    

    return clusters;


  }

}
#endif
