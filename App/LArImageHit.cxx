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

    _hit_removal = pset.get<bool>("UseHitRemoval");

    _use_data = pset.get<bool>("UseData");
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

    if (ev_hit == nullptr )
      throw DataFormatException("Could not locate hit data product!");

    std::vector<std::pair<size_t, size_t>> wire_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));
    std::vector<std::pair<size_t, size_t>> tick_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));



    for (auto const& h : *ev_hit) {
      if (h.Integral() < _charge_threshold) continue;

      if ( _hit_removal && h.GoodnessOfFit() < 0 ) continue;

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

    // We will store the number of hits per pixel (per plane) here; this affects hit comparison 
    // between mat object and ev_hit data product at later stage. It is preferable
    // that these 2 numbers match
    std::vector<::cv::Mat> orig_img_v ; 

    for (size_t plane = 0; plane < nplanes; ++plane) {

      auto & wire_range = wire_range_v[plane];
      auto & tick_range = tick_range_v[plane];

      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      //std::cout<<"nticks: "<<nticks<<", "<<nwires<<std::endl ;
      
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
	roi.setplane(plane);

	if (_crop_w_roi) {
	  wire_range = {lower_w,upper_w};
	  tick_range = {lower_t,upper_t};
	  nticks = tick_range.second - tick_range.first + 2;
	  nwires = wire_range.second - wire_range.first + 2;
	}
      }

     // no matter what we have to send ROI... it can go to the algorithm blank, that's fine
      // If we don't want to use ROI, need to pass blanks
      if (nwires >= 1e10 || nticks >= 1e10){
        _img_mgr.push_back(::cv::Mat(), ::larocv::ImageMeta(),::larocv::ROI());
        orig_img_v.emplace_back(::cv::Mat() );
      }   
      else{ 
        ::larocv::ImageMeta meta((double)nwires, (double)nticks, nwires, nticks, wire_range.first, tick_range.first, plane);
        if (_debug) meta.set_debug(true);
        if (_crop_w_roi) meta.set_roi_cropped(true);
        _img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta,roi);
        orig_img_v.emplace_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)) );
      }   
    }   


    for (auto const& h : *ev_hit) {

      auto const& wid = h.WireID();

      if (wid.Plane >= wire_range_v.size()) continue;

      if ( _hit_removal && h.GoodnessOfFit() < 0 ) continue;

      auto& mat = _img_mgr.img_at(wid.Plane);
      auto& orig_mat = orig_img_v.at(wid.Plane);

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

      // Fill image with charge information
      double charge = h.Integral() / _charge_to_gray_scale;
      charge += (double)(mat.at<unsigned char>(x, y));

      if (charge >= 255.) charge = 255.;
      if (charge < 0.) charge = 0.; 
      if (charge < 1) charge = 1; 

      mat.at<unsigned char>(x, y) = (unsigned char)((int)charge);

      // Also keep track of the number of hits per pixel 
      int n = (int)(orig_mat.at<unsigned char>(x, y)) + 1;
      orig_mat.at<unsigned char>(x, y) = (unsigned char)(n);

    }

    auto ev_chstatus = storage->get_data<event_chstatus>("chstatus");
    if ( !ev_chstatus || ev_chstatus->size() == 0 ) { 
      std::cout<<" Not finding channel statuses...problem for data only ... "<<std::endl ;
    }

    // normalize the tick range
    if (_pool_time_tick > 1) {

      // Status pair vector
      std::vector<std::pair<int,int>> wires_v ;

      for (size_t plane = 0; plane < nplanes; ++plane) {

	auto& img  = _img_mgr.img_at(plane);
	auto& meta = _img_mgr.meta_at(plane);
	//auto& roi  = _img_mgr.roi_at(plane);
        auto orig_img = orig_img_v.at(plane) ;

	::cv::Mat pooled(img.rows, img.cols / _pool_time_tick + 1, CV_8UC1,
			 cvScalar(0.));

	::cv::Mat pooled_meta(img.rows, img.cols / _pool_time_tick + 1, CV_8UC1,
			 cvScalar(0.));
	
	for (int row = 0; row < img.rows; ++row) {
	  uchar* p = img.ptr(row);
	  uchar* p_pool = orig_img.ptr(row);

	  for (int col = 0; col < img.cols; ++col) {
	    int pp = *p++;
	    int pp_pool = *p_pool++;

	    auto& ch = pooled.at<uchar>(row, col / _pool_time_tick);

	    int res = pp + (int)ch;

	    if (res > 255) res = 255;
              ch = (uchar)res;

	    auto& ch_meta = pooled_meta.at<uchar>(row, col / _pool_time_tick);

            if ( pp_pool > 0 ) 
              ch_meta = uchar(int(ch_meta) + pp_pool); //1) ; 
            
       	  }
	}

	meta.set_is_data(_use_data);

	if ( _use_data ){ 
	
	  if ( ev_chstatus->size() == 3 ){
	    //std::cout<<"Channel stat has 3 planes! "<<std::endl; 
	    auto ch_v = ev_chstatus->at(plane);
	    std::vector<int> status_v ;
	    for ( int j = 0; j < ch_v.status().size(); j++){
	      auto ch = ch_v.status().at(j);
	      if ( ch !=4 ){
	        status_v.emplace_back(j) ;
	      }
	    }

            int lower_bound = status_v.at(0);
	    int upper_bound = status_v.at(0);
	    for ( int k = 1; k < status_v.size(); k++){

              int ch_k = status_v.at(k) ;

	      if ( ch_k != upper_bound + 1 ){
	        //std::cout<<"Making pair: "<<lower_bound<<", "<<upper_bound<<std::endl;

	        auto p = std::make_pair(lower_bound,upper_bound);
	        wires_v.emplace_back(p);
	        lower_bound = ch_k ;
	      }

	      upper_bound = ch_k ;

	    }
	  }
	  meta.set_wires(wires_v) ;
	}

	// old parameters
	auto const& wire_range = wire_range_v[plane];
	auto const& tick_range = tick_range_v[plane];

	//copy pooled image into image
	img = pooled;
	meta = ::larocv::ImageMeta ((double)pooled.rows, (double)pooled.cols * _pool_time_tick,
				    pooled.rows, pooled.cols, wire_range.first, tick_range.first, plane);

        meta.add_pool_meta(pooled_meta);

	if (_debug) meta.set_debug(true);
	if (_crop_w_roi) meta.set_roi_cropped(true);
	
      }
    }
  }

}
#endif
