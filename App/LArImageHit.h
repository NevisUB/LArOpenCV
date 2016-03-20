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
      , _num_stored(0)
      , _charge_to_gray_scale(10)
      , _charge_threshold(5)
      , _run_analyze_image_cluster(false)
    {}

    /// Default destructor
    virtual ~LArImageHit(){}

  protected:

    void store_clusters(storage_manager* storage);
    
    void extract_image(storage_manager* storage);

    void _Report_() const;

    void _Configure_(const ::fcllite::PSet &pset);

    void AnalyzeImageCluster(storage_manager* storage);

  private:
    size_t _num_stored;
    double _charge_to_gray_scale;
    double _charge_threshold;
    // std::vector<size_t> _num_clusters_v;
    // std::vector<size_t> _num_unclustered_hits_v;
    // std::vector<size_t> _num_clustered_hits_v;

    size_t _num_clusters;
    size_t _num_unclustered_hits;
    size_t _num_clustered_hits;

    /// Flag to run the AnalyzeImageCluster function
    bool _run_analyze_image_cluster;
    /// Output file with the plots made by the AnalyzeImageCluster function
    TFile* _plotFile;
    /// Vector of histograms showing the MC-reconstructed start distance on the planes
    std::vector<TH1D*> _vhMCRecoStartDist;
    /// Distance between two 2D points
    inline double dist2D(const larcv::Point2D& a, const larcv::Point2D& b){ return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y)); }  

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
