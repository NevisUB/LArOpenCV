#ifndef __LARLITE_LARIMAGEHIT_CXX__
#define __LARLITE_LARIMAGEHIT_CXX__

#include "LArImageHit.h"
#include "Core/larbys.h"

#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"

#include "DataFormat/cluster.h"
#include "DataFormat/event_ass.h"
#include "DataFormat/hit.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/rawdigit.h"
#include "DataFormat/vertex.h"

namespace larlite {

  void LArImageHit::_Configure_(const ::fcllite::PSet& pset) {

    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    _charge_threshold = pset.get<double>("QMin");
    _pool_time_tick = pset.get<int>("PoolTimeTick");

    _vtx_producer  = pset.get<std::string>("VertexProducer");

    _roi_buffer_w = pset.get<float>("ROIBufferW");
    _roi_buffer_t = pset.get<float>("ROIBufferT");

    _make_roi = pset.get<bool>("MakeROI");
    _crop_w_roi = pset.get<bool>("CropROI");
    
    _time_offset = pset.get<float>("TimeOffset");
    _t_window_max = pset.get<float>("WindowTMax");
    _t_window_min = pset.get<float>("WindowTMin");

    _debug = pset.get<bool>("Debug");

    _plane_weights = pset.get<std::vector<float>>("MatchPlaneWeights");
  }
  
  void LArImageHit::_Report_() const {
    std::cout << "  # of clusters  ......... ";
    std::cout << Form("%-6zu ", _num_clusters);

    std::cout << " ... Average (per-event) ";
    double nclusters = _num_clusters;
    std::cout << Form("%g ", nclusters / ((double)(_num_stored)));
    std::cout << std::endl;

    std::cout << "  # of clustered hits ... ";
    // for(auto const& num : _num_clustered_hits_v) std::cout << Form("%-6zu
    // ",num);
    std::cout << Form("%-6zu ", _num_clustered_hits);

    std::cout << " ... Fractions: ";
    double clustered = _num_clustered_hits;
    double unclustered = _num_unclustered_hits;
    std::cout << Form("%g%% ", clustered / (clustered + unclustered));
    std::cout << std::endl;
  }

