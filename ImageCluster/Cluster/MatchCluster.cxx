#ifndef __MATCHCLUSTER_CXX__
#define __MATCHCLUSTER_CXX__

#include "MatchCluster.h"
#include "Refine2DVertex.h"
#include "VertexTrackCluster.h"
#include "BaseUtil.h"
#include <utility>

namespace larocv{

  /// Global larocv::MatchClusterFactory to register ClusterAlgoFactory
  static MatchClusterFactory __global_MatchClusterFactory__;

  void MatchCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexName");
    _vtx_algo_id = this->ID(vtx_algo_name);

    auto const cont_algo_name = pset.get<std::string>("VertexTrackClusterName");
    _contour_algo_id = this->ID(cont_algo_name);

  }

 void MatchCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {

    _book_keeper.Reset(); 

    //Pieces of algorithm for end point storing taken from previous algs in laropencv
    auto & m_data    = AlgoData<larocv::MatchClusterData>();
    auto const& contour_data     = this->AlgoData<larocv::VertexTrackClusterData> ( _contour_algo_id );  
    auto const& vtx_data         = this->AlgoData<larocv::Refine2DVertexData> ( _vtx_algo_id     );  
    auto vtx = vtx_data._cand_vtx_v ;

    Cluster2DArray_t oclusters = clusters;
    m_data._endpts_v_v[meta.plane()].reserve(oclusters.size());

    auto contour_v = contour_data._ctor_vv[meta.plane()] ;
    std::vector<std::vector<::cv::Point>> full_cont_v; 
    
    full_cont_v.resize(contour_v.size()) ;
    
    for( int c = 0; c < contour_v.size(); c++) {
      auto & con_i = contour_v[c];
      full_cont_v[c].reserve(con_i.size());
      for(int pt=0; pt < con_i.size(); pt++){
        ::cv::Point p(con_i[pt].x,con_i[pt].y); 
        full_cont_v[c].emplace_back(p);
        }
      oclusters[c]._contour = full_cont_v[c] ;     
      }

    Contour_t all_locations;
    ::cv::findNonZero(img, all_locations); // get the non zero points

    Point2D p2(vtx[meta.plane()].x,vtx[meta.plane()].y);

    for( const auto& loc: all_locations ) { 
      for( size_t i = 0; i < full_cont_v.size(); i++ ) { 
    
      if ( ::cv::pointPolygonTest(full_cont_v[i],loc,false) < 0 ) 
          continue;

        oclusters[i]._insideHits.emplace_back(loc.x, loc.y);
        oclusters[i]._sumCharge += (int) img.at<uchar>(loc.y, loc.x);
        // When point is found in contour, no others are checked; avoids double counting
        // Requires SimpleCuts to the alg chain after this; may have clusters with 0 hits
        break;
      }   
    }   
    for(auto& ocluster : oclusters) {

      float d = 0;
      int idx = -1; 
      if( ocluster._insideHits.size() == 0 ) continue;
    
      for(unsigned i=0;i<ocluster._insideHits.size();++i){
        auto& ihit = ocluster._insideHits[i];
        float dd = dist(ihit,p2); 
        if ( dd > d ) 
          { d = dd; idx = i; }
        }   
      if (idx < 0) throw larbys("why");
      auto pair = std::make_pair(ocluster._insideHits[idx].x, ocluster._insideHits[idx].y);
      //std::cout<<"End point is: "<< pair.first<<", "<<pair.second<<std::endl ;
    
      m_data._endpts_v_v[meta.plane()].emplace_back(pair);
       }   

     m_data._clusters[meta.plane()] = oclusters; 

     }



  bool MatchCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v) 
  {

    auto & m_data    = AlgoData<larocv::MatchClusterData>();
    auto end_pts     = m_data._endpts_v_v ;
    auto clusters = m_data._clusters ;
    auto & match_v  = m_data._matches ;

    ///////////////////////////// MATCH CODE FROM OLD ALG + BOOKEEPER CODE STARTS HERE
    std::vector<size_t> seed; seed.reserve(end_pts.size());
    for(auto const& c : end_pts) seed.push_back(c.size());

    auto comb_v = PlaneClusterCombinations(seed); 

    //std::cout<<"Combination size: "<<comb_v.size()<<std::endl ;
    for( auto const & comb : comb_v ){
      larocv::Cluster2DArray_t clus_perm;
      for( auto const & c : comb )
        clus_perm.emplace_back(clusters[c.first][c.second]); //first is plane, second is clusterID

      if (clus_perm.size() < 2) continue; 

      double t_min_abs = 9600; // smallest start point of the 3
      double t_max_abs = 0;    // largest start point of the three

      float max_hits = 0;
      float min_hits = 1e9;
      
      //Use data manager to get the algorithm that goes with "clusters", I guess.
      for(auto const& c : clus_perm){
      
        double min(0.0), max(0.0);
      
        getMinMaxTime(&c,min,max);
      
        if ( min < t_min_abs )
          t_min_abs = min;
        if ( max   > t_max_abs )
          t_max_abs = max;

        // Also find max and min hits
        if ( c._insideHits.size() > max_hits )
          max_hits = c._insideHits.size();

        if ( c._insideHits.size() < min_hits )
          min_hits = c._insideHits.size();

      }   

      // do the clusters overlap at all?
      bool overlap = true;
      double t_min_common(0.0), t_max_common(0.0);

      getMinMaxTime(&clus_perm[0],t_min_common,t_max_common);

      for (size_t i=1; i < clus_perm.size(); i++){

        double t_min(0.0), t_max(0.0); // min and max for current cluster
        getMinMaxTime(&clus_perm[i],t_min,t_max);

        // now find overlap
        if (t_max < t_min_common){
          overlap = false;
          break;
        }

        if (t_min > t_max_common){
          overlap = false;
          break;
        }

           if (t_min > t_min_common) t_min_common = t_min;
        if (t_max < t_max_common) t_max_common = t_max;

      }// for all clusters

      //// If one cluster has < 10% of the hits of the other cluster, ignore this pair
      //double hit_frac = min_hits / max_hits;

      if (overlap == false) continue; // || hit_frac < 0.1 ) return -1;

      //// calculate overlap
      double iou_score = (t_max_common - t_min_common) / (t_max_abs - t_min_abs);
      //std::cout<<"Iou is: "<<iou_score <<std::endl ;

      std::vector<unsigned int> tmp_index_v;
      tmp_index_v.reserve(comb.size());

      for(auto const& cinfo : comb) {

        auto const& plane = cinfo.first;
        auto const& index = cinfo.second;
        tmp_index_v.push_back(clusters[plane][index].ClusterID());
      
        }   
      
      if(iou_score>0)
        _book_keeper.Match(tmp_index_v,iou_score);

      }

      auto res_v = _book_keeper.GetResult();      

      match_v.resize(res_v.size());
      
      for(int r = 0; r < res_v.size(); r++){  // auto const & r : res_v ) 
        match_v[r].resize(res_v[r].size());
	//std::cout<<"\nResult size: "<<res_v[r].size()<<std::endl ;
	for(int i = 0; i < res_v[r].size(); i++){
	  //std::cout<<res_v[r][i]<<", ";
          match_v[r][i] = res_v[r][i];
	  }
	}

        return true ;
      }

  void MatchCluster::getMinMaxTime(const Cluster2D* cluster, double& min, double& max)
  {
    
    auto const& hits = cluster->_insideHits;
    
    min = 9600;
    max = 0;
    
    for (auto const& hit : hits){
      auto t = _pixel_y(cluster,hit.x);
    
      if (t > max) max = t;
      if (t < min) min = t;
    
    }// for all hits
    
    return;
  }

  double MatchCluster::_pixel_y(const Cluster2D* cluster,size_t pix) {
    auto const& px_h   = cluster->PixelHeight();
    auto const& origin = cluster->Origin();

    return (pix+0.5)*px_h + origin.y;
  }

  float MatchCluster::dist(const ::cv::Point& pt1, const Point2D& pt2) {
    float dx = pt1.x - pt2.x;
    float dy = pt1.y - pt2.y;
    return std::sqrt(dx*dx + dy*dy);
    }

}

#endif
