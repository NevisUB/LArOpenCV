#ifndef __LINEARVTXFILTER_CXX__
#define __LINEARVTXFILTER_CXX__

#include "LinearVtxFilter.h"
#include "DefectCluster.h"

namespace larocv {

  /// Global larocv::LinearVtxFilterFactory to register AlgoFactory
  static LinearVtxFilterFactory __global_LinearVtxFilterFactory__;


  //geo2d::Line<float> LinearVtxFilter::calculate_pca(const GEO2D_Contour_t& ctor) {
  geo2d::Line<float> LinearVtxFilter::calculate_pca(const std::vector<cv::Point_<float> > & ctor) {
    
    cv::Mat ctor_pts(ctor.size(), 2, CV_32FC1); //32 bit precision is fine
    
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<float>(i, 0) = ctor.at(i).x;
      ctor_pts.at<float>(i, 1) = ctor.at(i).y;
    }
    
    cv::PCA pca_ana(ctor_pts, cv::Mat(), CV_PCA_DATA_AS_ROW,0);
    
    geo2d::Line<float> pca_principle(geo2d::Vector<float>(pca_ana.mean.at<float>(0,0),
							  pca_ana.mean.at<float>(0,1)),
				     geo2d::Vector<float>(pca_ana.eigenvectors.at<float>(0,0),
							  pca_ana.eigenvectors.at<float>(0,1)));
    return pca_principle;
  }
  
  
  
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
    _r_min     = pset.get<float>("RMin",5);
    _r_max     = pset.get<float>("RMax",15);
    _r_div     = pset.get<float>("RDiv",10);
    _r_cut     = pset.get<float>("RCut",0.5);
    _angle_cut = pset.get<float>("AngleCut",160); // 20 degree angle...
    
    _thresh = pset.get<int>  ("Threshold",10);

    _radii_v.resize(_r_div);
    
    float step = (_r_max - _r_min) / _r_div;
    LAROCV_DEBUG() << "step : " << step << std::endl;
    
    for(int ctr=0; ctr <= _r_div; ++ctr)
      _radii_v[ctr] = _r_min + step*((float) ctr);
    
  }

  
  bool LinearVtxFilter::ScanRadii(const cv::Mat& img,const cv::Point_<float>& pt) {

    
    std::vector<float> xs_dist;
    LAROCV_DEBUG() << "Observing defect point : " << pt << std::endl;

    std::vector<std::vector<cv::Point_<float> > > qpt_vv;
    // GEO2D_ContourArray_t qpt_vv;    
    
    for(uint ridx=0; ridx < _radii_v.size(); ++ridx) {
      auto radii = _radii_v[ridx];

      geo2d::Circle<float> circ(pt,radii);
      auto xs_v = QPtOnCircle(img,circ);

      LAROCV_DEBUG() << "ridx: " << ridx << " w/ xs_v size: " << xs_v.size() << std::endl;
      LAROCV_DEBUG() << "Radii: " << radii << "... qpt_vv size: " << qpt_vv.size() << std::endl;

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

	LAROCV_DEBUG() << "Inserting pt: " << xs << "... at idx: " << min_qidx << std::endl;
	qpt_vv[min_qidx].emplace_back(xs);
      }
    }


    _xs_vvv.emplace_back(qpt_vv);
    

    std::vector<geo2d::Line<float> > qpt_dir_v;
    qpt_dir_v.resize(qpt_vv.size());
    for(uint qidx=0;qidx<qpt_vv.size();++qidx) {
      auto& qpt_v = qpt_vv[qidx];

      // std::cout << "qidx: " << qidx << "... qpt_v.size(): " << qpt_v.size() << std::endl;
      // std::cout << "np.array([";
      // for(auto& qpt : qpt_v) std::cout << qpt << ",";
      // std::cout << "])" << std::endl;
      
      if (qpt_v.size() <= 1) continue;
      
      qpt_dir_v[qidx] = calculate_pca(qpt_v);

      auto direction = qpt_v.back() - qpt_v.front();
      if (direction.x < 0) qpt_dir_v[qidx].dir *= -1.0;
      
    }

    for(uint qidx1=0;qidx1<qpt_dir_v.size();++qidx1)  { 
      for(uint qidx2=qidx1+1;qidx2<qpt_dir_v.size();++qidx2)  {

	auto& d1=qpt_dir_v[qidx1].dir;
	auto& d2=qpt_dir_v[qidx2].dir;
	
	if (d1.x==0 and d1.y==0) continue;
	if (d2.x==0 and d2.y==0) continue;
	
	float angle = acos(d1.dot(d2))*180/3.14159;
	LAROCV_DEBUG() << "qidx1, qidx2, d1, d2, angle... " << qidx1 << ", " << qidx2 << ", " << d1 << ", " << d2 << ", " << angle << std::endl;

	if (angle < _angle_cut) {
	  LAROCV_DEBUG() << "angle < angle_cut return true" << std::endl;
	  return true;
	}
	
      }
    }
    
    LAROCV_DEBUG() << "linearity detected, returning false" << std::endl;
    
    return false;
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
	bool notlinear = ScanRadii(thresh_img,defect_pt);
	if(!notlinear) continue;
	
	
	filter_vtx_v.emplace_back(defect_pt.x,defect_pt.y); //typcast in to float
      }
    }

    data._xs_v_v_v_v[meta.plane()] = _xs_vvv;
    _xs_vvv.clear();
      
  }

  bool LinearVtxFilter::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    return true;
  }
  
  
}
#endif
