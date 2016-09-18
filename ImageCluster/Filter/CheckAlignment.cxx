#ifndef __CHECKALIGNMENT_CXX__
#define __CHECKALIGNMENT_CXX__

#include "CheckAlignment.h"
#include "LArUtil/GeometryHelper.h"

namespace larocv{

  void CheckAlignment::_Configure_(const ::fcllite::PSet &pset)
  {
    _ratio_cut = pset.get<float>  ("RatioCut");
    _use_start_end=pset.get<bool>("UseStartEnd");
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
    //std::cout<<"\nPlane "<<meta.plane()<<std::endl ;
   
    for ( size_t i = 0; i < clusters.size(); i++){ 

      auto const & c = clusters[i]; 

      std::vector<double> direction(2,0.0);

      double sW,sT,eW,eT,dist_se;
      sW = c.roi.startpt.x;
      sT = c.roi.startpt.y;

      if ( _use_start_end ) {
	eW = c.roi.endpt.x;
	eT = c.roi.endpt.y;

	dist_se  = std::sqrt( std::pow(eW - sW,2) + std::pow(eT - sT,2));

	direction[0]= (eW - sW) / dist_se;
	direction[1]= (eT - sT) / dist_se;
	
      }
      else {
	
	dist_se = std::sqrt( c.roi.dir.x*c.roi.dir.x + c.roi.dir.y*c.roi.dir.y );
	//std::cout << dist_se << "\n";
	direction[0] = c.roi.dir.x / dist_se;
	direction[1] = c.roi.dir.y / dist_se;
	
      }
     
      //Now have 3 3d points; vertex, start, end; for each, check axis alignment

      double dist_roi = std::sqrt( std::pow(vtx2D.x - sW,2) + std::pow( vtx2D.y - sT,2));

      
      std::vector<double> vtx_to_start = { (sW - vtx2D.x) / dist_roi,
					   (sT - vtx2D.y) / dist_roi };
      //std::cout << dist_roi << " " << sW << " " << sT << " " << vtx2D.x << " " << vtx2D.y << " " << eT << " " << eW << " " << "\n";
      auto dot = direction[0] * vtx_to_start[0] + direction[1] * vtx_to_start[1] ;

      if((dot) > _ratio_cut )

	oclusters.emplace_back(c);       
      
    }
    //std::cout << oclusters.size() << "\n";
    return oclusters;
  }
  

}
#endif
