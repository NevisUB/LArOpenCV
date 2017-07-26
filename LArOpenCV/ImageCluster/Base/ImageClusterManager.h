/**
 * \file ImageClusterManager.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ImageClusterManager
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __IMAGECLUSTERMANAGER_H__
#define __IMAGECLUSTERMANAGER_H__

#include <iostream>
#include "ImageAnaBase.h"
#include "AlgoDataManager.h"
#include <TTree.h>

namespace larocv {
  /**
     \class ImageClusterManager
     @brief Class to execute a chain of algorithms to a given image input.
     ImageClusterManager executes a chain of algorithms (ImageClusterBase inherit class instances) \n
     to a provided cv::Mat image object. Each algorithm receives an image, clusters from a previous algorithm \n
     (for the 1st algorithm this is empty), and a meta data that needs to be modified by each algorithm to interpret \n
     the returning clusters' coordinate. After execution of algorithms it stores resulting clusters (many per algorithm) \n
     and a meta data (one per algorithm) that are accessible via unique id (AlgorithmID_t and ClusterID_t).\n
     At the execution of algorithms, a user must provide both image (cv::Mat) and image meta data (ImageMeta).\n
     Each cluster is represented as a 2D contour (Contour_t), and the class provides a utility method to identify \n
     a cluster that contains a specific 2D point (x,y) in original coordinate system. This is possible via provided \n
     meta data for an original image that contains image origin (left bottom corner) in the original coordinate, the width \n
     and height of an image, etc. + updated meta data from each algorithm execution.
  */
  class ImageClusterManager : public laropencv_base{
    
  public:
    
    /// Default constructor
    ImageClusterManager(const std::string name="ImageClusterManager");
    
    /// Default destructor
    ~ImageClusterManager(){}

    /// Name accessor, used to identify a block of configuration parameters via fhicl
    const std::string& Name() const { return _name; }
    /// Clears attributes except for _name
    void Reset();
    /// Clears data
    void ClearData();
    /// Accessor to a list of algorithms
    std::vector<std::string> GetClusterAlgNames() const;
    /// Algorithm getter via unique identifier (AlgorithmID_t)
    const ImageClusterBase* GetClusterAlg(const AlgorithmID_t id) const;
    /// Algorithm getter for read-write via unique identifier (AlgorithmID_t)
    ImageClusterBase* GetClusterAlgRW(const AlgorithmID_t id);
    /// Algorithm getter via unique identifier (string name)
    const ImageClusterBase* GetClusterAlg(const std::string name) const;
    /// Clustering algorithm ID getter via unique identifier (string name)
    AlgorithmID_t GetClusterAlgID(const std::string name) const;
    /// Read-in configuration object & enforce configurations to algorithms
    void Configure(const Config_t& main_cfg);
    /// Add image data for executing LArOpenCV modules
    void Add(::cv::Mat& img, const ImageMeta& meta, ImageSetID_t set_id = ImageSetID_t::kImageSetUnknown);
    /// Execute algorithms to construct clusters + corresponding meta data
    bool Process();
    /// Finalize after multiple Process call
    void Finalize(TFile* file=nullptr);
    /// Accessor to a specific meta data constructed by an algorithm (via algorithm id)
    const ImageMeta& MetaData(const ImageID_t img_id, const AlgorithmID_t alg_id) const;
    /// Report process summary
    void Report() const;
    /// Original input image getter
    std::vector<cv::Mat>& InputImages(ImageSetID_t set_id=ImageSetID_t::kImageSetUnknown);
    /// Return copy of originals
    std::vector<cv::Mat>& OriginalInputImages(ImageSetID_t set_id=ImageSetID_t::kImageSetUnknown);
    /// Original image metadata getter
    const std::vector<ImageMeta>& InputImageMetas(ImageSetID_t set_id=ImageSetID_t::kImageSetUnknown) const;
    /// Algorithm data manager accessor
    const data::AlgoDataManager& DataManager() const { return _algo_dataman; }
    /// Rune, SubRun, Event, & Entry  Setter
    void SetRSEE(uint run, uint subrun, uint event, uint entry)
    { _algo_dataman._run=run; _algo_dataman._subrun=subrun; _algo_dataman._event=event; _algo_dataman._entry=entry; }
    
  private:
    /// Name identifier: used to fetch a block of configuration parameters
    std::string _name;
    /// Boolean flag to enforce Configure method to be called before Process.
    bool _configured;
    /// Array of clustering algorithms to be executed
    std::vector<ImageClusterBase*> _cluster_alg_v;
    /// Map of clustering algorithm instance name to ID
    std::map<std::string,AlgorithmID_t> _cluster_alg_m;
    /// Vector of algo data storage
    std::vector<AlgorithmID_t> _store_cluster_alg_id_v;
    /// Array of images
    std::vector<std::vector<cv::Mat> > _raw_img_vv;
    /// Copy of originals
    std::vector<std::vector<cv::Mat> > _copy_img_vv;
    /// Array of metadata
    std::vector<std::vector<ImageMeta> > _raw_meta_vv;
    /// Boolean flag to measure process time + report
    bool _profile;
    /// Boolean flag to enable filter mode
    bool _filter;
    /// Stopwatch
    Watch _watch;
    /// Process counter (cumulative)
    size_t _process_count;
    /// Process time (cumulative)
    double _process_time;
    /// Algorithm data container
    data::AlgoDataManager _algo_dataman;
    /// Algorithm data storage TTree
    TTree* _tree;
    
  };
}
#endif
/** @} */ // end of doxygen group 