  void LArImageHit::extract_image(storage_manager* storage) {

    LAROCV_DEBUG((*this)) << "Extracting Image\n";
    
    auto const& geom = ::larutil::Geometry::GetME();
    auto const& geomH = ::larutil::GeometryHelper::GetME();
    const size_t nplanes = geom->Nplanes();

    LAROCV_DEBUG((*this)) << "Getting hit producer " << producer() << "\n";

    auto ev_hit = storage->get_data<event_hit>(producer());

    if (ev_hit == nullptr)
      throw DataFormatException("Could not locate hit data product!");

    std::vector<std::pair<size_t, size_t>> wire_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));
    std::vector<std::pair<size_t, size_t>> tick_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));

    for (auto const& h : *ev_hit) {
      if (h.Integral() < _charge_threshold) continue;

      auto const& wid = h.WireID();

      auto& wire_range = wire_range_v[wid.Plane];

      if (wire_range.first > wid.Wire) wire_range.first = wid.Wire;
      if (wire_range.second < wid.Wire) wire_range.second = wid.Wire;

      auto& tick_range = tick_range_v[wid.Plane];
      size_t peak_time = (size_t)(h.PeakTime());
      if (tick_range.first > peak_time)
	tick_range.first = (bool(peak_time) ? peak_time - 1 : 0);
      if (tick_range.second < (peak_time + 1))
	tick_range.second = peak_time + 1;
    }

    for (size_t plane = 0; plane < nplanes; ++plane) {

      auto & wire_range = wire_range_v[plane];
      auto & tick_range = tick_range_v[plane];

      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      
      ::larocv::ROI roi;

      if ( _make_roi ){

	auto ev_vertex = storage->get_data<event_vertex>(_vtx_producer);
        if (ev_vertex == nullptr || !ev_vertex->size() )
          throw DataFormatException("Could not locate vertex data product!");

	auto vtx = ev_vertex->at(0) ;
        std::vector<larocv::Point2D> roi_bounds ;
        auto buffer_w =  _roi_buffer_w / geomH->WireToCm(); // 70cm
        auto buffer_t =  _roi_buffer_t / geomH->TimeToCm(); // 70cm

        //TVector3 vtxXYZ( vtx.X(), vtx.Y(), vtx.Z() );
        std::vector<double> vtxXYZ = { vtx.X(), vtx.Y(), vtx.Z() };

	roi.set3Dvtx(vtxXYZ);

	auto vtxWT  = geomH->Point_3Dto2D(vtxXYZ,plane);
	
        auto vtx_w = vtxWT.w / geomH->WireToCm();
        auto vtx_t = vtxWT.t / geomH->TimeToCm() + _time_offset ;

        float upper_w(3456);
        float lower_w = ( vtx_w - buffer_w < 0 ? 0 : vtx_w - buffer_w );
        float upper_t = ( vtx_t + buffer_t > _t_window_max ? _t_window_max : vtx_t + buffer_t );
        float lower_t = ( vtx_t - buffer_t < _t_window_min ? _t_window_min : vtx_t - buffer_t );

        if( plane == 0 || plane == 1)
          upper_w = ( vtx_w + buffer_w > 2400 ? 2400 : vtx_w + buffer_w );
        else 
          upper_w = ( vtx_w + buffer_w > 3456 ? 3456 : vtx_w + buffer_w );

	LAROCV_DEBUG((*this)) << "Got vertex point (w,t): (" << vtx_w << "," << vtx_t << ")\n";
		
        roi_bounds.emplace_back(lower_w, upper_t); ///< origin
        roi_bounds.emplace_back(lower_w, lower_t);
        roi_bounds.emplace_back(upper_w, lower_t);
        roi_bounds.emplace_back(upper_w, upper_t);

	roi.setorigin(vtx_w - buffer_w,vtx_t - buffer_t);
	roi.setvtx(vtx_w,vtx_t);
	roi.setbounds(roi_bounds);

	if (_crop_w_roi) {
	  wire_range = {lower_w,upper_w};
	  tick_range = {lower_t,upper_t};
	  nticks = tick_range.second - tick_range.first + 2;
	  nwires = wire_range.second - wire_range.first + 2;
	}
      }
      
      ::larocv::ImageMeta meta((double)nwires, (double)nticks, nwires, nticks,
			       wire_range.first, tick_range.first, plane);
      
      if (_debug) meta.set_debug(true);
      if (_crop_w_roi) meta.set_roi_cropped(true);
      
      // no matter what we have to send ROI... it can go to the algorithm blank, that's fine
      // If we don't want to use ROI, need to pass blanks
      if (nwires >= 1e10 || nticks >= 1e10)
	_img_mgr.push_back(::cv::Mat(), ::larocv::ImageMeta(),::larocv::ROI());
      else 
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta,roi);
	
    }   

    for (auto const& h : *ev_hit) {

      auto const& wid = h.WireID();

      if (wid.Plane >= wire_range_v.size()) continue;

      auto& mat = _img_mgr.img_at(wid.Plane);

      // continue;
      auto const& wire_range = wire_range_v[wid.Plane];
      auto const& tick_range = tick_range_v[wid.Plane];

      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;

      size_t y = (size_t)(h.PeakTime() + 0.5) - tick_range.first;
      size_t x = wid.Wire - wire_range.first;

      // skip the hit if it's out of range
      if (y >= nticks || x >= nwires) 
	continue;

      double charge = h.Integral() / _charge_to_gray_scale;
      charge += (double)(mat.at<unsigned char>(x, y));

      if (charge >= 255.) charge = 255.;
      if (charge < 0.) charge = 0.;

      mat.at<unsigned char>(x, y) = (unsigned char)((int)charge);
    }

    // normalize the tick range
    if (_pool_time_tick > 1) {

      for (size_t plane = 0; plane < nplanes; ++plane) {

	auto& img  = _img_mgr.img_at(plane);
	auto& meta = _img_mgr.meta_at(plane);
	//auto& roi  = _img_mgr.roi_at(plane);

	::cv::Mat pooled(img.rows, img.cols / _pool_time_tick + 1, CV_8UC1,
			 cvScalar(0.));
	
	for (int row = 0; row < img.rows; ++row) {
	  uchar* p = img.ptr(row);

	  for (int col = 0; col < img.cols; ++col) {
	    int pp = *p++;

	    auto& ch = pooled.at<uchar>(row, col / _pool_time_tick);

	    int res = pp + (int)ch;
	    if (res > 255) res = 255;
	    ch = (uchar)res;
	  }
	}

	// old parameters
	auto const& wire_range = wire_range_v[plane];
	auto const& tick_range = tick_range_v[plane];


	//copy pooled image into image
	img = pooled;
	meta = ::larocv::ImageMeta ((double)pooled.rows, (double)pooled.cols * _pool_time_tick,
				    pooled.rows, pooled.cols, wire_range.first, tick_range.first, plane);
	

	if (_debug) meta.set_debug(true);
	if (_crop_w_roi) meta.set_roi_cropped(true);
	
	//	if (_make_roi){
	//
	//	  auto old_vtx = roi.roivtx();
	//          auto old_bb  = roi.roibounds();
	//
	//	  auto delta_origin_t = (roi.origin().y - tick_range.first) / _pool_time_tick ;
	//	  auto new_origin_t   = tick_range.first + delta_origin_t ; 
	//
	//	  roi = ::larocv::ROI( roi.width(), roi.height() / _pool_time_tick, roi.origin().x, new_origin_t, plane);
	//
	//	  auto delta_vtx_t = (old_vtx.y - tick_range.first) / _pool_time_tick ;
	//	  auto new_vtx_t   = tick_range.first + delta_vtx_t ; 
	//	
	//	  roi.setvtx(old_vtx.x, new_vtx_t);
	//
	//          std::vector<larocv::Point2D> pool_bounds ;
	//
	//          // First entry is always the lower left corner (origin)
	//          for(size_t b = 0; b < old_bb.size(); b++ ){
	//
	//            auto delta_bb_t = (old_bb[b].y - tick_range.first) / _pool_time_tick ;
	//            auto new_bb_t   = tick_range.first + delta_bb_t ; 
	//            pool_bounds.emplace_back(old_bb[b].x,new_bb_t);
	//            std::cout<<pool_bounds[b].x<<", "<<pool_bounds[b].y<<std::endl ;
	//
	//            }   
	//
	//           roi.setbounds(pool_bounds);
	//
	//	}
      }
    }
  }

}
#endif
