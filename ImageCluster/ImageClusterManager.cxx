#ifndef __IMAGECLUSTERMANAGER_CXX__
#define __IMAGECLUSTERMANAGER_CXX__

#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "ImageClusterManager.h"
#include "Core/larbys.h"
#include "ImageClusterFactory.h"
#include "Utilities.h"
namespace larcv {

  ImageClusterManager::ImageClusterManager(const std::string name)
    : laropencv_base(name)
    , _name(name)
    , _configured(false)
    , _profile(true)
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    Reset();
    LARCV_DEBUG((*this)) << "end" << std::endl;
  }
  
  void ImageClusterManager::Reset()
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    _configured = false;
    _alg_v.clear();
    _clusters_v.clear();
    _process_count=0;
    _process_time=0;
    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  ImageClusterBase* ImageClusterManager::GetAlg(const AlgorithmID_t id) const
  {
    if(id >= _alg_v.size()) throw larbys("Invalid algorithm ID requested...");
    return _alg_v[id];
  }

  ImageClusterBase* ImageClusterManager::GetAlg(const std::string name) const
  {
    return GetAlg(GetAlgID(name));
  }

  AlgorithmID_t ImageClusterManager::GetAlgID(const std::string name) const
  {
    auto iter = _alg_m.find(name);
    if(iter == _alg_m.end()) return kINVALID_ALGO_ID;
    return (*iter).second;
  }

  /*
  AlgorithmID_t ImageClusterManager::AddAlg(ImageClusterBase* alg)
  {
    if(!alg) throw larbys("Cannot add nullptr!");
    if(_configured) throw larbys("Cannot add algo after configured! (call Reset())");
    _alg_v.push_back(alg);
    return (_alg_v.size() - 1);
  }
  */
  
  void ImageClusterManager::Configure(const ::fcllite::PSet& main_cfg)
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    _profile = main_cfg.get<bool>("Profile");

    this->set_verbosity((msg::Level_t)(main_cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    _show_image = main_cfg.get<bool>("ShowImage",false);

    std::vector<std::string> instance_type_v = main_cfg.get<std::vector<std::string> >("AlgoType");
    std::vector<std::string> instance_name_v = main_cfg.get<std::vector<std::string> >("AlgoName");

    if(instance_type_v.size() != instance_name_v.size()) {
      LARCV_CRITICAL((*this)) << "AlgoType and AlgoName config parameters have different length! "
			      << "(" << instance_type_v.size() << " vs. " << instance_name_v.size() << ")" << std::endl;
      throw larbys();
    }

    _alg_v.clear();
    _alg_m.clear();
    for(size_t i=0; i<instance_type_v.size(); ++i) {
      auto const& name = instance_name_v[i];
      auto const& type = instance_type_v[i];
      if(_alg_m.find(name) != _alg_m.end()) {
	LARCV_CRITICAL((*this)) << "Duplicate algorithm name found!" << std::endl;
	throw larbys("Duplicate algorithm name found!");
      }
      
      _alg_m[name] = _alg_v.size();
      _alg_v.push_back(ImageClusterFactory::get().create(type,name));
    }

    for(auto& ptr : _alg_v) {
      ptr->Profile(_profile);
      ptr->set_verbosity(this->logger().level());
      ptr->Configure(main_cfg.get_pset(ptr->Name()));
    }
    _configured=true;
    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  void ImageClusterManager::Report() const
  {
    
    std::cout << "  ================== " << Name() << " Profile Report ==================" << std::endl
	      << "  # Process call = " << _process_count << " ... Total time = " << _process_time << " [s]" << std::endl;
    for(auto const& ptr : _alg_v) {
      std::cout << Form("  \033[93m%-20s\033[00m ... # call %-5zu ... total time %g [s] ... average time %g [s/process]",
			ptr->Name().c_str(), ptr->ProcessCount(), ptr->ProcessTime(), ptr->ProcessTime() / (double)(ptr->ProcessCount()))
		<< std::endl;
    }
    
  }

  void ImageClusterManager::Process(const ::cv::Mat& img, const ImageMeta& meta)
  {
    LARCV_DEBUG((*this)) << "start" << std::endl;
    
    if(!_configured) throw larbys("Must Configure() before Process()!");

    if(meta.num_pixel_row()!=img.rows)
      throw larbys("Provided metadata has incorrect # horizontal pixels w.r.t. image!");

    if(meta.num_pixel_column()!=img.cols)
      throw larbys("Provided metadata has incorrect # vertical pixels w.r.t. image!");

    _watch.Start();
    
    _orig_meta = meta;
    _meta_v.clear();
    _clusters_v.clear();
    _clusters_v.reserve(_alg_v.size());
    _meta_v.reserve(_alg_v.size());

    std::vector<cv::Mat> result_image_v;
    const size_t imshow_rows=800;
    const size_t imshow_cols=800;
    size_t imshow_xmin=1e12;
    size_t imshow_ymin=1e12;
    size_t imshow_xmax=0;
    size_t imshow_ymax=0;
    if(_show_image) {
      result_image_v.reserve(_alg_v.size()+1);
      //std::cout<<imshow_row_scale<<" : "<<imshow_col_scale<<std::endl;
      ::cv::Mat orig_image;
      ::cv::cvtColor(img,orig_image,CV_GRAY2RGB);
      result_image_v.push_back(orig_image);
    }

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

      if(_show_image) {
	/// Draw contours
	::cv::RNG rng;
	::cv::Mat result_image;
	::cv::cvtColor(img,result_image,CV_GRAY2RGB);
	auto const& contours = _clusters_v.back();
	for( size_t cindex = 0; cindex< contours.size(); cindex++ ) {
	  ::cv::Scalar color = ::cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
	  drawContours( result_image, contours, cindex, color, 2, 8, ::cv::noArray(), 0, ::cv::Point() );
	  for(auto const& contour : contours) {
	    ::cv::Rect bb = ::cv::boundingRect( ::cv::Mat(contour) );
	    std::cout<<bb.x<<" => "<<bb.x+bb.width<<" : "<<bb.y<<" => "<<bb.y+bb.height<<std::endl;
	    if(bb.x < imshow_xmin) imshow_xmin=bb.x;
	    if(bb.y < imshow_ymin) imshow_ymin=bb.y;
	    if(bb.x+bb.width > imshow_xmax) imshow_xmax=bb.x + bb.width;
	    if(bb.y+bb.height > imshow_ymax) imshow_ymax=bb.y + bb.height;
	  }
	}
	result_image_v.emplace_back(result_image);
      }
    }

    if(_show_image) {
      std::vector<std::string> window_name_v;
      window_name_v.push_back("Original Image");
      for(size_t alg_index=0; alg_index<_alg_v.size(); ++alg_index) 
	window_name_v.push_back(_alg_v[alg_index]->Name() + " Clusters (" + std::to_string(alg_index) + ")");

      larcv::Contour_t bb;
      bb.push_back(::cv::Point(0,0));
      bb.push_back(::cv::Point(0,img.cols));
      bb.push_back(::cv::Point(img.rows,img.cols));
      bb.push_back(::cv::Point(img.rows,0));
      if(imshow_xmax) {
	bb[0].x = bb[1].x = imshow_xmin;
	bb[0].y = bb[3].y = imshow_ymin;
	bb[2].x = bb[3].x = imshow_xmax;
	bb[1].y = bb[2].y = imshow_ymax;
      }
      std::cout<<img.cols<<" : "<<img.rows<<" ... "<<bb[0].x<<" : "<<bb[0].y<<" ... "<<bb[2].x<<" : "<<bb[2].y<<std::endl;
      /// Show in a window
      for(size_t img_index=0; img_index<result_image_v.size(); ++img_index) {
	auto& img = result_image_v[img_index];
	if(img_index) img = CreateSubMatRef(bb,img);
	const size_t imshow_width  = (img.rows > imshow_rows ? imshow_rows : img.rows);
	const size_t imshow_height = (img.cols > imshow_rows ? imshow_rows : img.cols);
	::cv::resize(img,img,::cv::Size(imshow_width,imshow_height),0,0,::cv::INTER_AREA);
	auto const& window_name = window_name_v[img_index];
	::cv::namedWindow(window_name.c_str(), CV_WINDOW_NORMAL);
	::cv::imshow(window_name.c_str(), img);
	//::cv::resizeWindow(window_name.c_str(), 800, 600);
      }

      cvWaitKey(0);

      for(size_t img_index=0; img_index<result_image_v.size(); ++img_index) {
	auto const& window_name = window_name_v[img_index];
	cvDestroyWindow(window_name.c_str());
      }
      window_name_v.clear();
      result_image_v.clear();
    }
    _process_time += _watch.WallTime();
    ++_process_count;
    LARCV_DEBUG((*this)) << "end" << std::endl;
  }
  
  const ImageMeta& ImageClusterManager::MetaData(const AlgorithmID_t alg_id) const
  {
    if(alg_id < _meta_v.size()) return _meta_v[alg_id];
    if(alg_id >= _meta_v.size()) throw larbys("Invalid algorithm ID requested");
    throw larbys("Execution of an algorithm not yet done!");
  }
  
  const Contour_t& ImageClusterManager::Cluster(const ClusterID_t cluster_id, const AlgorithmID_t alg_id) const
  {
    auto const& clusters = Clusters(alg_id);
    if(cluster_id >= clusters.size()) throw larbys("Invalid cluster ID requested");
    return clusters[cluster_id];
  }
  
  const ContourArray_t& ImageClusterManager::Clusters(const AlgorithmID_t alg_id) const
  {
    if(alg_id < _clusters_v.size()) return _clusters_v[alg_id];
    if(alg_id == kINVALID_ALGO_ID && _clusters_v.size()) return _clusters_v.back();
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

    //std::cout<<"Inspecting a point "<<x<<" : "<<y<<" ... ";

    auto pt = ::cv::Point2d((x-origin.x)/meta.pixel_width(),(y-origin.y)/meta.pixel_height());

    //std::cout<<pt.x<<" : "<<pt.y<<std::endl;

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
