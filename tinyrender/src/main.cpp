#include "tgaimage.h"
#include "canvas.h"
#include "model.h"

static const size_t width = 5000;
static const size_t height = 5000;

int main() {
  Canvas canvas(width, height);
  Model model("src/obj/african_head.obj");
  canvas.model(model);
  canvas.write_tga_file("output.tga");
}
