#ifndef __MATCHALGOBASE_CXX__
#define __MATCHALGOBASE_CXX__

#include "MatchAlgoBase.h"

namespace larocv {

  void MatchAlgoBase::ClearMatch() {
    LAROCV_DEBUG() << "start" << std::endl;
    for(auto& v : _pcluster_vv)
      v.clear();

    _MatchBookKeeper.Reset();
    _seed_v.clear();
    for(auto& v : _clusters_per_plane_vv) v.clear();
    for(auto& v : _valid_plane_v) v = false;

    _particle_ptr_v.clear();
    _particle_id_to_plane_v.clear();
    _match_v.clear();
    LAROCV_DEBUG() << "end" << std::endl;
  }

  void MatchAlgoBase::ClearEvent() {
    LAROCV_DEBUG() << "start" << std::endl;
    ClearMatch();
    for(auto& v : _img_v)  v = nullptr;
    for(auto& v : _meta_v) v = nullptr;
    LAROCV_DEBUG() << "end" << std::endl;
  }

  void MatchAlgoBase::Register(const data::ParticleCluster& pc,size_t plane) {
    _pcluster_vv.at(plane).push_back(&pc);
    _valid_plane_v[plane] = true;
    LAROCV_DEBUG() << "@plane=" << plane << " have " << _pcluster_vv.at(plane).size() << " clusters" << std::endl;
  }

  void MatchAlgoBase::Register(const cv::Mat& img, size_t plane) {
    _img_v.at(plane) = &img;
  }

  void MatchAlgoBase::Register(const ImageMeta& meta, size_t plane) {
    _meta_v.at(plane) = &meta;
  }

  const data::ParticleCluster* MatchAlgoBase::Particle(size_t plane,size_t id) {
    return _pcluster_vv.at(plane).at(id);
  }

