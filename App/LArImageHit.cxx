#ifndef __LARLITE_LARIMAGEHIT_CXX__
#define __LARLITE_LARIMAGEHIT_CXX__

#include "LArImageHit.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryUtilities.h"
#include "DataFormat/rawdigit.h"
//#include "Utils/NDArrayConverter.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"
#include "DataFormat/event_ass.h"
#include "Core/larbys.h"
#include "DataFormat/pfpart.h"
#include "DataFormat/mcshower.h"
#include "TMultiGraph.h"
#include "TGraph.h"
#include "TArrow.h"
#include "TCanvas.h"
#include "TColor.h"

namespace larlite {

  void LArImageHit::_Configure_(const ::fcllite::PSet &pset)
  {
    _charge_to_gray_scale = pset.get<double>("Q2Gray");
    //    _charge_threshold = pset.get<double>("QMin");
    // Using a try-catch block to avoid forcing a modification of the .fcl files
    try{
      _run_analyze_image_cluster = pset.get<bool>("RunAnalyze");

      std::cout << "\n Running AnalyzeImageCluster function in " << _name << std::endl;

      gROOT->SetBatch();

      _plotFile = nullptr;

      for(int plane = 0; plane < 3; plane++){
	_vhMCRecoStartDist.push_back( new TH1D(Form("hMCRecoStartDist_plane%i", plane),
					       Form("MC-reco cluster start distance on plane %i; Distance (cm); Entries / 5 cm", plane),
					       40, 0, 200) );
      }
    }
    catch(const std::exception& e){ _run_analyze_image_cluster = false; }
  }

  void LArImageHit::_Report_() const
  {
    std::cout << "  # of clusters  ......... ";
    std::cout << Form("%-6zu ",_num_clusters);


    std::cout << " ... Average (per-event) ";
    double nclusters = _num_clusters;
    std::cout << Form("%g ",nclusters / ((double)(_num_stored)));
    std::cout << std::endl;

    std::cout << "  # of clustered hits ... ";
    //for(auto const& num : _num_clustered_hits_v) std::cout << Form("%-6zu ",num);
    std::cout << Form("%-6zu ",_num_clustered_hits);

    std::cout << " ... Fractions: ";
    double clustered   = _num_clustered_hits;
    double unclustered = _num_unclustered_hits;
    std::cout << Form("%g%% ",clustered/(clustered+unclustered));
    std::cout << std::endl;

    _plotFile->cd();
    for(const auto& h: _vhMCRecoStartDist){
      h->Write();
      delete h;
    }

    _plotFile->Close();
    delete _plotFile;
  }
  
  void LArImageHit::store_clusters(storage_manager* storage)
  {
    ++_num_stored;

    auto geom = ::larutil::Geometry::GetME();
    if(_num_clusters == 0){
      _num_clusters = 0;
      _num_clustered_hits = 0;
      _num_unclustered_hits = 0;
    }

    auto ev_hit = storage->get_data<event_hit>(producer());
    
    if(!ev_hit) throw DataFormatException("Could not locate hit data product!");

    auto const& alg_mgr = algo_manager();

    _num_clusters = alg_mgr.NumClusters();

    //std::cout<<"# clusters: "<<_num_clusters<<std::endl;

    AssSet_t cluster_ass;
    cluster_ass.resize(_num_clusters);

    for(auto& ass_unit : cluster_ass) ass_unit.reserve(100);

    for(size_t hindex=0; hindex<ev_hit->size(); ++hindex) {

      auto const& h = (*ev_hit)[hindex];

      auto const& wid = h.WireID();

      auto const cid = alg_mgr.ClusterID(wid.Wire,h.PeakTime(),wid.Plane);

      if(cid == ::larcv::kINVALID_CLUSTER_ID) {
	_num_unclustered_hits += 1;
	continue;
      }

      _num_clustered_hits += 1;
      
      cluster_ass[cid].push_back(hindex);
    }

    auto ev_cluster = storage->get_data<event_cluster>("ImageClusterHit");
    auto ev_ass     = storage->get_data<event_ass>    ("ImageClusterHit");
    if(ev_cluster) {
      ev_cluster->reserve(_num_clusters);

      for(size_t cid=0; cid<_num_clusters; ++cid) {

	auto const& image_cluster = alg_mgr.Cluster(cid);
	
	if(cid != image_cluster.ClusterID())
	  throw DataFormatException("ClusterID ordering seems inconsistent!");

	::larlite::cluster c;
	c.set_view(geom->PlaneToView(image_cluster.PlaneID()));
	c.set_planeID(geo::PlaneID(0,0,image_cluster.PlaneID()));
	c.set_id(image_cluster.ClusterID());
	ev_cluster->push_back(c);
      }

      if(ev_ass)
	ev_ass->set_association(ev_cluster->id(),ev_hit->id(),cluster_ass);
    }

    auto ev_pfpart  = storage->get_data<event_pfpart> ("ImageClusterHit");
    if(ev_pfpart) {
      auto const match_info = alg_mgr.BookKeeper().GetResult();
      AssSet_t pfpart_ass;
      pfpart_ass.reserve(match_info.size());

      for(size_t pfp_index=0; pfp_index<match_info.size(); ++pfp_index) {

	pfpart p;
	ev_pfpart->push_back(p);
	AssUnit_t ass;
	for(auto const& cid : match_info[pfp_index]) ass.push_back(cid);

	pfpart_ass.push_back(ass);
      }

      if(ev_ass && ev_cluster) ev_ass->set_association(ev_pfpart->id(),ev_cluster->id(),pfpart_ass);
    }
    
  }

