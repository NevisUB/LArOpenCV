#ifndef __IMAGECLUSTERMANAGER_CXX__
#define __IMAGECLUSTERMANAGER_CXX__

#include <opencv2/imgproc/imgproc.hpp>
#include "ImageClusterManager.h"
#include "LArOpenCV/Core/larbys.h"
#include "AlgoFactory.h"
#include "BaseUtil.h"

namespace larocv {

  ImageClusterManager::ImageClusterManager(const std::string name)
    : laropencv_base(name)
    , _name(name)
    , _configured(false)
    , _profile(true)
    , _tree(nullptr)
  {
    LAROCV_DEBUG() << "start" << std::endl;
    Reset();
    LAROCV_DEBUG() << "end" << std::endl;
  }
  
  void ImageClusterManager::Reset()
  {
    LAROCV_DEBUG() << "start" << std::endl;
    _configured = false;
    _cluster_alg_v.clear();
    _process_count=0;
    _process_time=0;

    _raw_img_vv.clear();
    _copy_img_vv.clear();
    _raw_meta_vv.clear();
    
    _cluster_alg_m.clear();
    
    LAROCV_DEBUG() << "end" << std::endl;
  }
  
  void ImageClusterManager::Finalize(TFile* file)
  {
    if(file && _tree) {file->cd(); _tree->Write();}
    for(auto& alg : _cluster_alg_v) alg->Finalize(file);
    _configured = false;
  }
  
  void ImageClusterManager::ClearData()
  {
    _raw_img_vv.clear();
    _copy_img_vv.clear();
    _raw_meta_vv.clear();
  }

  std::vector<std::string> ImageClusterManager::GetClusterAlgNames() const
  {
    std::vector<std::string> name_v;
    for(auto const& ptr : _cluster_alg_v)
      name_v.push_back(ptr->Name());
    return name_v;
  }

  std::vector<cv::Mat>& ImageClusterManager::InputImages(ImageSetID_t set_id)
  {
    if(_copy_img_vv.empty()) 
      throw larbys("No image available!");

    if(set_id == ImageSetID_t::kImageSetUnknown)
      throw larbys("ImageSetID_t::kImageSetUnknown not a valid id");
    if((int)set_id >= (int)(_copy_img_vv.size())) {
      LAROCV_CRITICAL() << "Invalid image set id: " << (int)set_id << std::endl;
      throw larbys();
    }

    return _copy_img_vv.at((int)set_id);
  }

  std::vector<cv::Mat>& ImageClusterManager::OriginalInputImages(ImageSetID_t set_id) {
    
    if(_raw_img_vv.empty()) 
      throw larbys("No image available!");
    
    if(set_id == ImageSetID_t::kImageSetUnknown)
      throw larbys("ImageSetID_t::kImageSetUnknown not a valid id");
    if((int)set_id >= (int)(_raw_img_vv.size())) {
      LAROCV_CRITICAL() << "Invalid image set id: " << (int)set_id << std::endl;
      throw larbys();
    }
    
    return _raw_img_vv[(int)set_id];
  }
  
  const std::vector<ImageMeta>& ImageClusterManager::InputImageMetas(ImageSetID_t set_id) const
  {
    if(_raw_meta_vv.empty()) 
      throw larbys("No image available!");

    if(set_id == ImageSetID_t::kImageSetUnknown)
      throw larbys("ImageSetID_t::kImageSetUnknown not a valid id");
    if((int)set_id >= (int)(_raw_meta_vv.size())) {
      LAROCV_CRITICAL() << "Invalid image set id: " << (int)set_id << std::endl;
      throw larbys();
    }

    return _raw_meta_vv[(int)set_id];
  }

  const ImageClusterBase* ImageClusterManager::GetClusterAlg(const AlgorithmID_t id) const
  {
    if(id >= _cluster_alg_v.size()) throw larbys("Invalid algorithm ID requested...");
    return _cluster_alg_v[id];
  }

  ImageClusterBase* ImageClusterManager::GetClusterAlgRW(const AlgorithmID_t id)
  {
    if(id >= _cluster_alg_v.size()) throw larbys("Invalid algorithm ID requested...");
    return _cluster_alg_v[id];
  }

  const ImageClusterBase* ImageClusterManager::GetClusterAlg(const std::string name) const
  {
    return GetClusterAlg(GetClusterAlgID(name));
  }

