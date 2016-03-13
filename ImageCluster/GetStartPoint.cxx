#ifndef GETSTARTPOINT_CXX
#define GETSTARTPOINT_CXX

#include "GetStartPoint.h"
#include "ClusterRecoUtil/Base/CRUException.h"
#include "ClusterRecoUtil/Base/ClusterParams.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"
//#include "ClusterRecoUtil/Alg/GetAverages.h"
#include "ClusterRecoUtil/Alg/DefaultParamsAlg.h"
#include <map>
#include <iomanip>

namespace larcv {

  void GetStartPoint::_Configure_(const ::fcllite::PSet &pset)
  {
    _nhits = pset.get<int>("NHits");
  }

larcv::Cluster2DArray_t GetStartPoint::_Process_(const larcv::Cluster2DArray_t& clusters,
                                                 const ::cv::Mat& img,
                                                 larcv::ImageMeta& meta)
 {
  // Geometry Utilities
  auto geomUtil = ::larutil::GeometryUtilities::GetME();

  Cluster2DArray_t ts_clusters(clusters);

  for( int i = 0; i < ts_clusters.size(); i++){

    auto & cluster = ts_clusters[i];
    int plane = cluster.PlaneID();
    auto const& hits = cluster._insideHits;

    if( hits.size() < _nhits ) continue;

    // get hit list
    std::vector<Hit2D> px_hits;  

    for( auto & h : hits ){
      Hit2D px;
      px.w = h.y * geomUtil->WireToCm() ;
      px.t = h.x * geomUtil->TimeToCm() ;
      px.charge = (int)img.at<uchar>(h.y,h.x);
      px.plane = plane ;
      px_hits.push_back(px);
      }

     ::cluster::cluster_params params;
     params.SetHits(px_hits);

     cluster::GetAverages         averages         ;     
     averages.do_params_fill(params)      ;

     cluster::FillPolygon         polygon          ;     
     polygon.do_params_fill(params)      ;

     cluster::GetRoughStartPoint  roughStartPoint  ;     
     roughStartPoint.do_params_fill(params)      ;

     cluster::FindShoweringPoint  showeringpoint   ;     
     showeringpoint.do_params_fill(params)      ;

     cluster::SelectStartPoint    selectStartPoint ;     
     selectStartPoint.do_params_fill(params)      ;

     cluster::RefineStartPoints   refineStartPoint ;     
     refineStartPoint.do_params_fill(params)      ;

     auto start = params.start_point ;
     auto end = params.end_point ;
     auto poly = params.PolyObject ;
     //std::cout<<"start: "<<start.w<<", "<<start.t<<std::endl;
     cluster._startPt = larcv::Point2D(start.t / geomUtil->TimeToCm() , start.w / geomUtil->WireToCm());
     cluster._endPt = larcv::Point2D(end.t / geomUtil->TimeToCm() , end.w / geomUtil->WireToCm());

     std::vector<std::pair<float, float> > vertices;
    
     //std::cout<<"Poly size: "<<poly.Size()<<std::endl ;
     for (unsigned int i = 0; i < poly.Size(); i++){
       
       vertices.push_back( std::make_pair( poly.Point(i).second / geomUtil->TimeToCm(), 
	                                   poly.Point(i).first / geomUtil->WireToCm() ) ); 
        } 

     cluster.PolyObject = Polygon2D( vertices ) ;

     }

  return ts_clusters;
  }
} 

#endif
