#ifndef __LARLITE_LARIMAGEHIT_CXX__
#define __LARLITE_LARIMAGEHIT_CXX__

#include "LArImageHit.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"
#include "DataFormat/rawdigit.h"
//#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
#include "DataFormat/wire.h"
namespace larlite {

  void LArImageHit::store_clusters(storage_manager* storage) const
  {

    auto ev_hit = storage->get_data<event_hit>("HitImageCluster");
    
    if(!ev_hit) throw DataFormatException("Could not locate hit data product!");

    auto const& alg_mgr_v = algo_manager_set();

    for(auto const& h : *ev_hit) {

      auto const& wid = h.WireID();

      auto const& alg_mgr = alg_mgr_v[wid.Plane];
      
    }
  }

  void LArImageHit::extract_image(storage_manager* storage)
  {
    
    auto const& geom = ::larutil::Geometry::GetME();
    auto const& detp = ::larutil::DetectorProperties::GetME();
    const size_t nplanes = geom->Nplanes();
    const size_t nadcs = detp->NumberTimeSamples();

    auto ev_hit = storage->get_data<event_hit>(_producer);
    
    if(!ev_hit) throw DataFormatException("Could not locate hit data product!");
    
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
      ::cv::Mat mat(nwires, nticks, CV_8UC1, cvScalar(0.));
      //_img_mgr->emplace_back(std::move(mat));
      /*
      std::cout << "Creating ... " << wire_range.first << " => " << wire_range.second << " ... " << nwires
		<< " : " << tick_range.first << " => " << tick_range.second << " ... " << nticks
		<< " @ " << plane << std::endl;
      */
      for(size_t x=0; x<nwires; ++x)
	for(size_t y=0; y<nticks; ++y) mat.at<unsigned char>(x,y) = (unsigned char)5;
      //mat.at<unsigned char>(0,0) = (unsigned char)5;
      ::larcv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first);
      _img_mgr.push_back(mat,meta);
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
      
      //std::cout<<"Inserting " << x << " " << y << " @ " << wid.Plane << std::endl;
      
      mat.at<float>(x,y) = mat.at<float>(x,y) + (float)(h.Integral());
    }
    
  }

}
#endif
