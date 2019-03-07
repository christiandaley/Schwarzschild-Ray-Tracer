#include <assert.h>
#include <cmath>
#include "PostProcessor.h"



static Color **new_buffer(const int xres, const int yres) {
  Color **buf = new Color*[xres];
  for (int i = 0; i < xres; i++) {
    buf[i] = new Color[yres]();
  }

  return buf;
}

static void delete_buffer(Color **buf, const int xres) {
  for (int i = 0; i < xres; i++) {
    delete[] buf[i];
  }

  delete[] buf;
}

static Color **down_sample(Color **pixels, const int xres, const int yres, const double factor) {
  const int new_xres = xres / factor;
  const int new_yres = yres / factor;

  Color **buf = new_buffer(new_xres, new_yres);

  for (int i = 0; i < xres; i++) {
    for (int j = 0; j < yres; j++) {
      buf[(int)(i / factor)][(int)(j / factor)][0] += pixels[i][j][0];
      buf[(int)(i / factor)][(int)(j / factor)][1] += pixels[i][j][1];
      buf[(int)(i / factor)][(int)(j / factor)][2] += pixels[i][j][2];
    }
  }

  for (int i = 0; i < new_xres; i++) {
    for (int j = 0; j < new_yres; j++) {
      buf[i][j][0] /= (factor * factor);
      buf[i][j][1] /= (factor * factor);
      buf[i][j][2] /= (factor * factor);

    }
  }

  for (int i = 0; i < new_xres; i++) {
    for (int j = 0; j < new_yres; j++) {
      double s = buf[i][j][0] + buf[i][j][1] + buf[i][j][2];
      if (s < 375.0f) {
        buf[i][j][0] = 0;
        buf[i][j][1] = 0;
        buf[i][j][2] = 0;
      }
    }
  }

  return buf;
}

static Color **blurX(Color **buf, const int xres, const int yres, const double stddev) {
  Color **temp = new_buffer(xres, yres);
  const double a = 1.0 / sqrt(2.0 * M_PI * stddev * stddev);

  for (int i = 0; i < xres; i++) {
    for (int j = 0; j < yres; j++) {
      double c[3] = {};
      for (int k = MAX(0, i - stddev * 3.0); k < MIN(xres, i + stddev * 3.0); k++) {

        double weight = a * exp(-((double)((i - k) * (i - k))) / 2.0 / stddev / stddev);

        //weight = j == k;
        c[0] += weight * (double)buf[k][j][0];
        c[1] += weight * (double)buf[k][j][1];
        c[2] += weight * (double)buf[k][j][2];
      }

      temp[i][j][0] = c[0];
      temp[i][j][1] = c[1];
      temp[i][j][2] = c[2];

    }
  }

  return temp;
}

static Color **blurY(Color **buf, const int xres, const int yres, const double stddev) {
  Color **temp = new_buffer(xres, yres);
  const double a = 1.0 / sqrt(2.0 * M_PI * stddev * stddev);

  for (int i = 0; i < xres; i++) {
    for (int j = 0; j < yres; j++) {
      double c[3] = {};
      for (int k = MAX(0, j - stddev * 3.0); k < MIN(yres, j + stddev * 3.0); k++) {

        double weight = a * exp(-((double)((j - k) * (j - k))) / 2.0 / stddev / stddev);

        //weight = j == k;
        c[0] += weight * (double)buf[i][k][0];
        c[1] += weight * (double)buf[i][k][1];
        c[2] += weight * (double)buf[i][k][2];
      }

      temp[i][j][0] = c[0];
      temp[i][j][1] = c[1];
      temp[i][j][2] = c[2];

    }
  }

  return temp;
}

static void mix(Color **pixels, Color **buf, const int xres, const int yres, const double factor, const double dampening) {
  for (int i = 0; i < xres; i++) {
    for (int j = 0; j < yres; j++) {

      pixels[i][j][0] += (1.0 / dampening) * buf[(int)(i / factor)][(int)(j / factor)][0];
      pixels[i][j][1] += (1.0 / dampening) * buf[(int)(i / factor)][(int)(j / factor)][1];
      pixels[i][j][2] += (1.0 / dampening) * buf[(int)(i / factor)][(int)(j / factor)][2];

      pixels[i][j][0] = MIN(255, pixels[i][j][0]);
      pixels[i][j][1] = MIN(255, pixels[i][j][1]);
      pixels[i][j][2] = MIN(255, pixels[i][j][2]);
      continue;

      pixels[i][j][0] = buf[(int)(i / factor)][(int)(j / factor)][0];
      pixels[i][j][1] = buf[(int)(i / factor)][(int)(j / factor)][1];
      pixels[i][j][2] = buf[(int)(i / factor)][(int)(j / factor)][2];
    }
  }
}

void PostProcessor::Bloom(Color **pixel_buffer, const int xres, const int yres, const double stddev,
                          const double dampening, const double scale_factor) {

  Color **buff1 = down_sample(pixel_buffer, xres, yres, scale_factor);

  Color **blur1Y = blurY(buff1, xres / scale_factor, yres / scale_factor, stddev);
  Color **blur1X = blurX(blur1Y, xres / scale_factor, yres / scale_factor, stddev);

  mix(pixel_buffer, blur1X, xres, yres, scale_factor, dampening);

  //Color **blur1X = blurX(buff1, xres / 2.0, yres / 2.0, 30.0);

  //mix(pixel_buffer, blur1X, xres, yres, 2.0);


  delete_buffer(buff1, xres / scale_factor);
  delete_buffer(blur1X, xres / scale_factor);
  delete_buffer(blur1Y, xres / scale_factor);
}
