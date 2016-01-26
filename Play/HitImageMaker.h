/**
 * \file HitImageMaker.h
 *
 * \ingroup Play
 * 
 * \brief Class def header for a class HitImageMaker
 *
 * @author kazuhiro
 */

/** \addtogroup Play

    @{*/

#ifndef LARLITE_HITIMAGEMAKER_H
#define LARLITE_HITIMAGEMAKER_H

#include "Analysis/ana_base.h"
#include "opencv2/core/mat.hpp"
#include "Utils/DataTypes.h"
struct _object;
typedef _object PyObject;

#ifndef __CINT__
#include "Python.h"
#include "numpy/arrayobject.h"
#endif

namespace larlite {

  /**
     \class HitImageMaker
     User custom analysis class made by SHELL_USER_NAME
   */
  class HitImageMaker : public ana_base{
  
  public:

    /// Default constructor
    HitImageMaker();

    /// Default destructor
    virtual ~HitImageMaker(){}


    /** IMPLEMENT in HitImageMaker.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in HitImageMaker.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in HitImageMaker.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    PyObject* GetImage(const size_t plane);
    PyObject* GetCanny(const size_t plane);
    PyObject* GetContour(const size_t plane, const size_t contour_index);
    size_t NumContours(const size_t plane) const;

  protected:
    void init();
    std::vector<::cv::Mat> _mat_v;
    std::vector<::cv::Mat> _canny_v;
    
    std::vector< std::vector<larcv::Point2DArray> >_contour_v;
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
