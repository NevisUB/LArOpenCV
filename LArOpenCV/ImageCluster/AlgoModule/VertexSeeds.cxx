#ifndef __VERTEXSEEDS_CXX__
#define __VERTEXSEEDS_CXX__

#include "VertexSeeds.h"
#include "Geo2D/Core/Geo2D.h"

namespace larocv {

  /// Global VertexSeedsFactory to register AlgoFactory
  static VertexSeedsFactory __global_VertexSeedsFactory__;

  void VertexSeeds::_Configure_(const Config_t &pset)
  {

    _ClusterHIPMIP.Configure(pset);
    _DefectBreaker.Configure(pset);
    _PCACrossing.Configure(pset);
    
    Register(new data::VertexSeedsData);
  }

  GEO2D_ContourArray_t combine_ctor_arrs(const GEO2D_ContourArray_t& ctor_arr_1,
					 const GEO2D_ContourArray_t& ctor_arr_2) {
    GEO2D_ContourArray_t ctors;
    ctors.reserve(ctor_arr_1.size() + ctor_arr_2.size());

    ctors.insert( ctors.end(), ctor_arr_1.begin(), ctor_arr_1.end());
    ctors.insert( ctors.end(), ctor_arr_2.begin(), ctor_arr_2.end());

    return ctors;
    
  }
  
  Cluster2DArray_t VertexSeeds::_Process_(const Cluster2DArray_t& clusters,
					  const ::cv::Mat& img,
					  ImageMeta& meta,
					  ROI& roi)
  {

    // Cluster the HIPS and MIPS
    auto hip_mip_p = _ClusterHIPMIP.IsolateHIPMIP(img);

    LAROCV_DEBUG() << "Isolated " << hip_mip_p.first.size()
		   << " HIPs and " << hip_mip_p.second.size()
		   << " MIPS" << std::endl;
    
    // Merge the hips and the mips into a single array of contours
    // --> yes we can rethink the return of IsolateHIPMIP....
    auto ctor_arr_v = combine_ctor_arrs(hip_mip_p.first,hip_mip_p.second);

    LAROCV_DEBUG() << "Combined: " << ctor_arr_v.size() << std::endl;
    
    // loop over ctors in this plane
    for (const auto& ctor : ctor_arr_v) {
      LAROCV_DEBUG() << "Analyzing contour of size " << ctor.size() << std::endl;
      
      // split ctor and create a cluster compound (has defects, atomics, start/end pt)
      auto cluscomp = _DefectBreaker.SplitContour(ctor);

      LAROCV_DEBUG() << "I split this contour into " << cluscomp.size() << " atomics" << std::endl;
      LAROCV_DEBUG() << "Found " << cluscomp.get_defects().size() << " defects for seeds" << std::endl;
      
      // generate seeds from PCA
      auto intersections = _PCACrossing.ComputeIntersections(cluscomp,img);  

      LAROCV_DEBUG() << "Generated " << intersections.size() << std::endl;

      //Now... we have defects and pca inters as seeds... store them in algo data...

      //do it
    }


    // done...

    return Cluster2DArray_t();
  }
  

}
#endif
