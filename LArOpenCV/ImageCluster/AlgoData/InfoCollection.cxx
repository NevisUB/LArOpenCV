#ifndef __ALGODATA_INFOCOLLECTION_CXX__
#define __ALGODATA_INFOCOLLECTION_CXX__

#include "InfoCollection.h"

namespace larocv {
  namespace data {

    /// attribute clear method
    void Info3D::_Clear_() {
      static std::array<float,3> invalid_vec = {{kINVALID_FLOAT,
						 kINVALID_FLOAT,
						 kINVALID_FLOAT}};
      overall_pca_theta = kINVALID_FLOAT;
      overall_pca_phi = kINVALID_FLOAT;
      overall_pca_dir = invalid_vec;
      overall_pca_start_pt = invalid_vec;
      overall_pca_end_pt = invalid_vec;
      overall_pca_length = kINVALID_FLOAT;
      overall_pca_valid = true;
      overall_space_pts_v.clear();
      
      trunk_pca_theta = kINVALID_FLOAT;
      trunk_pca_phi = kINVALID_FLOAT;
      trunk_pca_dir = invalid_vec;
      trunk_pca_start_pt = invalid_vec;
      trunk_pca_end_pt = invalid_vec;
      trunk_pca_length = kINVALID_FLOAT;
      trunk_pca_valid = false;
      pixel_radius = kINVALID_FLOAT;
      trunk_space_pts_v.clear();
      
    }
  }
}
#endif
