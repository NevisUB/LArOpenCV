#ifndef __CLUSDEADWIREOVERLAP_CXX__
#define __CLUSDEADWIREOVERLAP_CXX__

#include "ClusDeadWireOverlap.h"
#include "FhiclLite/ConfigManager.h"

namespace larocv{

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

    for ( auto const & c : clusters ){

      int min_wire = 100000;
      int max_wire = -1;
      int range = 0;

      for( auto & i : c._insideHits ){
        if( i.y > max_wire )
          max_wire = i.y;
        if( i.y < min_wire )
          min_wire = i.y;
        }
      
      float clus_range = max_wire - min_wire;
      
      min_wire -= (clus_range / 3) ; //make fcl param
      max_wire += (clus_range / 3) ;
      std::cout<<"Clus range : "<<min_wire<<", "<<max_wire<<std::endl ;
      
      for( unsigned i=0; i < dead_wires.size() && dead_wires[i].second < max_wire ; ++i ) { 
    
        auto& dw = dead_wires [i];

         //std::cout<<"Dead wire range: " << dw.first<<", "<<dw.second<<std::endl ;
    
        if ( dw.first >= min_wire){
          if(  dw.second < max_wire )
            range += (dw.second - dw.first);
          else
            range += (max_wire - dw.first); 
          }   
        else{
          if( dw.second > min_wire )
            range += (dw.second - min_wire );
           } 
        } 
    
      auto score = 1. - float(range) / (max_wire - min_wire) ;
      if ( score < 0. ) score = 0.; 
      if ( meta.plane() == 2 ) score = 1. ;

      std::cout<<"These Scores though: "<<score<<std::endl ;
      if(score > 0.7 ) oclusters.emplace_back(c);

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
