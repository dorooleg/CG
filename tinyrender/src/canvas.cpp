#include "canvas.h"

Canvas::Canvas(size_t w, size_t h) : image_(w, h, TGAImage::RGB) {}

void Canvas::set(int x, int y, const TGAColor& color) {
  image_.set(x, y, color);
}

void Canvas::model(const Model& model) {
  for (size_t i = 0; i < model.nfaces(); i++) {
    std::vector<int> face = model.face(i);
    for (int j = 0; j < 3; j++) {
      Vec3f v0 = model.vert(face[j]);
      Vec3f v1 = model.vert(face[(j + 1) % 3]);
      int x0 = (v0.x + 1.) * image_.get_width() / 2.;   // NOLINT
      int y0 = (v0.y + 1.) * image_.get_height() / 2.;  // NOLINT
      int x1 = (v1.x + 1.) * image_.get_width() / 2.;   // NOLINT
      int y1 = (v1.y + 1.) * image_.get_height() / 2.;  // NOLINT
      line(x0, y0, x1, y1, TGAColor::WHITE);
    }
  }
}

void Canvas::line(int x0, int y0, int x1, int y1, const TGAColor& color) {
  bool steep = false;

  if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }

  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  const float dx = x1 - x0;
  for (int x = x0; x <= x1; x++) {
    const float t = (x - x0) / dx;
    const int y = y0 * (1. - t) + y1 * t;
    image_.set(steep ? y : x, steep ? x : y, color);
  }
}

void Canvas::write_tga_file(const std::string& filename) {
  image_.flip_vertically();
  image_.write_tga_file(filename.c_str());
  image_.flip_vertically();
}
