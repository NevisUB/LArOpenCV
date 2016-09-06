#ifndef __CHECKALIGNMENT_CXX__
#define __CHECKALIGNMENT_CXX__

#include "CheckAlignment.h"
#include "LArUtil/GeometryHelper.h"

namespace larocv{

  void CheckAlignment::_Configure_(const ::fcllite::PSet &pset)
  {
     _ratio_cut = pset.get<float>  ("RatioCut");
  }

  Cluster2DArray_t CheckAlignment::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta,
					larocv::ROI& roi)
  { 

    auto geomH  = larutil::GeometryHelper::GetME();

    Cluster2DArray_t oclusters;
    oclusters.reserve( clusters.size() );

    auto vtx2D = roi.roivtx_in_image(meta); 
   std::cout<<"\nPlane "<<meta.plane()<<std::endl ;
   
    _ratio_cut = 0 ; 
   
    for ( size_t i = 0; i < clusters.size(); i++){ 

       auto const & c = clusters[i]; 
        
       auto sW = ( c.roi.startpt.x ); // * geomH->WireToCm();
       auto sT = ( c.roi.startpt.y ); // * geomH->TimeToCm();

       auto eW = ( c.roi.endpt.x   ); // * geomH->WireToCm();
       auto eT = ( c.roi.endpt.y   ); // * geomH->TimeToCm();
      
       //Now have 3 3d points; vertex, start, end; for each, check axis alignment
       double dist_se  = sqrt( pow(eW - sW,2) + pow(eT - sT,2));
       double dist_roi = sqrt( pow(vtx2D.x - sW,2) + pow( vtx2D.y - sT,2));

       std::vector<double> start_to_end = { (eW - sW) / dist_se, 
                                            (eT - sT) / dist_se };

       std::vector<double> vtx_to_start = { (sW - vtx2D.x) / dist_roi,
                                            (sT - vtx2D.y) / dist_roi };

       auto dot = start_to_end[0] * vtx_to_start[0] + 
                  start_to_end[1] * vtx_to_start[1] ;

       if((dot) > _ratio_cut ){
          oclusters.emplace_back(c);       
             }

          std::cout<<"Score : " <<dot <<std::endl ;


     }
 
  return oclusters; }

}
#endif