  std::vector<std::vector<std::pair<size_t,size_t> > >
  MatchAlgoBase::Match() {

    std::vector<std::vector<std::pair<size_t,size_t> > > match_vv;

    assert(_seed_v.empty());
    _seed_v.resize(3);
    for(size_t plane = 0; plane < 3; ++plane)  {
      _seed_v[plane] = _pcluster_vv.at(plane).size();
      LAROCV_DEBUG() << "@plane=" << plane << " sz=" << _seed_v[plane] << std::endl;
    }

    size_t offset=0;
    for(size_t plane=0;plane<3;++plane) {
      auto& clusters_per_plane_v = _clusters_per_plane_vv[plane];
      const auto& pars_v = _pcluster_vv.at(plane);
      for(size_t parid=0; parid<pars_v.size(); ++parid) {
	const auto par = pars_v[parid];
	clusters_per_plane_v.push_back(parid+offset);
	_particle_ptr_v.push_back(par);
	_particle_id_to_plane_v.emplace_back(plane,parid);
      }
      offset += pars_v.size();
    }

    std::vector<std::vector<std::pair<size_t,size_t>>> temp_comb_vv, comb_vv;
    
    temp_comb_vv = PlaneClusterCombinations(_seed_v);
    comb_vv.reserve(temp_comb_vv.size());

    for(auto& temp_comb_v : temp_comb_vv) {
      if (temp_comb_v.size()==2) {
	if (!_valid_plane_v[temp_comb_v[0].first]) continue;
	if (!_valid_plane_v[temp_comb_v[1].first]) continue;
      }

      if (temp_comb_v.size()==3) {
	if (!_valid_plane_v[temp_comb_v[0].first]) continue;
	if (!_valid_plane_v[temp_comb_v[1].first]) continue;
	if (!_valid_plane_v[temp_comb_v[2].first]) continue;
      }
      comb_vv.emplace_back(std::move(temp_comb_v));
    }
    

    LAROCV_DEBUG()<<"--> Calculated " << comb_vv.size() << " combinations" << std::endl;
    
    for(const auto& comb_v : comb_vv) {
      LAROCV_DEBUG() << "@comb sz= "<< comb_v.size() << std::endl;
      if (comb_v.size()==2) {

	LAROCV_DEBUG() << "@2" << std::endl;
	auto plane0=comb_v[0].first;
	auto plane1=comb_v[1].first;
	
	LAROCV_DEBUG() << "planes=(" << plane0 << "," << plane1 << ")" << std::endl;

	auto cid0 = comb_v[0].second;
	auto cid1 = comb_v[1].second;

	auto id0 = _clusters_per_plane_vv[plane0][cid0];
	auto id1 = _clusters_per_plane_vv[plane1][cid1];
	
	auto score = this->Match(*(_img_v.at(plane0)),
				 *(_img_v.at(plane1)),
				 *(_meta_v.at(plane0)),
				 *(_meta_v.at(plane1)),
				 *(_particle_ptr_v.at(id0)),
				 *(_particle_ptr_v.at(id1)));
	
	_match_v.clear();
	_match_v.resize(2);
	_match_v[0] = id0;
	_match_v[1] = id1;
	
	if (plane0 == 2 or plane1 == 2)
	  score *= _plane_two_boost;

	LAROCV_DEBUG() << "score=" << score << std::endl;
	if (score<_threshold) continue;
	LAROCV_DEBUG() << "...pass" << std::endl;
	
	_MatchBookKeeper.Match(_match_v,score);
      }
      else if (comb_v.size()==3 && _match_three_planes) {
	LAROCV_DEBUG() << "@3" << std::endl;

	auto plane0 = comb_v[0].first;
	auto plane1 = comb_v[1].first;
	auto plane2 = comb_v[2].first;

	LAROCV_DEBUG() << "planes=(" << plane0 << "," << plane1 << "," << plane2 << ")" << std::endl;

	auto cid0 = comb_v[0].second;
	auto cid1 = comb_v[1].second;
	auto cid2 = comb_v[2].second;

	auto id0 = _clusters_per_plane_vv[plane0][cid0];
	auto id1 = _clusters_per_plane_vv[plane1][cid1];
	auto id2 = _clusters_per_plane_vv[plane2][cid2];

	LAROCV_DEBUG() << "ids=(" << id0 << "," << id1 << "," << id2 << ")" << std::endl;

	auto score = this->Match(*(_img_v.at(plane0)),
				 *(_img_v.at(plane1)),
				 *(_img_v.at(plane2)),
				 *(_meta_v.at(plane0)),
				 *(_meta_v.at(plane1)),
				 *(_meta_v.at(plane2)),
				 *(_particle_ptr_v.at(id0)),
				 *(_particle_ptr_v.at(id1)),
				 *(_particle_ptr_v.at(id2)));
	
	_match_v.clear();
	_match_v.resize(3);
	_match_v[0] = id0;
	_match_v[1] = id1;
	_match_v[2] = id2;

	score *= _three_planes_boost;

	LAROCV_DEBUG() << "score=" << score << std::endl;
	if (score<_threshold) continue;
	LAROCV_DEBUG() << "...pass" << std::endl;

	_MatchBookKeeper.Match(_match_v,score);
      }
      else {
	throw larbys("Observed unhandled combination in algorithm");
      }

    } // end combos

    LAROCV_DEBUG() << "Found " << _MatchBookKeeper.GetResult().size() << " particle matches" << std::endl;
    
    match_vv.reserve(_MatchBookKeeper.GetResult().size());
    
    std::vector<std::pair<size_t,size_t> > res_match_v;    
    for(const auto& match : _MatchBookKeeper.GetResult()) {
      res_match_v.clear();
      res_match_v.reserve(match.size()); // the number of matches contours
      for(auto par_id : match) 
	res_match_v.emplace_back(_particle_id_to_plane_v[par_id]);

      match_vv.emplace_back(std::move(res_match_v));
    }

    return match_vv;
  } // end match()

}


#endif
