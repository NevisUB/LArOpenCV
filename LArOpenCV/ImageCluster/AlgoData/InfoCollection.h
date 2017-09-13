#ifndef __ALGODATA_INFOCOLLECTION_H__
#define __ALGODATA_INFOCOLLECTION_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include <array>
#include "ParticleCluster.h"
#include "SpacePt.h"

/*
  @brief: storage for generic information, whatever you want to get the job done
*/
namespace larocv {

  namespace data {

    /**
       \class Info3D
       @brief Store 3D information
    */
    class Info2D : public AlgoDataArrayElementBase {
    public:
      Info2D()  { Clear(); }
      ~Info2D() {}

      void _Clear_();

      ParticleType_t ptype;
      
      int   nplanes;
      float length;
      float width;
      float perimeter;
      float area;
      float npixel;
      float qsum;
      float track_frac;
      float shower_frac;
      float mean_pixel_dist;
      float sigma_pixel_dist;
      float angular_sum;
      float triangle_d_sum;

      float length_max;
      float width_max;
      float perimeter_max;
      float area_max;
      float npixel_max;
      float qsum_max;
      float track_frac_max;
      float shower_frac_max;
      float mean_pixel_dist_max;
      float sigma_pixel_dist_max;
      float angular_sum_max;
      float triangle_d_max;

      float length_min;
      float width_min;
      float perimeter_min;
      float area_min;
      float npixel_min;
      float qsum_min;
      float track_frac_min;
      float shower_frac_min;
      float mean_pixel_dist_min;
      float sigma_pixel_dist_min;
      float angular_sum_min;
      float triangle_d_min;
      
    };

    /**
       \class Info3D
       @brief Store 3D information
    */
    class Info3D : public AlgoDataArrayElementBase {
    public:
      Info3D()  { Clear(); }
      ~Info3D() {}
      
      void _Clear_();
      
      // Overall PCA
      //
      float overall_pca_theta;
      float overall_pca_phi;
      std::array<float,3> overall_pca_dir;
      std::array<float,3> overall_pca_start_pt;
      std::array<float,3> overall_pca_end_pt;
      float overall_pca_length;
      bool overall_pca_valid;
      std::vector<SpacePt> overall_space_pts_v;
      
      //
      // Trunk PCA
      //
      float trunk_pca_theta;
      float trunk_pca_phi;
      std::array<float,3> trunk_pca_dir;
      std::array<float,3> trunk_pca_start_pt;
      std::array<float,3> trunk_pca_end_pt;
      bool trunk_pca_valid;
      float trunk_pca_length;
      float pixel_radius;
      std::vector<SpacePt> trunk_space_pts_v;

      
    };
    
    typedef AlgoDataArrayTemplate<Info2D> Info2DArray;
    typedef AlgoDataArrayTemplate<Info3D> Info3DArray;
    
  }
}
#endif
/** @} */ // end of doxygen group
