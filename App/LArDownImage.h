/**
 * \file LArDownImage.h
 *
 * \ingroup Play
 * 
 * \brief Class def header for a class LArDownImage
 *
 * @author Ariana Hackenburg 
 */

/** \addtogroup Play

    @{*/

#ifndef __LARLITE_LARDOWNIMAGE_H__
#define __LARLITE_LARDOWNIMAGE_H__

#include "Analysis/ana_base.h"
#include "Core/ImageManager.h"
#include "App/LArImageClusterBase.h"
#include "opencv2/core/mat.hpp"

namespace larlite {

  /**
     \class LArDownImage
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArDownImage : public LArImageClusterBase {
  
  public:

    /// Default constructor
    LArDownImage(const std::string name="LArDownImage")
      : LArImageClusterBase(name)
      , _num_stored(0)
      , _charge_to_gray_scale(10)
      , _charge_threshold(5)
      , _nbins_x(200)
      , _nbins_y(200)
      , _num_clusters_v()
      , _num_unclustered_hits_v()
      , _num_clustered_hits_v()
    {}

    /// Default destructor
    virtual ~LArDownImage(){}

  protected:

    void store_clusters(storage_manager* storage);
    
    void extract_image(storage_manager* storage);

    void _Report_() const;

    void _Configure_(const ::fcllite::PSet &pset);

  private:
    size_t _num_stored;
    double _charge_to_gray_scale;
    double _charge_threshold;
    int    _nbins_x;
    int    _nbins_y;
    std::vector<size_t> _num_clusters_v;
    std::vector<size_t> _num_unclustered_hits_v;
    std::vector<size_t> _num_clustered_hits_v;
    
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
