#ifndef __INHULLMERGE_CXX__
#define __INHULLMERGE_CXX__

#include "InHullMerge.h"
#include "LArUtil/GeometryHelper.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"
#include "ClusterRecoUtil/Base/ClusterParams.h"

namespace larocv {

  void InHullMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _area_separation  = pset.get<double> ( "AreaCut"    );
    _cone_length      = pset.get<double> ( "ConeLength" );
    _cone_angle       = pset.get<double> ( "ConeAngle"  );
    _n_hits           = pset.get<double> ( "MinHitsShower"  );
  }


  Cluster2DArray_t InHullMerge::_Process_(const larocv::Cluster2DArray_t& clusters,
                                            const ::cv::Mat& img,
                                            larocv::ImageMeta& meta, larocv::ROI& r)
  {
    //std::cout<<"\nNew plane : "<<meta.plane() <<std::endl;

    auto geoH = ::larutil::GeometryHelper::GetME();

    Cluster2DArray_t shower_v; shower_v.reserve(clusters.size());
    Cluster2DArray_t satellite_v; satellite_v.reserve(clusters.size());
    // Could be showers, but too aligned with a bigger cluster; don't 
    // treat as showers, but also don't allowed them to be merged with
    // other shower candidates
    Cluster2DArray_t dontmerge_v; dontmerge_v.reserve(clusters.size()); 

    //start -- debug
    std::stringstream ss1,ss2,ss3,ss4;
    ::larlite::user_info uinfo{};

    ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
    uinfo.store("ID",ss1.str());

    ss1.str(std::string());
    ss1.clear();
    //end -- debug


    /// SEPARATE SHOWERS AND SATELLITES
    Cluster2DArray_t orig_clus = clusters ;
    auto pi0st = r.roivtx_in_image(meta);

    /// 0) Find distance from cluster starts to roi vtx
    std::multimap<float, int> clus_dists ;

    for (int i = 0; i < clusters.size(); ++i) {
      auto cst  = clusters[i].roi.startpt ;
      auto dist = sqrt( pow(pi0st.x - cst.x, 2) + pow(pi0st.y - cst.y, 2) );

      clus_dists.emplace(dist, i);
      if (meta.plane() == 2)
        std::cout<<"Filling map : "<<clusters[i]._num_hits_pool <<std::endl;
    }
    
    /// 1) Set 2 closest clusters with >=15 hits to be showers -- everything else satellite
    int map_it = 0;
    for ( auto & c : clus_dists) {
      //if ( map_it <= 1 && orig_clus[c.second]._num_hits_pool >= _n_hits && c.first < 300 ) {
      if ( (map_it <= 1 && orig_clus[c.second]._num_hits_pool >= _n_hits && c.first < 300) 
           || orig_clus[c.second]._num_hits_pool > 60 ) {

	auto c2 = orig_clus[c.second] ; 

        shower_v.push_back(orig_clus[c.second]) ;
        map_it ++ ;
      }
      else
        satellite_v.push_back(orig_clus[c.second]);
    }

    std::cout<<"Number of showers and satellites: "<<shower_v.size()<<", "<<satellite_v.size()<<std::endl ;



        // 1b) Check that showers are not aligned; if they are, save shower with larger hits
	//     and keep searching
      //  if ( shower_v.size() > 0 ){
      //    
      //    for ( int s = 0 ; s < shower_v.size(); s++ ){
      //    auto c1 = shower_v.at(0) ;

      //    std::pair<float, float> start1( c1.roi.startpt.x, c1.roi.startpt.y );
      //    std::pair<float, float> start2( c2.roi.startpt.x, c2.roi.startpt.y );
      //    
      //    auto dist1 = sqrt ( pow(start1.first - pi0st.x,2) + pow(start1.second - pi0st.y,2)); 
      //    auto dist2 = sqrt ( pow(start2.first - pi0st.x,2) + pow(start2.second - pi0st.y,2)); 

      //    c1.roi.dir.x = ( c1.roi.startpt.x - pi0st.x ) / dist1;
      //    c1.roi.dir.y = ( c1.roi.startpt.y - pi0st.y ) / dist1;

      //    c2.roi.dir.x = ( c2.roi.startpt.x - pi0st.x ) / dist2;
      //    c2.roi.dir.y = ( c2.roi.startpt.y - pi0st.y ) / dist2;

      //    auto dot = (c1.roi.dir.x * c2.roi.dir.x + c1.roi.dir.y * c2.roi.dir.y ) ; 

      //    std::cout<<"Dot product is: "<<dot<<std::endl ;
      //    if ( dot > 0.95 ){
      //      if ( c1._num_hits_pool > c2._num_hits_pool ){

      //       if ( c2._num_hits_pool > 60 ) 
      //         dontmerge_v.push_back(c2) ;
      //       else
      //        satellite_v.push_back(c2) ;

      //      //continue;
      //      }
      //      else{
      //        shower_v.at(0) = c2 ;

      //       if ( c1._num_hits_pool > 60 )
      //         dontmerge_v.push_back(c1) ;
      //       else
      //        satellite_v.push_back(c1) ;
      //      //continue;
      //      }
      //    }
      //  }

      //} // If more than one shower, check the number of hits etc


    /// 0) Find cluster hit sizes and store in map 
    //std::multimap<float, int> clus_hits;

    //for (int i = 0; i < clusters.size(); ++i) clus_hits.emplace(1./clusters[i]._num_hits_pool, i);

    ///// 1) Set 2 largest clusters to be showers -- everything else satellite
    //int map_it = 0;
    //for ( auto & c : clus_hits) {
    //  if ( map_it <= 1 ) {
    //    shower_v.push_back(orig_clus[c.second]) ;
    //    map_it ++ ;
    //  }
    //  else
    //    satellite_v.push_back(orig_clus[c.second]);
    //}

    /// 2) Don't want to reuse merged satellites-- store status here
    std::map<size_t, bool> used_sats;
    for (unsigned k = 0; k < satellite_v.size(); ++k) used_sats[k] = false;

    Cluster2DArray_t result; result.reserve(clusters.size());

    /// DECIDE WHAT TO MERGE

    /// Some variables for checks on our final clusters
    Contour_t all_locations;
    ::cv::findNonZero(img, all_locations);
    std::vector<bool> hit_used(all_locations.size(), false);

    /// 0) Loop over showers (should only be 2 after step 1) above)
    for (unsigned i = 0; i < shower_v.size(); ++i) {

      auto & c1 = shower_v[i];

      std::cout<<"Prior: "<<c1._num_hits_pool<<std::endl ;

      /// Find the shower cluster's cone extent
      std::pair<float, float> start( c1.roi.startpt.x, c1.roi.startpt.y);
      std::pair<float, float> end  ( c1.roi.endpt.x  , c1.roi.endpt.y  );
      
      auto dist = sqrt ( pow(end.first - start.first,2) + pow(end.second - start.second,2)); 

      c1.roi.dir.x = ( c1.roi.startpt.x - pi0st.x ) / dist;
      c1.roi.dir.y = ( c1.roi.startpt.y - pi0st.y ) / dist;

      std::pair<float, float> dir  ( c1.roi.dir.y    , c1.roi.dir.x    );

      auto orig_angle = atan2(dir.first, dir.second) ;
      double length  = pow( pow(start.first - end.first, 2) + pow(start.second - end.second, 2), 0.5 ) ;
      auto length_cm_space  = pow( pow(6*(start.first - end.first) * geoH->TimeToCm(), 2) + pow((start.second - end.second) * geoH->WireToCm(), 2), 0.5 ) ;
      float mult = _cone_length / length_cm_space ;
      //std::cout<<"Mult: "<<mult<<", "<<std::endl;
      if ( mult > 8 ) mult = 8 ;
      double new_length = length * mult ;
      double cone_width = new_length * tan ( 3.14159 / 180. *_cone_angle ) / 2;
      //double cone_sides = new_length / cos ( _cone_angle );

      ::cv::Point second_pt, third_pt;
      std::vector<::cv::Point> cone_contour;
      ::cv::Point first_pt (start.first, start.second) ;

      auto new_end_x = start.first  + new_length * cos(orig_angle) ;
      auto new_end_y = start.second + new_length * sin(orig_angle) ;

      second_pt.x = ( new_end_x + cone_width * sin(orig_angle));
      third_pt.x  = ( new_end_x - cone_width * sin(orig_angle));

      if ( dir.first * dir.second < 0 ) {
        second_pt.y = ( new_end_y + cone_width * cos(orig_angle));
        third_pt.y  = ( new_end_y - cone_width * cos(orig_angle));
      }
      else if ( dir.first * dir.second >= 0 ) {
        second_pt.y = ( new_end_y - cone_width * cos(orig_angle));
        third_pt.y  = ( new_end_y + cone_width * cos(orig_angle));
      }

      cone_contour.push_back(first_pt);
      cone_contour.push_back(second_pt);
      cone_contour.push_back(third_pt);
      c1._contour = cone_contour ;
      c1._cone_contour = cone_contour ;

      std::map<float, larocv::Contour_t> merge_us ;

      /// 1) Loop over satellites and decide whether or not to associate with shower
      for (unsigned j = 0; j < satellite_v.size(); ++j) {

        if ( used_sats[j] ) continue;

        auto& c2 = satellite_v[j];
        std::vector<::cv::Point> small_hits = c2._insideHits ;

	bool mergeit = false;

        for (auto& hit : small_hits) {

          if ( ::cv::pointPolygonTest(cone_contour, hit, false) < 0 ) continue;
          else{
	    mergeit = true;
	    break;
	  } 
        }

	if( !mergeit ) continue;

	std::cout<<"Adding a cluster!" <<c2._num_hits_pool <<std::endl;

        c1._area += c2._area ;
        c1._insideHits.reserve( c2._insideHits.size() + c1._insideHits.size() );
        c1._insideHits.insert( c1._insideHits.end(), c2._insideHits.begin(), c2._insideHits.end() );
        c1.roi.endpt.x = c2.roi.endpt.x;
        c1.roi.endpt.y = c2.roi.endpt.y ;
        c1._sumCharge += c2._sumCharge ;
        c1._perimeter += c2._perimeter ;
        c1._num_hits_pool += c2._num_hits_pool ;
	//c1._contour = cone_contour ; 

        used_sats[j] = true;
      }

      std::vector<cv::Point> hullpts_v ;
      cv::convexHull(c1._insideHits,hullpts_v);
      c1._contour = hullpts_v; 

      result.emplace_back(c1);
      std::cout<<"RESULTS: "<<result.size()<<", "<<c1._num_hits_pool<<std::endl ;

    //***************************************************
    //***Debug Mode
    if ( !meta.debug() ) continue;    

    std::stringstream ss_x,ss_y;
    ss_x  << "Cone_" << c1.ClusterID()<<"_x";
    ss_y  << "Cone_" << c1.ClusterID()<<"_y";

    for (int i = 0; i < 3; i ++ ) {

        double x1 = meta.XtoTimeTick(c1._cone_contour[i].x);
        double y1 = meta.YtoWire(c1._cone_contour[i].y);
      
        uinfo.append(ss_x.str(),x1); 
        uinfo.append(ss_y.str(),y1); 
        
	if ( i == 2 ){
        double x1 = meta.XtoTimeTick(c1._cone_contour[0].x);
        double y1 = meta.YtoWire(c1._cone_contour[0].y);
      
        uinfo.append(ss_x.str(),x1); 
        uinfo.append(ss_y.str(),y1); 
	}

       }

        ss_x.str(std::string());
        ss_x.clear();

        ss_y.str(std::string());
        ss_y.clear();

    }

    for (size_t i = 0; i < used_sats.size(); i++) {
      if ( !used_sats[i] ) result.push_back(satellite_v[i]);
    }

   if ( meta.debug() ) 
   meta.ev_user()->emplace_back(uinfo);


   std::cout<<"FINAL: "<<result.size()<<std::endl ;

    return result;
  }

}
#endif
