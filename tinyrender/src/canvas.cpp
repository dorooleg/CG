#include "canvas.h"
#include <utility>
#include <iostream>

Canvas::Canvas(size_t w, size_t h) : image_(w, h, TGAImage::RGB) {}

void Canvas::set(int x, int y, const TGAColor& color) {
  image_.set(x, y, color);
}

void Canvas::model(const Model& model) {
  Vec3f light_dir(0, 0, -1);
  for (int i = 0; i < model.nfaces(); i++) {
    std::vector<int> face = model.face(i);
    Vec2i screen_coords[3];
    Vec3f world_coords[3];
    for (size_t j = 0; j < 3; j++) {
      Vec3f v = model.vert(face[j]);
      screen_coords[j] =                                 // NOLINT
          Vec2i((v.x + 1.) * image_.get_width() / 2.,    // NOLINT
                (v.y + 1.) * image_.get_height() / 2.);  // NOLINT
      world_coords[j] = v;                               // NOLINT
    }
    Vec3f n = (world_coords[2] - world_coords[0]) ^
              (world_coords[1] - world_coords[0]);
    n.normalize();
    float intensity = n * light_dir;
    if (intensity > 0) {
      triangle(
          screen_coords[0], screen_coords[1], screen_coords[2],
          TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
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

void Canvas::line(const Vec2i& p1, const Vec2i& p2, const TGAColor& color) {
  line(p1.x, p1.y, p2.x, p2.y, color);  // NOLINT
}

void Canvas::write_tga_file(const std::string& filename) {
  image_.flip_vertically();
  image_.write_tga_file(filename.c_str());
  image_.flip_vertically();
}

void Canvas::triangle(Vec2i t0, Vec2i t1, Vec2i t2, const TGAColor& color) {
  if (t0.y == t1.y && t0.y == t2.y)  // NOLINT
    return;                          // i dont care about degenerate triangles

  if (t0.y > t1.y)  // NOLINT
    std::swap(t0, t1);
  if (t0.y > t2.y)  // NOLINT
    std::swap(t0, t2);
  if (t1.y > t2.y)  // NOLINT
    std::swap(t1, t2);
  int total_height = t2.y - t0.y;                    // NOLINT
  for (int y = t0.y; y <= t1.y; y++) {               // NOLINT
    int segment_height = t1.y - t0.y + 1;            // NOLINT
    float alpha = (float)(y - t0.y) / total_height;  // NOLINT
    float beta = (float)(y - t0.y) /                 // NOLINT
                 segment_height;  // be careful with divisions by zero
    Vec2i A = t0 + (t2 - t0) * alpha;
    Vec2i B = t0 + (t1 - t0) * beta;
    if (A.x > B.x)  // NOLINT
      std::swap(A, B);
    for (int j = A.x; j <= B.x; j++) {  // NOLINT
      set(j, y, color);  // attention, due to int casts t0.y+i != A.y
    }
  }

  for (int y = t1.y; y <= t2.y; y++) {               // NOLINT
    int segment_height = t2.y - t1.y + 1;            // NOLINT
    float alpha = (float)(y - t0.y) / total_height;  // NOLINT
    float beta = (float)(y - t1.y) /                 // NOLINT
                 segment_height;  // be careful with divisions by zero
    Vec2i A = t0 + (t2 - t0) * alpha;
    Vec2i B = t1 + (t2 - t1) * beta;
    if (A.x > B.x)  // NOLINT
      std::swap(A, B);
    for (int j = A.x; j <= B.x; j++) {  // NOLINT
      set(j, y, color);  // attention, due to int casts t0.y+i != A.y
    }
  }
}
