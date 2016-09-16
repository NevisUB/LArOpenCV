#ifndef __VTXALIGN_CXX__
#define __VTXALIGN_CXX__

#include "VtxAlign.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"

namespace larocv{

  void VtxAlign::_Configure_(const ::fcllite::PSet &pset)
  {
  _require_3planes = pset.get<bool>  ("RequireThreePlanes");
  _ratio_cut       = pset.get<float>  ("RatioCut");
  }

  double VtxAlign::_Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& roi_vtx) 
  { 
  _require_3planes = false; 

  if(_require_3planes && clusters.size() == 2) return -1;

  auto geom  = larutil::Geometry::GetME();
  auto geomH  = larutil::GeometryHelper::GetME();

  for (auto c : clusters ){
    if ( c->_numHits() < 20) 
      return -1 ; 
    }

  double ratio = 1; 

   for ( size_t i = 0; i < clusters.size(); i++){ 

     for ( size_t j = i+1; j < clusters.size(); j++){ 

       //if (clusters[i]->_numHits < 20 || clusters[j]->_numHits < 20 ){
       //  ratio = -1;
       //  continue;
       //	 }
       double sY, sZ, eY, eZ ;
       
       auto sX = ( clusters[i]->roi.startpt.y + clusters[j]->roi.startpt.y ) / 2 * geomH->TimeToCm();
       auto eX = ( clusters[i]->roi.endpt.y + clusters[j]->roi.endpt.y ) / 2 * geomH->TimeToCm();
       
       auto sW0 = clusters[i]->roi.startpt.x ;
       auto sW1 = clusters[j]->roi.startpt.x ;

       auto eW0 = clusters[i]->roi.endpt.x ;
       auto eW1 = clusters[j]->roi.endpt.x ;

       geom->IntersectionPoint(sW0, sW1, clusters[i]->PlaneID(),clusters[j]->PlaneID(), sY, sZ );
       geom->IntersectionPoint(eW0, eW1, clusters[i]->PlaneID(),clusters[j]->PlaneID(), eY, eZ );

       //Now have 3 3d points; vertex, start, end; for each, check axis alignment
       double dist_se = sqrt( pow(eX - sX,2) + pow(eY - sY,2) + pow(eZ - sZ,2));
       double dist_roi = sqrt( pow(roi_vtx[0] - sX,2) + pow(roi_vtx[1] - sY,2) + pow(roi_vtx[2] - sZ,2));

       std::vector<double> start_to_end = { (eX - sX) / dist_se, 
                                            (eY - sY) / dist_se,
					    (eZ - sZ) / dist_se };

       std::vector<double> vtx_to_start = { (sX - roi_vtx[0]) / dist_roi,
                                            (sY - roi_vtx[1]) / dist_roi,
					    (sZ - roi_vtx[2]) / dist_roi };

       auto dot = start_to_end[0] * vtx_to_start[0] + 
                  start_to_end[1] * vtx_to_start[1] + 
		  start_to_end[2] * vtx_to_start[2] ;

       ratio *= dot;
       if(ratio > _ratio_cut ){
        std::cout<<"Clust size : " <<clusters.size() <<std::endl ;
        for (auto c : clusters )
	  std::cout<<c->_numHits()<<", " ;
	  std::cout<<std::endl ;
	  //std::cout<<clusters[i]->_numHits<<", "<<clusters[j]->_numHits<<std::endl ;

	  }


       }

     }

     //std::cout<<" ... we sjhould exit each iter "<<ratio<<std::endl ;

     if (ratio == 1 ) return -1 ;

      if( ratio  > _ratio_cut )
       std::cout<<"   Ratio! "<<ratio<<std::endl ;

  return (ratio > _ratio_cut ? ratio : -1 ) ; // negative means they did not match
  }

}
#endif
