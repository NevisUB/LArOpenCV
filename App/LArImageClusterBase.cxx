#ifndef LARLITE_LARIMAGECLUSTERBASE_CXX
#define LARLITE_LARIMAGECLUSTERBASE_CXX

#include "LArImageClusterBase.h"
#include "LArUtil/Geometry.h"
#include "FhiclLite/ConfigManager.h"
namespace larlite {

  LArImageClusterBase::LArImageClusterBase(const std::string name) : ana_base()
  {
    _name=name;
    _fout=0;
    _producer="";
    _alg_mgr_v.resize( ::larutil::Geometry::GetME()->Nplanes() );
    _store_original_img=false;
    _contour_tree=nullptr;
  }

  const std::vector<larcv::ImageClusterManager>& LArImageClusterBase::algo_manager_set() const
  { return _alg_mgr_v; }
  
  larcv::ImageClusterManager& LArImageClusterBase::algo_manager(size_t plane_id)
  {
    if(plane_id >= _alg_mgr_v.size()) throw ::larcv::larbys("ImageClusterManager not found (invalid plane ID)!");
    return _alg_mgr_v[plane_id];
  }

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

    if(_producer.empty()) throw ::larcv::larbys("No producer specified...");

    for(auto& mgr : _alg_mgr_v) mgr.Configure(cfg_mgr.Config().get_pset(mgr.Name()));

    if(!_contour_tree){
      _contour_tree = new TTree("contour_tree","Contour Tree" );
      _contour_tree->Branch("area",&_area,"area/F");
      _contour_tree->Branch("perimeter",&_perimeter,"perimeter/F");
      _contour_tree->Branch("bb_height",&_bb_height,"bb_height/F");
      _contour_tree->Branch("bb_width",&_bb_width,"bb_width/F");
      _contour_tree->Branch("max_con_width",&_max_con_width,"max_con_width/F");
      _contour_tree->Branch("min_con_width",&_min_con_width,"min_con_width/F");
      }

    return true;
  }
  
  bool LArImageClusterBase::analyze(storage_manager* storage) {

    _img_mgr.clear();
    _orig_img_mgr.clear();

    ::larcv::Watch watch_all, watch_one;
    watch_all.Start();
    watch_one.Start();
    this->extract_image(storage);
    _process_time_image_extraction += watch_one.WallTime();
      
    if(_store_original_img) {
      for(size_t plane=0; plane<_img_mgr.size(); ++plane) {
	::cv::Mat img;
	_img_mgr.img_at(plane).copyTo(img);
	_orig_img_mgr.push_back(img,_img_mgr.meta_at(plane));
      }
    }

    if(_img_mgr.size() != _alg_mgr_v.size()) throw ::larcv::larbys("# of imaged created != # of planes!");

    _filler.Clear() ;
    for(size_t plane = 0; plane < _alg_mgr_v.size(); ++plane) {

      auto& alg_mgr = _alg_mgr_v[plane];
      auto const& img = _img_mgr.img_at(plane);
      auto const& meta = _img_mgr.meta_at(plane);
      if(!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      
      alg_mgr.Process(img,meta);

      auto & clus = alg_mgr.Clusters() ;
      _filler.Fill(clus);
    }

//    for(int i=0; i<_filler.Areas().size(); i++){
//      _area = _filler.Areas()[i] ;
//      _perimeter = _filler.Perimeters()[i];
//      _bb_height = _filler.BBHeights()[i];
//      _bb_width = _filler.BBWidths()[i];
//      _max_con_width = _filler.MaxContourWidths()[i];
//      _min_con_width = _filler.MinContourWidths()[i];
//      _contour_tree->Fill();
//      }

    watch_one.Start();
    this->store_clusters(storage);
    _process_time_cluster_storage += watch_one.WallTime();

    _process_time_analyze += watch_all.WallTime();
    
    ++_process_count;

    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());

    return true;
  }

  void LArImageClusterBase::Report() const {

    if(_profile) {
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
    for(size_t plane=0; plane < _alg_mgr_v.size(); ++plane) {
      std::cout << "  \033[95mImageClusterManager\033[00m @ Plane " << plane << std::endl;
      _alg_mgr_v[plane].Report();
      std::cout << std::endl;
    }

   if(_fout){
     _fout->cd();
     for(auto& mgr : _alg_mgr_v) mgr.Finalize(_fout);
     //_contour_tree->Write();
    }   

    return true;
  }

}
#endif
