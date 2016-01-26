#ifndef LARLITE_LARIMAGECLUSTERBASE_CXX
#define LARLITE_LARIMAGECLUSTERBASE_CXX

#include "LArImageClusterBase.h"
#include "LArUtil/Geometry.h"

namespace larlite {

  LArImageClusterBase::LArImageClusterBase() : ana_base()
  {
    _name="LArImageMaker";
    _fout=0;
    _producer="";
    _config_file="";
    _alg_mgr_v.resize( ::larutil::Geometry::GetME()->Nplanes() );
    _store_original_img=false;
  }

  const std::vector<larcv::ImageClusterManager>& LArImageClusterBase::algo_manager_set() const
  { return _alg_mgr_v; }
  
  larcv::ImageClusterManager& LArImageClusterBase::algo_manager(size_t plane_id)
  {
    if(plane_id >= _alg_mgr_v.size()) throw ::larcv::larbys("ImageClusterManager not found (invalid plane ID)!");
    return _alg_mgr_v[plane_id];
  }

  bool LArImageClusterBase::initialize() {

    if(_producer.empty()) throw ::larcv::larbys("No producer specified...");

    for(auto& mgr : _alg_mgr_v) mgr.Configure(_config_file);

    return true;
  }
  
  bool LArImageClusterBase::analyze(storage_manager* storage) {

    _img_mgr.clear();
    _orig_img_mgr.clear();

    this->extract_image(storage);

    if(_store_original_img) {
      for(size_t plane=0; plane<_img_mgr.size(); ++plane) {
	::cv::Mat img;
	_img_mgr.img_at(plane).copyTo(img);
	_orig_img_mgr.push_back(img,_img_mgr.meta_at(plane));
      }
    }

    if(_img_mgr.size() != _alg_mgr_v.size()) throw ::larcv::larbys("# of imaged created != # of planes!");

    for(size_t plane = 0; plane < _alg_mgr_v.size(); ++plane) {

      auto& alg_mgr = _alg_mgr_v[plane];
      auto const& img = _img_mgr.img_at(plane);
      auto const& meta = _img_mgr.meta_at(plane);
      alg_mgr.Process(img,meta);
      
    }
      
    this->store_clusters(storage);

    return true;
  }

  bool LArImageClusterBase::finalize() {
    
    return true;
  }

}
#endif
