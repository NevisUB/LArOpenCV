#ifndef __ALGODATA_DEFECTCLUSTERDATA_H__
#define __ALGODATA_DEFECTCLUSTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Core/Line.h"

namespace larocv {

  class AtomicContour {
  public:
    
    AtomicContour(){Clear();}
    AtomicContour(const GEO2D_Contour_t& ctor,size_t parent_idx)
      :
      _ctor(ctor),
      _parent_idx(parent_idx)
    {}
    
    ~AtomicContour(){}

    void Clear() { _ctor.clear(); _parent_idx = -1; }
    
    GEO2D_Contour_t _ctor;
    size_t _parent_idx;
    
  };

  class ContourDefect {
  public:

    ContourDefect() {}
    ContourDefect(const geo2d::Vector<int>& pt_start,
		  const geo2d::Vector<int>& pt_end,
		  const geo2d::Vector<int>& pt_defect,
		  float dist,
		  const geo2d::Line<float>& split_line)
      :
      _pt_start(pt_start),
      _pt_end(pt_end),
      _pt_defect(pt_defect),
      _dist(dist),
      _split_line(split_line){}

    ~ContourDefect() {}

    void Clear() {
      _pt_start.x  = _pt_start.y  = -1;
      _pt_end.x    = _pt_end.y    = -1;
      _pt_defect.x = _pt_defect.y = -1;
      _dist = -1;
      _split_line.pt.x  = _split_line.pt.y  = -1;
      _split_line.dir.x = _split_line.dir.y = -1;
    }
    
    geo2d::Vector<float> _pt_start;
    geo2d::Vector<float> _pt_end;
    geo2d::Vector<float> _pt_defect;
    
    float _dist;
    
    geo2d::Line<float> _split_line;
   
  };
  
  /*
    \class DefectClusterPlaneData
   */
  class DefectClusterPlaneData {
  public:
    DefectClusterPlaneData() { Clear(); }
    ~DefectClusterPlaneData() {}

    /// list of atomic contours for this plane
    std::vector< AtomicContour > _atomic_ctor_v;
    std::vector< ContourDefect > _ctor_defect_v;
    std::vector< std::vector< size_t > > _atomic_ctor_ass_vv;//outer index is cluster index on this plane; inner index is the atomic index in outter index cluster; in this way you get atomic on the same cluster
    int _n_input_ctors;
    
    
    /// Attribute clear method
    void Clear();
  };

  /*
    \class DefectClusterData
   */
  class DefectClusterData : public larocv::AlgoDataBase {
  public:
    DefectClusterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~DefectClusterData(){}

    void Clear();

    std::vector<larocv::DefectClusterPlaneData> _plane_data;
    
  };
}
#endif
