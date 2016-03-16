/**
 * \file yolo_play.h
 *
 * \ingroup YoloAPI
 * 
 * \brief Class def header for a class yolo_play
 *
 * @author kazuhiro
 */

/** \addtogroup YoloAPI

    @{*/
#ifndef __LARCV_YOLOAPI_H__
#define __LARCV_YOLOAPI_H__

#include <string>

namespace larcv {
  /**
     \class yolo_play
  */
  class yolo_play{
    
  public:
    
    /// Default constructor
    yolo_play(){}

    /// Default destructor
    virtual ~yolo_play(){}

    void run(std::string cfg, std::string weights, std::string img_file);
    
  };
}

#endif
/** @} */ // end of doxygen group 

