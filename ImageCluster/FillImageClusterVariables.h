/**
 * \file FillImageClusterVariables.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class FillImageClusterVariables
 *
 * @author ah673
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef FILLIMAGECLUSTERVARIABLES_H
#define FILLIMAGECLUSTERVARIABLES_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/core/mat.hpp"
#include "ImageClusterTypes.h"

/**
   \class FillImageClusterVariables
   User defined class FillImageClusterVariables ... these comments are used to generate
   doxygen documentation!
 */

namespace larcv{

class FillImageClusterVariables{

public:

  /// Default constructor
  FillImageClusterVariables(){}

  /// Default destructor
  ~FillImageClusterVariables(){}

  void Fill( Contour_t cv_contour ); //std::vector<::cv::Point>& cv_contour ) ;

  std::vector<float> Areas() { return _area_v ; }

  std::vector<float> Perimeters() { return _perimeter_v ; }

  std::vector<float> BBHeights() { return _bb_height_v ; }

  std::vector<float> BBWidths() { return _bb_width_v ; }

  std::vector<float> MaxContourWidths() { return _max_con_width_v ; }

  std::vector<float> MinContourWidths() { return _min_con_width_v; }

private:

  std::vector<float> _area_v ;
  std::vector<float> _perimeter_v ;
  std::vector<float> _bb_height_v ;
  std::vector<float> _bb_width_v ;
  std::vector<float> _max_con_width_v ;
  std::vector<float> _min_con_width_v ;

  };

}

#endif
/** @} */ // end of doxygen group 

