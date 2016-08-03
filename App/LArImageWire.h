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
    LArImageWire(const std::string name="LArImageWire")
      : LArImageClusterBase(name)
      ,_charge_to_gray_scale(0)
      ,_pool_time_tick(0)
      ,_num_stored(0)
      ,_num_clusters(0)
      ,_num_unclustered_hits(0)
      ,_num_clustered_hits(0)
      ,_use_roi(false)
	
    {}

    /// Default destructor
    virtual ~LArImageWire(){}

  protected:

    void _Report_() const {}

    void _Configure_(const ::fcllite::PSet&);

    void store_clusters(storage_manager* storage);

    void extract_image(storage_manager* storage);

    void AnalyzeImageCluster(storage_manager* storage) {}
    
  private:

    double _charge_to_gray_scale;
    int    _pool_time_tick;
    
    size_t _num_stored;
    
    size_t _num_clusters;
    size_t _num_unclustered_hits;
    size_t _num_clustered_hits;

    bool _use_roi;
    
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
