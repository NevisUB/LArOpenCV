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
#include "ImageClusterBase.h"
namespace larcv {
  /**
     \class ImageClusterManager
     @brief Class to execute a chain of algorithms to a given image input.
     larcv::ImageClusterManager executes a chain of algorithms (ImageClusterBase inherit class instances) \n
     to a provided cv::Mat image object. Each algorithm receives an image, clusters from a previous algorithm \n
     (for the 1st algorithm this is empty), and a meta data that needs to be modified by each algorithm to interpret \n
     the returning clusters' coordinate. After execution of algorithms it stores resulting clusters (many per algorithm) \n
     and a meta data (one per algorithm) that are accessible via unique id (larcv::AlgorithmID_t and larcv::ClusterID_t).\n
     At the execution of algorithms, a user must provide both image (cv::Mat) and image meta data (larcv::ImageMeta).\n
     Each cluster is represented as a 2D contour (larcv::Contour_t), and the class provides a utility method to identify \n
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
    /// Algorithm adder: returns a unique identifier with which you can retrieve back
    //AlgorithmID_t AddAlg(ImageClusterBase* alg); // deprecated: use configuration method + factory
    /// Algorithm getter via unique identifier (AlgorithmID_t)
    ImageClusterBase* GetAlg(const AlgorithmID_t id) const;
    /// Algorithm getter via unique identifier (string name)
    ImageClusterBase* GetAlg(const std::string name) const;
    /// Algorithm ID getter via unique identifier (string name)
    AlgorithmID_t GetAlgID(const std::string name) const;
    /// Read-in configuration object & enforce configurations to algorithms
    void Configure(const ::fcllite::PSet& main_cfg);
    /// Execute algorithms to construct clusters + corresponding meta data
    void Process(const ::cv::Mat& img, const larcv::ImageMeta& meta);
    /// Accessor to a specific meta data constructed by an algorithm (via algorithm id)
    const ImageMeta& MetaData(const AlgorithmID_t alg_id) const;
    /// Accessor to a specific cluster constructed by an algorithm (via algorithm + cluster id)
    const Contour_t& Cluster(const ClusterID_t cluster_id, const AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// Accessor to a set of clusters constructed by an algorithm (via algorithm id)
    const ContourArray_t& Clusters(const AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// For a specified algorithm, find a cluster that contains coordinate (x,y). By default "last algorithm" is used.
    ClusterID_t ClusterID(const double x, const double y, AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// Report process summary
    void Report() const;
    
  private:
    /// Name identifier: used to fetch a block of configuration parameters
    std::string _name;
    /// Boolean flag to enforce Configure method to be called before Process.
    bool _configured;
    /// Array of algorithms to be executed
    std::vector<larcv::ImageClusterBase*> _alg_v;
    /// Map of algorithm instance name to ID
    std::map<std::string,larcv::AlgorithmID_t> _alg_m;
    /// Array of resulting clusters per algorithms
    std::vector<larcv::ContourArray_t> _clusters_v;
    /// Array of meta data: one per set of clusters (updated by each algorithm)
    std::vector<larcv::ImageMeta> _meta_v;
    /// Original meta data
    larcv::ImageMeta _orig_meta;
    /// Boolean flag to measure process time + report
    bool _profile;
    /// Stopwatch
    Watch _watch;
    /// Process counter (cumulative)
    size_t _process_count;
    /// Process time (cumulative)
    double _process_time;
    /// Boolean to stop & show image at the end of process
    bool _show_image;
  };
}
#endif
/** @} */ // end of doxygen group 

