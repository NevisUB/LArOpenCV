#ifndef __ALGOFUNCTIONTYPES_H__
#define __ALGOFUNCTIONTYPES_H__

namespace larocv {
  namespace data {

    struct VertexTrackInfoCollection {

      Vertex3D vtx3d;
      std::vector<const Parti
      std::vector<std::vector<const ParticleCluster*> >      part_v;
      std::vector<std::vector<const TrackClusterCompound*> > comp_v;
      

    };
  }
}


