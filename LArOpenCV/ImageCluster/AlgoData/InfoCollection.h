#ifndef __ALGODATA_INFOCOLLECTION_H__
#define __ALGODATA_INFOCOLLECTION_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include <array>

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
      
      /// attribute clear method
      void _Clear_() {}
      
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
      

      //
      // Trunk PCA
      //
      float trunk_pca_theta;
      float trunk_pca_phi;
      std::array<float,3> trunk_pca_dir;
      std::array<float,3> trunk_pca_start_pt;
      std::array<float,3> trunk_pca_end_pt;
      float trunk_pca_length;
      float pixel_radius;
      
    };
    
    typedef AlgoDataArrayTemplate<Info2D> Info2DArray;
    typedef AlgoDataArrayTemplate<Info3D> Info3DArray;
    
  }
}
#endif
/** @} */ // end of doxygen group
