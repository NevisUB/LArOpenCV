#ifndef __ALGO_CXX__
#define __ALGO_CXX__

#include "Algo.h"

namespace larocv {


  void Algo::_Configure_(const ::fcllite::PSet &pset)
  {
  }

  larocv::Cluster2DArray_t Algo::_Process_(const larocv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larocv::ImageMeta& meta,
					   larocv::ROI& roi)
  {

    if ( clusters.size() )
      throw larbys("This algo can only be executed first in algo chain!");

    ///////////////////////////////////////////////
    //Threshold the input image to certain ADC value, this is the working image
    int THRESH_LOWER=20;
    ::cv::Mat img_thresh_m;
    threshold(img, img_thresh_m,THRESH_LOWER,255,0);
    
    ///////////////////////////////////////////////
    //Threshold the input image to HIP ADC value, this is our HIP
    int HIP_LEVEL=80;
    ::cv::Mat hip_thresh_m;
    threshold(img, hip_thresh_m,HIP_LEVEL,255,0);

    ///////////////////////////////////////////////
    //Contour finding on the HIP
    ContourArray_t hip_ctor_v;
    std::vector<::cv::Vec4i> hip_cv_hierarchy_v;
    hip_ctor_v.clear();
    hip_cv_hierarchy_v.clear();
    
    ::cv::findContours(hip_thresh_m,hip_ctor_v,hip_cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);

    ///////////////////////////////////////////////
    //Filter the HIP contours to a minimum size, the ones that are
    int min_hip_size = 6;
    ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(hip_ctor_v.size());
    
    for (const auto& hip_ctor : hip_ctor_v)
      if ( hip_ctor.size() > min_hip_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);

    // swap them b.c. I don't like _tmp stuffs
    std::swap(hip_ctor_v,hip_ctor_v_tmp);
    
    ///////////////////////////////////////////////
    //Masking away the hip in the original image

    //lets find the non zero pixels hiding in the hip contours. Lets mask them out of the original image
    //OK this means lets identify the point inside the HIP contour, then go straight into the
    //original image and zero that guy out to create a MIP image
    ::cv::Mat mip_thresh_m = img_thresh_m.clone();

    Contour_t all_locations;
    ::cv::findNonZero(mip_thresh_m, all_locations); // get the non zero points
    
     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < hip_ctor_v.size(); i++ ) {
	 if ( ::cv::pointPolygonTest(hip_ctor_v[i],loc,false) < 0 )  // not inside
	   continue;
	 //something here can we zero out
	 
	 mip_thresh_m.at<uchar>(loc.y, loc.x) = 0;
       }
     }

     ///////////////////////////////////////////////
     //run the contour finder on the MIP

     //it's already binarized?
     ContourArray_t mip_ctor_v;
     std::vector<::cv::Vec4i> mip_cv_hierarchy_v;
     mip_ctor_v.clear();
     mip_cv_hierarchy_v.clear();
     
     ::cv::findContours(mip_thresh_m,mip_ctor_v,mip_cv_hierarchy_v,
			CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_SIMPLE);     
    ///////////////////////////////////////////////
    //Filter the MIP contours to a minimum size, the ones that are
     int min_mip_size = 6;
     ContourArray_t mip_ctor_v_tmp;
     mip_ctor_v_tmp.reserve(mip_ctor_v.size());
     
     for (const auto& mip_ctor : mip_ctor_v)
       if (mip_ctor.size() > min_mip_size)
	 mip_ctor_v_tmp.emplace_back(mip_ctor);
     
     // swap them b.c. I don't like _tmp stuffs
     std::swap(mip_ctor_v,mip_ctor_v_tmp);
     
     ///////////////////////////////////////////////
     //unify the contours
     ContourArray_t all_ctor_v;
     all_ctor_v.reserve(mip_ctor_v.size() + hip_ctor_v.size());
     all_ctor_v.insert( all_ctor_v.end(), mip_ctor_v.begin(), mip_ctor_v.end());
     all_ctor_v.insert( all_ctor_v.end(), hip_ctor_v.begin(), hip_ctor_v.end() );

     ///////////////////////////////////////////////
     //convex hull on each contour, lets save the output
     
     //with the MIP and HIP clusters in hand, lets calculate convex hull per cluster and find defects
     std::vector<std::vector<int> > hullpts_v;
     hullpts_v.resize(all_ctor_v.size());

     std::vector<std::vector<::cv::Vec4i> > defects_v;
     defects_v.resize(all_ctor_v.size());

     std::vector<std::vector<double> > defects_double_v;
     defects_double_v.resize(all_ctor_v.size());
     
     for (unsigned i = 0; i < all_ctor_v.size(); ++i ) {
       auto& cluster   = all_ctor_v[i];
       auto& hullpts   = hullpts_v[i];
       auto& defects   = defects_v[i];
       auto& defects_d = defects_double_v[i];

       //make the hull
       ::cv::convexHull(cluster, hullpts);
       //close the hull up
       hullpts.push_back(hullpts.at(0));

       //make the contour that olds actual hull points, instead of indicies
       std::vector<::cv::Point> hullcontour;
       hullcontour.resize(hullpts.size()-1);
       
       for(unsigned u=0;u<hullcontour.size();++u) 
	 hullcontour[u] = cluster[ hullpts[u] ];

       //compute the defects
       ::cv::convexityDefects(cluster,hullpts,defects);

       //lets put the defects into bonafide vector of doubles
       defects_d.resize( defects.size() );
       
       for( int j = 0; j < defects.size(); ++j )
	 defects_d[j]  = ( ( (float) defects[j][3] ) / 256.0 ); 
       
     }

     Cluster2DArray_t result_v;
     result_v.resize(all_ctor_v.size());

     for(unsigned i=0;i<all_ctor_v.size();++i) 
       result_v[i]._contour = all_ctor_v[i];
     
    return result_v;
  }
  
}

#endif
