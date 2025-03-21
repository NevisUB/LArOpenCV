#ifndef __NEARESTCONECLUSTER_CXX__
#define __NEARESTCONECLUSTER_CXX__

#include "NearestConeCluster.h"

namespace larocv{

  /// Global larocv::NearestConeClusterFactory to register AlgoFactory
  static NearestConeClusterFactory __global_NearestConeClusterFactory__;
  
  void NearestConeCluster::_Configure_(const Config_t &pset)
  {}

  Cluster2DArray_t NearestConeCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta, larocv::ROI& roi)
  { 
    
    //std::cout<<"\n Plane is: "<<meta.plane()<<std::endl ;

    Cluster2DArray_t shower_v; shower_v.reserve(clusters.size());
    Cluster2DArray_t satellite_v; satellite_v.reserve(clusters.size());
    
    /// SEPARATE SHOWERS AND SATELLITES
    Cluster2DArray_t orig_clus = clusters ;
    
    for(int i=0; i < clusters.size(); ++i){
      if( clusters[i]._cone_contour.size() == 3 )
        shower_v.push_back(clusters[i]);
      else
        satellite_v.push_back(clusters[i]);
      }   

    Cluster2DArray_t result; result.reserve(clusters.size());

    /// Iterating over satellites first-- do this to use min distance to cone
    std::vector<std::vector<int>> shower_to_sat_merge (shower_v.size(),std::vector<int>(0));
    shower_to_sat_merge.reserve(satellite_v.size());

    for(unsigned i = 0; i < satellite_v.size(); ++i) {

        auto c2 = satellite_v[i];

        float COM_t_s(0.), COM_w_s(0.);

        for (auto& hit: c2._insideHits ){
          int charge = (int) img.at<uchar>(hit.y, hit.x);
          COM_t_s += hit.x * charge;
          COM_w_s += hit.y * charge;
          }

        ::cv::Point COM (COM_t_s / c2._sumCharge , COM_w_s / c2._sumCharge ) ;

        float min_dist = 1000000.;
        int min_dist_index = -1; 

          for(unsigned j = 0; j < shower_v.size(); ++j) {

            auto& c1 = shower_v[j];
            float dist = fabs(::cv::pointPolygonTest(c1._cone_contour,COM,true));
	    //std::cout<<"Dist is : "<<dist<<std::endl ;
            if( dist < min_dist ){
              min_dist = dist ;
              min_dist_index = j ;
              } 
            }

	  if( min_dist == 1000000 ) continue ;

	  //std::cout <<" Min dist is: "<<min_dist <<std::endl ;
          auto& c1 = shower_v[min_dist_index];
          c1._area += c2._area ;
          c1._insideHits.reserve( c2._insideHits.size() + c1._insideHits.size() );
          c1._insideHits.insert( c1._insideHits.end(), c2._insideHits.begin(), c2._insideHits.end() );
          c1._sumCharge += c2._sumCharge ;
          c1._perimeter += c2._perimeter ;
           
          /// Add the contours once we know everything we have to merge
          shower_to_sat_merge[min_dist_index].push_back(i);
          } 

        /// DECIDE WHAT TO MERGE FIRST (this is to avoid contour twists)
        for(unsigned j = 0; j < shower_v.size(); ++j) {

          auto& c1 = shower_v[j];
          std::map<float,larocv::Contour_t> merge_us ;
          for(unsigned i = 0; i < shower_to_sat_merge[j].size(); ++i) {
            auto c2 = satellite_v[i];

            float COM_t_s(0.), COM_w_s(0.);

            for (auto& hit: c2._insideHits){
              int charge = (int) img.at<uchar>(hit.y, hit.x);
              COM_t_s += hit.x * charge;
              COM_w_s += hit.y * charge;
              }

              ::cv::Point COM (COM_t_s / c2._sumCharge , COM_w_s / c2._sumCharge ) ;
              if( shower_to_sat_merge[j][i] == i )
               _order_sats(c1,c2,merge_us,COM);

             }

          for( auto & m : merge_us){
            std::vector<::cv::Point> c3;
            _combine_two_contours(c1._contour,m.second,c3,1./m.first);
            c1._contour = c3 ;
            }
         
           }

        for(unsigned j = 0; j < shower_v.size(); ++j) 
          result.push_back(shower_v[j]); 
  
  return result; 
  }

   void NearestConeCluster::_order_sats(larocv::Cluster2D& c1, larocv::Cluster2D & c2, std::map<float,larocv::Contour_t> & merge_us, const ::cv::Point & COM ){
     int min_index = 0;
     float min_dist = 10000;
     for ( int p = 1; p < c1._contour.size(); ++p ){
       auto const & pt = c1._contour[p] ;
       float dist = sqrt ( pow(pt.x - COM.x,2) + pow(pt.y - COM.y,2));
       if ( dist < min_dist ){
         min_dist = dist ;
         min_index = p ;
         }
       }

     if ( merge_us.find(1./min_index) != merge_us.end() ){
       while(true){
         min_index--;
         if( merge_us.find(1./min_index) == merge_us.end() ){
           merge_us[1./min_index] = c2._contour;
           break ;
          }
         if( min_index == 0 ) break;
         }
       }
     else
       merge_us[1./min_index] = c2._contour;

     }

 void NearestConeCluster::_combine_two_contours(const larocv::Contour_t& c1, const larocv::Contour_t& c2, larocv::Contour_t& c3, const int & offset) {
    c3.clear();
    c3.reserve( c1.size() + c2.size() );
    c3.insert( c3.end(), c1.begin(), c1.begin() + offset);
    c3.insert( c3.end(), c2.begin(), c2.end() );
    c3.insert( c3.end(), c1.begin() + offset, c1.end());
  }

}
#endif
