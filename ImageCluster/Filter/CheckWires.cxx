#ifndef __CHECKWIRES_CXX__
#define __CHECKWIRES_CXX__

#include "CheckWires.h"
#include "FhiclLite/ConfigManager.h"

namespace larocv{

  void CheckWires::_Configure_(const ::fcllite::PSet &pset)
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

  Cluster2DArray_t CheckWires::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta,
					larocv::ROI& roi)
  { 
  
    //Wire loading code from Vic's DeadWireCombine Merge algorithm
    auto const & l_w = roi.roibounds_in_image(meta,0).y;
    auto const & r_w = roi.roibounds_in_image(meta,2).y;

    auto const & vtx_l_w = roi.roivtx_in_image(meta).y - (roi.width()/3); 
    auto const & vtx_r_w = roi.roivtx_in_image(meta).y + (roi.width()/3);

    // Load wires per plane
    auto dead_wires = LoadWires(meta);

    int range = 0;

    //std::cout<<"\n\nPLANE IS: "<<meta.plane()<<std::endl;
    //std::cout<<"Left and right bounds :" << l_w<<", "<<r_w<<", "<<std::endl ;

    for( unsigned i=0; i < dead_wires.size() && dead_wires[i].second < r_w ; ++i ) {
        
        auto& dw = dead_wires [i];

         //std::cout<<"Dead wire range: " << dw.first<<", "<<dw.second<<std::endl ;
        
        if ( dw.first >= l_w ){
          if ( dw.second < r_w ){
            if( dw.first >= vtx_l_w && dw.first < vtx_r_w)
              range += 2*(dw.second - dw.first);
            else
               range += (dw.second - dw.first);
             }
          else
            range += (r_w - dw.first); 
             }
        }
    
    auto score = 1. - float(range) / roi.width () ;
    // This can happen if the weighting of dead wires makes range > width
    if ( score < 0. ) score = 0.;

    if ( meta.plane() == 2 ) score = 1. ;

    //std::cout<<"Range is :" <<range <<", "<<roi.width()<<", score: "<<score<<std::endl ;

  meta.set_score(score) ;
  std::cout<<"Meta score :" <<meta.score() <<std::endl ;
  
  return clusters; 
}

  std::vector<std::pair<int,int> > CheckWires::LoadWires(ImageMeta& meta)
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
