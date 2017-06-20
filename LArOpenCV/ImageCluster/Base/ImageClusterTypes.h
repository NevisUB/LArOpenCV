#ifndef __IMAGECLUSTERTYPES_H__
#define __IMAGECLUSTERTYPES_H__

#include "opencv2/core/mat.hpp"
#include "LArOpenCV/Core/LArOCVTypes.h"
#include "Geo2D/Core/Vector.h"

namespace larocv {

  enum ParticleType_t {
    kUnknown,
    kPi0,
    kParticleTypeMax
  };

  enum AlgorithmType_t {
    kAlgoCluster,   ///< ClusterAlgoBase inherit class
    kAlgoImageAna,  ///< ImageAnaBase inherit class
    kAlgoMatch,     ///< MatchAlgoBase inherit class
    kAlgoReCluster, ///< ReClusterAlgoBase inherit class
    kAlgoTypeMax
  };

  /// larocv::ImageClusterBase inherit algorithm's unique identifier within larocv::ClusterImageManager
  typedef size_t AlgorithmID_t;
  /// larocv::AlgoDataBase inherit data rep's identifier
  typedef size_t AlgoDataID_t;
  /// A generic type for an "index of an array"
  typedef size_t Index_t;
  /// A unique identifier for a set of images
  enum class ImageSetID_t {
    kImageSetWire,    ///< wire waveform image
    kImageSetTrack,   ///< track image
    kImageSetShower,  ///< shower image
    kImageSetThruMu,  ///< thrumu image
    kImageSetStopMu,  ///< stopmu image
    kImageSetUnknown  ///< unspecified (always keep it @ end of enum)
  };
  typedef size_t ImageID_t;
  /// A cluster's unique identifier within larocv::ImageClusterManager
  typedef size_t ClusterID_t;
  /// A 2D contour type
  typedef std::vector<cv::Point> Contour_t;
  /// A set of 2D contours
  typedef std::vector<larocv::Contour_t> ContourArray_t;
  /// A constant to signify invalid "id" in general
  static const size_t kINVALID_ID = larocv::kINVALID_SIZE;
  /// A constant to signify, in particular, an invalid larocv::AlgorithmID_t
  static const AlgorithmID_t kINVALID_ALGO_ID = kINVALID_ID;
  /// A constant to signify, in particular, an invalid larocv::ClusterID_t
  static const ClusterID_t kINVALID_CLUSTER_ID = kINVALID_ID;

  typedef std::vector<std::vector<geo2d::Vector<int> > > GEO2D_ContourArray_t;
  typedef std::vector<geo2d::Vector<int> >               GEO2D_Contour_t;

  namespace data {
    enum AlgoDataCategory_t {
      kDataCatArray,        ///< AlgoDataArrayBase inherit class
      kDataCatArrayElement, ///< AlgoDataArrayElementBase inherit class
      kDataCatDefault       ///< Anything else
    };
    enum AlgoDataType_t {
      kDataTypeUnknown
    };
  }
}
#endif