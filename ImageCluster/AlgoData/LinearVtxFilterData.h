#ifndef __ALGODATA_LINEARVTXFILTERDATA_H__
#define __ALGODATA_LINEARVTXFILTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Refine2DVertexData.h"

namespace larocv {

  /*
    \class LinearVtxFilterPlaneData
   */
  class LinearVtxFilterPlaneData {
  public:
    LinearVtxFilterPlaneData() { Clear(); }
    ~LinearVtxFilterPlaneData() {}
    
    /// Attribute clear method
    void Clear();
  };

  /*
    \class LinearVtxFilterData
   */
  class LinearVtxFilterData : public larocv::AlgoDataBase {
  public:
    LinearVtxFilterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~LinearVtxFilterData(){}

    void Clear();

    std::vector<larocv::LinearVtxFilterPlaneData> _plane_data;

    std::vector<size_t> _vtx3d_kink_ass_v;
    std::vector<size_t> _vtx3d_curve_ass_v;
    
  };
}
#endif
