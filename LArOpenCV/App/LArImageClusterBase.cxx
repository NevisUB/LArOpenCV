#ifndef LARLITE_LARIMAGECLUSTERBASE_CXX
#define LARLITE_LARIMAGECLUSTERBASE_CXX

#include "LArImageClusterBase.h"
#include "LArUtil/Geometry.h"
#include "BasicTool/FhiclLite/ConfigManager.h"

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

    return true;
  }

  bool LArImageClusterBase::analyze(storage_manager* storage) { return true; }

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


  void LArImageClusterBase::store_clusters(storage_manager* storage) { return; }

}
#endif
