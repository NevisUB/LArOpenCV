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
#include "ClusterAlgoBase.h"
#include "MatchAlgoBase.h"
#include "ReClusterAlgoBase.h"
#include "ImageClusterViewer.h"
#include "MatchBookKeeper.h"
#include "AlgoDataManager.h"
#include <TTree.h>
namespace larocv {
  /**
     \class ImageClusterManager
     @brief Class to execute a chain of algorithms to a given image input.
     larocv::ImageClusterManager executes a chain of algorithms (ImageClusterBase inherit class instances) \n
     to a provided cv::Mat image object. Each algorithm receives an image, clusters from a previous algorithm \n
     (for the 1st algorithm this is empty), and a meta data that needs to be modified by each algorithm to interpret \n
     the returning clusters' coordinate. After execution of algorithms it stores resulting clusters (many per algorithm) \n
     and a meta data (one per algorithm) that are accessible via unique id (larocv::AlgorithmID_t and larocv::ClusterID_t).\n
     At the execution of algorithms, a user must provide both image (cv::Mat) and image meta data (larocv::ImageMeta).\n
     Each cluster is represented as a 2D contour (larocv::Contour_t), and the class provides a utility method to identify \n
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

    void Test();

    /// Name accessor, used to identify a block of configuration parameters via fhicl
    const std::string& Name() const { return _name; }
    /// Clears attributes except for _name
    void Reset();
    /// Clears data
    void ClearData();
    /// Algorithm getter via unique identifier (AlgorithmID_t)
    const ClusterAlgoBase& GetClusterAlg(const AlgorithmID_t id) const;
    /// Algorithm getter via unique identifier (string name)
    const ClusterAlgoBase& GetClusterAlg(const std::string name) const;
    /// Clustering algorithm ID getter via unique identifier (string name)
    AlgorithmID_t GetClusterAlgID(const std::string name) const;
    /// Matching agorithm getter
    const MatchAlgoBase& GetMatchAlg() const;
    /// ReClustering algorithm getter
    const ReClusterAlgoBase& GetReClusterAlg() const;
    /// Read-in configuration object & enforce configurations to algorithms
    void Configure(const ::fcllite::PSet& main_cfg);
    /// Execute algorithms to construct clusters + corresponding meta data
    // void Add(const ::cv::Mat& img, const larocv::ImageMeta& meta);
    /// Execute algorithms to construct clusters + corresponding meta data + roi data
    void Add(const ::cv::Mat& img, const larocv::ImageMeta& meta, const larocv::ROI& roi);
    /// Execute algorithms to construct clusters + corresponding meta data
    void Process();
    /// Finalize after multiple Process call
    void Finalize(TFile* file=nullptr);
    /// Accessor to total number of clusters
    size_t NumClusters(const AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// Accessor to a specific meta data constructed by an algorithm (via algorithm id)
    const ImageMeta& MetaData(const ImageID_t img_id, const AlgorithmID_t alg_id) const;
    /// Accessor to a specific roi data constructed by an algorithm (via algorithm id)
    const ROI& ROIData(const ImageID_t img_id, const AlgorithmID_t alg_id) const;
    /// Accessor to a specific cluster constructed by an algorithm (via algorithm + cluster id)
    const Cluster2D& Cluster(const ClusterID_t cluster_id, const AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// Accessor to a set of clusters constructed by an algorithm (via algorithm id)
    const Cluster2DArray_t& Clusters(const ImageID_t img_id, const AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// For a specified algorithm, find a cluster that contains coordinate (x,y). By default "last algorithm" is used.
    ClusterID_t ClusterID(const double x, const double y, size_t plane, AlgorithmID_t alg_id=kINVALID_ALGO_ID) const;
    /// Report process summary
    void Report() const;
    /// Match result getter
    const MatchBookKeeper& BookKeeper() const { return _book_keeper; }
    /// Original input image getter
    const std::vector<cv::Mat> InputImages() const { return _raw_img_v; }
    /// Original image metadata getter
    const std::vector<larocv::ImageMeta> InputImageMetas() const { return _raw_meta_v; }
    /// Original image roi getter
    const std::vector<larocv::ROI> InputROIs() const { return _raw_roi_v; }
    /// Plane weights getter and setter
    std::vector<float>& MatchPlaneWeights() { return _match_plane_weights; }
    /// Algorithm data manager accessor
    const AlgoDataManager& DataManager() const { return _algo_dataman; }
  private:
    /// Name identifier: used to fetch a block of configuration parameters
    std::string _name;
    /// Boolean flag to enforce Configure method to be called before Process.
    bool _configured;
    /// Array of clustering algorithms to be executed
    std::vector<larocv::ClusterAlgoBase*> _cluster_alg_v;
    /// Array of matching algorithms to be executed
    larocv::MatchAlgoBase* _match_alg;
    /// Array of re-clustering algorithms to be executed
    larocv::ReClusterAlgoBase* _recluster_alg;
    /// Map of clustering algorithm instance name to ID
    std::map<std::string,larocv::AlgorithmID_t> _cluster_alg_m;
    /// Array of images
    std::vector<cv::Mat> _raw_img_v;
    /// Array of metadata
    std::vector<larocv::ImageMeta> _raw_meta_v;
    /// Array of roidata 
    std::vector<larocv::ROI> _raw_roi_v;
    /// Array of resulting clusters per algorithm per image (outer index = algorithm, inner index = image)
    std::vector<std::vector<larocv::Cluster2DArray_t> > _clusters_v_v;
    /// Array of meta data: one per algorithm per image (outer index = algorithm, inner index = image)
    std::vector<std::vector<larocv::ImageMeta> > _meta_v_v;
    /// Array of roi data: one per algorithm per image (outer index = algorithm, inner index = image)
    std::vector<std::vector<larocv::ROI> > _roi_v_v;
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
    /// Viewer instance
    ImageClusterViewer _viewer;
    /// MatchBookKeeper
    MatchBookKeeper _book_keeper;
    /// MatchPlaneWeights
    std::vector<float> _match_plane_weights;
    /// Switch for dead wire check
    bool _enable_wire_check ;
    /// Algorithm data container
    AlgoDataManager _algo_dataman;
    /// Algorithm data storage TTree
    TTree* _tree;
    int _required_plane;
    bool _use_two_plane;
  };
}
#endif
/** @} */ // end of doxygen group 

