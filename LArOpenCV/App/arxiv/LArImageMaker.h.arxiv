/**
 * \file LArImageMaker.h
 *
 * \ingroup Play
 * 
 * \brief Class def header for a class LArImageMaker
 *
 * @author kazuhiro
 */

/** \addtogroup Play

    @{*/

#ifndef LARLITE_LARIMAGEMAKER_H
#define LARLITE_LARIMAGEMAKER_H

#include "Analysis/ana_base.h"
#include "Core/ImageManager.h"

namespace larlite {

  /**
     \class LArImageMaker
     User custom analysis class made by SHELL_USER_NAME
   */
  class LArImageMaker : public ana_base {
  
  public:

    /// Default constructor
    LArImageMaker();

    /// Default destructor
    virtual ~LArImageMaker(){}


    /** IMPLEMENT in LArImageMaker.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in LArImageMaker.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in LArImageMaker.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void set_producers(const std::string rawdigit_producer="",
		       const std::string wire_producer="",
		       const std::string hit_producer="")
    {
      _rawdigit_producer = rawdigit_producer;
      _wire_producer = wire_producer;
      _hit_producer = hit_producer;
    }

  protected:
    void init();

    std::vector< ::cv::Mat> _mat_v;

    ::larcv::ImageManager *_rawdigit_img_mgr;
    ::larcv::ImageManager *_wire_img_mgr;
    ::larcv::ImageManager *_hit_img_mgr;

    std::string _rawdigit_producer;
    std::string _wire_producer;
    std::string _hit_producer;

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
