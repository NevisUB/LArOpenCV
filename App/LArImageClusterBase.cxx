#ifndef LARLITE_LARIMAGECLUSTERBASE_CXX
#define LARLITE_LARIMAGECLUSTERBASE_CXX

#include "LArImageClusterBase.h"
#include "LArUtil/Geometry.h"
#include "FhiclLite/ConfigManager.h"

#include "DataFormat/rawdigit.h"
#include "DataFormat/hit.h"
#include "DataFormat/cluster.h"

//#include "DataFormat/PiZeroROI.h"

#include "DataFormat/user_info.h"
#include "DataFormat/event_ass.h"
#include "DataFormat/vertex.h"
#include "DataFormat/pfpart.h"

namespace larlite {

  LArImageClusterBase::LArImageClusterBase(const std::string name) : ana_base()
  {
    _name = name;
    _fout = 0;
    _num_stored = 0;
    _num_clusters = 0;
    _producer = "";
    _store_original_img = false;
    _event = 0;
  }

  void LArImageClusterBase::set_config(const std::string cfg_file)
  { _config_file = cfg_file; }

  larocv::ImageClusterManager& LArImageClusterBase::algo_manager()
  { return _alg_mgr; }

  bool LArImageClusterBase::initialize() {

    ::fcllite::ConfigManager cfg_mgr(_name);

    cfg_mgr.AddCfgFile(_config_file);

    auto const& main_cfg = cfg_mgr.Config().get_pset(_name);

    _profile = main_cfg.get<bool>("Profile");
    _producer = main_cfg.get<std::string>("Producer");
    _store_original_img = main_cfg.get<bool>("StoreOriginalImage");
    _process_count = 0;
    _process_time_image_extraction = 0;
    _process_time_analyze = 0;
    _process_time_cluster_storage = 0;

    this->_Configure_(main_cfg);

    if (_producer.empty()) throw ::larocv::larbys("No producer specified...");

    _alg_mgr.Configure(cfg_mgr.Config().get_pset(_alg_mgr.Name()));

    _alg_mgr.MatchPlaneWeights() = _plane_weights;
    
    return true;
  }

