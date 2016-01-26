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
    virtual bool initialize();

    /** IMPLEMENT in LArImageClusterBase.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in LArImageClusterBase.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void set_producer(const std::string producer="")
    { _producer = producer; }

    larcv::ImageClusterManager& AlgoManager() { return _alg_mgr; }

  protected:

    ::larcv::ImageManager _img_mgr;
    ::larcv::ImageClusterManager _alg_mgr;
    std::string _producer;

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
