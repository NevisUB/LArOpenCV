#ifndef CALCANGLE_CXX
#define CALCANGLE_CXX

#include "CalcAngle.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/LineSegment.h"
#include "Geo2D/Core/spoon.h"
#include "Geo2D/Core/Geo2D.h"
#define PI 3.1415926

#include <set>
#include <cassert>

namespace larocv {


  void CalcAngle::Configure(const Config_t& pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    
    _pixels_number = pset.get<int>("PixelsNumber", 10);
    
  }


  void CalcAngle::AngleWithCircleResolution( double & angle,  
					     const cv::Mat img,
					     const GEO2D_Contour_t par_ctor,
					     geo2d::Vector<float> edge ){

    data::CircleVertex circle_vertex;
    circle_vertex.center = edge;
    circle_vertex.radius = 10.0;
    CalcAngle::AngleWithCircleResolution( angle, img, par_ctor, circle_vertex );
    
  }

  void CalcAngle::AngleWithCircleResolution( double & angle,  
					     const cv::Mat img,
					     const GEO2D_Contour_t par_ctor,
					     data::CircleVertex circle_vertex ){
    
    int par_pixel_amount = par_ctor.size();	  
    
    auto masked_ctor_start_og  = MaskImage(img,par_ctor,0,false); 	
    auto masked_ctor_start     = masked_ctor_start_og.clone();
    auto masked_ctor_start_img = FindNonZero(masked_ctor_start);	  

    masked_ctor_start_og = Threshold(masked_ctor_start_og, 10, 255);
    
    geo2d::Circle<float> par_circle;
    par_circle.center = circle_vertex.center;
    par_circle.radius = 100;
    double par_angle;
    double par_pct;
	  
    if ( masked_ctor_start_img.size() < 2 ) return;
    
    ParticleAngle(masked_ctor_start_img, masked_ctor_start_img, par_circle, par_pct, par_angle );
    
    //
    // Threshold
    //
    masked_ctor_start = Threshold(masked_ctor_start, 10, 255);
    masked_ctor_start_img = FindNonZero(masked_ctor_start);
    
    if ( masked_ctor_start_img.size() < 2 ) return;
    
    //auto start_point = circle_vertex.center;
    //geo2d::Vector<float> end_point;
    //FindEdge(masked_ctor_start_img, start_point, end_point);
    //float max_radius = pow((pow(start_point.x-end_point.x,2)+pow(start_point.y-end_point.y,2)),0.5);
    
    masked_ctor_start_img = FindNonZero(masked_ctor_start);
    
    float max_radius = 100 ;
    int r_size = int(max_radius/1.);
    std::vector<float> radius_v;
    radius_v.clear();
    radius_v.resize(r_size, circle_vertex.radius);
    for(int ridx = 0; ridx < r_size ; ++ridx) radius_v.at(ridx) += ridx;
    
    //double angle;
    double pct;
    geo2d::Vector<float> end_point;
    geo2d::Vector<float> end_point_last;
    geo2d::Circle<float> circle;
    double angle_this; // Angle calculated with current radius
    double angle_last; // Angle calculated with previous radius
    geo2d::Circle<float> circle_last;
    double pct_last;
    
    for(int ridx = 0; ridx < r_size ; ++ridx){
      
      if(ridx == 0) {
	angle_this = -1.0*kINVALID_DOUBLE;
	angle_last = -1.0*kINVALID_DOUBLE;
      }
      
      geo2d::Circle<float> circle_this;
      circle_this.radius = radius_v.at(ridx);
      circle_this.center = circle_vertex.center;
	    
      double pct_this;
      auto masked_ctor_this = MaskImage(masked_ctor_start,circle_this,0,false); 	
      
      auto pixels_number = _pixels_number;
      auto masked_ctor_start_sz = CountNonZero(masked_ctor_start);
      if(masked_ctor_start_sz < _pixels_number) 
	pixels_number = masked_ctor_start_sz;
      
      auto masked_ctor_this_img = FindNonZero(masked_ctor_this);
      if ( ridx > 0 && (masked_ctor_this_img.size() < pixels_number) ) continue;
      
      float pi_threshold = 10;
      float supression = 2;
      auto xs_v_this = QPointOnCircle(masked_ctor_this, circle_this, pi_threshold, supression);
      
      if (masked_ctor_this_img.size() < 2) continue;
      ParticleAngle(masked_ctor_start_img, masked_ctor_this_img, circle_this, pct_this, angle_this );
      
      if (angle_this == -1.0*kINVALID_DOUBLE) continue;
      
      if ( xs_v_this.size() == 0 && par_pixel_amount == masked_ctor_this_img.size() && ridx == 0) {
	angle =  angle_this;
	pct = pct_this;
	circle = circle_this;
	break;
      }
      
      if ( xs_v_this.size() == 0 && par_pixel_amount == masked_ctor_this_img.size() && ridx > 1 ) {
	angle =  angle_last;
	pct = pct_last;
	circle = circle_last;
	end_point = end_point_last;
	break;
      }
      
      if ( xs_v_this.size() == 2 && ridx > 1 ) {
	angle =  angle_last;
	pct = pct_last;
	circle = circle_last;
	end_point = end_point_last;
	break;
      }
      
      if ( ridx == 0 ) continue; 
      
      float resolution_last = 360 / (2 * PI * radius_v.at(ridx-1));
      
      
      circle_last.radius = radius_v.at(ridx-1);
      circle_last.center = circle_vertex.center;
	    
      circle_this.center = circle_vertex.center;
      
      auto xs_v_last = QPointOnCircle(masked_ctor_this, circle_this, pi_threshold, supression);
      if (xs_v_last.size()==1) {
	end_point_last = xs_v_last.at(0); 
	end_point = end_point_last;
      }
      
      auto masked_ctor_last = MaskImage(masked_ctor_start_og,circle_last,0,false);
      if ( FindNonZero(masked_ctor_last).size() <_pixels_number ) continue;
      
      auto masked_ctor_last_img = FindNonZero(masked_ctor_last);
      if (masked_ctor_last_img.size()<2) continue;
      ParticleAngle(masked_ctor_start_img, masked_ctor_last_img, circle_last, pct_last, angle_last );
      
      circle = circle_last;
      angle =  angle_last;
      pct = pct_last;
      
      //bool is_this_pos = angle_this > 0 ;
      //bool is_last_pos = angle_last > 0 ;
      //if ( is_this_pos &&  is_last_pos) continue;
      //if (!is_this_pos && !is_last_pos) continue;
      
      if ((std::abs(angle_last-angle_this) > resolution_last) || 
	  (angle_last-angle_this == 0 )){
	angle =  angle_last;
	pct = pct_last;
	angle_last+=1;
	angle_last = (int)angle_last;
	if (angle_last != 270 && angle_last !=90 )break;
      }
    }
  }
  
