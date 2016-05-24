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
#include "Core/LArCVTypes.h"
#include "TH1.h"

namespace larlite {

  /**
     \class LArImageHit
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageHit : public LArImageClusterBase {
  
  public:

    /// Default constructor
    LArImageHit(const std::string name="LArImageHit")
      : LArImageClusterBase(name)
      , _charge_to_gray_scale(10)
      , _charge_threshold(5)
      , _pool_time_tick(0)
      , _use_roi(false)
      , _run_analyze_image_cluster(false)
    {}

    /// Default destructor
    virtual ~LArImageHit(){}

  protected:

    void extract_image(storage_manager* storage);

    void _Report_() const;

    void _Configure_(const ::fcllite::PSet &pset);

    void AnalyzeImageCluster(storage_manager* storage);

  private:
    double _charge_to_gray_scale;
    double _charge_threshold;
    int    _pool_time_tick;
    bool   _use_roi;

    int _padw;
    int _padt;

    bool _use_shower_roi;

    std::vector<int> _max_w;
    int _max_t;
    
    std::string _roi_producer;
    /// Flag to run the AnalyzeImageCluster function
    bool _run_analyze_image_cluster;
    /// Output file with the plots made by the AnalyzeImageCluster function
    TFile* _plotFile;
    /// Vector of histograms showing the MC-reconstructed start distance on the planes
    std::vector<TH1D*> _vhMCRecoStartDist;

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
