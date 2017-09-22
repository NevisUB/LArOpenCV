#ifndef CALCANGLE_H
#define CALCANGLE_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SuperClusterer.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "Geo2D/Core/Geo2D.h"
/*
  @brief: the class is supposed to do contour/img analysis to merge broken tracks into one. 
*/


namespace larocv {
  
  class CalcAngle : public laropencv_base {
    
  public:
    
    /// Default constructor
  CalcAngle() : 
    laropencv_base("CalcAngle")
      {}
    
    /// Default destructor
    ~CalcAngle(){}
    
    void Configure(const Config_t& pset);

    void ParticleAngle(GEO2D_Contour_t ctor_origin, 
		       GEO2D_Contour_t ctor, 
		       geo2d::Circle<float> circle, 
		       double& pct, double& angle);
    
    double Getx2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct);
    
    double Gety2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct);
    
    void AngleWithCircleResolution( double & angle,  
				    const cv::Mat img,
				    const GEO2D_Contour_t par_ctor,
				    data::CircleVertex circle_vertex );

    void AngleWithCircleResolution( double & angle,  
				    const cv::Mat img,
				    const GEO2D_Contour_t par_ctor,
				    geo2d::Vector<float> edge );
    
  private:
    SuperClusterer _SuperClusterer;

    int _pixels_number;
  };
}

#endif
/** @} */ // end of doxygen group 

