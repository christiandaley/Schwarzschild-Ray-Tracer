#include <png++/png.hpp>
#include "Object.h"

void Object::load_tx(const char *filename) {
  this->hasTexture = true;
  png::image<png::rgb_pixel> image(filename);
  tx_width = image.get_width();
  tx_height = image.get_height();

  if (this->bmp != NULL) {
    delete[] this->bmp;
  }

  this->bmp = new Color[tx_width * tx_height];
  for (int j = 0; j < tx_height; j++) {
    for (int i = 0; i < tx_width; i++) {
      png::rgb_pixel pixel = image.get_pixel(i, j);
      bmp[j * tx_width + i][0] = pixel.red;
      bmp[j * tx_width + i][1] = pixel.green;
      bmp[j * tx_width + i][2] = pixel.blue;

    }
  }
}

void Object::tx_lookup(const double x, const double y, Color color) const {
  const int ix = (int)x;
  const int iy = (int)y;

  const double xweight = 1.0 - (x - (double)ix);
  const double yweight = 1.0 - (y - (double)iy);
  const double w1 = xweight * yweight;
  const double w2 = (1.0 - xweight) * yweight;
  const double w3 = xweight * (1.0 - yweight);
  const double w4 = (1.0 - xweight) * (1.0 - yweight);

  color[0] = w1 * this->bmp[iy * tx_width + ix][0]
             + w2 * this->bmp[iy * tx_width + ix + 1][0]
             + w3 * this->bmp[(iy + 1) * tx_width + ix][0]
             + w4 * this->bmp[(iy + 1) * tx_width + ix + 1][0];

  color[1] = w1 * this->bmp[iy * tx_width + ix][1]
            + w2 * this->bmp[iy * tx_width + ix + 1][1]
            + w3 * this->bmp[(iy + 1) * tx_width + ix][1]
            + w4 * this->bmp[(iy + 1) * tx_width + ix + 1][1];

  color[2] = w1 * this->bmp[iy * tx_width + ix][2]
            + w2 * this->bmp[iy * tx_width + ix + 1][2]
            + w3 * this->bmp[(iy + 1) * tx_width + ix][2]
            + w4 * this->bmp[(iy + 1) * tx_width + ix + 1][2];
}

Object::~Object() {
  if (this->hasTexture) {
    delete[] this->bmp;
  }
}
