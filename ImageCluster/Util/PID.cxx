#ifndef __PID_CXX__
#define __PID_CXX__

#include "PID.h"

namespace larocv{

  /// Global larocv::PIDFactory to register AlgoFactory
  static PIDFactory __global_PIDFactory__;
  
  void PID::_Configure_(const ::fcllite::PSet &pset)
  {
    _pdg = pset.get<int> ("PDG");
  }

  Cluster2DArray_t PID::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta, larocv::ROI& roi)
  { 
    Cluster2DArray_t pid_clusters(clusters); 
    for( auto & c : pid_clusters ){
      if( _pdg == 11 )
        c._particle_type = kPi0 ;
      else
        c._particle_type = kUnknown ;
      }

    return pid_clusters; 
    }
  }
#endif
