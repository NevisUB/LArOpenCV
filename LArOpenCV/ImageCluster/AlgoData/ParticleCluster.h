#ifndef __PARTICLE_CLUSTER_H__
#define __PARTICLE_CLUSTER_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/Vector.h"
#include <array>

/*
  @brief: ParticleCluster.h holds a contour and may represent a unique particle
*/
namespace larocv {
  namespace data {
    
    enum class ParticleType_t : unsigned
    { kUnknown=0, kTrack, kShower };
    
    /**
       \class ParticleCluster
       @brief A cluster associated with a vertex and hence represents a particle
    */
    
    class ParticleCluster : public AlgoDataArrayElementBase {
    public:
      ParticleCluster() : AlgoDataArrayElementBase()
      { Clear(); }
      ~ParticleCluster() {}
      
      void _Clear_() {
	type=ParticleType_t::kUnknown;
	_ctor.clear();
	_vertex_dqds.clear();
	_truncated_dqds.clear();
	_angle_scan_end_point.x = -9999;
	_angle_scan_end_point.y = -9999;
	_atom_end_point.x = -9999;
	_atom_end_point.y = -9999;
	_atom.clear();
	_supercluster_v.clear();
      }
      
      ParticleType_t type;    ///< particle type from enum
      GEO2D_Contour_t _ctor;  ///< contour to define a cluster

      //
      // START: Ruis stuff
      //
      GEO2D_Particle_t _atom; ///< Pts inside the particle contour

      // 2D angle
      double _angle;
      double _par_angle;
      double _adiff;

      geo2d::Line<float> _pca;
      geo2d::Circle<float> _circle;

      // dQdS
      float _dqds_mean;
      std::vector<float> _vertex_dqds;
      std::vector<float> _truncated_dqds;
      geo2d::Vector<float> _angle_scan_end_point; // last scanned radius from angleanalysis
      geo2d::Vector<float> _atom_end_point;       // angle edge point w.r.t the vertex pt

      //VertexParticleClusterMaker
      std::vector<GEO2D_Contour_t> _supercluster_v;// to be removed
      GEO2D_Contour_t _mergedctor;// to be removed
      
      float _dedx;
      //
      // END: Ruis stuff
      //
      
    };

    /**
       \class Particle
       @brief A group of particle clusters associated together to make an interaction
    */
    class Particle : public AlgoDataArrayElementBase {
    public:
      Particle() : AlgoDataArrayElementBase()
      { Clear(); }
      ~Particle() {}
      void _Clear_() { type = ParticleType_t::kUnknown; _par_v.clear(); _par_v.resize(3);}

      ParticleType_t type; //< type of particle
      
      std::vector<ParticleCluster> _par_v; ///< Particle cluster per plane, can be empty
      float score; ///< score
      
    };
    
    /**
       \class ParticleClusterArray
    */
    typedef AlgoDataArrayTemplate<Particle> ParticleArray;
    typedef AlgoDataArrayTemplate<ParticleCluster> ParticleClusterArray;
 
  }
}
#endif
/** @} */ // end of doxygen group
