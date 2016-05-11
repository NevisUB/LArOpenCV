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

// For MCTruth viewer
#include "DataFormat/mcshower.h"
#include "TArrow.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TGraph.h"
#include "TMultiGraph.h"


namespace larlite {

  void LArImageHit::_Configure_(const ::fcllite::PSet& pset) {
    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    _charge_threshold = pset.get<double>("QMin");
    _pool_time_tick = pset.get<int>("PoolTimeTick");

    _use_roi = pset.get<bool>("UseROI");
    _roi_producer = pset.get<std::string>("ROIProducer");

    // Using a try-catch block to avoid forcing a modification of the .fcl files
    try {
      _run_analyze_image_cluster = pset.get<bool>("RunAnalyze");

      std::cout << "\n Running AnalyzeImageCluster function in " << _name
		<< std::endl;

      gROOT->SetBatch();

      _plotFile = nullptr;

      for (int plane = 0; plane < 3; plane++) {
	_vhMCRecoStartDist.push_back(
				     new TH1D(Form("hMCRecoStartDist_plane%i", plane),
					      Form("MC-reco cluster start distance on plane %i; Distance "
						   "(cm); Entries / 5 cm",
						   plane),
					      40, 0, 200));
      }
    } catch (const std::exception& e) {
      _run_analyze_image_cluster = false;
    }
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

    if (_plotFile) {
      _plotFile->cd();
      for (const auto& h : _vhMCRecoStartDist) {
	h->Write();
	delete h;
      }

      _plotFile->Close();
      delete _plotFile;
    }
  }