  void CalcAngle::ParticleAngle(GEO2D_Contour_t ctor_origin, 
				    GEO2D_Contour_t ctor, 
				    geo2d::Circle<float> circle, 
				    double& pct, double& angle) {
    
    float vtx2d_x =  circle.center.x;
    float vtx2d_y =  circle.center.y;
    
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      ctor.at(idx).x =  ctor.at(idx).x- circle.center.x;
      ctor.at(idx).y =  ctor.at(idx).y- circle.center.y;
    }
    
    auto meanx = Getx2vtxmean(ctor_origin, vtx2d_x, vtx2d_y, pct );
    auto meany = Gety2vtxmean(ctor_origin, vtx2d_x, vtx2d_y, pct );
    
    auto dir = CalcPCA(ctor).dir;

    if(meanx * dir.x < 0 || meany* dir.y <0) {dir.x *=-1; dir.y*=-1;}
    
    angle = atan2(dir.y, dir.x)*180 / M_PI;

    if (angle < 0) angle += 360;
    }
  
  double CalcAngle::Getx2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct)
  {
    double ctr_pos = 0.0;
    double ctr_neg = 0.0;
    double sum = 0;
    double mean = -1.0*kINVALID_DOUBLE;
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      sum += ctor.at(idx).x - x2d;
      if (ctor.at(idx).x - x2d > 0) ctr_pos++;
      if (ctor.at(idx).x - x2d < 0) ctr_neg++;
    }
    pct = std::abs(ctr_pos - ctr_neg) / (double)ctor.size();
    if (ctor.size()>0) mean = sum / (double) ctor.size();
    return mean;
  }
  
  double CalcAngle::Gety2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct)
  {
    double ctr_pos = 0.0;
    double ctr_neg = 0.0;
    double sum = 0;
    double mean = -1.0*kINVALID_DOUBLE;
    for(size_t idx= 0;idx < ctor.size(); ++idx){
      sum += ctor.at(idx).y - y2d;
      if (ctor.at(idx).y - y2d > 0) ctr_pos++;
      if (ctor.at(idx).y - y2d < 0) ctr_neg++;
    }
    pct = std::abs(ctr_pos - ctr_neg)/ (double)ctor.size();
    if (ctor.size()>0) mean = sum / (double) ctor.size();
    return mean;
  }  

}
#endif
