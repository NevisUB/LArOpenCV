#ifndef __VERTEXTRACKANGLE_CXX__
#define __VERTEXTRACKANGLE_CXX__

#include "Refine2DVertex.h"
#include "VertexTrackAngle.h"

namespace larocv{

  /// Global larocv::VertexTrackAngleFactory to register ClusterAlgoFactory
  static VertexTrackAngleFactory __global_VertexTrackAngleFactory__;

  void VertexTrackAngle::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexName");
    _vtx_algo_id = this->ID(vtx_algo_name);

  }

  geo2d::VectorArray<float> VertexTrackAngle::QPointOnCircles(const ::cv::Mat& img, const geo2d::Vector<float>& center)
  {
    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, center, _radius, ::cv::WARP_FILL_OUTLIERS);

    std::vector<std::vector<geo2d::Vector<float> > > pt_vv;
    std::vector<std::vector<bool> > used_vv;

    float pxsize = (float)(_radius) / (float)(polarimg.cols);

    size_t min_col = (size_t)(_min_radius / pxsize + 0.5);

    size_t step_size = (size_t)(_step_size / pxsize + 0.5);

    for(size_t col=min_col; col < polarimg.cols; col += step_size) {

      std::vector<geo2d::Vector<float> > pt_v;
      std::vector<bool> used_v;

      std::vector<std::pair<int,int> > range_v;
      std::pair<int,int> range(-1,-1);
      
      for(size_t row=0; row<polarimg.rows; ++row) {
	
	float q = (float)(polarimg.at<unsigned char>(row, col));
	if(q < _pi_threshold) {
	  if(range.first >= 0) {
	    range_v.push_back(range);
	    range.first = range.second = -1;
	  }
	  continue;
	}
	//std::cout << row << " / " << polarimg.rows << " ... " << q << std::endl;
	if(range.first < 0) range.first = range.second = row;
	
	else range.second = row;
	
      }
      // Check if end should be combined w/ start
      if(range_v.size() >= 2) {
	if(range_v[0].first == 0 && (range_v.back().second+1) == polarimg.rows) {
	  range_v[0].first = range_v.back().first - (int)(polarimg.rows);
	  range_v.pop_back();
	}
      }
      // Compute xs points
      for(auto const& r : range_v) {
	
	//std::cout << "XS @ " << r.first << " => " << r.second << " ... " << polarimg.rows << std::endl;
	float angle = ((float)(r.first + r.second))/2.;
	if(angle < 0) angle += (float)(polarimg.rows);
	angle = angle * M_PI * 2. / ((float)(polarimg.rows));
	
	geo2d::Vector<float> pt;
	pt.x = circle.center.x + _radius * cos(angle);
	pt.y = circle.center.y + _radius * sin(angle);
	
	pt_v.push_back(pt);      
      }
      used_v.resize(pt_v.size(),false);
      pt_vv.emplace_back(std::move(pt_v));
      used_vv.emplace_back(std::move(used_v));
    }

    // Next, figure out combinations

    
    
    return res;
  }


  void VertexTrackAngle::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
  }

  bool VertexTrackAngle::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {return true;}

}

#endif
