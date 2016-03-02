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
    _store_original_img=false;
  }

  larcv::ImageClusterManager& LArImageClusterBase::algo_manager()
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

    if(_producer.empty()) throw ::larcv::larbys("No producer specified...");

    //for(auto& mgr : _alg_mgr_v) mgr.Configure(cfg_mgr.Config().get_pset(mgr.Name()));
    _alg_mgr.Configure(cfg_mgr.Config().get_pset(_alg_mgr.Name()));
    
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

    //if(_img_mgr.size() != _alg_mgr_v.size()) throw ::larcv::larbys("# of imaged created != # of planes!");
    
    // for(size_t plane = 0; plane < _alg_mgr_v.size(); ++plane) {
    for(size_t plane=0; plane<_img_mgr.size(); ++plane) {
      // auto& alg_mgr    = _alg_mgr_v[plane];
      auto const& img  = _img_mgr.img_at(plane);
      auto const& meta = _img_mgr.meta_at(plane);
      if(!meta.num_pixel_row() || !meta.num_pixel_column()) continue;
      
      _alg_mgr.Add(img,meta);
      _alg_mgr.Process();
      std::cout<<"Matched pairs: "<<_alg_mgr.BookKeeper().GetResult().size()<<std::endl;
    }

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
    std::cout << "  \033[95mImageClusterManager\033[00m\n";
    _alg_mgr.Report();
    


    if(_fout){
      _fout->cd();
      _alg_mgr.Finalize(_fout);
    }   

    return true;
  }

}
#endif
