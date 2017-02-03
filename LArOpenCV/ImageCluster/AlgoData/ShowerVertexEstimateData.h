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


#ifndef __ALGODATA_SHOWERVERTEXESTIMATEDATA_H__
#define __ALGODATA_SHOWERVERTEXESTIMATEDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {

  namespace data {

    /*
      \class ShowerVertexEstimateData
      @brief Algorithm data created by Refine2DVertex, stores essential attributes for analysis
    */
    class ShowerVertexEstimateData : public larocv::data::AlgoDataBase {
    public:
      ShowerVertexEstimateData()
      { Clear();}
      ~ShowerVertexEstimateData(){}
      /// attribute clear
      void Clear() {}
      
    private:


    };
  }
}
#endif
/** @} */ // end of doxygen group
