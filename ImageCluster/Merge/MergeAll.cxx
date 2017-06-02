#ifndef __MERGEALL_CXX__
#define __MERGEALL_CXX__

#include "MergeAll.h"
#include "LArUtil/GeometryHelper.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"
#include "ClusterRecoUtil/Base/ClusterParams.h"

namespace larocv {

  void MergeAll::_Configure_(const ::fcllite::PSet &pset)
  {
  }


  Cluster2DArray_t MergeAll::_Process_(const larocv::Cluster2DArray_t& clusters,
                                            const ::cv::Mat& img,
                                            larocv::ImageMeta& meta, larocv::ROI& r)
  {
    //std::cout<<"\nNew plane : "<<meta.plane() <<std::endl;

    auto geoH = ::larutil::GeometryHelper::GetME();

    Cluster2DArray_t oclusters;
    oclusters.reserve(clusters.size());

    std::vector<::cv::Point> overall_ctor;
    overall_ctor.reserve(1000); 

    for (int i = 0; i < clusters.size(); ++i) {
      //get one overlapping contour
      auto& ctor2 = clusters[i]._contour;

      //put the points from this overlapping contour into this overall_contour;
      for(auto& pt : ctor2) overall_ctor.emplace_back(pt);

      }

      //make a new hull, give it some space
      std::vector<int> hullpts;
      hullpts.reserve(overall_ctor.size());

      if ( !overall_ctor.size() ) return clusters; 
  
      std::cout<<"Overall contour size: "<<overall_ctor.size()<<std::endl ;

      //calculate the hull
      ::cv::convexHull(overall_ctor, hullpts);

      //make a new cluster
      Cluster2D cluster;

      //with a contour of fixed size...
      cluster._contour.resize(hullpts.size());

      //fill the contour with hull points!
      for(unsigned u=0;u<hullpts.size();++u) 
        cluster._contour[u] = overall_ctor[ hullpts[u] ];

      std::vector<::cv::Point> all_locations;
      bool first_fill = false ;
 
      ::cv::findNonZero(img, all_locations);

      for( const auto& loc: all_locations ) {
  
        cluster._insideHits.emplace_back(loc.x, loc.y);
        cluster._sumCharge += (int) img.at<uchar>(loc.y, loc.x);
        }

      oclusters.emplace_back(cluster);

    return oclusters ;
  }

}
#endif
