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
#include "ImageCluster/ImageClusterManager.h"

namespace larlite {

  /**
     \class LArImageClusterBase
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageClusterBase : public ana_base {
  
  public:

    /// Default constructor
    LArImageClusterBase();

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

    void set_producer(const std::string producer="")
    { _producer = producer; }

    larcv::ImageClusterManager& algo_manager(size_t plane_id);

    const std::vector<larcv::ImageClusterManager>& algo_manager_set() const;

    void set_config(const std::string fname)
    { _config_file = fname; }

    void store_original_image(bool store=true) { _store_original_img=store; }

  private:

    std::string _config_file;
    std::vector<larcv::ImageClusterManager> _alg_mgr_v;

  protected:
    virtual void store_clusters(storage_manager* storage) const = 0;
    virtual void extract_image(storage_manager* storage) = 0;
    ::larcv::ImageManager _img_mgr;
    ::larcv::ImageManager _orig_img_mgr;
    std::string _producer;
    bool _store_original_img;
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
