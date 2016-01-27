#ifndef __LARLITE_LARIMAGEWIRE_CXX__
#define __LARLITE_LARIMAGEWIRE_CXX__

#include "LArImageWire.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"
#include "DataFormat/wire.h"
namespace larlite {

  void LArImageWire::store_clusters(storage_manager* storage)
  {
    static bool warn_once = false;
    if(!warn_once) {
      print(msg::kWARNING,__FUNCTION__,"Cluster storage is not implemented...");
      warn_once = true;
    }
  }

  void LArImageWire::extract_image(storage_manager* storage)
  {
    auto const& geom = ::larutil::Geometry::GetME();
    auto const& detp = ::larutil::DetectorProperties::GetME();
    const size_t nplanes = geom->Nplanes();
    const size_t nadcs = detp->NumberTimeSamples();

    auto ev_wire = storage->get_data<event_wire>(producer());

    if(!ev_wire) throw DataFormatException("Could not locate Wire data product!");
    
    std::vector< std::pair<size_t,size_t> > wire_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    std::vector< std::pair<size_t,size_t> > tick_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    
    for(auto const& wire_data : *ev_wire) {
      
      auto const& wid = geom->ChannelToWireID(wire_data.Channel());
      
      auto& wire_range = wire_range_v[wid.Plane];
      if(wire_range.first  > wid.Wire) wire_range.first  = wid.Wire;
      if(wire_range.second < wid.Wire) wire_range.second = wid.Wire;
      
      auto& tick_range = tick_range_v[wid.Plane];
      
      auto const& roi_v = wire_data.SignalROI();
      
      for(auto const& roi : roi_v.get_ranges()) {	
	size_t start_tick = roi.begin_index();
	size_t last_tick = start_tick + roi.size() - 1;
	if(tick_range.first  > start_tick) tick_range.first  = start_tick;
	if(tick_range.second < last_tick)  tick_range.second = last_tick;
      }
    }

    for(size_t plane=0; plane<nplanes; ++plane) {
      auto const& wire_range = wire_range_v[plane];
      auto const& tick_range = tick_range_v[plane];
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      ::larcv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first);
      
      if(!nticks || !nwires)
	_img_mgr.push_back(cv::Mat(),meta);
      else
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_32FC1, cvScalar(0.)),meta);
    }
    
    for(auto const& wire_data : *ev_wire) {
      
      auto const& wid = geom->ChannelToWireID(wire_data.Channel());

      auto& mat = _img_mgr.img_at(wid.Plane);
      
      auto const& roi_v = wire_data.SignalROI();

      auto const& wire_range = wire_range_v[wid.Plane];
      auto const& tick_range = tick_range_v[wid.Plane];
      
      for(auto const& roi : roi_v.get_ranges()) {
	size_t start_tick = roi.begin_index();
	for(size_t adc_index=0; adc_index < roi.size(); ++adc_index) {
	  size_t x = wid.Wire - wire_range.first;
	  size_t y = start_tick + adc_index - tick_range.first;
	  mat.at<float>(x,y) = mat.at<float>(x,y) + roi[adc_index];
	}
      }
    }

  }

}
#endif
