#ifndef __IMAGECLUSTERMANAGER_CXX__
#define __IMAGECLUSTERMANAGER_CXX__

#include <opencv2/imgproc/imgproc.hpp>
#include "ImageClusterManager.h"
#include "Core/larbys.h"
#include "FhiclLite/ConfigManager.h"
namespace larcv {

  void ImageClusterManager::Reset()
  {
    _configured = false;
    _alg_v.clear();
    _clusters_v.clear();
  }

  ImageClusterBase* ImageClusterManager::GetAlg(const AlgorithmID_t id) const
  {
    if(id >= _alg_v.size()) throw larbys("Invalid algorithm ID requested...");
    return _alg_v[id];
  }

  AlgorithmID_t ImageClusterManager::AddAlg(ImageClusterBase* alg)
  {
    if(!alg) throw larbys("Cannot add nullptr!");
    if(_configured) throw larbys("Cannot add algo after configured! (call Reset())");
    _alg_v.push_back(alg);
    return (_alg_v.size() - 1);
  }

  void ImageClusterManager::Configure(const std::string cfg_file)
  {
    ::fcllite::ConfigManager cfg_mgr(Name());
    
    cfg_mgr.AddCfgFile(cfg_file);

    auto const& main_cfg = cfg_mgr.Config();

    for(auto& ptr : _alg_v)
      ptr->Configure(main_cfg.get_pset(ptr->Name()));

    _configured=true;
  }

  void ImageClusterManager::Process(const ::cv::Mat& img, const ImageMeta& meta)
  {
    if(!_configured) throw larbys("Must Configure() before Process()!");

    if(meta.num_pixel_row()!=img.rows)
      throw larbys("Provided metadata has incorrect # horizontal pixels w.r.t. image!");

    if(meta.num_pixel_column()!=img.rows)
      throw larbys("Provided metadata has incorrect # vertical pixels w.r.t. image!");

    _orig_meta = meta;
    _meta_v.clear();
    _clusters_v.clear();
    _clusters_v.reserve(_alg_v.size());
    _meta_v.reserve(_alg_v.size());

    for(auto& alg_ptr : _alg_v) {
      
      if(!alg_ptr) throw larbys("Invalid algorithm pointer!");

      if(alg_ptr == _alg_v.front()) {

	ContourArray_t clusters;
	_meta_v.push_back(_orig_meta);
	_clusters_v.emplace_back(alg_ptr->Process(clusters,img,_meta_v.back()));

      }else{

	auto const& clusters = _clusters_v.back();
	_meta_v.push_back(_orig_meta);
	_clusters_v.emplace_back(alg_ptr->Process(clusters,img,_meta_v.back()));

      }
      // Sanity check on meta data
      auto const& result_meta = _meta_v.back();
      if(result_meta.height() > _orig_meta.height()) {
	std::cerr << "Return meta data by " << alg_ptr->Name() << " exceeds original image height!" << std::endl;
	throw larbys();
      }
      if(result_meta.width() > _orig_meta.width()) {
	std::cerr << "Return meta data by " << alg_ptr->Name() << " exceeds original image width!" << std::endl;
	throw larbys();
      }

    }
  }

  const ImageMeta& ImageClusterManager::MetaData(const AlgorithmID_t alg_id) const
  {
    if(alg_id < _meta_v.size()) return _meta_v[alg_id];
    if(alg_id >= _meta_v.size()) throw larbys("Invalid algorithm ID requested");
    throw larbys("Execution of an algorithm not yet done!");
  }
  
  const Contour_t& ImageClusterManager::Cluster(const AlgorithmID_t alg_id, const ClusterID_t cluster_id) const
  {
    auto const& clusters = Clusters(alg_id);
    if(cluster_id >= clusters.size()) throw larbys("Invalid cluster ID requested");
    return clusters[cluster_id];
  }
  
  const ContourArray_t& ImageClusterManager::Clusters(const AlgorithmID_t alg_id) const
  {
    if(alg_id < _clusters_v.size()) return _clusters_v[alg_id];
    if(alg_id >= _alg_v.size()) throw larbys("Invalid algorithm ID requested");
    throw larbys("Execution of an algorithm not yet done!");
  }
  
  ClusterID_t ImageClusterManager::ClusterID(const double x, const double y, AlgorithmID_t alg_id) const
  {

    ClusterID_t result = kINVALID_CLUSTER_ID;

    if(_clusters_v.empty()) return result;

    if(alg_id == kINVALID_ALGO_ID) alg_id = _clusters_v.size() - 1;

    auto const& clusters = _clusters_v[alg_id];

    auto const& meta = _meta_v[alg_id];

    auto const& origin = meta.origin();

    if(x < origin.x || x > (origin.x + meta.width())) return result;

    if(y < origin.y || x > (origin.y + meta.height())) return result;

    auto pt = ::cv::Point2d((x-origin.x)/meta.pixel_width(),(y-origin.y)/meta.pixel_width());

    for(size_t id=0; id<clusters.size(); ++id) {

      auto const& c = clusters[id];
      
      double inside = ::cv::pointPolygonTest(c,pt,false);

      if(inside < 0) continue;

      result = id;

      break;
    }

    return result;
  }
}
#endif
