#ifndef VOXEL_H
#define VOXEL_H

namespace larocv {

  class Voxel {
  public:
    Voxel(){}
    ~Voxel(){}
    
    std::vector<float> x_v;
    std::vector<float> y_v;
    std::vector<float> z_v;

    size_t nx() const { return x_v.size(); }
    size_t ny() const { return y_v.size(); }
    size_t nz() const { return z_v.size(); }
    
    data::Vertex3D pt(size_t i, size_t j, size_t k) const {
      if (i >= nx()) throw larbys("i out of x range");
      if (j >= ny()) throw larbys("j out of y range");
      if (k >= nz()) throw larbys("k out of y range");

      data::Vertex3D res;
      res.x = x_v[i];
      res.y = y_v[j];
      res.z = z_v[k];
      return res;
    }
    
    float min_x() const { return *(std::min_element(std::begin(x_v),std::end(x_v))); }
    float min_y() const { return *(std::min_element(std::begin(y_v),std::end(y_v))); }
    float min_z() const { return *(std::min_element(std::begin(z_v),std::end(z_v))); }

    float max_x() const { return *(std::max_element(std::begin(x_v),std::end(x_v))); }
    float max_y() const { return *(std::max_element(std::begin(y_v),std::end(y_v))); }
    float max_z() const { return *(std::max_element(std::begin(z_v),std::end(z_v))); }

    bool Overlap(const Voxel& voxel) const {

      if ((this->max_x() < voxel.min_x()) or (this->min_x() > voxel.max_x())) return false;
      if ((this->max_y() < voxel.min_y()) or (this->min_y() > voxel.max_y())) return false;
      if ((this->max_z() < voxel.min_z()) or (this->min_z() > voxel.max_z())) return false;
      
      return true;
    }

    
    Voxel Merge(const Voxel& voxel) {
      Voxel res;

      res.x_v.reserve(this->nx() + voxel.nx());
      res.y_v.reserve(this->ny() + voxel.ny());
      res.z_v.reserve(this->nz() + voxel.nz());
      
      for(auto v : this->x_v) res.x_v.push_back(v);
      for(auto v : this->y_v) res.y_v.push_back(v);
      for(auto v : this->z_v) res.z_v.push_back(v);

      for(auto v : voxel.x_v) res.x_v.push_back(v);
      for(auto v : voxel.y_v) res.y_v.push_back(v);
      for(auto v : voxel.z_v) res.z_v.push_back(v);
      
      return res;
    }
  };

  class VoxelArray {
  public:
    VoxelArray() {}
    VoxelArray(Voxel&& voxel)
    { _voxel_v.emplace_back(voxel); }
    VoxelArray(const Voxel& voxel)
    { _voxel_v.push_back(voxel); }
    ~VoxelArray() {}
    
    const std::vector<Voxel> as_vector() const { return _voxel_v; }
    void emplace_back(Voxel&& voxel)  { _voxel_v.emplace_back(std::move(voxel)); }
    void push_back(const Voxel& voxel) { _voxel_v.push_back(voxel); }
    void reserve(size_t sz) { _voxel_v.reserve(sz); }
    void resize(size_t sz)  { _voxel_v.resize(sz); }
    size_t size() const { return _voxel_v.size(); }
    void clear() { _voxel_v.clear(); }

    size_t nx() const { size_t n=0; for(const auto& v : _voxel_v) n+=v.nx(); return n; }
    size_t ny() const { size_t n=0; for(const auto& v : _voxel_v) n+=v.ny(); return n; }
    size_t nz() const { size_t n=0; for(const auto& v : _voxel_v) n+=v.nz(); return n; }
    
    bool Overlap(const VoxelArray& voxelarray) {
      for(const auto& that_voxel : voxelarray.as_vector()) {
	for(const auto& this_voxel : this->_voxel_v)  {
	  if (this_voxel.Overlap(that_voxel)) {
	    return true;
	  }
	}
      }
      return false;
    }

    VoxelArray Merge(const VoxelArray& voxelarray) {
      VoxelArray resarray;

      resarray.reserve(this->size() + voxelarray.size());

      for(const auto& v : this->_voxel_v) resarray.push_back(v);
      for(const auto& v : voxelarray.as_vector()) resarray.push_back(v);

      return resarray;
    }

  private:
    std::vector<Voxel> _voxel_v;
    
  };
    
  
}  
#endif
