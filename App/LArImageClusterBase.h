/**
 * \file LArImageClusterBase.h
 *
 * \ingroup Play
 * 
 * \brief Class def header for a class LArImageClusterBase
 *
 * @author kazuhiro
 */

/** \addtogroup Play

    @{*/

#ifndef __LARLITE_LARIMAGECLUSTERBASE_H__
#define __LARLITE_LARIMAGECLUSTERBASE_H__

#include "Analysis/ana_base.h"
#include "Core/ImageManager.h"
#include "ImageCluster/Base/ImageClusterManager.h"
#include "ImageCluster/Base/ImageClusterViewer.h"

namespace larlite {

  /**
     \class LArImageClusterBase
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageClusterBase : public ana_base{
  
  public:

    /// Default constructor
    LArImageClusterBase(const std::string name="LArImageClusterBase");

    /// Default destructor
    virtual ~LArImageClusterBase(){}

    /** IMPLEMENT in LArImageClusterBase.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    bool initialize();

    /** IMPLEMENT in LArImageClusterBase.cc! 
        Analyze a data event-by-event  
    */
    bool analyze(storage_manager* storage);

    /** IMPLEMENT in LArImageClusterBase.cc! 
        Finalize method to be called after all events processed.
    */
    bool finalize();

    larocv::ImageClusterManager& algo_manager();

    const std::string& producer() const { return _producer; }

    void set_config(const std::string cfg_file);

  private:

    ::larocv::ImageClusterManager _alg_mgr;
    ::larocv::ImageManager _orig_img_mgr;

    bool _store_original_img;
    std::string _producer;
    bool _profile;
    std::string _config_file;
    double _process_count;
    double _process_time_image_extraction;
    double _process_time_analyze;
    double _process_time_cluster_storage;
    void Report() const;

  protected:

    virtual void _Report_() const = 0;
    virtual void _Configure_(const ::fcllite::PSet&) = 0;
    void store_clusters(storage_manager* storage);
    virtual void extract_image(storage_manager* storage) = 0;
    virtual void AnalyzeImageCluster(storage_manager* storage) = 0;
    ::larocv::ImageManager _img_mgr;

    size_t _num_stored;
    size_t _num_clusters;
    size_t _num_unclustered_hits;
    size_t _num_clustered_hits;
    
    bool _store_contours;
    bool _debug;

    std::string _vtx_producer;
    std::vector<float> _plane_weights;

    int _event ;
    bool _hit_removal ;
    bool _chain_modules ;

  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
