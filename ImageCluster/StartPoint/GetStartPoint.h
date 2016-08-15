// ariana Hackenburg

#ifndef GETSTARTPOINT_H
#define GETSTARTPOINT_H

#include <iostream>
#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"
#include "LArUtil/GeometryHelper.h"

namespace larocv {

  class GetStartPoint : public larocv::ClusterAlgoBase {

  public:

    /// Default constructor
    GetStartPoint(const std::string name = "GetStartPoint") :
      ClusterAlgoBase(name),
      _nhits(25)
    {}  

    /// Default destructor
    virtual ~GetStartPoint(){}

    void Finalize(TFile*) {}

    protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
                                      const ::cv::Mat& img,
                                      larocv::ImageMeta& meta, larocv::ROI& roi);


  private:

    int _nhits;

  };

class GetStartPointFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    GetStartPointFactory() { ClusterAlgoFactory::get().add_factory("GetStartPoint",this); }
    /// dtor
    ~GetStartPointFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new GetStartPoint(instance_name); }
  };
  /// Global larocv::GetStartPointFactory to register ClusterAlgoFactory
  static GetStartPointFactory __global_GetStartPointFactory__;

} 

#endif