  AlgorithmID_t ImageClusterManager::GetClusterAlgID(const std::string name) const
  {
    auto iter = _cluster_alg_m.find(name);
    if(iter == _cluster_alg_m.end()) return kINVALID_ALGO_ID;
    return (*iter).second;
  }

  void ImageClusterManager::Configure(const Config_t& main_cfg)
  {
    LAROCV_DEBUG() << "Start" << std::endl;
    _profile = main_cfg.get<bool>("Profile",true);

    this->set_verbosity((msg::Level_t)(main_cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));
    logger::get_shared().set(this->logger().level());

    _filter = main_cfg.get<bool>("EnableFilter",false);
    
    auto cluster_instance_type_v = main_cfg.get<std::vector<std::string> >("ClusterAlgoType");
    auto cluster_instance_name_v = main_cfg.get<std::vector<std::string> >("ClusterAlgoName");

    auto store_cluster_instance_name_v = main_cfg.get<std::vector<std::string> >("StoreAlgoName",{});
						   
    if(cluster_instance_type_v.size() != cluster_instance_name_v.size()) {
      LAROCV_CRITICAL() << "Clustering: AlgoType and AlgoName config parameters have different length! "
			<< "(" << cluster_instance_type_v.size() << " vs. " << cluster_instance_name_v.size() << ")" << std::endl;
      throw larbys();
    }

    _cluster_alg_v.clear();
    _cluster_alg_m.clear();
    
    for(size_t i=0; i<cluster_instance_type_v.size(); ++i) {
      auto const& name = cluster_instance_name_v[i];
      auto const& type = cluster_instance_type_v[i];
      if(_cluster_alg_m.find(name) != _cluster_alg_m.end()) {
	LAROCV_CRITICAL() << "Duplicate algorithm name found!" << std::endl;
	throw larbys("Duplicate algorithm name found!");
      }
      
      _cluster_alg_m[name] = _cluster_alg_v.size();
      auto ptr = AlgoFactory::get().create(type,name);
      switch(ptr->Type()) {
      case kAlgoImageAna:
	LAROCV_INFO() << "Instantiated ImageAna algorithm type " << type
		      << " w/ name " << name << " @ " << ptr << std::endl;
	break;
      default:
	LAROCV_CRITICAL() << "Cannot register type " << type
			  << " algorithm (neither ImageAna nor Cluster algo type)" << std::endl;
	throw larbys();
      }
      ptr->_id = _cluster_alg_v.size();
      ptr->_dataman_ptr = &_algo_dataman;
      _algo_dataman.Register(ptr->_id,name);
      _cluster_alg_v.push_back(ptr);
    }

    for(auto& ptr : _cluster_alg_v) {
      ptr->Profile(_profile);
      ptr->set_verbosity(this->logger().level());
      ptr->Configure(main_cfg.get_pset(ptr->Name()));
    }

    LAROCV_INFO() << "Selecting AlgoData to store" << std::endl;
    _store_cluster_alg_id_v.clear();
    _store_cluster_alg_id_v.reserve(store_cluster_instance_name_v.size());
    for(const auto& name : store_cluster_instance_name_v) {
      if(_cluster_alg_m.find(name) == _cluster_alg_m.end()) {
	LAROCV_CRITICAL() << "Unable to store unknown cluster algo name: " << name << std::endl;	
	throw larbys();
      }
      auto algo_id = _cluster_alg_m[name];
      LAROCV_DEBUG() << "Selected algo " << name << " @ id " << algo_id << std::endl;
      _store_cluster_alg_id_v.push_back(algo_id);
    }
    
    if(main_cfg.get<bool>("StoreAlgoData",false)) {
      _tree = new TTree("larocv_tree","");
      LAROCV_DEBUG() << "Registering larocv_tree" << std::endl;
      _algo_dataman.Register(_tree,_store_cluster_alg_id_v);
    }
    
    _configured=true;
    LAROCV_DEBUG() << "Return" << std::endl;
  }

