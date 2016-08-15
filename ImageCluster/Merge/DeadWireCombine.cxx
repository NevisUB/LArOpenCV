#ifndef __DEADWIRECOMBINE_CXX__
#define __DEADWIRECOMBINE_CXX__

#include "DeadWireCombine.h"
#include "FhiclLite/ConfigManager.h"

namespace larocv{

  
  void DeadWireCombine::_Configure_(const ::fcllite::PSet &pset)
  {

    _min_width = pset.get<int>("MinWireWidth"); //should maybe correlated to blur size
    _closeness = pset.get<int>("CloseToDeath");
    _tolerance = pset.get<double>("CrossTolerance");

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
  
  Cluster2DArray_t DeadWireCombine::_Process_(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    auto dead_wires = LoadWires(meta);

    //minimum distance paired point
    std::vector<paired_point> min_d_pp;
      
    //get the shortest distance between each cluster
    //put into struct called paired_point
    //std::cout << "\t>> get the shortest distance... \n";

    for( unsigned i=0; i < clusters.size(); ++i ){
      auto& c1 = clusters[i];

      for( unsigned j=i; j < clusters.size()-1; ++j ){
	auto& c2 = clusters[j];

	auto pp = min_dist(c1,c2);

	pp.i1 = i; //index of cluster 1
	pp.i2 = j; //index of cluster 2
	
	min_d_pp.emplace_back( pp );	
      }
    }

    std::map<int, std::vector<paired_point> > dw_crossing;

    //loop over all sets of paired points
    for(auto& pp : min_d_pp ) {

      // loop over wire ranges and see if we cross a wire region
      for( unsigned i=0; i<dead_wires.size(); ++i ) {

	auto& dwp = dead_wires [i];

	// clusters are separated in two ways across dead wires
	// first point is to left of first wire, second point is right of right wire
	// or
	// second point is left of first wire, second point is right of right wire
	
	if (  pp.p1.y <= dwp.first && pp.p2.y >= dwp.second ) {

	  if ( std::abs( pp.p1.y - dwp.first  ) > _closeness ) continue; //left was too far away
	  if ( std::abs( pp.p2.y - dwp.second ) > _closeness ) continue; //right was too far away

	  dw_crossing[i].emplace_back( pp ); //there is a crossing
		  
	}

	else if ( pp.p2.y < dwp.first && pp.p1.y > dwp.second ) {

	  if ( std::abs( pp.p2.y - dwp.first  ) > _closeness ) continue; //left was too far away
	  if ( std::abs( pp.p1.y - dwp.second ) > _closeness ) continue; //right was too far away

	  dw_crossing[i].emplace_back( pp ); //there is a crossing
	 		  
	}

      } //end loop over dead wires

    } // end loop over paired clusters


    // dw_crossing is map, key is dead wire index, keyed value is a paired_point struct that contains
    // information about a pair of clusters that happen to cross that particular wire, with no restriction
    // on how temporally separated they may be, here is where we decide... lets just do ONE type of crossing
    // then i blow my brains out
    std::map<int,bool> merged; for(int i=0;i<clusters.size();++i) merged[i]=false;
    
    for( auto& dw : dw_crossing ) {
      
      auto& widx = dw.first;  // dead wire region index
      auto& pp_v = dw.second; // paired point vector

      if ( pp_v.size() == 1 ) {
	auto& pp = pp_v[0];
	
	auto dx = std::abs( pp.p1.y - pp.p2.y );	
	auto dy = std::abs( pp.p1.x - pp.p2.x );

	// sanity check if dx !~ dw.first - dw.second fuck
	if ( std::abs( dx - ( dead_wires[widx].second - dead_wires[widx].first ) ) > 2.0*_closeness )
	  { std::cout << "dx is much larger than expected: dx " << dx << " wire spacing: " <<
	      dead_wires[widx].second - dead_wires[widx].first << "\n";
	    throw std::exception(); }
	
	if ( dy > _tolerance*dx )
	  continue;
	
	auto i = pp.i1;
	auto j = pp.i2;
	
	merged[i] = true; merged[j] = true;

	oclusters.emplace_back( join_clusters( *pp.c1, *pp.c2) );
      }
      else {
	std::cout << "found pp_v.size() :" << pp_v.size()
		  << " more than 1 broken up cluster on this wire region\n";
      }
    }
    
    for( int i = 0; i < clusters.size(); ++i ) {
      if ( merged[i] ) continue;

      oclusters.emplace_back( clusters[ i ] );
      
    }
    
    return oclusters;
  }

  paired_point DeadWireCombine::min_dist( const Cluster2D& c1, const Cluster2D& c2 ) {
    
    float d = 99999;
    const ::cv::Point* p1;
    const ::cv::Point* p2;
    
    paired_point pp;
    
    for( unsigned i = 0; i < c1._numHits; ++i )  {

      auto& h1 = c1._insideHits[i];
      
      for( unsigned j = i; j < c2._numHits-1; ++j )  {
	auto& h2 = c2._insideHits[j];

	auto dd = dist(h1.x,h2.x,h1.y,h2.y);

	if ( dd < d ) { d = dd; p1 = &h1; p2 = &h2; }

      }
    }

    pp.d  =   d;
    pp.p1 = *p1;
    pp.p2 = *p2;
    pp.c1 = &c1;
    pp.c2 = &c2;
    
    return pp;
      
  }


  Cluster2D DeadWireCombine::join_clusters(const Cluster2D& c1, const Cluster2D& c2) {

    Cluster2D out = c1;

    //copy the hits over
    out._insideHits.insert(std::end(out._insideHits), std::begin(c2._insideHits), std::end(c2._insideHits));
	
    //add the contours (good idea?)
    out._contour.insert( std::end(out._contour),std::begin(c2._contour),std::end(c2._contour) );

    //set total number of hits
    out._numHits = out._insideHits.size();
    
    return out;
  }
    

  std::vector<std::pair<int,int> > DeadWireCombine::LoadWires(ImageMeta& meta)
  {

    // if (_loaded[ meta.plane() ]) return;
    
    auto& dead_wires = _wires_v[ meta.plane() ];

    //convert to pixel coordinates, ignore small wire gaps
    std::vector<std::pair<int,int> > dead_wires_px; dead_wires_px.reserve(dead_wires.size());
      
    // std::cout << "on plane : " << meta.plane() << " with dead_wires.size() " << dead_wires.size() << "\n";

    for( unsigned j=0; j < dead_wires.size(); ++j ) {
	
      auto& dwr = dead_wires[j];

      auto df = (dwr.first  - meta.origin().x)/meta.pixel_width();
      auto ds = (dwr.second - meta.origin().x)/meta.pixel_width();


      //outside wire range, ignore
      if ( df < 0 || df < 0  ) continue;

      //wires must have big gap
      if ( ds - df <= _min_width ) continue;

      // std::cout << "df... " << df << "  ds... " << ds << "\n";

      dead_wires_px.emplace_back(df,ds);
    }
      
    return dead_wires_px;
    
  }
    
  

}
#endif
