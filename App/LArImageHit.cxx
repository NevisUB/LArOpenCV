#ifndef __LARLITE_LARIMAGEHIT_CXX__
#define __LARLITE_LARIMAGEHIT_CXX__

#include "LArImageHit.h"
#include "Core/larbys.h"

#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"

#include "DataFormat/PiZeroROI.h"

#include "DataFormat/cluster.h"
#include "DataFormat/event_ass.h"
#include "DataFormat/hit.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/rawdigit.h"

namespace larlite {

  void LArImageHit::_Configure_(const ::fcllite::PSet& pset) {

    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    _charge_threshold = pset.get<double>("QMin");
    _pool_time_tick = pset.get<int>("PoolTimeTick");

    _use_roi = pset.get<bool>("UseROI");
    _roi_producer = pset.get<std::string>("ROIProducer");
    _use_shower_roi = pset.get<bool>("UserShowerROI");
    
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
    const size_t nplanes = geom->Nplanes();

    LAROCV_DEBUG((*this)) << "Getting hit producer " << producer() << "\n";

    auto ev_hit = storage->get_data<event_hit>(producer());
    if (ev_hit == nullptr)
      throw DataFormatException("Could not locate hit data product!");

    std::vector<std::pair<size_t, size_t>> wire_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));
    std::vector<std::pair<size_t, size_t>> tick_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));

    ::larlite::event_PiZeroROI* ev_roi = nullptr;
    if (_use_roi) {
      LAROCV_DEBUG((*this)) << "Requesting use of ROI from producer " << _roi_producer << "\n";
      ev_roi = storage->get_data<event_PiZeroROI>(_roi_producer);
      
      if (ev_roi->size() == 0) {
	throw DataFormatException("Could not locate ROI data product and you have UseROI: True!");
      }

      if ( ev_roi->size() > 1 ) { throw larcaffe::larbys("More than one ROI, not implemented!\n"); }

      std::vector < std::pair< int, int > >  wr_v;
      std::vector < std::pair< int, int > >  tr_v;

      if ( _use_shower_roi ) { 
	wr_v = (*ev_roi)[0].GetPiZeroWireROI();
	tr_v = (*ev_roi)[0].GetPiZeroTimeROI();
      } else {
	wr_v = (*ev_roi)[0].GetWireROI();
	tr_v = (*ev_roi)[0].GetTimeROI();
      }

      for (uint k = 0; k < nplanes; ++k) {
	//wire
	wire_range_v[k].first   = wr_v[k].first;
	wire_range_v[k].second  = wr_v[k].second;

	//time
	tick_range_v[k].first   = tr_v[k].first;
	tick_range_v[k].second  = tr_v[k].second;
      }
	

    } else { //do not use the ROI information

      LAROCV_DEBUG((*this)) << "Not using ROI\n";

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
    }

    //// In this block, we set ROI parameters from larcv
    std::vector<larcaffe::ROI> temp_roi_v;

    if ( _use_larcv_roi ){
    
      auto roi_v = _ev_roi->ROIArray() ;

      for(int i = 0; i < roi_v.size(); i++){
            
        auto r = roi_v[i];
            
        // OK...yuck. But let's do it.
        ::larcaffe::ROI l_roi( int(r.Index()), r.Type(),r.Shape(), r.MCSTIndex(), r.MCTIndex(), r.EnergyDeposit(),
                                 r.EnergyInit(), r.PdgCode(), r.ParentPdgCode(), r.TrackID(), r.ParentTrackID(),
                                 r.X(), r.Y(), r.Z(), r.T(), r.Px(), r.Py(), r.Pz(),
                                 r.ParentX(), r.ParentY(), r.ParentZ(), r.ParentPx(), r.ParentPy(), r.ParentPz() );
                                 //r.NuCurrentType(), r.NuInteractionType() );

        temp_roi_v.emplace_back(l_roi);

        }   
      }   
    //// larcv ROI params should now be set!

    for (size_t plane = 0; plane < nplanes; ++plane) {

      auto const& wire_range = wire_range_v[plane];
      auto const& tick_range = tick_range_v[plane];
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      ::larocv::ImageMeta meta((double)nwires, (double)nticks, nwires, nticks,
			       wire_range.first, tick_range.first, plane);
      
      if (_use_roi) {
	auto vtx = (*ev_roi)[0].GetVertex()[plane];
	auto trkend = (*ev_roi)[0].GetTrackEnd()[plane];
	meta.setvtx(vtx.second, vtx.first);
	meta.settrkend(trkend.second, trkend.first);
      }

      // Only fill the roi info if directed to. 
      // Same info for all 3 planes
      if (_use_larcv_roi){
        meta.ClearROIs();
        meta.SetLArCVROIs(temp_roi_v);
        }

      if (nwires >= 1e10 || nticks >= 1e10)
	_img_mgr.push_back(::cv::Mat(), ::larocv::ImageMeta());
      else
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta);
         
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
	auto& img = _img_mgr.img_at(plane);
	auto& meta = _img_mgr.meta_at(plane);

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

	img = pooled;
	meta = ::larocv::ImageMeta((double)pooled.rows, (double)pooled.cols * _pool_time_tick,
				   pooled.rows, pooled.cols, wire_range.first, tick_range.first, plane);

        if (_use_larcv_roi){
          meta.ClearROIs();
          meta.SetLArCVROIs(temp_roi_v);
          }

	if (_use_roi) {
	  //const auto& vtx = (*ev_roi)[0].GetVertex()[plane];
	  // you wont believe this but I have to make a strict copy of the vertex out
	  // of the data product or somehoe GetTrackEnd() will overwrite the reference
	  // i dare you to change below to const reference it's scary
	  auto vtx = (*ev_roi)[0].GetVertex()[plane];
	  auto trkend = (*ev_roi)[0].GetTrackEnd()[plane];
	  meta.setvtx(vtx.second, vtx.first);
	  meta.settrkend(trkend.second, trkend.first);
	}
      
      }
    }
  }

}
#endif