  bool LArImageClusterBase::analyze(storage_manager* storage) {
    
    std::cout<<"\n\nOn event: "<<_event <<std::endl ;
    _event++ ;

    _img_mgr.clear();
    _orig_img_mgr.clear();
    _alg_mgr.ClearData();
    
    ::larocv::Watch watch_all, watch_one;
    watch_all.Start();
    watch_one.Start();
    
    // try getting an image for this event, if possible
    try {
      this->extract_image(storage);
    }
    
    catch (const DataFormatException &e) {
      std::cout << e.what() << std::endl;
      // save data-products that would be created in store_clusters
      // this will prevent event mis-alignment

      auto ev_cluster = storage->get_data<event_cluster>("ImageClusterHit");
      auto ev_pfpart  = storage->get_data<event_pfpart> ("ImageClusterHit");
      auto ev_hit_ass = storage->get_data<event_ass>    ("ImageClusterHit");
      auto ev_vtx     = storage->get_data<event_vertex> (_vtx_producer);
      auto ev_vtx_ass = storage->get_data<event_ass>    ("ImageClusterHit");

      ::larlite::event_user* ev_user;
      if (_debug)
	ev_user = storage->get_data<event_user>("ImageClusterHit");

      //silence compiler
      (void)ev_cluster;
      (void)ev_pfpart;
      (void)ev_hit_ass;
      (void)ev_user;
      (void)ev_vtx_ass;
      (void)ev_vtx;
      
      storage->set_id(storage->run_id(), storage->subrun_id(), storage->event_id());
      return false;
    }


    _process_time_image_extraction += watch_one.WallTime();

    _store_original_img = false;
    if (_store_original_img) {
      for (size_t plane = 0; plane < _img_mgr.size(); ++plane) {
	::cv::Mat img;
	_img_mgr.img_at(plane).copyTo(img);
	_orig_img_mgr.push_back(img, _img_mgr.meta_at(plane));
      }
    }
    
    ::larlite::event_user* ev_user;

    if (_debug)
      ev_user = storage->get_data<event_user>("ImageClusterHit");

    for (size_t plane = 0; plane < _img_mgr.size(); ++plane) {

      auto const& img  = _img_mgr.img_at(plane);
      auto      & meta = _img_mgr.meta_at(plane);
      auto const& roi  = _img_mgr.roi_at(plane);

      if (_debug) meta.set_ev_user(ev_user);
      
      if (!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      
      _alg_mgr.Add(img, meta, roi);

    }
    
    _alg_mgr.Process();
    
    watch_one.Start();

    this->store_clusters(storage);
    
    _process_time_cluster_storage += watch_one.WallTime();

    _process_time_analyze += watch_all.WallTime();

    ++_process_count;

    storage->set_id(storage->run_id(), storage->subrun_id(), storage->event_id());

    //AnalyzeImageCluster(storage);

    return true;
  }

  void LArImageClusterBase::Report() const {

    if (_profile) {
      std::cout << std::endl
		<< "  =================== " << _name << " (analysis unit) Time Report ======================" << std::endl
		<< "  # analyze() call ........ " << _process_count << std::endl
		<< "  \033[95mAnalyze time\033[00m ............ Total " << _process_time_analyze
		<< " [s] ... Average " << _process_time_analyze / _process_count << " [s]" << std::endl
		<< "  \033[95mImage extraction time\033[00m ... Total " << _process_time_image_extraction
		<< " [s] ... Average " << _process_time_image_extraction / _process_count << " [s]" << std::endl
		<< "  \033[95mCluster storage time\033[00m .... Total " << _process_time_cluster_storage
		<< " [s] ... Average " << _process_time_cluster_storage / _process_count << " [s]" << std::endl
		<< std::endl;
      this->_Report_();
      std::cout << std::endl;
    }
  }

  bool LArImageClusterBase::finalize() {

    Report();
    std::cout << "  \033[95mImageClusterManager\033[00m\n";
    _alg_mgr.Report();

    if (_fout) {
      _fout->cd();
      _alg_mgr.Finalize(_fout);
    }

    return true;
  }


  void LArImageClusterBase::store_clusters(storage_manager* storage)
  {
    ++_num_stored;

    if (_num_clusters == 0) {
      _num_clusters = 0;
      _num_clustered_hits = 0;
      _num_unclustered_hits = 0;
    }

    auto geom  = ::larutil::Geometry::GetME();

    auto ev_hit = storage->get_data<event_hit>(producer());

    if (!ev_hit) throw DataFormatException("Could not locate hit data product!");

    auto const& alg_mgr = algo_manager();

    _num_clusters = alg_mgr.NumClusters();

    // prepare cluster -> hit association
    // temporary vector
    AssSet_t temp_cluster_hit_ass;
    temp_cluster_hit_ass.resize(_num_clusters);
    // vector to store
    AssSet_t cluster_hit_ass;
    cluster_hit_ass.reserve(_num_clusters);
    //std::cout<<"LArImageClusterBase, about to store clusters and peace out... "<<std::endl ;
    
    for (auto& ass_unit : temp_cluster_hit_ass)
      ass_unit.reserve(100);

    for (size_t hindex = 0; hindex < ev_hit->size(); ++hindex) {
      
      auto const& h = (*ev_hit)[hindex];

      if ( h.GoodnessOfFit() < 0 && _hit_removal )  continue;

      auto const& wid = h.WireID();

      auto const cid = alg_mgr.ClusterID(wid.Wire, h.PeakTime(), wid.Plane);

      if (cid == ::larocv::kINVALID_CLUSTER_ID) {
	_num_unclustered_hits += 1;
	continue;
      }

      _num_clustered_hits += 1;

      temp_cluster_hit_ass[cid].push_back(hindex);
      
    }// for all hits
    int test_id = -1;
    for(auto & temp_cluster_hit_a : temp_cluster_hit_ass) {
      test_id ++ ;
      if (!temp_cluster_hit_a.size()) continue;

      //if ( temp_cluster_hit_a.size() >= 10 ) { //<= 20){ 
      //  auto const& im = alg_mgr.Cluster(test_id);
      //  std::cout<<"ImageCluster hit size: "<<temp_cluster_hit_a.size()<<", "<<im.PlaneID()<<std::endl ;
      //}
      cluster_hit_ass.emplace_back(temp_cluster_hit_a); // do not std::move it, we need to check nonzero later
    }
    auto ev_cluster = storage->get_data<event_cluster>("ImageClusterHit");
    auto ev_hit_ass = storage->get_data<event_ass>    ("ImageClusterHit");
    auto ev_vtx_ass = storage->get_data<event_ass>    ("ImageClusterHit");
    auto ev_pfpart  = storage->get_data<event_pfpart> ("ImageClusterHit");
    auto ev_vtx     = storage->get_data<event_vertex> (_vtx_producer);

    // hot fix for the case that the ImageCluster contains a pixel
    // but when transforming the hits into image coordinates, we find this
    // contour contains no hits at all. This contour could have been matched with
    // other clusters as well. We need a way to remove it, lets do it like this;
    std::vector<int> cid_good_list(_num_clusters,0);
    int offset=0;
    
    if (ev_cluster) {
      
      ev_cluster->reserve(_num_clusters);
      for (size_t cid = 0; cid < _num_clusters; ++cid) {
	cid_good_list[cid] = offset;
	if (!temp_cluster_hit_ass[cid].size()) {
	  cid_good_list[cid] = 1;
	  offset-=1;
	  std::cout << "[Funkiness Detection] storage_manager TTree entry " << storage->get_index() <<  "\n";
	  continue;
	}
	
	auto const& imgclus = alg_mgr.Cluster(cid);
	
	if (cid != imgclus.ClusterID())
	  throw DataFormatException("ClusterID ordering seems inconsistent!");
	
	::larlite::cluster c;
	
	// set cluster properties
	// x is the time coordinate
	// and Height is also for time
	
	double st = imgclus.XtoTimeTick( imgclus._startPt.x );
	double sw = imgclus.YtoWire    ( imgclus._startPt.y );
	double et = imgclus.XtoTimeTick( imgclus._endPt.x   );
	double ew = imgclus.YtoWire    ( imgclus._endPt.y   );

	//angle, calculated from direction
	double sa = imgclus.AngleofDir();
	
	if ( sw < 0 or sw > geom->Nwires(imgclus.PlaneID()) ) {
	  std::cout << "start wire out of range:" << std::endl;
	  std::cout << "Plane : " << imgclus.PlaneID() << " w/ [start,end] wire -> [" << int(sw) << ", " << int(ew) << "]" << std::endl;
	  std::cout << std::endl;
	}
	else if (ew < 0 or ew > geom->Nwires(imgclus.PlaneID()) ) {
	  std::cout << "end wire out of range:" << std::endl;
	  std::cout << "Plane : " << imgclus.PlaneID() << " w/ [start,end] wire -> [" << int(sw) << ", " << int(ew) << "]" << std::endl;
	  std::cout << std::endl;
	}
	
	
	//set reco params
	c.set_start_wire(sw, 1);
	c.set_end_wire(ew, 1);

	c.set_start_tick(st, 1);
	c.set_end_tick(et, 1);

	c.set_start_angle(sa);
	
	// set plane / id information
	c.set_view(geom->PlaneToView(imgclus.PlaneID()));
	c.set_planeID(geo::PlaneID(0, 0, imgclus.PlaneID()));
	c.set_id(imgclus.ClusterID()+offset);

	// add to event_cluster
	ev_cluster->push_back(c);
      }

      if (ev_cluster->size() != cluster_hit_ass.size()) {
	std::cout << "ev_cluster->size() : " << ev_cluster->size() << " and cluster_hit_ass.size() : " <<  cluster_hit_ass.size() << "\n";
	throw larocv::larbys("There are more cluster-associated hits than there are in ev_cluster. Get your ass over here and fix it\n");
      }
      
      // if we have crated a cluster -> hit association
      if (ev_hit_ass)
	ev_hit_ass->set_association(ev_cluster->id(), ev_hit->id(), cluster_hit_ass);
      
      // if we have created a cluster -> vertex association
      //if (ev_vtx_ass)
      // ev_vtx_ass->set_association(ev_cluster->id(), ev_vtx->id(), cluster_vtx_ass);

    }
    
    if (ev_pfpart) {
      
      auto const match_info = alg_mgr.BookKeeper().GetResult();
      
      AssSet_t pfpart_ass;
      pfpart_ass.reserve(match_info.size());
      
      for (size_t pfp_index = 0; pfp_index < match_info.size(); ++pfp_index) {
	pfpart p;
	ev_pfpart->push_back(p);
	AssUnit_t ass;
	for (auto const& cid : match_info[pfp_index]) {
	  if (cid_good_list.at(cid) == 1) { continue; }
	  int ccid = cid+cid_good_list.at(cid);
	  ass.push_back(ccid);
      }
	pfpart_ass.push_back(ass);
      }

      if (ev_hit_ass && ev_cluster) ev_hit_ass->set_association(ev_pfpart->id(), ev_cluster->id(), pfpart_ass);
      if (ev_vtx) {
        AssSet_t vtx_ass;
        vtx_ass.reserve(ev_pfpart->size());

        for (size_t pfp_index = 0; pfp_index < ev_pfpart->size(); ++pfp_index){
          AssUnit_t ass;
          ass.push_back(0); 
          vtx_ass.push_back(ass);
	}

        if (ev_vtx_ass && ev_vtx) ev_vtx_ass->set_association(ev_pfpart->id(), ev_vtx->id(), vtx_ass);

      }

    }

    return;
  }

}
#endif
