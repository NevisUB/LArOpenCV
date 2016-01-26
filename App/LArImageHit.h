/**
 * \file LArImageHit.h
 *
 * \ingroup Play
 * 
 * \brief Class def header for a class LArImageHit
 *
 * @author kazuhiro
 */

/** \addtogroup Play

    @{*/

#ifndef LARLITE_LARIMAGEHIT_H
#define LARLITE_LARIMAGEHIT_H

#include "Analysis/ana_base.h"
#include "Core/ImageManager.h"
#include "LArImageClusterBase.h"

namespace larlite {

  /**
     \class LArImageHit
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageHit : public LArImageClusterBase {
  
  public:

    /// Default constructor
    LArImageHit() : LArImageClusterBase(){}

    /// Default destructor
    virtual ~LArImageHit(){}

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
