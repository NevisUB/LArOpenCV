#ifndef __LINEARVTXFILTER_CXX__
#define __LINEARVTXFILTER_CXX__

#include "LinearVtxFilter.h"

#include "DefectCluster.h"

namespace larocv {

  /// Global larocv::LinearVtxFilterFactory to register AlgoFactory
  static LinearVtxFilterFactory __global_LinearVtxFilterFactory__;

  geo2d::VectorArray<float> LinearVtxFilter::QPtOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle)
  {
    geo2d::VectorArray<float> res;

    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, circle.center, circle.radius*2, ::cv::WARP_FILL_OUTLIERS);

    size_t col = (size_t)(polarimg.cols / 2);
			  
    std::vector<std::pair<int,int> > range_v;
    std::pair<int,int> range(-1,-1);

    for(size_t row=0; row<polarimg.rows; ++row) {

      float q = (float)(polarimg.at<unsigned char>(row, col));

      //vic: unsure about this
      float pi_threshold=10.0;
      if(q < pi_threshold) {
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
      pt.x = circle.center.x + circle.radius * cos(angle);
      pt.y = circle.center.y + circle.radius * sin(angle);

      res.push_back(pt);      
    }
    return res;
  }



  
  void LinearVtxFilter::_Configure_(const ::fcllite::PSet &pset)
  {
    _r_min  = pset.get<float>("RMin",5);
    _r_max  = pset.get<float>("RMax",15);
    _r_div  = pset.get<float>("RDiv",10);
    _r_cut  = pset.get<float>("RCut",0.5);
    
    _thresh = pset.get<int>  ("Threshold",10);

    _radii_v.resize(_r_div);
    
    float step = (_r_max - _r_min) / _r_div;
    LAROCV_DEBUG() << "step : " << step << std::endl;
    
    for(int ctr=0; ctr <= _r_div; ++ctr)
      _radii_v[ctr] = _r_min + step*((float) ctr);
  }




  geo2d::Line<float> LinearVtxFilter::SquarePCA(const ::cv::Mat& img,
						geo2d::Vector<float> pt,
						const size_t side)
  {
					       
    auto small_img = ::cv::Mat(img,::cv::Rect(pt.x-side, pt.y-side, 2*side+1, 2*side+1));
    ::cv::Mat thresh_small_img;
    //::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    ::cv::threshold(small_img,thresh_small_img,1,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);

    if(points.size() < 2) {
      LAROCV_DEBUG() << "SquarePCA approx cannot be made (# points " << points.size() << " < 2)" << std::endl;
      throw larbys("SquarePCA found no point...");
    }
    
    cv::Mat mat_points(points.size(), 2, CV_32FC1);
    for (unsigned i = 0; i < mat_points.rows; ++i) {
      mat_points.at<float>(i, 0) = points[i].x;
      mat_points.at<float>(i, 1) = points[i].y;
    }

    ::cv::PCA pcaobj(mat_points,::cv::Mat(),::cv::PCA::DATA_AS_ROW,0);
    
    pt.x += pcaobj.mean.at<float>(0,0) - side;
    pt.y += pcaobj.mean.at<float>(0,1) - side;
    
    auto dir = geo2d::Vector<float>(pcaobj.eigenvectors.at<float>(0,0),
				    pcaobj.eigenvectors.at<float>(0,1));

    //LAROCV_DEBUG() << "Mean @ (" << pt.x << "," << pt.y << ") ... dir (" << dir.x << "," << dir.y << std::endl;

    return geo2d::Line<float>(pt,dir);
  }

  

  bool LinearVtxFilter::ScanRadii(const cv::Mat& img, const cv::Point_<float>& pt) {
    
    std::vector<float> xs_dist;
    LAROCV_DEBUG() << "Observing defect point : " << pt << std::endl;

    std::vector<std::vector<cv::Point_<float> > > qpt_vv;    
    
    for(uint ridx=0; ridx < _radii_v.size(); ++ridx) {
      auto radii = _radii_v[ridx];
      LAROCV_DEBUG() << "Radii: " << radii << "... qpt_vv size: " << qpt_vv.size() << std::endl;

      geo2d::Circle<float> circ(pt,radii);
      auto xs_v = QPtOnCircle(img,circ);
      LAROCV_DEBUG() << "ridx: " << ridx << " w/ xs_v size: " << xs_v.size() << std::endl;
      
      if (!ridx) {
	qpt_vv.resize(xs_v.size());
	for(uint xidx=0;xidx<xs_v.size();++xidx)
	  qpt_vv[xidx].emplace_back(xs_v[xidx]);
	continue;
      }

      LAROCV_DEBUG() << "ridx: " << ridx << " w/ xs_v size: " << xs_v.size() << std::endl;	

      for(uint xidx=0;xidx<xs_v.size();++xidx) {
	
	auto xs = xs_v[xidx];
	
	int min_qidx=-1;
	float min_d=6.e6;

	for(uint qidx=0;qidx<qpt_vv.size();++qidx) {
	  auto d = geo2d::dist(xs,qpt_vv[qidx].back());
	  if (d < min_d) {
	    min_qidx = qidx;
	    min_d    = d;
	  }
	}

	LAROCV_DEBUG() << "Got min_qidx: " << min_qidx << "... min_d: " << min_d << std::endl;
	if (min_qidx < 0) throw larbys("no way");
	
	if ( min_d > _r_cut * radii  ) {
	  LAROCV_DEBUG() << "Far away from all with cut: " << _r_cut*radii << "... inserting pt: " << xs << std::endl;
	  std::vector<cv::Point_<float> > tmp = { xs };
	  qpt_vv.emplace_back(std::move(tmp));
	  continue;
	}
	
	qpt_vv[min_qidx].emplace_back(xs);
	
      }
    }
    
  }

  void LinearVtxFilter::_Process_(const larocv::Cluster2DArray_t& clusters,
				  const ::cv::Mat& img,
				  larocv::ImageMeta& meta,
				  larocv::ROI& roi)
  {
    const auto& defectcluster_data = AlgoData<DefectClusterData>(this->ID() - 1);
    const auto& atomic_defect_pts_v_v = defectcluster_data._atomic_defect_pts_v_v_v[meta.plane()];

    auto& data = AlgoData<LinearVtxFilterData>();
    auto& filter_vtx_v = data._filter_vtx_v_v[meta.plane()];
      
    cv::Mat thresh_img;
    cv::threshold(img,thresh_img,_thresh,255,0);
    
    for(uint track_cidx = 0; track_cidx < atomic_defect_pts_v_v.size(); ++ track_cidx) {
      const auto& atomic_defect_pts_v = atomic_defect_pts_v_v[track_cidx];

      //for each defect point
      for (const auto& defect_pt : atomic_defect_pts_v) {

	//determine this linearity
	if ( !ScanRadii(thresh_img,defect_pt) )continue;

	filter_vtx_v.emplace_back(defect_pt.x,defect_pt.y); //typcast in to float
      }
    }
    
  }

  bool LinearVtxFilter::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    return true;
  }
  
  
}
#endif
