#ifndef __CLUSTERMERGE_CXX__
#define __CLUSTERMERGE_CXX__

#include "ClusterMerge.h"
#include "LArOpenCV/Core/larbys.h"
#include "Geo2D/Core/Vector.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using larocv::larbys;

namespace larocv {

  void
  ClusterMerge::Configure(const Config_t &pset) {
    //_N        = pset.get<int>("Keep");
    _trilen   = pset.get<float>("TriLen");   // in pixels
    _triangle = pset.get<float>("TriAngle"); // in degrees    
  }


  GEO2D_Contour_t
  ClusterMerge::FlashlightMerge(geo2d::Vector<float> startpt,
				const GEO2D_ContourArray_t& super_ctor_v,
				const GEO2D_Contour_t& parent_ctor) {

    auto flashlight_v = this->GenerateFlashlights(startpt,super_ctor_v);
    auto start_index  = this->StartIndex(super_ctor_v,parent_ctor);
    auto merged_ctor  = this->MergeFlashlights(flashlight_v,start_index);
    return merged_ctor;
    
  }

  GEO2D_Contour_t
  ClusterMerge::FlashlightMerge(geo2d::Vector<float> startpt,
				const GEO2D_ContourArray_t& super_ctor_v,
				const GEO2D_ContourArray_t& exclude_ctor_v,
				const GEO2D_Contour_t& parent_ctor) {
    
    auto super_flashlight_v   = this->GenerateFlashlights(startpt,super_ctor_v);
    auto exclude_flashlight_v = this->GenerateFlashlights(startpt,exclude_ctor_v);
    auto start_super_index    = this->StartIndex(super_ctor_v,parent_ctor);
    auto start_exclude_index  = this->StartIndex(exclude_ctor_v,parent_ctor);

    for(size_t flashid=0;flashid<exclude_flashlight_v.size();++flashid) {
      if (flashid==start_exclude_index) continue;
      auto& flash = exclude_flashlight_v[flashid];
      super_flashlight_v.emplace_back(std::move(flash));
    }
    auto merged_ctor = this->MergeFlashlights(super_flashlight_v,start_super_index);
    return merged_ctor;
  }

  size_t
  ClusterMerge::StartIndex(const GEO2D_ContourArray_t& super_ctor_v,
			   const GEO2D_Contour_t& parent_ctor) {
    auto const super_cluster_id = larocv::FindContainingContour(super_ctor_v, parent_ctor);
    if (super_cluster_id==kINVALID_SIZE) {
      LAROCV_CRITICAL() << "Parent contour could not be located in supers" << std::endl;
      throw larbys();
    }
    return super_cluster_id;
  }
  
  GEO2D_Contour_t
  ClusterMerge::MergeFlashlights(const GEO2D_ContourArray_t& flashlight_v, 
				 size_t start_index) {
				 
    GEO2D_Contour_t res_ctor;
    std::set<size_t> overlap_s;
    overlap_s.insert(start_index);
    
    for(size_t flashid=0;flashid<flashlight_v.size();flashid++) {
      if (flashid==start_index) continue;
      const auto& thisflash = flashlight_v[flashid];
      for (auto oid : overlap_s) {
	if (oid==flashid) continue;
	const auto& thatflash=flashlight_v[oid];
	auto overlap = larocv::AreaOverlap(thisflash,thatflash);
	if (overlap>0) overlap_s.insert(flashid);
      }
    }

    res_ctor.reserve(overlap_s.size()*6);

    for (auto oid : overlap_s)
      for(const auto& pt : flashlight_v[oid])
	res_ctor.emplace_back(pt);
    
    res_ctor = ConvexHull(res_ctor);
    
    return res_ctor;
  }
  
  GEO2D_ContourArray_t
  ClusterMerge::GenerateFlashlights(geo2d::Vector<float> startpt,
				    const GEO2D_ContourArray_t& super_ctor_v) {
    
    
    // get the associated super contour for this parent contour
    GEO2D_ContourArray_t flashlight_v;
    for(size_t superid=0; superid<super_ctor_v.size(); ++superid){
      
      auto& ctor =  super_ctor_v[superid];

      auto minrect = cv::minAreaRect(ctor);
      cv::Point2f vertices[4];
      minrect.points(vertices); 

      std::vector<geo2d::Vector<float> > minrect_v
      {vertices[0],vertices[1],vertices[2],vertices[3]};
      
      // find the two verticies that are farthest from the start point, this is other edge
      geo2d::Vector<float> stop, sbot, etop, ebot;

      //sort based on distance to the ostart, far away means it's on the other side
      //if I was smart I would check the distance to the end point but im stupid
      
      std::sort(std::begin(minrect_v),std::end(minrect_v),
		[&startpt](const geo2d::Vector<float>& p1, const geo2d::Vector<float>& p2)
		{ return geo2d::dist(p1,startpt) > geo2d::dist(p2,startpt); } );
      
      //set the top and bottom end, the two verticies furthest away from the start point
      etop = minrect_v[0].y > minrect_v[1].y ? minrect_v[0] : minrect_v[1];
      ebot = etop == minrect_v[0] ? minrect_v[1] : minrect_v[0];

      //set the top and bottom start, the two points closest to the start point
      stop = minrect_v[2].y > minrect_v[3].y ? minrect_v[2] : minrect_v[3];
      sbot = stop == minrect_v[2] ? minrect_v[3] : minrect_v[2];

      //now we have the top and bottom points to do the projection outwards.
      auto dx =  etop.x - stop.x; 
      auto dy =  etop.y - stop.y;
      
      float odout  = _trilen;
      float bangle = _triangle * 3.14159/180.;

      auto& bbox = minrect;
      float oangle = bbox.angle * 3.14159/180.0;

      if (bbox.size.width < bbox.size.height)
	oangle += 90 * 3.14159/180.0;
      
      float p1_cos_a = oangle + bangle;
      float p1_sin_a = oangle + bangle;

      float p2_cos_a = oangle - bangle;
      float p2_sin_a = oangle - bangle;
      
      auto xx1 = odout*std::cos(p1_cos_a);
      auto yy1 = odout*std::sin(p1_sin_a);

      auto xx2 = odout*std::cos(p2_cos_a);
      auto yy2 = odout*std::sin(p2_sin_a);
      
      geo2d::Vector<float> p1(xx1,yy1);
      geo2d::Vector<float> p2(xx2,yy2);
	
      if ( (dx < 0 and dy < 0) or (dx < 0 and dy > 0)) { 

	p1 = ebot - p1;     
	p2 = etop - p2;
	std::swap(p1,p2);
      }
      //odd case
      else if ( dx==0 and dy<0 ) {
	p1 = ebot - p1;     
	p2 = etop - p2;
      }
      //another odd case
      else if ( dy == 0 and dx < 0) {
	p1 = ebot - p1;     
	p2 = etop - p2;
	std::swap(p1,p2);
      }
      else {
	p1 = p1 + etop;
	p2 = p2 + ebot;
      }
      
      //replace the contour with the flashlight
      GEO2D_Contour_t flashlight(6,cv::Point_<int>(kINVALID_INT,kINVALID_INT));
      flashlight[0]=std::move(sbot);
      flashlight[1]=std::move(stop);
      flashlight[2]=std::move(etop);
      flashlight[3]=std::move(p1);
      flashlight[4]=std::move(p2);
      flashlight[5]=std::move(ebot);
      flashlight_v.emplace_back(std::move(flashlight));
    } //end making flashlight;

    return flashlight_v;
  }
}

#endif
