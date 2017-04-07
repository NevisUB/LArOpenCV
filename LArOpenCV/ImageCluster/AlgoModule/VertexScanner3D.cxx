#ifndef __VERTEXSCANNER3D_CXX__
#define __VERTEXSCANNER3D_CXX__

#include "VertexScanner3D.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrack.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  /// Global VertexScanner3DFactory to register AlgoFactory
  static VertexScanner3DFactory __global_VertexScanner3DFactory__;

  void VertexScanner3D::_Configure_(const Config_t &pset)
  {
    _VertexScan3D.set_verbosity(this->logger().level());
    _VertexScan3D.Configure(pset.get<Config_t>("VertexScan3D"));

    auto name_vertex3d      = pset.get<std::string>("Vertex3DName", "");
    auto name_super_cluster = pset.get<std::string>("SuperClusterName");
    auto name_linear_track  = pset.get<std::string>("LinearTrackName");
    auto name_compound      = pset.get<std::string>("TrackParticleName", "");

    if (!name_vertex3d.empty()) {
      _vertex3d_id      = this->ID(name_vertex3d);
      if (_vertex3d_id == kINVALID_ALGO_ID) {
        LAROCV_CRITICAL() << "Vertex3D algorithm name does not exist!" << std::endl;
        throw larbys();
      }
    }

    if (!name_compound.empty()) {
      _compound_id      = this->ID(name_compound);
      if (_compound_id == kINVALID_ALGO_ID) {
        LAROCV_CRITICAL() << "TrackParticle algorithm name does not exist!" << std::endl;
        throw larbys();
      }
    }

    _super_cluster_id = this->ID(name_super_cluster);
    _linear_track_id  = this->ID(name_linear_track);

    Register(new data::VertexSeed3DArray);
  }

  bool VertexScanner3D::_PostProcess_(std::vector<cv::Mat>& img_v)
  {
    _vtx3d_cands_v.clear();
    _seed_cands_v.clear();
    
    std::vector<const data::ParticleClusterArray*> super_cluster_v;
    std::vector<const data::ParticleClusterArray*> part_cluster_v;
    std::vector<const data::TrackClusterCompoundArray*> compound_v;

    for (size_t plane = 0; plane < img_v.size(); ++plane) {
      super_cluster_v.push_back(&(AlgoData<data::ParticleClusterArray>(_super_cluster_id, plane)));
      if (_compound_id != kINVALID_ALGO_ID) {
        part_cluster_v.push_back(&(AlgoData<data::ParticleClusterArray>(_compound_id, plane)));
        compound_v.push_back(&(AlgoData<data::TrackClusterCompoundArray>(_compound_id, plane + img_v.size())));
      }
    }

    std::vector<data::VertexTrackInfoCollection> input_vtxinfo_v;
    if (_vertex3d_id != kINVALID_ALGO_ID) {
      auto const& input_vtx3d_v = AlgoData<data::Vertex3DArray>(_vertex3d_id, 0);
      input_vtxinfo_v = data::OrganizeVertexInfo(AssManager(),
						 input_vtx3d_v,
						 super_cluster_v,
						 part_cluster_v,
						 compound_v);
    }

    LAROCV_INFO() << "input_vtxinfo_v size " << input_vtxinfo_v.size() << std::endl;
    for (size_t idx = 0; idx < input_vtxinfo_v.size(); ++idx) {
      auto const & res = input_vtxinfo_v[idx];
      LAROCV_INFO() << "Track vertex " << idx
                    << " @ (" << res.vtx3d->x << "," << res.vtx3d->y << "," << res.vtx3d->z << ") w/ nplanes " << res.vtx3d->vtx2d_v.size() << std::endl;
      LAROCV_DEBUG() << "... at address " << res.vtx3d << std::endl;
      
    }
    
    auto const& linear_track_data = AlgoData<data::LinearTrack3DArray>(_linear_track_id, 0);

    for (const auto& ltrack : linear_track_data.as_vector()) {
      if (ltrack.edge1.vtx2d_v.size()) {
        LAROCV_INFO() << "Registering LinearTrack edge1 @ (x,y,z)=(" 
        << ltrack.edge1.x << "," << ltrack.edge1.y << "," << ltrack.edge1.z << ")" << std::endl;
        RegisterSeed(ltrack.edge1, data::SeedType_t::kEdge, img_v);
      }
      if (ltrack.edge2.vtx2d_v.size()) {
        LAROCV_INFO() << "Registering LinearTrack edge2 @ (x,y,z)=(" 
        << ltrack.edge2.x << "," << ltrack.edge2.y << "," << ltrack.edge2.z << ")" << std::endl;
        RegisterSeed(ltrack.edge2, data::SeedType_t::kEdge, img_v);
      }
    }
    RegisterSeed(input_vtxinfo_v,img_v);

    std::multimap<double,larocv::data::VertexSeed3D> seed_cands_m;
    LAROCV_DEBUG() << "See " << _seed_cands_v.size() << " candidates" << std::endl;
    
    for(auto const& cand_vtx3d : _seed_cands_v) {
      auto vtx3d = _VertexScan3D.RegionScan3D(cand_vtx3d, img_v);
      size_t num_good_plane=0;
      double dtheta_sum=0;
      for(auto const& cvtx2d : vtx3d.cvtx2d_v) {
        if(cvtx2d.xs_v.size()<2) continue;
        num_good_plane++;
        dtheta_sum += cvtx2d.sum_dtheta();
      }
      if(num_good_plane<2) continue;
      dtheta_sum /= (double)num_good_plane;
      LAROCV_DEBUG() << "Registering vertex seed type="<<(uint)cand_vtx3d.type
		     <<" @ ("<<vtx3d.x<<","<<vtx3d.y<<","<<vtx3d.z<<")"<<std::endl;

      data::VertexSeed3D seed(vtx3d);
      seed.type = cand_vtx3d.type;
      seed_cands_m.emplace(dtheta_sum, seed);
      _vtx3d_cands_v.emplace_back(std::move(vtx3d));
    } // end candidate vertex seed

    auto& data = AlgoData<data::VertexSeed3DArray>(0);
    for(auto const& score_vtx3d : seed_cands_m) {
      double min_dist=1.e9;
      auto const& cand_vtx3d = score_vtx3d.second;
      LAROCV_DEBUG() << "inspect vertex seed type="<<(uint)cand_vtx3d.type
		     <<" @("<<cand_vtx3d.x<<","<<cand_vtx3d.y<<","<<cand_vtx3d.z<<")"<<std::endl;
      bool invalid=false;
      for(auto const& vtx3d : data.as_vector()) {
	LAROCV_DEBUG() <<"... compared @("<<cand_vtx3d.x<<","<<cand_vtx3d.y<<","<<cand_vtx3d.z<<")"<<std::endl;
        double dx = std::fabs(cand_vtx3d.x - vtx3d.x);
        double dy = std::fabs(cand_vtx3d.y - vtx3d.y);
        double dz = std::fabs(cand_vtx3d.z - vtx3d.z);
	LAROCV_DEBUG() << "so (dx,dy,dz)=("<<dx<<","<<dy<<","<<dz<<std::endl;
        if(dx < _VertexScan3D._dx/2.) { invalid=true; break;}
        if(dy < _VertexScan3D._dy/2.) { invalid=true; break;}
        if(dz < _VertexScan3D._dz/2.) { invalid=true; break;}
      }
      if(invalid) continue;
      LAROCV_DEBUG() << "... candidate accepted" << std::endl;
      data.push_back(cand_vtx3d);
    }

    return true;
  }

  void VertexScanner3D::RegisterSeed(const larocv::data::Vertex3D& vtx3d,
				     const data::SeedType_t type,
                                     std::vector<cv::Mat> img_v)
  {
    // >=2 planes must have >1 xs points
    size_t num_valid_planes = 0;
    try { 
      for (size_t plane = 0; plane < img_v.size(); ++plane) {
        auto cvtx = _VertexScan3D.CreateCircleVertex(img_v[plane], vtx3d, plane);
	LAROCV_INFO() << "Plane " << plane << " @ radius " << cvtx.radius << " # xs = " << cvtx.xs_v.size() << std::endl;
        if (cvtx.xs_v.size() > 1) num_valid_planes++;
      }
    }catch(const larbys&){
      LAROCV_INFO() << "Failed to create CircleVertex... skipping this point!" << std::endl;
      return;
    }
    if (num_valid_planes < 2) {
      LAROCV_INFO() << "Seed @ (x,y,z) = ("
                    << vtx3d.x << "," << vtx3d.y << "," << vtx3d.z << ") skipped... (< 2 valid planes)" << std::endl;
      return;
    }
    LAROCV_INFO() << "Seed @ (x,y,z) = ("
                  << vtx3d.x << "," << vtx3d.y << "," << vtx3d.z << ") REGISTERED..." << std::endl;

    data::VertexSeed3D seed(vtx3d);
    seed.type = type;
    _seed_cands_v.push_back(seed);
    return;
  }

  void VertexScanner3D::RegisterSeed(const std::vector<data::VertexTrackInfoCollection>& vtxinfo_v,
                                     std::vector<cv::Mat> img_v)

  {

    std::vector<bool> good_plane_v(_VertexScan3D.num_planes(), false);
    std::vector<larocv::data::Vertex3D> track_vtx_v;

    LAROCV_INFO() << "Attempting register of " << vtxinfo_v.size() << " track vertex infos" << std::endl;
    for (auto const& vtxinfo : vtxinfo_v) {

      for (size_t plane_id = 0; plane_id < good_plane_v.size(); ++plane_id)
        good_plane_v[plane_id] = false;

      // Get useful attribute references
      auto const& particle_vv = vtxinfo.particle_vv;
      auto const& compound_vv = vtxinfo.compound_vv;

      if (particle_vv.size() != _VertexScan3D.num_planes()) {
        LAROCV_CRITICAL() << "# planes for ParticleClusterArray incorrect..." << std::endl;
        throw larbys();
      }
      if (compound_vv.size() != _VertexScan3D.num_planes()) {
        LAROCV_CRITICAL() << "# planes for CompoundClusterArray incorrect..." << std::endl;
        throw larbys();
      }

      LAROCV_INFO() << "Inspecting track vertex seed @ (" << vtxinfo.vtx3d->x << "," << vtxinfo.vtx3d->y << "," << vtxinfo.vtx3d->z << ")" << std::endl;
      if (vtxinfo.vtx3d->vtx2d_v.size() != _VertexScan3D.num_planes()) {
        LAROCV_CRITICAL() << "... this vertex num planes " << vtxinfo.vtx3d->vtx2d_v.size() << " is invalid" << std::endl;
        throw larbys();
      }

      track_vtx_v.push_back(*(vtxinfo.vtx3d));

      // Next fill edge-vertex
      for (size_t plane1 = 0; plane1 < _VertexScan3D.num_planes(); ++plane1) {
        for (size_t plane2 = plane1 + 1; plane2 < _VertexScan3D.num_planes(); ++plane2) {

          std::vector<geo2d::Vector<float> > edge1_v;
          for (auto const& compound : compound_vv[plane1])
            edge1_v.push_back(compound->end_pt());

          std::vector<geo2d::Vector<float> > edge2_v;
          for (auto const& compound : compound_vv[plane2])
            edge2_v.push_back(compound->end_pt());

          // Loop over possible combinations
          larocv::data::Vertex3D pt;
          for (auto const& edge1 : edge1_v) {
            for (auto const& edge2 : edge2_v) {

              if (!_VertexScan3D.geo().YZPoint(edge1, plane1, edge2, plane2, pt))
                continue;

              RegisterSeed(pt, data::SeedType_t::kEnd, img_v);
            }
          }
        }
      }
    }

    for (auto const& pt : track_vtx_v)
      RegisterSeed(pt, data::SeedType_t::kTrack, img_v);

  }

}
#endif
