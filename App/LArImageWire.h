/**
 * \file LArImageWire.h
 *
 * \ingroup App
 * 
 * \brief Class def header for a class LArImageWire
 *
 * @author kazuhiro
 */

/** \addtogroup App

    @{*/

#ifndef __LARLITE_LARIMAGEWIRE_H__
#define __LARLITE_LARIMAGEWIRE_H__

#include "Analysis/ana_base.h"
#include "Core/ImageManager.h"
#include "LArImageClusterBase.h"

namespace larlite {

  /**
     \class LArImageWire
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageWire : public LArImageClusterBase {
  
  public:

    /// Default constructor
    LArImageWire() : LArImageClusterBase() {}

    /// Default destructor
    virtual ~LArImageWire(){}

  protected:

    void store_clusters(storage_manager* storage) const;

    void extract_image(storage_manager* storage);
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
