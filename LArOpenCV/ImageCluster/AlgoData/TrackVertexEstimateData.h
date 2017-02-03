/**
 * \file TrackVertexEstimateData.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class TrackVertexEstimateData and sub classes
 *
 * @author kazuhiro
 */

/** \addtogroup AlgoData

    @{*/
#ifndef __ALGODATA_TRACKVERTEXESTIMATEDATA_H__
#define __ALGODATA_TRACKVERTEXESTIMATEDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {

  namespace data {

    /*
      \class TrackVertexEstimateData
      @brief Algorithm data created by Refine2DVertex, stores essential attributes for analysis
    */
    class TrackVertexEstimateData : public larocv::data::AlgoDataBase {
    public:
      TrackVertexEstimateData()
      { Clear();}
      ~TrackVertexEstimateData(){}
      /// attribute clear
      void Clear() {}
      
    private:

    };
  }
}
#endif
/** @} */ // end of doxygen group
