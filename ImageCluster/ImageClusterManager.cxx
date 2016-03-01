#ifndef __IMAGECLUSTERMANAGER_CXX__
#define __IMAGECLUSTERMANAGER_CXX__

#include <opencv2/imgproc/imgproc.hpp>
#include "ImageClusterManager.h"
#include "Core/larbys.h"
#include "ClusterAlgoFactory.h"
#include "MatchAlgoFactory.h"
#include "ReClusterAlgoFactory.h"
#include "Utilities.h"

namespace larcv {

  ImageClusterManager::ImageClusterManager(const std::string name)
    : laropencv_base(name)
    , _name(name)
    , _configured(false)
    , _profile(true)
    , _match_alg(nullptr)
    , _recluster_alg(nullptr)
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    Reset();
    LARCV_DEBUG((*this)) << "end" << std::endl;
  }
  
  void ImageClusterManager::Reset()
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    _configured = false;
    _cluster_alg_v.clear();
    _clusters_v_v.clear();
    _process_count=0;
    _process_time=0;
    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  void ImageClusterManager::Finalize(TFile* file)
  {
    for(auto& alg : _cluster_alg_v   ) alg->Finalize(file);
    _match_alg->Finalize(file);
    _recluster_alg->Finalize(file);
    _configured = false;
  }

  const ClusterAlgoBase& ImageClusterManager::GetClusterAlg(const AlgorithmID_t id) const
  {
    if(id >= _cluster_alg_v.size()) throw larbys("Invalid algorithm ID requested...");
    return *_cluster_alg_v[id];
  }

  const ClusterAlgoBase& ImageClusterManager::GetClusterAlg(const std::string name) const
  {
    return GetClusterAlg(GetClusterAlgID(name));
  }

  AlgorithmID_t ImageClusterManager::GetClusterAlgID(const std::string name) const
  {
    auto iter = _cluster_alg_m.find(name);
    if(iter == _cluster_alg_m.end()) return kINVALID_ALGO_ID;
    return (*iter).second;
  }

  const MatchAlgoBase& ImageClusterManager::GetMatchAlg() const
  { return *_match_alg; }

  const ReClusterAlgoBase& ImageClusterManager::GetReClusterAlg() const
  { return *_recluster_alg; }

  /*
  AlgorithmID_t ImageClusterManager::AddAlg(ImageClusterBase* alg)
  {
    if(!alg) throw larbys("Cannot add nullptr!");
    if(_configured) throw larbys("Cannot add algo after configured! (call Reset())");
    _cluster_alg_v.push_back(alg);
    return (_cluster_alg_v.size() - 1);
  }
  */
  
  void ImageClusterManager::Configure(const ::fcllite::PSet& main_cfg)
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    _profile = main_cfg.get<bool>("Profile");

    this->set_verbosity((msg::Level_t)(main_cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    _show_image = main_cfg.get<bool>("ShowImage",false);

    if(_show_image)
      _viewer.Configure(main_cfg.get_pset(_viewer.Name()));
    
    std::vector<std::string> cluster_instance_type_v = main_cfg.get<std::vector<std::string> >("ClusterAlgoType");
    std::vector<std::string> cluster_instance_name_v = main_cfg.get<std::vector<std::string> >("ClusterAlgoName");

    if(cluster_instance_type_v.size() != cluster_instance_name_v.size()) {
      LARCV_CRITICAL((*this)) << "Clustering: AlgoType and AlgoName config parameters have different length! "
			      << "(" << cluster_instance_type_v.size() << " vs. " << cluster_instance_name_v.size() << ")" << std::endl;
      throw larbys();
    }

    std::vector<std::string> match_instance_type_v = main_cfg.get<std::vector<std::string> >("MatchAlgoType");
    std::vector<std::string> match_instance_name_v = main_cfg.get<std::vector<std::string> >("MatchAlgoName");

    if(match_instance_type_v.size() != match_instance_name_v.size()) {
      LARCV_CRITICAL((*this)) << "Matching: AlgoType and AlgoName config parameters have different length! "
			      << "(" << match_instance_type_v.size() << " vs. " << match_instance_name_v.size() << ")" << std::endl;
      throw larbys();
    }

    std::vector<std::string> recluster_instance_type_v = main_cfg.get<std::vector<std::string> >("ReClusterAlgoType");
    std::vector<std::string> recluster_instance_name_v = main_cfg.get<std::vector<std::string> >("ReClusterAlgoName");

    if(recluster_instance_type_v.size() != recluster_instance_name_v.size()) {
      LARCV_CRITICAL((*this)) << "Re-Clustering: AlgoType and AlgoName config parameters have different length! "
			      << "(" << recluster_instance_type_v.size() << " vs. " << recluster_instance_name_v.size() << ")" << std::endl;
      throw larbys();
    }

    _cluster_alg_v.clear();
    _cluster_alg_m.clear();
    for(size_t i=0; i<cluster_instance_type_v.size(); ++i) {
      auto const& name = cluster_instance_name_v[i];
      auto const& type = cluster_instance_type_v[i];
      if(_cluster_alg_m.find(name) != _cluster_alg_m.end()) {
	LARCV_CRITICAL((*this)) << "Duplicate algorithm name found!" << std::endl;
	throw larbys("Duplicate algorithm name found!");
      }
      
      _cluster_alg_m[name] = _cluster_alg_v.size();
      _cluster_alg_v.push_back(ClusterAlgoFactory::get().create(type,name));
    }

    for(auto& ptr : _cluster_alg_v) {
      ptr->Profile(_profile);
      ptr->set_verbosity(this->logger().level());
      ptr->Configure(main_cfg.get_pset(ptr->Name()));
    }

    auto const match_alg_type = main_cfg.get<std::string>("MatchAlgoType","");
    auto const match_alg_name = main_cfg.get<std::string>("MatchAlgoName","");
    if(!match_alg_type.empty()) {
      _match_alg = MatchAlgoFactory::get().create(match_alg_type,match_alg_name);
      _match_alg->Profile(_profile);
      _match_alg->set_verbosity(this->logger().level());
      _match_alg->Configure(main_cfg.get<fcllite::PSet>(_match_alg->Name()));
    }

    auto const recluster_alg_type = main_cfg.get<std::string>("ReClusterAlgoType","");
    auto const recluster_alg_name = main_cfg.get<std::string>("ReClusterAlgoName","");
    if(!recluster_alg_type.empty()) {
      _recluster_alg = ReClusterAlgoFactory::get().create(recluster_alg_type,recluster_alg_name);
      _recluster_alg->Profile(_profile);
      _recluster_alg->set_verbosity(this->logger().level());
      _recluster_alg->Configure(main_cfg.get<fcllite::PSet>(_recluster_alg->Name()));
    }

    _configured=true;
    LARCV_DEBUG((*this)) << "end" << std::endl;
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

  void ImageClusterManager::Add(const ::cv::Mat& img, const ImageMeta& meta)
  {
    _raw_img_v.push_back(img);
    _raw_meta_v.push_back(meta);
  }
  
  void ImageClusterManager::Process()
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    
    if(!_configured) throw larbys("Must Configure() before Process()!");
    
    _watch.Start();

    _book_keeper.Reset();
    _meta_v_v.clear();
    _meta_v_v.resize(_cluster_alg_v.size());
    
    _clusters_v_v.clear();
    _clusters_v_v.resize(_cluster_alg_v.size());
    
    for(size_t img_index=0; img_index<_raw_img_v.size(); ++img_index) {

      auto const& meta = _raw_meta_v[img_index];
      auto const& img  = _raw_img_v[img_index];

      if(meta.num_pixel_row()!=img.rows)
	throw larbys("Provided metadata has incorrect # horizontal pixels w.r.t. image!");
      
      if(meta.num_pixel_column()!=img.cols)
	throw larbys("Provided metadata has incorrect # vertical pixels w.r.t. image!");

      //
      // First-pass clustering
      //
      
      for(size_t alg_index=0; alg_index<_cluster_alg_v.size(); ++alg_index) {

	auto& alg_ptr    = _cluster_alg_v[alg_index];
	auto& meta_v     = _meta_v_v[alg_index];
	auto& clusters_v = _clusters_v_v[alg_index];
	
	if(!alg_ptr) throw larbys("Invalid algorithm pointer!");
	
	if(alg_ptr == _cluster_alg_v.front()) {
	  
	  Cluster2DArray_t clusters;
	  meta_v.push_back(meta);
	  clusters_v.emplace_back(alg_ptr->Process(clusters,img,meta_v.back()));
	  
	}else{

	  auto const& prev_clusters = _clusters_v_v[alg_index-1][img_index];
	  auto prev_meta = _meta_v_v[alg_index-1][img_index];
	  clusters_v.emplace_back(alg_ptr->Process(prev_clusters,img,prev_meta));
	  meta_v.push_back(prev_meta);
	  
	}
	
	// Assign cluster IDs
	ClusterID_t offset=0;

	for(size_t clusters_index=0; (clusters_index+1)<clusters_v.size(); ++clusters_index)
	  
	  offset += clusters_v[clusters_index].size();

	for(size_t cluster_index=0; cluster_index < clusters_v.back().size(); ++cluster_index) {
	  
	  auto& c = clusters_v.back()[cluster_index];
	  c._cluster_id = offset + cluster_index;
	  c._image_id = img_index;
	  c._plane_id = meta.plane();
	}
	
	// Sanity check on meta data
	auto const& result_meta = meta_v.back();
	if(result_meta.height() > meta.height()) {
	  std::cerr << "Return meta data by " << alg_ptr->Name() << " exceeds original image height!" << std::endl;
	  throw larbys();
	}
	if(result_meta.width() > meta.width()) {
	  std::cerr << "Return meta data by " << alg_ptr->Name() << " exceeds original image width!" << std::endl;
	  throw larbys();
	}
      }
    }

    //
    // Run matching
    //
    if(_match_alg) {

      auto const& clusters_v = _clusters_v_v.back();
      
      std::vector< std::vector< const larcv::Cluster2D* > > c_per_plane;

      std::vector< std::vector< const larcv::ImageMeta* > > meta_per_plane;

      for(size_t img_id=0; img_id<clusters_v.size(); ++img_id) {

	auto const& meta = _meta_v_v.back()[img_id];
	auto const plane = meta.plane();

	if(c_per_plane.size() <= plane) {
	  c_per_plane.resize(plane+1);
	  meta_per_plane.resize(plane+1);
	}

	for(auto const& c : clusters_v[img_id]) {
	  c_per_plane[plane].push_back((const larcv::Cluster2D*)(&c));
	  meta_per_plane[plane].push_back((const larcv::ImageMeta*)(&meta));
	}
	
      }

      std::vector<size_t> seed; seed.reserve(c_per_plane.size());
      for(auto const& c : c_per_plane) seed.push_back(c.size());

      auto comb_v = PlaneClusterCombinations(seed);

      for(auto const& comb : comb_v) {

	//Assemble a vector of clusters
	std::vector<const larcv::Cluster2D*> input_clusters;
	std::vector<unsigned int> tmp_index_v;
	input_clusters.reserve(comb.size());
	tmp_index_v.reserve(comb.size());
	
	for(auto const& cinfo : comb) {

	  auto const& plane = cinfo.first;
	  auto const& index = cinfo.second;

	  input_clusters.push_back(c_per_plane[plane][index]);
	  tmp_index_v.push_back(c_per_plane[plane][index]->ClusterID());
	}
	
	auto score = _match_alg->Process(input_clusters);

	if(score>0)

	  _book_keeper.Match(tmp_index_v,score);

      }
    }
    
    _process_time += _watch.WallTime();
    ++_process_count;

    if(_show_image) {
      std::vector<std::string> window_name_v(_cluster_alg_v.size());
      for(size_t i=0; i<_cluster_alg_v.size(); ++i) window_name_v[i] = _cluster_alg_v[i]->Name();
      //ContourArray_t contours_v;
      //contours_v.reserve(_clusters_v.size());
      //for(auto const& c : _clusters_v) contours_v.push_back(c._contour);
      //_viewer.Display(img,contours_v,window_name_v);
    }

    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  size_t ImageClusterManager::NumClusters(const AlgorithmID_t alg_id) const
  {
    AlgorithmID_t target_alg_id = (alg_id != kINVALID_ALGO_ID ? alg_id : _clusters_v_v.size()-1);
    if(target_alg_id >= _clusters_v_v.size()) throw larbys("Invalid algorithm ID requested");

    auto& clusters_v = _clusters_v_v[target_alg_id];

    if(clusters_v.empty()) return 0; // No image is registered

    for(size_t i=(clusters_v.size()-1); i>=0; --i) {

      if(clusters_v[i].empty()) continue;

      return clusters_v[i].back().ClusterID() + 1;

    }
    
    return 0;
  }
  
  const ImageMeta& ImageClusterManager::MetaData(const ImageID_t img_id, const AlgorithmID_t alg_id) const
  {
    if(alg_id >= _meta_v_v.size()) throw larbys("Invalid algorithm ID requested");
    if(img_id >= _meta_v_v[alg_id].size()) throw larbys("Invalid image ID requested");

    return _meta_v_v[alg_id][img_id];
  }
  
  const Cluster2D& ImageClusterManager::Cluster(const ClusterID_t cluster_id, const AlgorithmID_t alg_id) const
  {
    AlgorithmID_t target_alg_id = (alg_id != kINVALID_ALGO_ID ? alg_id : _clusters_v_v.size()-1);

    if(target_alg_id >= _meta_v_v.size()) throw larbys("Invalid algorithm ID requested");

    auto const& clusters_v = _clusters_v_v[target_alg_id];

    ClusterID_t current_id = 0;
    for(size_t cluster_index=0; cluster_index < clusters_v.size(); ++cluster_index) {

      auto const& clusters = clusters_v[cluster_index];
      
      if(current_id + clusters.size() < cluster_id) {
	current_id += clusters.size();
	continue;
      }

      return clusters[cluster_id - current_id];
    }

    throw larbys("Invalid cluster ID requested!");
  }
  
  const Cluster2DArray_t& ImageClusterManager::Clusters(const ImageID_t img_id,const AlgorithmID_t alg_id) const
  {
    AlgorithmID_t target_alg_id = (alg_id != kINVALID_ALGO_ID ? alg_id : _clusters_v_v.size()-1);
        
    if(target_alg_id >= _meta_v_v.size()) throw larbys("Invalid algorithm ID requested");

    auto const& clusters_v = _clusters_v_v[target_alg_id];

    if(img_id >= clusters_v.size()) throw larbys("Invalid image ID requested");
    
    return clusters_v[img_id];
  }
  
  ClusterID_t ImageClusterManager::ClusterID(const double x, const double y, AlgorithmID_t alg_id) const
  {

    ClusterID_t result = kINVALID_CLUSTER_ID;

    if(_clusters_v_v.empty()) return result;

    if(alg_id == kINVALID_ALGO_ID) alg_id = _clusters_v_v.size() - 1;

    auto const& clusters_v = _clusters_v_v[alg_id];
    
    auto const& meta_v = _meta_v_v[alg_id];

    ClusterID_t offset=0;

    for(size_t img_index=0; img_index < clusters_v.size(); ++img_index) {

      auto const& clusters = clusters_v[img_index];
      auto const& meta = meta_v[img_index];
    
      auto const& origin = meta.origin();
    
      if(x < origin.x || x > (origin.x + meta.width())) {
	offset += clusters.size();
	continue;
      }
      
      if(y < origin.y || x > (origin.y + meta.height())) {
	offset += clusters.size();
	continue;
      }
    
      //std::cout<<"Inspecting a point "<<x<<" : "<<y<<" ... ";
    
      auto pt = ::cv::Point2d((y-origin.y)/meta.pixel_height(), (x-origin.x)/meta.pixel_width()); 

      //std::cout<<pt.x<<" : "<<pt.y<<std::endl;
      
      for(size_t id=0; id<clusters.size(); ++id) {
	
	auto const& c = clusters[id];
      
	double inside = ::cv::pointPolygonTest(c._contour,pt,false);
      
	if(inside < 0) continue;
      
	result = id + offset;
	
	break;
      }

      if(result != kINVALID_CLUSTER_ID) break;
    }
    
    return result;
  }

  void ImageClusterManager::Test()
  {
    std::vector<size_t> seed_v;
    seed_v.push_back(3);
    seed_v.push_back(3);
    seed_v.push_back(3);
    auto result = PlaneClusterCombinations(seed_v);
    for(size_t i=0;i<result.size(); ++i) {
      std::cout<<"Index: "<<i<<" ";
      for(size_t j=0; j<result[i].size(); ++j)
	std::cout<<"("<<result[i][j].first<<","<<result[i][j].second<<") ";
      std::cout<<std::endl;
    }
  }
}
#endif
