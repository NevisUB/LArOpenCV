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

#ifndef __LARLITE_LARIMAGEHIT_H__
#define __LARLITE_LARIMAGEHIT_H__

#include "Analysis/ana_base.h"
#include "Core/ImageManager.h"
#include "App/LArImageClusterBase.h"
#include "Core/LArOCVTypes.h"
#include "Core/laropencv_base.h"

namespace larlite {

  /**
     \class LArImageHit
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageHit : public LArImageClusterBase, public ::larocv::laropencv_base {
  
  public:

    /// Default constructor
    LArImageHit(const std::string name="LArImageHit")
      : LArImageClusterBase(name)
      , _charge_to_gray_scale(10)
      , _charge_threshold(5)
      , _pool_time_tick(0)
      , _use_roi(false)
      , _roi_buffer_w(70.)
      , _roi_buffer_t(70.)
    {}

    /// Default destructor
    virtual ~LArImageHit(){}

  protected:

    void extract_image(storage_manager* storage);

    void _Report_() const;

    void _Configure_(const ::fcllite::PSet &pset);

    void AnalyzeImageCluster(storage_manager* storage) {}
    
  private:
    double _charge_to_gray_scale;
    double _charge_threshold;
    int    _pool_time_tick;
    bool   _use_roi;

    bool _use_shower_roi;
    std::string _roi_producer;

    float _roi_buffer_w;
    float _roi_buffer_t;

    std::string _vtx_producer;

    bool _make_roi;
    
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