  void LArImageHit::extract_image(storage_manager* storage)
  {
    
    auto const& geom = ::larutil::Geometry::GetME();
    const size_t nplanes = geom->Nplanes();
    
    auto ev_hit = storage->get_data<event_hit>(producer());

    if(ev_hit==nullptr) throw DataFormatException("Could not locate hit data product!");
    
    std::vector< std::pair<size_t,size_t> > wire_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    std::vector< std::pair<size_t,size_t> > tick_range_v(nplanes,std::pair<size_t,size_t>(1e12,0));
    
    for(auto const& h : *ev_hit) {

      if(h.Integral() < _charge_threshold) continue;
      
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
      ::larcv::ImageMeta meta((double)nwires,(double)nticks,nwires,nticks,wire_range.first,tick_range.first,plane);
      
      if ( nwires >= 1e10 || nticks >= 1e10 )
	_img_mgr.push_back(::cv::Mat(),::larcv::ImageMeta());
      else
	_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_8UC1, cvScalar(0.)),meta);
      
      //if(!nticks || !nwires)
      //_img_mgr.push_back(cv::Mat(),meta);
      //else
      //_img_mgr.push_back(::cv::Mat(nwires, nticks, CV_32FC1, cvScalar(0.)),meta);
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
      
      //if(y>=nticks || x>=nwires) { throw std::exception(); }
      if(y>=nticks || x>=nwires) { std::cout << "\t==> Skipping hit\n"; continue; } // skip this hit
      
      //std::cout<<"Inserting " << x << " " << y << " @ " << wid.Plane << std::endl;
      double charge = h.Integral() / _charge_to_gray_scale;
      charge += (double)(mat.at<unsigned char>(x,y));

      if(charge >= 255.) charge = 255.;
      if(charge < 0.)    charge = 0.;
      
