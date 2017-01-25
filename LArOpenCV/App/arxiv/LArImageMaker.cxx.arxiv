#ifndef __LARLITE_LARIMAGEMAKER_CXX__
#define __LARLITE_LARIMAGEMAKER_CXX__

#include "LArImageMaker.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/DetectorProperties.h"
#include "DataFormat/rawdigit.h"
//#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
#include "DataFormat/wire.h"
namespace larlite {

  LArImageMaker::LArImageMaker()
    : _rawdigit_img_mgr(nullptr)
    , _wire_img_mgr(nullptr)
    , _hit_img_mgr(nullptr)
  { _name="LArImageMaker"; _fout=0;  init(); }

  void LArImageMaker::init()
  {}    
  //{ import_array(); }

  bool LArImageMaker::initialize() {

    if(!_rawdigit_img_mgr && !_rawdigit_producer.empty())
      _rawdigit_img_mgr = new ::larcv::ImageManager;

    if(!_hit_img_mgr && !_hit_producer.empty())
      _hit_img_mgr = new ::larcv::ImageManager;
    
    if(!_wire_img_mgr && !_wire_producer.empty())
      _wire_img_mgr = new ::larcv::ImageManager;

    return true;
  }
  
  bool LArImageMaker::analyze(storage_manager* storage) {
    
    auto const& geom = ::larutil::Geometry::GetME();
    auto const& detp = ::larutil::DetectorProperties::GetME();
    const size_t nplanes = geom->Nplanes();
    const size_t nadcs = detp->NumberTimeSamples();

    if(!_rawdigit_producer.empty()) {

      auto ev_digit = storage->get_data<event_rawdigit>(_rawdigit_producer);

      if(!ev_digit) throw DataFormatException("Could not locate RawDigit data product!");

      for(size_t plane=0; plane<nplanes; ++plane) {
	::cv::Mat mat(geom->Nwires(plane), nadcs, CV_32FC1, cvScalar(0.));
	_rawdigit_img_mgr->emplace_back(std::move(mat));
      }

      for(auto const& digit : *ev_digit) {

	auto const& wid = geom->ChannelToWireID(digit.Channel());

	auto& mat = _rawdigit_img_mgr->at(wid.Plane);

	auto const& adcs = digit.ADCs();

	for(size_t tick=0; tick<adcs.size(); ++tick)
	  mat.at<float>(wid.Wire,tick) = mat.at<float>(wid.Wire,tick) + adcs[tick];
      }
    }

    if(!_wire_producer.empty()) {

      auto ev_wire = storage->get_data<event_wire>(_wire_producer);

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
	::cv::Mat mat(wire_range.second - wire_range.first + 1,
		      tick_range.second - tick_range.second + 1,
		      CV_32FC1, cvScalar(0.));
	_wire_img_mgr->emplace_back(std::move(mat));
      }

      for(auto const& wire_data : *ev_wire) {

	auto const& wid = geom->ChannelToWireID(wire_data.Channel());

	auto& mat = _wire_img_mgr->at(wid.Plane);

	auto const& roi_v = wire_data.SignalROI();

	for(auto const& roi : roi_v.get_ranges()) {
	  size_t start_tick = roi.begin_index();
	  for(size_t adc_index=0; adc_index < roi.size(); ++adc_index)
	    mat.at<float>(wid.Wire,start_tick+adc_index) = mat.at<float>(wid.Wire,start_tick+adc_index) + roi[adc_index];
	}
      }
    }

    if(!_hit_producer.empty()) {
      std::cout<<std::endl;
      _mat_v.clear();

      auto ev_hit = storage->get_data<event_hit>(_hit_producer);

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
	//_hit_img_mgr->emplace_back(std::move(mat));
	std::cout << "Creating ... " << wire_range.first << " => " << wire_range.second << " ... " << nwires
		  << " : " << tick_range.first << " => " << tick_range.second << " ... " << nticks
		  << " @ " << plane << std::endl;
	for(size_t x=0; x<nwires; ++x)
	  for(size_t y=0; y<nticks; ++y) mat.at<unsigned char>(x,y) = (unsigned char)5;
	//mat.at<unsigned char>(0,0) = (unsigned char)5;
	_mat_v.emplace_back(mat);
      }

      for(auto const& h : *ev_hit) {

	auto const& wid = h.WireID();

	if(wid.Plane >= wire_range_v.size()) continue;

	//auto& mat = _hit_img_mgr->at(wid.Plane);
	auto& mat = _mat_v.at(wid.Plane);

	//continue;
	auto const& wire_range = wire_range_v[wid.Plane];
	auto const& tick_range = tick_range_v[wid.Plane];

	size_t nticks = tick_range.second - tick_range.first + 2;
	size_t nwires = wire_range.second - wire_range.first + 2;
	
	size_t y = (size_t)(h.PeakTime()+0.5) - tick_range.first;
	size_t x = wid.Wire - wire_range.first;

	if(y>=nticks || x>=nwires) throw std::exception();

	//std::cout<<"Inserting " << x << " " << y << " @ " << wid.Plane << std::endl;

	int charge = (int)(h.Integral());

	charge += mat.at<unsigned char>(x,y);

	if(charge>=256) charge=255;

	mat.at<unsigned char>(x,y) = (unsigned char)charge;
	//mat.at<unsigned char>(x,y) = (unsigned char)0;
	
	//mat.at<float>(wid.Wire,tick) = mat.at<float>(wid.Wire,tick) + h.Integral();
      }
      //std::cout<<_mat_v[0]<<std::endl;
      //std::cout<<_mat_v[1]<<std::endl;
      //std::cout<<_mat_v[2]<<std::endl;
      ::cv::imshow("aho",_mat_v[2]);
      usleep(1e6);
      //throw std::exception();
    }

    if(_rawdigit_img_mgr) _rawdigit_img_mgr->clear();
    if(_wire_img_mgr)     _wire_img_mgr->clear();
    if(_hit_img_mgr)      _hit_img_mgr->clear();
    
    return true;
  }

  bool LArImageMaker::finalize() {

    return true;
  }

  /*
  PyObject* LArImageMaker::GetImage(const size_t plane)
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }

    ::larcv::convert::NDArrayConverter converter;
    return converter.toNDArray(_mat_v[plane]);
      
  }

  PyObject* LArImageMaker::GetCanny(const size_t plane)
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }

    ::larcv::convert::NDArrayConverter converter;
    return converter.toNDArray(_canny_v[plane]);
      
  }

  size_t LArImageMaker::NumContours(const size_t plane) const
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }
    return _contour_v[plane].size();
  }

  PyObject* LArImageMaker::GetContour(const size_t plane, const size_t contour_index)
  {
    if(plane >= _mat_v.size()) {
      print(msg::kERROR,__FUNCTION__,"Invalid plane ID requested...");
      throw std::exception();
    }

    if(contour_index > _contour_v[plane].size()){
      print(msg::kERROR,__FUNCTION__,"Invalid contour ID requested...");
      throw std::exception();
    }
    
    int* dim_data = new int[2];
    dim_data[0] = 2;
    dim_data[1] = (int)(_contour_v[plane][contour_index].size());

    return PyArray_FromDimsAndData( 2, dim_data, NPY_DOUBLE, (char*) &(_contour_v[plane][contour_index].raw_data()[0]) );
				    
  }
  */
}
#endif
