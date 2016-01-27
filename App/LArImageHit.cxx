#ifndef __LARLITE_LARIMAGEHIT_CXX__
#define __LARLITE_LARIMAGEHIT_CXX__

#include "LArImageHit.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"
#include "DataFormat/rawdigit.h"
//#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/event_ass.h"

namespace larlite {

  void LArImageHit::_Report_() const
  {
    std::cout << "  # of clustered hits per plane : ";
    for(auto const& num : _num_clustered_hits_v) std::cout << Form("%-6zu ",num);
    std::cout << " ... Fractions: ";
    for(size_t plane=0; plane<_num_clustered_hits_v.size(); ++plane) {
      double clustered = _num_clustered_hits_v[plane];
      double unclustered = _num_unclustered_hits_v[plane];
      std::cout << Form("%g%% ",clustered/(clustered+unclustered));
    }
    std::cout << std::endl;
  }
  
  void LArImageHit::store_clusters(storage_manager* storage)
  {
    if(_num_clustered_hits_v.empty()) {
      auto geom = ::larutil::Geometry::GetME();
      _num_clustered_hits_v.resize(geom->Nplanes(),0);
      _num_unclustered_hits_v.resize(geom->Nplanes(),0);
    }
    
    auto ev_hit = storage->get_data<event_hit>(producer());
    
    if(!ev_hit) throw DataFormatException("Could not locate hit data product!");

    auto ev_cluster = storage->get_data<event_cluster>("ImageClusterHit");
    auto ev_ass     = storage->get_data<event_ass>("ImageClusterHit");

    auto const& alg_mgr_v = algo_manager_set();

    size_t nclusters=0;
    for(auto const& alg_mgr : alg_mgr_v)
      nclusters += alg_mgr.Clusters().size();

    if(ev_cluster) ev_cluster->reserve(nclusters);

    AssSet_t ass_set;
    ass_set.reserve(nclusters);
    
    for(auto const& h : *ev_hit) {

      auto const& wid = h.WireID();

      auto const& alg_mgr = alg_mgr_v[wid.Plane];

      auto const& cid = alg_mgr.ClusterID(wid.Wire,h.PeakTime());

      if(cid == ::larcv::kINVALID_CLUSTER_ID) _num_unclustered_hits_v[wid.Plane] += 1;
      else _num_clustered_hits_v[wid.Plane] += 1;

    }

  }

  void LArImageHit::extract_image(storage_manager* storage)
  {
    
    auto const& geom = ::larutil::Geometry::GetME();
    auto const& detp = ::larutil::DetectorProperties::GetME();
    const size_t nplanes = geom->Nplanes();
    const size_t nadcs = detp->NumberTimeSamples();

    auto ev_hit = storage->get_data<event_hit>(producer());

    if(ev_hit==nullptr) throw DataFormatException("Could not locate hit data product!");
    
    std::vector< std::pair<size_t,size_t> > wire_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    std::vector< std::pair<size_t,size_t> > tick_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    
    for(auto const& h : *ev_hit) {
      
      auto const& wid = h.WireID();
      
      auto& wire_range = wire_range_v[wid.Plane];
      if(wire_range.first  > wid.Wire) wire_range.first  = wid.Wire;
      if(wire_range.second < wid.Wire) wire_range.second = wid.Wire;
      
      auto& tick_range = tick_range_v[wid.Plane];
      size_t peak_time = (size_t)(h.PeakTime());
      if(tick_range.first  > peak_time    ) tick_range.first  = ( bool(peak_time) ? peak_time - 1 : 0 );
      if(tick_range.second < (peak_time+1)) tick_range.second = peak_time+1;
    }
    
    for(size_t plane=0; plane<nplanes; ++plane) {
      auto const& wire_range = wire_range_v[plane];
      auto const& tick_range = tick_range_v[plane];
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      ::larcv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first);
      
      //if(!nticks || !nwires)
      //_img_mgr.push_back(cv::Mat(),meta);
      //else
      _img_mgr.push_back(::cv::Mat(nwires, nticks, CV_32FC1, cvScalar(0.)),meta);
      //_img_mgr->emplace_back(std::move(mat));
      /*
      std::cout << "Creating ... " << wire_range.first << " => " << wire_range.second << " ... " << nwires
		<< " : " << tick_range.first << " => " << tick_range.second << " ... " << nticks
		<< " @ " << plane << std::endl;
      for(size_t x=0; x<nwires; ++x)
	for(size_t y=0; y<nticks; ++y) mat.at<unsigned char>(x,y) = (unsigned char)5;
      */
      //mat.at<unsigned char>(0,0) = (unsigned char)5;
    }
    
    for(auto const& h : *ev_hit) {
      
      auto const& wid = h.WireID();
      
      if(wid.Plane >= wire_range_v.size()) continue;
      
      auto& mat = _img_mgr.img_at(wid.Plane);
      
      //continue;
      auto const& wire_range = wire_range_v[wid.Plane];
      auto const& tick_range = tick_range_v[wid.Plane];
      
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      
      size_t y = (size_t)(h.PeakTime()+0.5) - tick_range.first;
      size_t x = wid.Wire - wire_range.first;
      
      if(y>=nticks || x>=nwires) throw std::exception();
      
      std::cout<<"Inserting " << x << " " << y << " @ " << wid.Plane << std::endl;

      
      mat.at<float>(x,y) = mat.at<float>(x,y) + (float)(h.Integral());
    }
    
  }

}
#endif