      mat.at<unsigned char>(x,y) = (unsigned char)((int)charge);
    }
    
  }



  void LArImageHit::AnalyzeImageCluster(storage_manager* storage)
  {
    if( !_run_analyze_image_cluster ) return;

    if( !_plotFile ){

      std::string _plotFilePrefix = storage->output_filename().substr(0, storage->output_filename().find(".root"));

      _plotFile = new TFile( Form("%s_%s_PlotBook.root", _plotFilePrefix.c_str(), _name.c_str()), "RECREATE");

      std::cout << "\n Writing plots to " << _plotFile->GetName() << std::endl;

    }

    int auxevent = storage->get_index();

    // Event-by-event analysis

    /*____________________________________________________*/
    //    std::cout << "===> MC truth information" << std::endl;

    event_mcshower* ev_showers = storage->get_data<event_mcshower>("mcreco");

    if( !ev_showers ) throw DataFormatException("Could not locate mcshower data product!");

    std::vector<std::vector<double>> showerEnds, showerStarts;

    // Loop over MC showers
    for( size_t s = 0; s < ev_showers->size(); s++){
      mcshower& shower = ev_showers->at(s);
      // The end of the mcshower is the start of the cluster
      std::vector<double> endxyz = {shower.End().X(), shower.End().Y(), shower.End().Z()};
      showerEnds.push_back(endxyz);
      // The start of the mcshower is the decay vertex which creates the subsequent cluster
      std::vector<double> startxyz = {shower.Start().X(), shower.Start().Y(), shower.Start().Z()};
      showerStarts.push_back(startxyz);
    }

    /*____________________________________________________*/
    //    std::cout << "===> LArOpenCV information" << std::endl;

    // Get ImageClusterManager instance
    auto const& alg_mgr = algo_manager();
    // Original image
    auto const& img_v   = alg_mgr.InputImages();
    // Original image meta
    auto const& meta_v  = alg_mgr.InputImageMetas();

    auto geom = larutil::GeometryUtilities::GetME();

    TCanvas* cimg = new TCanvas( Form("c_eve_%i", auxevent), Form("Event %i", auxevent), 500, 1500 );
    cimg->Divide(1,3);

    std::vector<TMultiGraph*> mgCollector;
    std::vector<std::vector<TArrow*>> aCollector;

    // Loop over images
    for(size_t img_id = 0; img_id < img_v.size(); ++img_id){

      cimg->cd(img_id + 1);

      // Image & meta object
      auto const& img  = img_v[img_id];
      auto const& meta = meta_v[img_id];
      
      // Retrieve final clusters on this image
      auto const& clusters = alg_mgr.Clusters(img_id);

      //      std::cout << "Image " << img_id << " @ Plane " << meta.plane() << " has " << clusters.size() << " clusters!" << std::endl;

      if( !clusters.size() ) continue;

      TMultiGraph* mgimgplane = new TMultiGraph("mgimgplane", "mgimgplane");
      std::vector<TArrow*> clusterSenses;

      // Loop over clusters within a plane
      for(size_t c = 0; c < clusters.size(); c++){

	auto const& cluster = clusters[c];

	TGraph *graphHits = new TGraph(cluster._insideHits.size());
	int ipoint = 0;
	// Loop over hits within a cluster
	for(const auto& hit : cluster._insideHits) {
	  graphHits->SetPoint(ipoint, hit.x, hit.y);
	  ipoint++;
	}
	graphHits->SetMarkerStyle(7);
	graphHits->SetMarkerColor(c+2);
	mgimgplane->Add(graphHits, "P");

	TArrow* clusterSense = new TArrow(cluster._startPt.x, cluster._startPt.y, cluster._endPt.x, cluster._endPt.y, 0.015, ">");
	clusterSense->SetLineColor(TColor::GetColorDark(c+2));
	clusterSense->SetLineWidth(2);
	clusterSense->SetFillStyle(kFEmpty);
	clusterSenses.push_back(clusterSense);
	// To make the multigraph aware of the start/end points
	TGraph *graphClusterStartEnd = new TGraph(2);
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

      TGraph *graphShowerEnds = new TGraph(showerEnds.size());

      // Matrix of shower-cluster 2D-distances
      std::vector<std::vector<double>> matShowerClusterDist;
      matShowerClusterDist.resize(showerEnds.size());
      for(size_t s = 0; s < showerEnds.size(); s++){ matShowerClusterDist[s].resize(clusters.size()); }

      // Loop over MC shower ends
      // Get MC info here and project it to the plane being studied
      for(size_t s = 0; s < showerEnds.size(); s++){
	double* showerEndxyz = &showerEnds.at(s)[0];
	auto const& showerEnd2Dpp = geom->Get2DPointProjection( showerEndxyz, meta.plane() );
	// Images coordinates: horizontal is time, vertical is wire
	larcv::Point2D showerEndImg( (showerEnd2Dpp.t - meta.origin().y)/meta.pixel_height(), (showerEnd2Dpp.w - meta.origin().x)/meta.pixel_width() );
	graphShowerEnds->SetPoint((int)s, showerEndImg.x, showerEndImg.y);

	// Compute all shower-cluster 2D-distances
	for(size_t c = 0; c < clusters.size(); c++){
	  // matShowerClusterDist[s][c] = dist2D(clusters[c]._startPt, showerEndImg); // Distance in pixels
	  auto const& clusterStart = clusters[c]._startPt;
	  // Distance in cm
	  matShowerClusterDist[s][c] = std::sqrt( std::pow( (clusterStart.x - showerEndImg.x) * meta.pixel_height() * (geom->TimeToCm()), 2) 
						  + std::pow( (clusterStart.y - showerEndImg.y) * meta.pixel_width() * (geom->WireToCm()), 2) );
	}

      }
      graphShowerEnds->SetMarkerStyle(kOpenCircle);
      graphShowerEnds->SetMarkerColor(kOrange+1);
      graphShowerEnds->SetMarkerSize(1.5);
      mgimgplane->Add(graphShowerEnds, "P");

      // Because cluster start points might be chosen wrong, both MC showers could be closer to the same cluster
      // To avoid double counting, each cluster will be related to a single shower. The relation is determined by which cluster is closest to the shower
      // Already-related shower-cluster pairs are excluded after each iteration

      // Vector holding the sorted shower-cluster distances
      std::vector<double> vminDist;
      // Number of clusters must be bigger than number of showers
      while( clusters.size() >= showerEnds.size() && vminDist.size() < showerEnds.size() ){
	// Find the minimum
	double minDist =  matShowerClusterDist[0][0];
	size_t minDistS = 0;
	size_t minDistC = 0;
	for(size_t s = 0; s < matShowerClusterDist.size(); s++){
	  for(size_t c = 0; c < matShowerClusterDist[s].size(); c++){
	    if( matShowerClusterDist[s][c] < minDist ){
	      minDist = matShowerClusterDist[s][c];
	      minDistS = s;
	      minDistC = c;
	    }
	  }
	}
	vminDist.push_back(minDist);
	// Remove shower and cluster used
	matShowerClusterDist.erase(matShowerClusterDist.begin() + minDistS); // remove row
	for(size_t s = 0; s < matShowerClusterDist.size(); s++){
	  matShowerClusterDist[s].erase(matShowerClusterDist[s].begin() + minDistC); // remove column
	}
      }

      // Fill distance histograms
      for(const auto& distance : vminDist){
	(_vhMCRecoStartDist.at(meta.plane()))->Fill( distance );
      }

      TGraph *graphShowerStarts = new TGraph(showerStarts.size());
      // Loop over MC shower starts
      // Get MC info here and project it to the plane being studied
      for(size_t s = 0; s < showerStarts.size(); s++){
	double* showerStartxyz = &showerStarts.at(s)[0];
	auto const& showerStart2Dpp = geom->Get2DPointProjection( showerStartxyz, meta.plane() );
	// Images coordinates: horizontal is time, vertical is wire
	larcv::Point2D showerStartImg( (showerStart2Dpp.t - meta.origin().y)/meta.pixel_height(), (showerStart2Dpp.w - meta.origin().x)/meta.pixel_width() );
	graphShowerStarts->SetPoint((int)s, showerStartImg.x, showerStartImg.y);
      }
      graphShowerStarts->SetMarkerStyle(kOpenStar);
      graphShowerStarts->SetMarkerColor(kViolet);
      graphShowerStarts->SetMarkerSize(1.5);
      mgimgplane->Add(graphShowerStarts, "P");

      mgimgplane->Draw("A");
      mgimgplane->SetNameTitle(Form("mg_plane_%i", (int)meta.plane()), Form("Plane %i; Time (pixels); Wire (pixels)", (int)meta.plane()));
      // Loop over arrows defining sense
      for(const auto& sense: clusterSenses){
	sense->Draw();
      }
      aCollector.push_back(clusterSenses);

      mgCollector.push_back(mgimgplane);

    }
    _plotFile->cd();
    cimg->Write();
    // cimg->SaveAs(".png"); cimg->SaveAs(".png"); // Twice because https://root.cern.ch/phpBB3/viewtopic.php?t=19596

    delete cimg;
    for(auto& mg : mgCollector){delete mg;} // This also deletes all the TGraphs owned by the TMultiGraph
    for(auto& v : aCollector){
      for(auto const& a: v){ delete a;}}

  }


}
#endif
