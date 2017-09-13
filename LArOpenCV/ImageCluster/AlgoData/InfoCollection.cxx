#ifndef __ALGODATA_INFOCOLLECTION_CXX__
#define __ALGODATA_INFOCOLLECTION_CXX__

#include "InfoCollection.h"

namespace larocv {
  namespace data {

      /// attribute clear method
    void Info2D::_Clear_() { 
      
      ptype = ParticleType_t::kUnknown; 
    
      nplanes=kINVALID_INT;

      length=kINVALID_FLOAT;
      width=kINVALID_FLOAT;
      perimeter=kINVALID_FLOAT;
      area=kINVALID_FLOAT;
      npixel=kINVALID_FLOAT;
      qsum=kINVALID_FLOAT;
      track_frac=kINVALID_FLOAT;
      shower_frac=kINVALID_FLOAT;
      mean_pixel_dist=kINVALID_FLOAT;
      sigma_pixel_dist=kINVALID_FLOAT;
      angular_sum=kINVALID_FLOAT;
      triangle_d_sum=kINVALID_FLOAT;

      length_max=kINVALID_FLOAT;
      width_max=kINVALID_FLOAT;
      perimeter_max=kINVALID_FLOAT;
      area_max=kINVALID_FLOAT;
      npixel_max=kINVALID_FLOAT;
      qsum_max=kINVALID_FLOAT;
      track_frac_max=kINVALID_FLOAT;
      shower_frac_max=kINVALID_FLOAT;
      mean_pixel_dist_max=kINVALID_FLOAT;
      sigma_pixel_dist_max=kINVALID_FLOAT;
      angular_sum_max=kINVALID_FLOAT;
      triangle_d_max=kINVALID_FLOAT;

      length_min=kINVALID_FLOAT;
      width_min=kINVALID_FLOAT;
      perimeter_min=kINVALID_FLOAT;
      area_min=kINVALID_FLOAT;
      npixel_min=kINVALID_FLOAT;
      qsum_min=kINVALID_FLOAT;
      track_frac_min=kINVALID_FLOAT;
      shower_frac_min=kINVALID_FLOAT;
      mean_pixel_dist_min=kINVALID_FLOAT;
      sigma_pixel_dist_min=kINVALID_FLOAT;
      angular_sum_min=kINVALID_FLOAT;
      triangle_d_min=kINVALID_FLOAT;

    }

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
