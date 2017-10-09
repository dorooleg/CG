#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <model.h>
#include <algorithm.h>

Model::Model(const std::string& filename) {
  std::ifstream in(filename, std::ifstream::in);

  if (in.fail()) {
    return;
  }

  std::string line;
  float norm = 0.f;
  while (!in.eof()) {
    std::getline(in, line);
    trim(line);
    std::istringstream iss(line);

    char trash;
    if (line.compare(0, 2, "v ") == 0) {
      iss >> trash;
      Vec3f v;
      for (float& i : v.raw) {  // NOLINT
        iss >> i;
        norm = std::max(norm, std::abs(i));
      }
      verts_.push_back(v);
    } else if (line.compare(0, 2, "f ") == 0) {
      std::vector<int> f;
      int idx;
      iss >> trash;
      while (iss >> idx) {
        idx--;  // in wavefront obj all indices start at 1, not zero
        f.push_back(idx);
        for (size_t i = 0; i < 2; i++) {
          if (iss.get() == '/') {
            iss >> idx;
          } else {
            iss.unget();
          }
        }
      }
      faces_.push_back(f);
    }
  }

  if (norm > 1.0) {
    for (auto& v : verts_) {
      for (float& i : v.raw) {  // NOLINT
        i /= norm;
      }
    }
  }

  std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

int Model::nverts() const {
  return static_cast<int>(verts_.size());
}

int Model::nfaces() const {
  return static_cast<int>(faces_.size());
}

std::vector<int> Model::face(int idx) const {
  return faces_[idx];
}

Vec3f Model::vert(int i) const {
  return verts_[i];
}