  void ImageClusterManager::Report() const
  {
    
    std::cout << "  ================== " << Name() << " Profile Report ==================" << std::endl
	      << "  # Process call = " << _process_count << " ... Total time = " << _process_time << " [s]" << std::endl;
    for(auto const& ptr : _cluster_alg_v) {
      std::cout << Form("  \033[93m%-20s\033[00m ... # call %-5zu ... total time %g [s] ... average time %g [s/process]",
			ptr->Name().c_str(), ptr->ProcessCount(), ptr->ProcessTime(), ptr->ProcessTime() / (double)(ptr->ProcessCount()))
		<< std::endl;
    }
    
  }

  void ImageClusterManager::Add( ::cv::Mat& img, const ImageMeta& meta, ImageSetID_t set_id)
  {
    if(set_id == ImageSetID_t::kImageSetUnknown) {
      LAROCV_CRITICAL() << "kImageSetUnknwon is not a valid ImageSetID_t value" << std::endl;
      throw larbys();
    }
    size_t set_idx = (size_t)set_id;
    LAROCV_INFO() << "Adding an image @ set id " << set_idx << std::endl;
    _raw_img_vv.resize(set_idx+1);
    _copy_img_vv.resize(set_idx+1);
    _raw_meta_vv.resize(set_idx+1);
    
    _raw_img_vv[set_idx].push_back(img);
    _copy_img_vv[set_idx].push_back(img.clone());
    _raw_meta_vv[set_idx].push_back(meta);
  }
  
  bool ImageClusterManager::Process()
  {
    LAROCV_DEBUG() << "Start Processing" << std::endl;
    
    if(!_configured) throw larbys("Must Configure() before Process()!");
    
    _watch.Start();

    _algo_dataman.ClearData();

    for(auto& ptr : _cluster_alg_v) ptr->Reset();
    
    // Sanity check of image dimensions
    for(size_t set_index=0; set_index < _copy_img_vv.size(); ++set_index) {
      auto const& copy_img_v = _copy_img_vv.at(set_index);
      auto const& raw_meta_v = _raw_meta_vv.at(set_index);
      
      LAROCV_DEBUG() << "Pre-Check image set: " << set_index << "\n";
      
      for(size_t img_index=0; img_index<copy_img_v.size(); ++img_index) {

	LAROCV_DEBUG() << "  Pre-Check image index: " << img_index << "\n";
	LAROCV_DEBUG() << "    # meta .... " << raw_meta_v.size() << "\n";
	LAROCV_DEBUG() << "    # image ... " << copy_img_v.size() << "\n";
      
	auto const& meta = raw_meta_v.at(img_index);
	auto& img  = copy_img_v.at(img_index);

	if(meta.num_pixel_row()!=img.rows)
	  throw larbys("Provided metadata has incorrect # horizontal pixels w.r.t. image!");
	
	if(meta.num_pixel_column()!=img.cols)
	  throw larbys("Provided metadata has incorrect # vertical pixels w.r.t. image!");
      }
    }

    //
    // First-pass clustering
    //
    for(size_t alg_index=0; alg_index<_cluster_alg_v.size(); ++alg_index) {
      
      auto& alg_ptr    = _cluster_alg_v[alg_index];
      if(!alg_ptr) throw larbys("Invalid algorithm pointer!");
      
      alg_ptr->SetData(_copy_img_vv, _raw_meta_vv);

    }
    
    for(size_t alg_index=0; alg_index<_cluster_alg_v.size(); ++alg_index) {
      
      auto& alg_ptr    = _cluster_alg_v[alg_index];
      if(!alg_ptr) throw larbys("Invalid algorithm pointer!");
      
      if(alg_ptr->Type() == kAlgoImageAna)
	((ImageAnaBase*)alg_ptr)->Process();
      else
	throw larbys("Not supported algorithm type!");
    }

    bool good_state = true;
    for(size_t alg_index=0; alg_index<_cluster_alg_v.size(); ++alg_index) {
      
      auto& alg_ptr    = _cluster_alg_v[alg_index];
      if(!alg_ptr) throw larbys("Invalid algorithm pointer!");

      if(alg_ptr->Type() == kAlgoImageAna)
	good_state = good_state && ((ImageAnaBase*)alg_ptr)->PostProcess();
      else
	throw larbys("Not supported algorithm type!");
      
      if(!good_state && _filter) {
	LAROCV_WARNING() << "Break state assigned by algorithm " << alg_ptr->Name() << " (ID=" << alg_ptr->ID() << ")" << std::endl;
	break;
      }
    }

    if(_tree) _tree->Fill();
    
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
}
#endif
