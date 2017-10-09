#pragma once

#include "tgaimage.h"
#include "model.h"
#include <string>

class Canvas {
 public:
  Canvas(size_t w, size_t h);

  void set(int x, int y, const TGAColor& color);
  void line(int x0, int y0, int x1, int y1, const TGAColor& color);
  void write_tga_file(const std::string& filename);
  void model(const Model& model);

 private:
  TGAImage image_;
};
