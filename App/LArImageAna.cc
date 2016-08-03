//placeholder for ROOT analysis module
//written @ the michigan workshop. I wanted to remove
//from LArImageHit and make it shareable between LArImage* classes



// For MCTruth viewer
#include "TH1.h"
#include "DataFormat/mcshower.h"
#include "TArrow.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TGraph.h"
#include "TMultiGraph.h"

void AnalyzeImageCluster(storage_manager* storage);

/// Flag to run the AnalyzeImageCluster function
bool _run_analyze_image_cluster;

/// Output file with the plots made by the AnalyzeImageCluster function
TFile* _plotFile;

/// Vector of histograms showing the MC-reconstructed start distance on the planes
std::vector<TH1D*> _vhMCRecoStartDist;


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
      larocv::Point2D showerEndImg(
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
      larocv::Point2D showerStartImg(
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





    if (_plotFile) {
      _plotFile->cd();
      for (const auto& h : _vhMCRecoStartDist) {
	h->Write();
	delete h;
      }

      _plotFile->Close();
      delete _plotFile;
    }
