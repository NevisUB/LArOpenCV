#ifndef __INCONECLUSTER_CXX__
#define __INCONECLUSTER_CXX__

#include "InConeCluster.h"
#include "LArUtil/GeometryHelper.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"
#include "ClusterRecoUtil/Base/ClusterParams.h"

namespace larcv {

  void InConeCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _area_separation  = pset.get<double> ( "AreaCut"    );
    _cone_length      = pset.get<double> ( "ConeLength" );
    _cone_angle       = pset.get<double> ( "ConeAngle"  );
    _n_hits           = pset.get<double> ( "MinHitsShower"  );
  }


  Cluster2DArray_t InConeCluster::_Process_(const larcv::Cluster2DArray_t& clusters,
                                            const ::cv::Mat& img,
                                            larcv::ImageMeta& meta)
  {
    //std::cout<<"\nNew plane : "<<meta.plane() <<std::endl;

    auto geoH = ::larutil::GeometryHelper::GetME();

    Cluster2DArray_t shower_v; shower_v.reserve(clusters.size());
    Cluster2DArray_t satellite_v; satellite_v.reserve(clusters.size());

    /// SEPARATE SHOWERS AND SATELLITES

    /// 0) Find distance from cluster starts to roi vtx
    Cluster2DArray_t orig_clus = clusters ;
    std::multimap<float, int> clus_dists ;

    for (int i = 0; i < clusters.size(); ++i) {
      auto pi0st = clusters[i].roi.vtx;
      auto cst  = clusters[i].roi.startpt ;
      auto dist = sqrt( pow(pi0st.x - cst.x, 2) + pow(pi0st.y - cst.y, 2) );

      clus_dists.emplace(dist, i);
    }

    /// 1) Set 2 closest clusters with >=25 hits to be showers -- everything else satellite
    int map_it = 0;
    for ( auto & c : clus_dists) {
      if ( map_it <= 1 && orig_clus[c.second]._numHits >= _n_hits && c.first < 300 ) {
        shower_v.push_back(orig_clus[c.second]) ;
        map_it ++ ;
      }
      else
        satellite_v.push_back(orig_clus[c.second]);
    }

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

      auto c1 = shower_v[i];

      /// Find the shower cluster's cone extent
      std::pair<float, float> start( c1.roi.startpt.x, c1.roi.startpt.y);
      std::pair<float, float> end  ( c1.roi.endpt.x  , c1.roi.endpt.y  );
      
      // is this a hack to get the direction right, sometime the cone
      // projects out in the wrong direction and collects the wrong
      // amount of charge
      // std::cout << "c1.reco.dir.y" << c1.reco.dir.y << "\n";
      // std::cout << "c1.reco.dir.x" << c1.reco.dir.x << "\n";
      if  ( c1.roi.endpt.x > c1.roi.startpt.x )  {
        if ( c1.reco.dir.x < 0 ) {
          c1.reco.dir.y *= -1.0;
          c1.reco.dir.x *= -1.0;
        }
      }
      else {
        if ( c1.reco.dir.x > 0 ) {
          c1.reco.dir.y *= -1.0;
          c1.reco.dir.x *= -1.0;
        }
      }

      std::pair<float, float> dir  ( c1.reco.dir.y    , c1.reco.dir.x    );

      auto orig_angle = atan2(dir.first, dir.second) ;
      double length  = pow( pow(start.first - end.first, 2) + pow(start.second - end.second, 2), 0.5 ) ;
      auto length_cm_space  = pow( pow((start.first - end.first) * geoH->TimeToCm(), 2) + pow((start.second - end.second) * geoH->WireToCm(), 2), 0.5 ) ;
      float mult = _cone_length / length_cm_space ;
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
      c1._cone_contour = cone_contour ;

      std::map<float, larcv::Contour_t> merge_us ;

      /// 1) Loop over satellites and decide whether or not to associate with shower
      for (unsigned j = 0; j < satellite_v.size(); ++j) {

        if ( used_sats[j] ) continue;

        auto& c2 = satellite_v[j];
        std::vector<::cv::Point> small_hits = c2._insideHits ;

        float COM_t_s(0.), COM_w_s(0.);

        for (auto& hit : small_hits) {
          int charge = (int) img.at<uchar>(hit.y, hit.x);
          COM_t_s += hit.x * charge;
          COM_w_s += hit.y * charge;
        }

        ::cv::Point COM (COM_t_s / c2._sumCharge , COM_w_s / c2._sumCharge ) ;

        if ( ::cv::pointPolygonTest(cone_contour, COM, false) < 0 ) continue;

        c1._area += c2._area ;
        c1._insideHits.reserve( c2._insideHits.size() + c1._insideHits.size() );
        c1._insideHits.insert( c1._insideHits.end(), c2._insideHits.begin(), c2._insideHits.end() );
        c1._numHits = c1._insideHits.size();
        c1.roi.endpt.x = c2.roi.endpt.x;
        c1.roi.endpt.y = c2.roi.endpt.y ;
        c1._sumCharge += c2._sumCharge ;
        c1._perimeter += c2._perimeter ;

        /// 2) Attempt to detangle. Store each satellite to be merged + the index of contour
        _order_sats(c1, c2, merge_us, COM);

        used_sats[j] = true;
      }

      /// 3) Merge stuff !
      for ( auto & m : merge_us) {

        std::vector<::cv::Point> c3;
        _combine_two_contours(c1._contour, m.second, c3, 1. / m.first);
        c1._contour = c3 ;

      }

      /// 4) Make sure we're not passing any showers that'll be found to have no hits (more twist issues)
      bool hit_inside = false ;
      int loc_i = 0;
      for ( const auto& loc : all_locations ) {

        if ( !hit_used[loc_i] && ::cv::pointPolygonTest(c1._contour, loc, false) >= 0 ) {
          hit_inside = true;
          hit_used[loc_i] = true;
        }
        loc_i ++ ;
      }

      if ( hit_inside )
        result.push_back(c1) ;
    }

    for (size_t i = 0; i < used_sats.size(); i++) {
      if ( !used_sats[i] ) result.push_back(satellite_v[i]);
    }

    return result;
  }

  void InConeCluster::_order_sats(larcv::Cluster2D& c1, larcv::Cluster2D & c2, std::map<float, larcv::Contour_t> & merge_us, const ::cv::Point & COM ) {
    int min_index = 0;
    float min_dist = 10000;
    for ( int p = 1; p < c1._contour.size(); ++p ) {
      auto const & pt = c1._contour[p] ;
      float dist = sqrt ( pow(pt.x - COM.x, 2) + pow(pt.y - COM.y, 2));
      if ( dist < min_dist ) {
        min_dist = dist ;
        min_index = p ;
      }
    }

    if ( merge_us.find(1. / min_index) != merge_us.end() ) {
      while (true) {
        min_index--;
        if ( merge_us.find(1. / min_index) == merge_us.end() ) {
          //std::cout<<"Making connection index smaller "<<std::endl;
          merge_us[1. / min_index] = c2._contour;
          break ;
        }
        if ( min_index == 0 ) break;
      }
    }
    else
      merge_us[1. / min_index] = c2._contour;

  }

  void InConeCluster::_combine_two_contours(const larcv::Contour_t& c1, const larcv::Contour_t& c2, larcv::Contour_t& c3) {
    c3.clear();
    c3.reserve( c1.size() + c2.size() );
    c3.insert( c3.end(), c1.begin(), c1.end() );
    c3.insert( c3.end(), c2.begin(), c2.end() );
  }

  void InConeCluster::_combine_two_contours(const larcv::Contour_t& c1, 
                                            const larcv::Contour_t& c2, 
                                            larcv::Contour_t& c3, 
                                            const int & offset) {
    c3.clear();
    c3.reserve( c1.size() + c2.size() );
    c3.insert( c3.end(), c1.begin(), c1.begin() + offset);
    c3.insert( c3.end(), c2.begin(), c2.end() );
    c3.insert( c3.end(), c1.begin() + offset, c1.end());
  }


}
#endif
