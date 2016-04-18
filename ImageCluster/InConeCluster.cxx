#ifndef __INCONECLUSTER_CXX__
#define __INCONECLUSTER_CXX__

#include "InConeCluster.h"

namespace larcv{

  void InConeCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _area_separation  = pset.get<double> ( "AreaCut"    );
    _hit_cut          = pset.get<double> ( "HitCut"    );
    _length_mult      = pset.get<double> ( "LengthMult" );
    _cone_angle       = pset.get<double> ( "ConeAngle"  );
  }
  
  
  Cluster2DArray_t InConeCluster::_Process_(const larcv::Cluster2DArray_t& clusters,
					     const ::cv::Mat& img,
					     larcv::ImageMeta& meta)
  {
    //std::cout<<"New plane : "<<meta.plane() <<std::endl;

    Cluster2DArray_t shower_v; shower_v.reserve(clusters.size());
    Cluster2DArray_t satellite_v; satellite_v.reserve(clusters.size());

    for(auto & cluster : clusters) {
      //if ( cluster._area > _area_separation )
      if ( cluster._numHits > _hit_cut )
	shower_v.push_back(cluster);
      else
	satellite_v.push_back(cluster);
      }

    std::map<size_t,bool> used_sats;
    std::vector<::cv::Point> c3;
    c3.clear();
      
      for(unsigned k = 0; k < satellite_v.size(); ++k) used_sats[k] = false;

      //Define cone parameters
      double length;
      std::pair<float,float> start ;
      std::pair<float,float> end ;
      std::vector<::cv::Point> small_hits;
      Cluster2DArray_t result; result.reserve(clusters.size());
      
      for(unsigned i = 0; i < shower_v.size(); ++i) {

	auto c1 = shower_v[i]; 
        start = std::make_pair(c1.roi.startpt.x, c1.roi.startpt.y);
        end   = std::make_pair(c1.roi.endpt.x, c1.roi.endpt.y) ;

        std::pair<float,float> dir ( c1.roi.dir.y, c1.roi.dir.x ); 
        auto orig_angle = atan2(dir.first, dir.second) ;
        length  = pow( pow(start.first - end.first,2) + pow(start.second - end.second,2),0.5 ) ;
        double new_length = length * _length_mult ;
        double cone_width = new_length * tan ( 3.14159 / 180. *_cone_angle ) / 2;
        //double cone_sides = new_length / cos ( _cone_angle ); 
	//std::cout<<"x and y direction : "<<dir.first<<", "<<dir.second<<std::endl ;
	//std::cout<<"cone width : "<<cone_width<<std::endl ;

        ::cv::Point second_pt, third_pt;
        std::vector<::cv::Point> cone_contour;
        ::cv::Point first_pt (start.first,start.second) ;
        cone_contour.push_back(first_pt);

	auto new_end_x = start.first  + new_length * cos(orig_angle) ; 
	auto new_end_y = start.second + new_length * sin(orig_angle) ; 

        second_pt.x = (new_end_x + cone_width * sin(orig_angle));
        third_pt.x = ( new_end_x - cone_width * sin(orig_angle));

        if ( dir.first * dir.second < 0 ){
          second_pt.y = ( new_end_y + cone_width * cos(orig_angle));
          third_pt.y  = ( new_end_y - cone_width * cos(orig_angle)); 
          }
        else if ( dir.first * dir.second >= 0 ){
          second_pt.y = ( new_end_y - cone_width * cos(orig_angle));
          third_pt.y  = ( new_end_y + cone_width * cos(orig_angle)); 
          }
       

        cone_contour.push_back(second_pt); cone_contour.push_back(third_pt);

	c1._cone_contour = cone_contour ;

	for(unsigned j = 0; j < satellite_v.size(); ++j) {
	  
	  if ( used_sats[j] ) continue;

          auto& c2 = satellite_v[j];
          small_hits = c2._insideHits ;

          float COM_t_s(0.), COM_w_s(0.); 

          for (auto& hit: small_hits){
            int charge = (int) img.at<uchar>(hit.y, hit.x);  
            COM_t_s += hit.x * charge;
            COM_w_s += hit.y * charge;
            }
          COM_t_s /= c2._sumCharge;
          COM_w_s /= c2._sumCharge;

          ::cv::Point COM (COM_t_s, COM_w_s) ;
         
          if( ::cv::pointPolygonTest(cone_contour,COM,false) < 0 ) continue;
        
          //std::cout<<"We've merged things! "<<c1._area<<", "<<c2._area<<std::endl ;
	  _combine_two_contours(c1._contour,c2._contour,c3);
       
          c1._area += c2._area ;
          c1._insideHits.reserve( c2._insideHits.size() + c1._insideHits.size() );
          c1._insideHits.insert( c1._insideHits.end(), c2._insideHits.begin(), c2._insideHits.end() );
          c1._contour = c3 ;
          c1._endPt.x = COM_t_s ;
          c1._endPt.y = COM_w_s ;
          c1._sumCharge += c2._sumCharge ;
          c1._perimeter += c2._perimeter ;

	  used_sats[j] = true;
        
	}
       result.push_back(c1) ;
      }

      for(size_t i = 0; i < used_sats.size(); i++){
        if ( !used_sats[i] ) result.push_back(satellite_v[i]);
        }

    return result;
  }

  void InConeCluster::_combine_two_contours(const larcv::Contour_t& c1, const larcv::Contour_t& c2, larcv::Contour_t& c3) {
    c3.clear();
    c3.reserve( c1.size() + c2.size() );
    c3.insert( c3.end(), c1.begin(), c1.end() );
    c3.insert( c3.end(), c2.begin(), c2.end() );
  }

}
#endif
