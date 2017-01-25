#ifndef __VERTEXPARTICLECLUSTER_H__
#define __VERTEXPARTICLECLUSTER_H__

#include "Core/laropencv_base.h"
#include "Core/VectorArray.h"
#include "FhiclLite/PSet.h"
#include "AlgoData/VertexClusterData.h"

//#define UNIT_TEST
//#ifdef UNIT_TEST
//#include "PyUtil/PyUtil.h"
//#endif

namespace larocv {
 
  class VertexParticleCluster : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexParticleCluster();
    
    /// Default destructor
    virtual ~VertexParticleCluster(){}

    /// Optional configuration method
    void Configure(const ::fcllite::PSet &pset);

    /// Main function to create particle cluster using polar coordinate
    std::vector<GEO2D_Contour_t>
    CreateParticleCluster(const ::cv::Mat& img,
			  const data::CircleVertex& vtx2d);
    /*
#ifdef UNIT_TEST
    /// For unit test purpose in python
    std::vector<GEO2D_Contour_t>
    CreateParticleCluster(PyObject*,
			  const data::CircleVertex& vtx2d);
#endif
    */
    
    //
    // Public algorithm configuration attributes
    //
    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    
    float _theta_hi;
    float _theta_lo;

    unsigned short _pi_threshold;

    bool   _use_theta_half_angle;
    double _contour_dist_threshold;
    
  private:

    void CreateSuperCluster(const ::cv::Mat& img);

    size_t FindSuperCluster(const ::geo2d::Vector<float>& vtx2d) const;

    GEO2D_ContourArray_t
    ParticleHypothesis(const ::cv::Mat& img,
		       const data::CircleVertex& vtx);
    
    GEO2D_ContourArray_t _super_cluster_v;
    GEO2D_ContourArray_t _seed_cluster_v;
    GEO2D_ContourArray_t _child_cluster_v;
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

