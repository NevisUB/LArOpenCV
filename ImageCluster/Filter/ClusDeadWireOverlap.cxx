#ifndef __CLUSDEADWIREOVERLAP_CXX__
#define __CLUSDEADWIREOVERLAP_CXX__

#include "ClusDeadWireOverlap.h"
#include "FhiclLite/ConfigManager.h"

namespace larocv{

  /// Global larocv::ClusDeadWireOverlapFactory to register ClusterAlgoFactory
  static ClusDeadWireOverlapFactory __global_ClusDeadWireOverlapFactory__;

  void ClusDeadWireOverlap::_Configure_(const ::fcllite::PSet &pset)
  {
    //Wire loading code from Vic's DeadWireCombine Merge algorithm
    auto deadwirepath = pset.get< std::string >("DeadWirePath");
    std::cout << "\t>> Configuring DeadWireCombine with fcl: " << deadwirepath << " <<\n";
    ::fcllite::ConfigManager algo_mgr("DeadWireCombine");
    algo_mgr.AddCfgFile(deadwirepath);
    auto const& algo_cfg = algo_mgr.Config().get_pset("DeadWireCombine");

    auto p0_l = algo_cfg.get< std::vector<int> >("Plane0Left");
    auto p0_r = algo_cfg.get< std::vector<int> >("Plane0Right");
    
    for(int i=0;i<p0_l.size();++i)
      _wires_v[0].push_back( { p0_l[i] , p0_r[i] } );
    
    auto p1_l = algo_cfg.get< std::vector<int> >("Plane1Left");
    auto p1_r = algo_cfg.get< std::vector<int> >("Plane1Right");

    for(int i=0;i<p1_l.size();++i)
      _wires_v[1].push_back( { p1_l[i] , p1_r[i] } );
    
    auto p2_l = algo_cfg.get< std::vector<int> >("Plane2Left");
    auto p2_r = algo_cfg.get< std::vector<int> >("Plane2Right");

    for(int i=0;i<p2_l.size();++i)
      _wires_v[2].push_back( { p2_l[i] , p2_r[i] } );
 
  }

  Cluster2DArray_t ClusDeadWireOverlap::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta,
					larocv::ROI& roi)
  { 

    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());
  
   //Wire loading code from Vic's DeadWireCombine Merge algorithm


    auto const & vtx_l_w = roi.roivtx_in_image(meta).y - (roi.width()/3); 
    auto const & vtx_r_w = roi.roivtx_in_image(meta).y + (roi.width()/3);

    // Load wires per plane
    auto dead_wires = LoadWires(meta);

    std::cout<<"PLANE IS: "<<meta.plane()<<std::endl;
    std::vector<std::pair<float,float>> min_max_wire ;
    std::vector<std::pair<float,float>> min_max_time;

    for ( auto const & c : clusters ){

      float min_wire = 100000;
      float max_wire = -1;
      float min_time = 100000;
      float max_time = -1;
      int range = 0;

      for( auto & i : c._insideHits ){
        if( i.y > max_wire ){
          max_wire = i.y;
          max_time = i.x;
          }
        if( i.y < min_wire ){
          min_wire = i.y;
          min_time = i.x;
         }
        }
      
      float clus_range = max_wire - min_wire;
      
      min_wire -= (clus_range / 5) ; //make fcl param
      max_wire += (clus_range / 5) ;
      std::cout<<"Clus range : "<<min_wire<<", "<<max_wire<<std::endl ;
 
      min_max_wire.push_back(std::make_pair(min_wire,max_wire) ) ;
      min_max_time.push_back(std::make_pair(min_time,max_time) ) ;
      
      for( unsigned i=0; i < dead_wires.size() && dead_wires[i].second < max_wire ; ++i ) { 
    
        auto& dw = dead_wires [i];

         //std::cout<<"Dead wire range: " << dw.first<<", "<<dw.second<<std::endl ;
    
        if ( dw.first >= min_wire){
          //if(  dw.second < max_wire )
            range += (dw.second - dw.first);
          //else
           // range += (max_wire - dw.first); 
          }   
        else{
          if( dw.second > min_wire )
            range += (dw.second - dw.first);
            //range += (dw.second - min_wire );
           } 
        } 
    
      auto score = 1. - float(range) / (max_wire - min_wire) ;
      if ( score < 0. ) score = 0.; 
      //if ( meta.plane() == 2 ) score = 1. ;

      std::cout<<"These Scores though: "<<score<<std::endl ;
      if(score > 0.7 ) oclusters.emplace_back(c);


      }

      if ( meta.debug() ) {

      std::stringstream ss1, ss2, ss3, ss4;

      ::larlite::user_info uinfo{};
      ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
      uinfo.store("ID",ss1.str());

      ss1.str(std::string());
      ss1.clear();

      //uinfo.store("NClusters",(int)result_v.size());

      LAROCV_DEBUG() << "Writing debug information for " << clusters.size() << "\n";

      for(size_t i = 0; i < oclusters.size(); ++i){

        const auto& cluster = oclusters[i];

        ////////////////////////////////////////////
        /////Contour points

        ss1  <<  "MinWire_" << i ; 
        ss2  <<  "MaxWire_" << i ;
        ss3  <<  "MinTime_" << i ;
        ss4  <<  "MaxTime_" << i ;

        float min = min_max_wire[i].first + meta.origin().x;  
        float max = min_max_wire[i].second + meta.origin().x;

        uinfo.append(ss1.str(),min);
        uinfo.append(ss2.str(),max);
        uinfo.append(ss3.str(),(min_max_time[i].first + meta.origin().y + 2400));
        uinfo.append(ss4.str(),(min_max_time[i].second + meta.origin().y + 2400));

        ss1.str(std::string());
        ss1.clear();
        ss2.str(std::string());
        ss2.clear();
        ss3.str(std::string());
        ss3.clear();
        ss4.str(std::string());
        ss4.clear();
      }

      meta.ev_user()->emplace_back(uinfo);

      std::cout<<"Meta origin: "<<meta.origin().x <<", "<<meta.origin().y <<std::endl ;
    }
  
  return oclusters; 
  }

  std::vector<std::pair<int,int> > ClusDeadWireOverlap::LoadWires(ImageMeta& meta)
  {

    auto& dead_wires = _wires_v[ meta.plane() ];

    std::vector<std::pair<int,int> > dead_wires_px; dead_wires_px.reserve(dead_wires.size());

    for( unsigned j=0; j < dead_wires.size(); ++j ) {

      auto& dwr = dead_wires[j];

      auto df = float(dwr.first  - meta.origin().x)/meta.pixel_width();
      auto ds = float(dwr.second - meta.origin().x)/meta.pixel_width();

      //outside wire range, ignore
      if ( df < 0 || ds < 0  ) continue;

      dead_wires_px.emplace_back(df,ds);
    }

    return dead_wires_px;
  }



}
#endif
