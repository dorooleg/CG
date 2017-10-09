#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <cmath>
#include "tgaimage.h"

const TGAColor TGAColor::WHITE(255, 255, 255, 255);  // NOLINT
const TGAColor TGAColor::RED(255, 0, 0, 255);        // NOLINT

TGAImage::TGAImage() : data(nullptr), width(0), height(0), bytespp(0) {}

TGAImage::TGAImage(int w, int h, int bpp)
    : data(nullptr), width(w), height(h), bytespp(bpp) {
  uint64_t nbytes = width * height * bytespp;  // NOLINT
  data = new unsigned char[nbytes];
  memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage& img) {
  width = img.width;
  height = img.height;
  bytespp = img.bytespp;
  uint64_t nbytes = width * height * bytespp;  // NOLINT
  data = new unsigned char[nbytes];
  memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage() {
  delete[] data;
}

TGAImage& TGAImage::operator=(const TGAImage& img) {
  if (this != &img) {
    delete[] data;
    width = img.width;
    height = img.height;
    bytespp = img.bytespp;
    uint64_t nbytes = width * height * bytespp;  // NOLINT
    data = new unsigned char[nbytes];
    memcpy(data, img.data, nbytes);
  }
  return *this;
}

bool TGAImage::read_tga_file(const char* filename) {
  delete[] data;
  data = nullptr;
  std::ifstream in;
  in.open(filename, std::ios::binary);
  if (!in.is_open()) {
    std::cerr << "can't open file " << filename << "\n";
    in.close();
    return false;
  }
  TGA_Header header{};
  in.read(reinterpret_cast<char*>(&header), sizeof(header));  // NOLINT
  if (!in.good()) {
    in.close();
    std::cerr << "an error occured while reading the header\n";
    return false;
  }
  width = header.width;
  height = header.height;
  bytespp = header.bitsperpixel >> 3;
  if (width <= 0 || height <= 0 ||
      (bytespp != GRAYSCALE && bytespp != RGB && bytespp != RGBA)) {
    in.close();
    std::cerr << "bad bpp (or width/height) value\n";
    return false;
  }
  uint64_t nbytes = bytespp * width * height;  // NOLINT
  data = new unsigned char[nbytes];
  if (3 == header.datatypecode || 2 == header.datatypecode) {
    in.read(reinterpret_cast<char*>(data), nbytes);  // NOLINT
    if (!in.good()) {
      in.close();
      std::cerr << "an error occured while reading the data\n";
      return false;
    }
  } else if (10 == header.datatypecode || 11 == header.datatypecode) {
    if (!load_rle_data(in)) {
      in.close();
      std::cerr << "an error occured while reading the data\n";
      return false;
    }
  } else {
    in.close();
    std::cerr << "unknown file format " << static_cast<int>(header.datatypecode)
              << "\n";
    return false;
  }
  if ((header.imagedescriptor & 0x20) == 0) {
    flip_vertically();
  }
  if ((header.imagedescriptor & 0x10) != 0) {
    flip_horizontally();
  }
  std::cerr << width << "x" << height << "/" << bytespp * 8 << "\n";
  in.close();
  return true;
}

bool TGAImage::load_rle_data(std::ifstream& in) {
  uint64_t pixelcount = width * height;  // NOLINT
  uint64_t currentpixel = 0;
  uint64_t currentbyte = 0;
  TGAColor colorbuffer;
  do {
    unsigned char chunkheader = 0;
    chunkheader = in.get();
    if (!in.good()) {
      std::cerr << "an error occured while reading the data\n";
      return false;
    }
    if (chunkheader < 128) {
      chunkheader++;
      for (int i = 0; i < chunkheader; i++) {
        in.read(reinterpret_cast<char*>(colorbuffer.raw), bytespp);  // NOLINT
        if (!in.good()) {
          std::cerr << "an error occured while reading the header\n";
          return false;
        }
        for (int t = 0; t < bytespp; t++) {
          data[currentbyte++] = colorbuffer.raw[t];  // NOLINT
        }
        currentpixel++;
        if (currentpixel > pixelcount) {
          std::cerr << "Too many pixels read\n";
          return false;
        }
      }
    } else {
      chunkheader -= 127;
      in.read(reinterpret_cast<char*>(colorbuffer.raw), bytespp);  // NOLINT
      if (!in.good()) {
        std::cerr << "an error occured while reading the header\n";
        return false;
      }
      for (int i = 0; i < chunkheader; i++) {
        for (int t = 0; t < bytespp; t++) {
          data[currentbyte++] = colorbuffer.raw[t];  // NOLINT
        }
        currentpixel++;
        if (currentpixel > pixelcount) {
          std::cerr << "Too many pixels read\n";
          return false;
        }
      }
    }
  } while (currentpixel < pixelcount);
  return true;
}

bool TGAImage::write_tga_file(const char* filename, bool rle) {
  unsigned char developer_area_ref[4] = {0, 0, 0, 0};
  unsigned char extension_area_ref[4] = {0, 0, 0, 0};
  unsigned char footer[18] = {'T', 'R', 'U', 'E', 'V', 'I', 'S', 'I', 'O',
                              'N', '-', 'X', 'F', 'I', 'L', 'E', '.', '\0'};
  std::ofstream out;
  out.open(filename, std::ios::binary);
  if (!out.is_open()) {
    std::cerr << "can't open file " << filename << "\n";
    out.close();
    return false;
  }
  TGA_Header header{};
  memset(&header, 0, sizeof(header));
  header.bitsperpixel = bytespp << 3;
  header.width = width;
  header.height = height;
  header.datatypecode =
      (bytespp == GRAYSCALE ? (rle ? 11 : 3) : (rle ? 10 : 2));
  header.imagedescriptor = 0x20;  // top-left origin
  out.write(reinterpret_cast<char*>(&header), sizeof(header));  // NOLINT
  if (!out.good()) {
    out.close();
    std::cerr << "can't dump the tga file\n";
    return false;
  }
  if (!rle) {
    out.write(reinterpret_cast<char*>(data),  // NOLINT
              width * height * bytespp);      // NOLINT
    if (!out.good()) {
      std::cerr << "can't unload raw data\n";
      out.close();
      return false;
    }
  } else {
    if (!unload_rle_data(out)) {
      out.close();
      std::cerr << "can't unload rle data\n";
      return false;
    }
  }
  out.write(reinterpret_cast<char*>(developer_area_ref),  // NOLINT
            sizeof(developer_area_ref));
  if (!out.good()) {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.write(reinterpret_cast<char*>(extension_area_ref),  // NOLINT
            sizeof(extension_area_ref));
  if (!out.good()) {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.write(reinterpret_cast<char*>(footer), sizeof(footer));  // NOLINT
  if (!out.good()) {
    std::cerr << "can't dump the tga file\n";
    out.close();
    return false;
  }
  out.close();
  return true;
}

// TODO(oleg): it is not necessary to break a raw chunk for two equal pixels
// (for the
// matter of the resulting size)
bool TGAImage::unload_rle_data(std::ofstream& out) {
  const uint64_t max_chunk_length = 128;
  uint64_t npixels = width * height;  // NOLINT
  uint64_t curpix = 0;
  while (curpix < npixels) {
    uint64_t chunkstart = curpix * bytespp;
    uint64_t curbyte = curpix * bytespp;
    uint64_t run_length = 1;
    bool raw = true;
    while (curpix + run_length < npixels && run_length < max_chunk_length) {
      bool succ_eq = true;
      for (int t = 0; succ_eq && t < bytespp; t++) {
        succ_eq = (data[curbyte + t] == data[curbyte + t + bytespp]);  // NOLINT
      }
      curbyte += bytespp;
      if (1 == run_length) {
        raw = !succ_eq;
      }
      if (raw && succ_eq) {
        run_length--;
        break;
      }
      if (!raw && !succ_eq) {
        break;
      }
      run_length++;
    }
    curpix += run_length;
    out.put(raw ? run_length - 1 : run_length + 127);
    if (!out.good()) {
      std::cerr << "can't dump the tga file\n";
      return false;
    }
    out.write(reinterpret_cast<char*>(data + chunkstart),  // NOLINT
              (raw ? run_length * bytespp : bytespp));
    if (!out.good()) {
      std::cerr << "can't dump the tga file\n";
      return false;
    }
  }
  return true;
}

TGAColor TGAImage::get(int x, int y) {
  if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height) {
    return TGAColor();
  }
  return TGAColor(data + (x + y * width) * bytespp, bytespp);  // NOLINT
}

bool TGAImage::set(int x, int y, TGAColor c) {
  if (data == nullptr || x < 0 || y < 0 || x >= width || y >= height) {
    return false;
  }
  memcpy(data + (x + y * width) * bytespp, c.raw, bytespp);  // NOLINT
  return true;
}

int TGAImage::get_bytespp() {
  return bytespp;
}

int TGAImage::get_width() {
  return width;
}

int TGAImage::get_height() {
  return height;
}

bool TGAImage::flip_horizontally() {
  if (data == nullptr) {
    return false;
  }
  int half = width >> 1;
  for (int i = 0; i < half; i++) {
    for (int j = 0; j < height; j++) {
      TGAColor c1 = get(i, j);
      TGAColor c2 = get(width - 1 - i, j);
      set(i, j, c2);
      set(width - 1 - i, j, c1);
    }
  }
  return true;
}

bool TGAImage::flip_vertically() {
  if (data == nullptr) {
    return false;
  }
  uint64_t bytes_per_line = width * bytespp;  // NOLINT
  auto* line = new unsigned char[bytes_per_line];
  int half = height >> 1;
  for (int j = 0; j < half; j++) {
    uint64_t l1 = j * bytes_per_line;
    uint64_t l2 = (height - 1 - j) * bytes_per_line;
    memmove(line, data + l1, bytes_per_line);       // NOLINT
    memmove(data + l1, data + l2, bytes_per_line);  // NOLINT
    memmove(data + l2, line, bytes_per_line);       // NOLINT
  }
  delete[] line;
  return true;
}

unsigned char* TGAImage::buffer() {
  return data;
}

void TGAImage::clear() {
  memset(data, 0, width * height * bytespp);  // NOLINT
}

bool TGAImage::scale(int w, int h) {
  if (w <= 0 || h <= 0 || data == nullptr) {
    return false;
  }
  auto* tdata = new unsigned char[w * h * bytespp];
  int nscanline = 0;
  int oscanline = 0;
  int erry = 0;
  unsigned long nlinebytes = w * bytespp;      // NOLINT
  unsigned long olinebytes = width * bytespp;  // NOLINT
  for (int j = 0; j < height; j++) {
    int errx = width - w;
    int nx = -bytespp;
    int ox = -bytespp;
    for (int i = 0; i < width; i++) {
      ox += bytespp;
      errx += w;
      while (errx >= width) {
        errx -= width;
        nx += bytespp;
        memcpy(tdata + nscanline + nx, data + oscanline + ox,  // NOLINT
               bytespp);
      }
    }
    erry += h;
    oscanline += olinebytes;
    while (erry >= height) {
      if (erry >= height << 1) {  // it means we jump over a scanline
        memcpy(tdata + nscanline + nlinebytes, tdata + nscanline,  // NOLINT
               nlinebytes);
      }
      erry -= height;
      nscanline += nlinebytes;
    }
  }
  delete[] data;
  data = tdata;
  width = w;
  height = h;
  return true;
}
