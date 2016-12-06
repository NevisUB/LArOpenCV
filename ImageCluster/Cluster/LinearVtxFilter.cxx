#ifndef __LINEARVTXFILTER_CXX__
#define __LINEARVTXFILTER_CXX__

#include "LinearVtxFilter.h"

namespace larocv {

  /// Global larocv::LinearVtxFilterFactory to register AlgoFactory
  static LinearVtxFilterFactory __global_LinearVtxFilterFactory__;

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
  
  
  geo2d::VectorArray<float>
  LinearVtxFilter::QPtOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle)
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
      if(q < _thresh) {
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
    if(range.first>=0 && range.second>=0) range_v.push_back(range);
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
    _r_min     = pset.get<float>("RMin",3);
    _r_max     = pset.get<float>("RMax",13);
    _r_div     = pset.get<float>("RDiv",10);
    _r_cut     = pset.get<float>("RCut",0.5);
    _angle_cut = pset.get<float>("AngleCut",165); // 15 degree angle...
    
    _r_width  = pset.get<int>("CovWidth" ,4);
    _r_height = pset.get<int>("CovHeight",4);
    
    _thresh = pset.get<int>("Threshold",10);
    
    auto const refine2d_algo_name = pset.get<std::string>("Refine2DVertexAlgo","refine_vtx");
    _refine2d_algo_id = this->ID(refine2d_algo_name);

    _kink_threshold = pset.get<int>("NKinks",1);
    
    _radii_v.resize(_r_div);
    
    float step = (_r_max - _r_min) / _r_div;
    LAROCV_DEBUG() << "step : " << step << std::endl;
    
    for(int ctr=0; ctr <= _r_div; ++ctr)
      _radii_v[ctr] = _r_min + step*((float) ctr);
    
  }

  
  std::vector<std::vector<geo2d::Vector<float> > >
  LinearVtxFilter::ScanRadiiQpts(const cv::Mat& img, const cv::Point_<float>& pt) {
    
    std::vector<float> xs_dist;
    LAROCV_DEBUG() << "Observing point : " << pt << std::endl;
    
    std::vector<std::vector<geo2d::Vector<float> > > qpt_vv;

    int start=-1;
    
    for(uint ridx=0; ridx < _radii_v.size(); ++ridx) {
      auto radii = _radii_v[ridx];

      geo2d::Circle<float> circ(pt,radii);
      auto xs_v = QPtOnCircle(img,circ);

      LAROCV_DEBUG() << "ridx: " << ridx << " w/ xs_v size: " << xs_v.size() << std::endl;
      LAROCV_DEBUG() << "Radii: " << radii << "... qpt_vv size: " << qpt_vv.size() << std::endl;

      if (xs_v.size()>0 and start<0) start=1;
      
      if ( start == 1 ) {
	qpt_vv.resize(xs_v.size());
	for(uint xidx=0;xidx<xs_v.size();++xidx)
	  qpt_vv[xidx].emplace_back(xs_v[xidx]);

	start = 0;
	LAROCV_DEBUG() << "start found, resizing" << std::endl;
	continue;
      }

      LAROCV_DEBUG() << "ridx: " << ridx << " w/ xs_v size: " << xs_v.size() << std::endl;	
      
      for(uint xidx=0;xidx<xs_v.size();++xidx) {
	
	auto xs = xs_v[xidx];
	
	int min_qidx=-1;
	float min_d=6.e6;

	for(uint qidx=0;qidx<qpt_vv.size();++qidx) {
	  auto d = geo2d::dist(xs,qpt_vv[qidx].back());
	  if (d < min_d) { min_qidx = qidx; min_d = d; }
	}

	if (min_qidx < 0) throw larbys("min_qidx < 0 still, die");
	
	if ( min_d > _r_cut * radii  ) {
	  LAROCV_DEBUG() << "Far away from all others with cut: " << _r_cut*radii << "... inserting pt: " << xs << std::endl;
	  std::vector<geo2d::Vector<float> > tmp = { xs };
	  qpt_vv.emplace_back(std::move(tmp));
	  continue;
	}
	
	LAROCV_DEBUG() << "Inserting pt: " << xs << "... at idx: " << min_qidx << std::endl;
	qpt_vv[min_qidx].emplace_back(xs);
      } //end loop over xs_v
      
    } //end loop over radii

    LAROCV_DEBUG() << "Returning qpt_vv.size(): " << qpt_vv.size() << std::endl;
    return qpt_vv;
  }

  void LinearVtxFilter::DetermineQPointAngle(data::CircleSetting& circ_set) {
    
    const auto& qpt_vv = circ_set._xs_vv;
    LAROCV_DEBUG() << "Got qptvv xs_vv size: " << qpt_vv.size() << std::endl;
    
    //no qpoint associations found
    if (qpt_vv.size() == 0) return;

    std::stringstream ss;
    ss << "{";
    for(const auto& qpt_v : qpt_vv)
      ss << qpt_v.size() << ",";
    ss << "}";
    LAROCV_DEBUG() << "Inner sizes : " << ss.str() << std::endl;
    
    std::vector<geo2d::Line<float> > qpt_dir_v;
    
    qpt_dir_v.resize(qpt_vv.size());
    for(uint qidx=0;qidx<qpt_vv.size();++qidx) {
      auto& qpt_v = qpt_vv[qidx];
      
      if (qpt_v.size() <= 1) continue;
      
      qpt_dir_v[qidx] = calculate_pca(qpt_v);
      
      auto direction = qpt_v.back() - qpt_v.front();
      if (direction.x < 0) qpt_dir_v[qidx].dir *= -1.0;
      
    }

    //we need need the two largest d1 && d2
    int qidx1=-1;
    int qidx2=-1;
    int qidx1_size=-1;
    int qidx2_size=-1;

    //get the index of largest
    for(uint qidx=0;qidx<qpt_vv.size();++qidx) {
      int s=qpt_vv[qidx].size();
      if (s>qidx1_size) { qidx1_size=s; qidx1=qidx; }
    }
    
    //get the index of the second largest
    for(uint qidx=0;qidx<qpt_vv.size();++qidx) {
      if (qidx==qidx1) continue;
      int s=qpt_vv[qidx].size();
      if (s>qidx2_size) { qidx2_size=s; qidx2=qidx; }
    }

    LAROCV_DEBUG() << "qidx1: " << qidx1 << "... qidx2: " << qidx2 << std::endl;
    
    if (qidx1<0) throw larbys("qidx1 NOT FOUND");

    if (qidx2<0) return;

    const auto& d1=qpt_dir_v[qidx1].dir;
    const auto& d2=qpt_dir_v[qidx2].dir;
    
    LAROCV_DEBUG() << "(d1, d2): (" << d1 << ", " << d2 << ")" << std::endl;
    
    if (d1.x==0 and d1.y==0) return;
    if (d2.x==0 and d2.y==0) return;
    
    float angle = acos(d1.dot(d2))*180/3.14159;
    
    LAROCV_DEBUG() << "...angle: " << angle << std::endl;

    circ_set._idx1=qidx1;
    circ_set._idx2=qidx2;
    circ_set._angle = angle;
  }


  cv::Rect
  LinearVtxFilter::edge_aware_box(const cv::Mat& img,geo2d::Vector<float> center,int hwidth,int hheight)
  {
    geo2d::Vector<int> tl(center.x-hwidth,center.y-hheight);

    int width  = 2*hwidth+1;
    int height = 2*hheight+1;
    
    if (tl.x < 0) tl.x = 0;
    if (tl.y < 0) tl.y = 0;

    if (tl.x > img.cols - height -1) tl.x = img.cols - height - 1;
    if (tl.y > img.rows - width  -1) tl.y = img.rows - width  - 1;
    
    return cv::Rect(tl.x,tl.y,width,height);
  }
  
  
  void LinearVtxFilter::DetermineQPointR(const cv::Mat& img,
					 data::CircleSetting& circ_set,
					 const geo2d::Vector<float>& center) {

    auto small_bbox = edge_aware_box(img,center,_r_width,_r_height);
	  
    LAROCV_DEBUG() << "Small bbox calculated: " << small_bbox << std::endl;
    LAROCV_DEBUG() << "Image dimensions : [" << img.rows << "," << img.cols << "]" << std::endl;
	
    auto small_img  = cv::Mat(img,small_bbox);
    cv::Mat thresh_small_img;
    cv::threshold(small_img,thresh_small_img,10,1,CV_THRESH_BINARY);
	
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);

    float mean_x(0),mean_y(0);

    for(auto& pt : points) { mean_x += pt.x; mean_y += pt.y; }

    mean_x/=(float)points.size();
    mean_y/=(float)points.size();
    
    float std_x(0.0),std_y(0.0),std_xy(0.0);	

    for(auto& pt : points) {
      std_x  += (pt.x - mean_x)*(pt.x - mean_x);
      std_y  += (pt.y - mean_y)*(pt.y - mean_y);
      std_xy += (pt.y - mean_y)*(pt.x - mean_x);
    }

    std_x=sqrt(std_x);
    std_y=sqrt(std_y);

    float r = 0.;

    if ( std_x > 0 and std_y > 0 )
      r = std_xy / ( std_x * std_y );
    else
      r=1.0; // no variation, it's a straight line
    
    r = std::abs(r);

    LAROCV_DEBUG() << "Set local r " << r << std::endl;
    circ_set._local_r = r;
    
  }
  
  void LinearVtxFilter::_Process_(const larocv::Cluster2DArray_t& clusters,
				  const ::cv::Mat& img,
				  larocv::ImageMeta& meta,
				  larocv::ROI& roi)
  {}

  bool LinearVtxFilter::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    
    auto& data = AlgoData<data::LinearVtxFilterData>();
    
    const auto& refine2d_data = AlgoData<data::Refine2DVertexData>(_refine2d_algo_id);
    const auto& vtx3d_v = refine2d_data.get_vertex(); //indices: 3d vtx id

    size_t n_vtx = vtx3d_v.size();

    data.Resize(n_vtx);
    auto& circle_setting_array_v = data._circle_setting_array_v;
    
    data._r_height= _r_height;
    data._r_width = _r_width;
    data._radii_v = _radii_v;
    
    std::vector<cv::Mat> thresh_img_v;
    thresh_img_v.resize(img_v.size());
    
    for(uint img_id=0;img_id<img_v.size();++img_id)
      cv::threshold(img_v[img_id],thresh_img_v[img_id],_thresh,255,0);
    
    for(size_t vtx3d_id=0; vtx3d_id < n_vtx; ++vtx3d_id){

      //get this circle setting array
      auto& circle_setting_array = circle_setting_array_v[vtx3d_id];
      
      //this 3d vertex
      const auto& vtx3d = vtx3d_v[vtx3d_id];

      for(short plane_id = 0; plane_id < 3; ++plane_id) {
       
	//get this circle vertex
	data::CircleVertex circle_vtx;
	
	try{ circle_vtx = refine2d_data.get_circle_vertex(vtx3d_id,plane_id); }
	//catch invalid plane (no circle vtx on this plane)
	catch(...){ continue; }
	
	//get this circle setting
	auto& circle_setting = circle_setting_array.get_circle_setting(plane_id);
	
	//get the associated 2D thresholded image
	const auto& img = thresh_img_v.at(plane_id);

	// std::stringstream ss;
	// ss << plane_id << ".png";
	// cv::imwrite(ss.str().c_str(),img);
	
	// determine these crossing point-paths
	
	circle_setting._xs_vv = ScanRadiiQpts(img,circle_vtx.center);
	
	//get the two longest paths, compute PCA, angle between them
	DetermineQPointAngle(circle_setting);

	//in the local area, compute the R value
	DetermineQPointR    (img,circle_setting,circle_vtx.center);
	
      }

    }
    
    return true;
  }
  
  
}
#endif