  void LArImageHit::extract_image(storage_manager* storage) {
    auto const& geom = ::larutil::Geometry::GetME();
    const size_t nplanes = geom->Nplanes();

    auto ev_hit = storage->get_data<event_hit>(producer());
    if (ev_hit == nullptr)
      throw DataFormatException("Could not locate hit data product!");

    std::vector<std::pair<size_t, size_t>> wire_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));
    std::vector<std::pair<size_t, size_t>> tick_range_v(nplanes, std::pair<size_t, size_t>(1e12, 0));

    ::larlite::event_PiZeroROI* ev_roi = nullptr;
    if (_use_roi) {
      ev_roi = storage->get_data<event_PiZeroROI>(_roi_producer);
      
      if (ev_roi->size() == 0) {
	throw DataFormatException("Could not locate ROI data product and you have UseROI: True!");
      }

      if ( ev_roi->size() > 1 ) { throw larcaffe::larbys("More than one ROI, not implemented!\n"); }

      auto wr_v = (*ev_roi)[0].GetWireROI();
      auto tr_v = (*ev_roi)[0].GetTimeROI();

      for (uint k = 0; k < nplanes; ++k) {
	wire_range_v[k] = wr_v[k];
	tick_range_v[k] = tr_v[k];
      }

    } else {
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

    for (size_t plane = 0; plane < nplanes; ++plane) {
      auto const& wire_range = wire_range_v[plane];
      auto const& tick_range = tick_range_v[plane];
      size_t nticks = tick_range.second - tick_range.first + 2;
      size_t nwires = wire_range.second - wire_range.first + 2;
      ::larcv::ImageMeta meta((double)nwires, (double)nticks, nwires, nticks,
			      wire_range.first, tick_range.first, plane);
      
      if (_use_roi) {
	auto vtx = (*ev_roi)[0].GetVertex()[plane];
	auto trkend = (*ev_roi)[0].GetTrackEnd()[plane];
	meta.setvtx(vtx.second, vtx.first);
	meta.settrkend(trkend.second, trkend.first);
	
      }

      if (nwires >= 1e10 || nticks >= 1e10)
	_img_mgr.push_back(::cv::Mat(), ::larcv::ImageMeta());
      else
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),
			   meta);
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


      // skip this hit
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

	// columns == ticks
	// std::cout << "plane: " << plane << " img rows: " << img.rows << "
	// columns: " << img.cols << "\n";

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
	meta = ::larcv::ImageMeta((double)pooled.rows, (double)pooled.cols * _pool_time_tick,
				  pooled.rows, pooled.cols, wire_range.first, tick_range.first, plane);

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

  void LArImageHit::AnalyzeImageCluster(storage_manager* storage) {
    if (!_run_analyze_image_cluster) return;

    if (!_plotFile) {
      std::string _plotFilePrefix = storage->output_filename().substr(0, storage->output_filename().find(".root"));

      _plotFile = new TFile(
			    Form("%s_%s_PlotBook.root", _plotFilePrefix.c_str(), _name.c_str()),
			    "RECREATE");

      std::cout << "\n Writing plots to " << _plotFile->GetName() << std::endl;
    }

    int auxevent = storage->get_index();

    // Event-by-event analysis

    /*____________________________________________________*/
    //    std::cout << "===> MC truth information" << std::endl;

    event_mcshower* ev_showers = storage->get_data<event_mcshower>("mcreco");

    if (!ev_showers)
      throw DataFormatException("Could not locate mcshower data product!");

    std::vector<std::vector<double>> showerEnds, showerStarts;

    // Loop over MC showers
    for (size_t s = 0; s < ev_showers->size(); s++) {
      mcshower& shower = ev_showers->at(s);
      // The end of the mcshower is the start of the cluster
      std::vector<double> endxyz = {shower.End().X(), shower.End().Y(),
				    shower.End().Z()};
      showerEnds.push_back(endxyz);
      // The start of the mcshower is the decay vertex which creates the
      // subsequent cluster
      std::vector<double> startxyz = {shower.Start().X(), shower.Start().Y(),
				      shower.Start().Z()};
      showerStarts.push_back(startxyz);
    }

    /*____________________________________________________*/
    //    std::cout << "===> LArOpenCV information" << std::endl;

    // Get ImageClusterManager instance
    auto const& alg_mgr = algo_manager();
    // Original image
    auto const& img_v = alg_mgr.InputImages();
    // Original image meta
    auto const& meta_v = alg_mgr.InputImageMetas();

    auto geomH = larutil::GeometryHelper::GetME();

    TCanvas* cimg = new TCanvas(Form("c_eve_%i", auxevent),
				Form("Event %i", auxevent), 500, 1500);
    cimg->Divide(1, 3);

    std::vector<TMultiGraph*> mgCollector;
    std::vector<std::vector<TArrow*>> aCollector;

    // Loop over images
    for (size_t img_id = 0; img_id < img_v.size(); ++img_id) {
      cimg->cd(img_id + 1);

      // Image & meta object
      // auto const& img  = img_v[img_id];
      auto const& meta = meta_v[img_id];

      // Retrieve final clusters on this image
      auto const& clusters = alg_mgr.Clusters(img_id);

      // std::cout << "Image " << img_id << " @ Plane " << meta.plane() << " has "
      // << clusters.size() << " clusters!" << std::endl;

      if (!clusters.size()) continue;

      TMultiGraph* mgimgplane = new TMultiGraph("mgimgplane", "mgimgplane");
      std::vector<TArrow*> clusterSenses;

      // Loop over clusters within a plane
      for (size_t c = 0; c < clusters.size(); c++) {
	auto const& cluster = clusters[c];

	TGraph* graphHits = new TGraph(cluster._insideHits.size());
	int ipoint = 0;
	// Loop over hits within a cluster
	for (const auto& hit : cluster._insideHits) {
	  graphHits->SetPoint(ipoint, hit.x, hit.y);
	  ipoint++;
	}
	graphHits->SetMarkerStyle(7);
	graphHits->SetMarkerColor(c + 2);
	mgimgplane->Add(graphHits, "P");

	TArrow* clusterSense =
          new TArrow(cluster._startPt.x, cluster._startPt.y, cluster._endPt.x,
                     cluster._endPt.y, 0.015, ">");
	clusterSense->SetLineColor(TColor::GetColorDark(c + 2));
	clusterSense->SetLineWidth(2);
	clusterSense->SetFillStyle(kFEmpty);
	clusterSenses.push_back(clusterSense);
	
	// To make the multigraph aware of the start/end points
	TGraph* graphClusterStartEnd = new TGraph(2);
	graphClusterStartEnd->SetPoint(0, cluster._startPt.x, cluster._startPt.y);
	graphClusterStartEnd->SetPoint(1, cluster._endPt.x, cluster._endPt.y);
	graphClusterStartEnd->SetMarkerStyle(kFullCircle);
	graphClusterStartEnd->SetMarkerColor(clusterSense->GetLineColor());
	mgimgplane->Add(graphClusterStartEnd, "P");

	// Not available yet
	// TGraph *graphVtx = new TGraph(1);
	// graphVtx->SetPoint(0, cluster._vertex_2D.x, cluster._vertex_2D.y);
	// graphVtx->SetMarkerStyle(kFullStar);
	// graphVtx->SetMarkerColor(c+2);
	// mgimgplane->Add(graphVtx, "P");
      }

      TGraph* graphShowerEnds = new TGraph(showerEnds.size());

      // Matrix of shower-cluster 2D-distances
      std::vector<std::vector<double>> matShowerClusterDist;
      matShowerClusterDist.resize(showerEnds.size());
      for (size_t s = 0; s < showerEnds.size(); s++) {
	matShowerClusterDist[s].resize(clusters.size());
      }

      // Loop over MC shower ends
      // Get MC info here and project it to the plane being studied
      for (size_t s = 0; s < showerEnds.size(); s++) {
	double* showerEndxyz = &showerEnds.at(s)[0];
	auto const& showerEnd2Dpp =
          geomH->Point_3Dto2D(showerEndxyz, meta.plane());
	// Images coordinates: horizontal is time, vertical is wire
	larcv::Point2D showerEndImg(
				    (showerEnd2Dpp.t - meta.origin().y) / meta.pixel_height(),
				    (showerEnd2Dpp.w - meta.origin().x) / meta.pixel_width());
	graphShowerEnds->SetPoint((int)s, showerEndImg.x, showerEndImg.y);

	// Compute all shower-cluster 2D-distances
	for (size_t c = 0; c < clusters.size(); c++) {
	  // matShowerClusterDist[s][c] = dist2D(clusters[c]._startPt,
	  // showerEndImg); // Distance in pixels
	  auto const& clusterStart = clusters[c]._startPt;
	  // Distance in cm
	  matShowerClusterDist[s][c] =
            std::sqrt(std::pow((clusterStart.x - showerEndImg.x) *
			       meta.pixel_height() * (geomH->TimeToCm()),
                               2) +
                      std::pow((clusterStart.y - showerEndImg.y) *
			       meta.pixel_width() * (geomH->WireToCm()),
                               2));
	}
      }
      graphShowerEnds->SetMarkerStyle(kOpenCircle);
      graphShowerEnds->SetMarkerColor(kOrange + 1);
      graphShowerEnds->SetMarkerSize(1.5);
      mgimgplane->Add(graphShowerEnds, "P");

      // Because cluster start points might be chosen wrong, both MC showers could
      // be closer to the same cluster
      // To avoid double counting, each cluster will be related to a single
      // shower. The relation is determined by which cluster is closest to the
      // shower
      // Already-related shower-cluster pairs are excluded after each iteration

      // Vector holding the sorted shower-cluster distances
      std::vector<double> vminDist;
      // Number of clusters must be bigger than number of showers
      while (clusters.size() >= showerEnds.size() &&
	     vminDist.size() < showerEnds.size()) {
	// Find the minimum
	double minDist = matShowerClusterDist[0][0];
	size_t minDistS = 0;
	size_t minDistC = 0;
	for (size_t s = 0; s < matShowerClusterDist.size(); s++) {
	  for (size_t c = 0; c < matShowerClusterDist[s].size(); c++) {
	    if (matShowerClusterDist[s][c] < minDist) {
	      minDist = matShowerClusterDist[s][c];
	      minDistS = s;
	      minDistC = c;
	    }
	  }
	}
	vminDist.push_back(minDist);
	// Remove shower and cluster used
	matShowerClusterDist.erase(matShowerClusterDist.begin() +
				   minDistS);  // remove row
	for (size_t s = 0; s < matShowerClusterDist.size(); s++) {
	  matShowerClusterDist[s].erase(matShowerClusterDist[s].begin() +
					minDistC);  // remove column
	}
      }

      // Fill distance histograms
      for (const auto& distance : vminDist) {
	(_vhMCRecoStartDist.at(meta.plane()))->Fill(distance);
      }

      TGraph* graphShowerStarts = new TGraph(showerStarts.size());
      // Loop over MC shower starts
      // Get MC info here and project it to the plane being studied
      for (size_t s = 0; s < showerStarts.size(); s++) {
	double* showerStartxyz = &showerStarts.at(s)[0];
	auto const& showerStart2Dpp =
          geomH->Point_3Dto2D(showerStartxyz, meta.plane());
	// Images coordinates: horizontal is time, vertical is wire
	larcv::Point2D showerStartImg(
				      (showerStart2Dpp.t - meta.origin().y) / meta.pixel_height(),
				      (showerStart2Dpp.w - meta.origin().x) / meta.pixel_width());
	graphShowerStarts->SetPoint((int)s, showerStartImg.x, showerStartImg.y);
      }
      graphShowerStarts->SetMarkerStyle(kOpenStar);
      graphShowerStarts->SetMarkerColor(kViolet);
      graphShowerStarts->SetMarkerSize(1.5);
      mgimgplane->Add(graphShowerStarts, "P");

      mgimgplane->Draw("A");
      mgimgplane->SetNameTitle(
			       Form("mg_plane_%i", (int)meta.plane()),
			       Form("Plane %i; Time (pixels); Wire (pixels)", (int)meta.plane()));
      // Loop over arrows defining sense
      for (const auto& sense : clusterSenses) {
	sense->Draw();
      }
      aCollector.push_back(clusterSenses);

      mgCollector.push_back(mgimgplane);
    }
    _plotFile->cd();
    cimg->Write();
    // cimg->SaveAs(".png"); cimg->SaveAs(".png"); // Twice because
    // https://root.cern.ch/phpBB3/viewtopic.php?t=19596

    delete cimg;
    for (auto& mg : mgCollector) {
      delete mg;
    }  // This also deletes all the TGraphs owned by the TMultiGraph
    for (auto& v : aCollector) {
      for (auto const& a : v) {
	delete a;
      }
    }
  }
}
#endif
