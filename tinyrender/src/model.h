#pragma once

#include <vector>
#include "geometry.h"

class Model final {
 public:
  explicit Model(const std::string& filename);

  ~Model() = default;

  int nverts() const;

  int nfaces() const;

  Vec3f vert(int i) const;

  std::vector<int> face(int idx) const;

 private:
  std::vector<Vec3f> verts_;
  std::vector<std::vector<int> > faces_;
};
