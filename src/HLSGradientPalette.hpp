#ifndef HLSGRADIENTPALETTE_HPP_
#define HLSGRADIENTPALETTE_HPP_

#include <cstdint>
#include <array>
#include <cmath>
#include "GradientPalette.hpp"

struct FColorRGB {
  float red;
  float green;
  float blue;
};

struct ColorHLS {
  float hue;
  float luminance;
  float saturation;
};

ColorHLS rgbToHLS(const FColorRGB& rgb) {
  float maxc = fmaxf(rgb.red, fmaxf(rgb.blue, rgb.green));
  float minc = fminf(rgb.red, fminf(rgb.blue, rgb.green));
  float sumc = (maxc + minc);
  float rangec = (maxc - minc);

  ColorHLS hls;
  hls.luminance = sumc / 2.0f;
  if (minc == maxc) {
    hls.hue = 0.0f;
    hls.saturation = 0.0f;
    return hls;
  }
  if (hls.luminance <= 0.5f) {
    hls.saturation = rangec / sumc;
  }
  else {
    hls.saturation = rangec / (2.0f - maxc - minc);
  }
  float rc = (maxc - rgb.red)   / rangec;
  float gc = (maxc - rgb.green) / rangec;
  float bc = (maxc - rgb.blue)  / rangec;

  if (rgb.red == maxc) {
    hls.hue = bc - gc;
  }
  else if(rgb.green == maxc) {
    hls.hue = 2.0f + rc - bc;
  }
  else {
    hls.hue = 4.0f + gc - rc;
  }
  hls.hue = std::fmod(hls.hue / 6.0f, 1.0f);
  return hls;
}

float v(float m1, float m2, float hue) {
  hue = std::fmod(hue,1.0f);
  if (hue < 1.0f / 6.0f) {
    return m1 + (m2 - m1) * hue * 6.0f;
  }
  if (hue < 0.5f) {
    return m2;
  }
  if (hue < 2.0f / 3.0f) {
    return m1 + (m2 - m1) * (2.0f / 3.0f - hue) * 6.0f;
  }
  return m1;
}

FColorRGB fromRGB(const ColorRGB& rgb) {
  return FColorRGB{static_cast<float>(rgb.red) / 31.0f,
                   static_cast<float>(rgb.green) / 63.0f,
                   static_cast<float>(rgb.blue) / 31.0f};
}

ColorRGB toRGB(const FColorRGB& rgb) {
  return ColorRGB{static_cast<uint16_t>(rgb.red * 31),
                  static_cast<uint16_t>(rgb.green * 63),
                  static_cast<uint16_t>(rgb.blue * 31)};
}

FColorRGB hlsToRGB(const ColorHLS& hls) {
  if(hls.saturation == 0.0f) {
    return FColorRGB{1.0f, 1.0f, 1.0f};
  }
  float m1,m2;
  if(hls.luminance <= 0.5f) {
    m2 = hls.luminance * (1.0f + hls.saturation);
  }
  else {
    m2 = hls.luminance + hls.saturation - (hls.luminance * hls.saturation);
  }
  m1 = 2.0f * hls.luminance - m2;
  return FColorRGB{ v(m1,m2,hls.hue + 1.0f / 3.0f),
                    v(m1,m2,hls.hue),
                    v(m1,m2,hls.hue - 1.0f / 3.0f)};
}

template <std::size_t N>
void makeHLSGradientPalette(const uint16_t background,
                            const uint16_t start,
                            const uint16_t end,
                            std::array<uint16_t, N>& dstPalette)
{
  int palLen = N - 1;
  ColorHLS stHLS = rgbToHLS(fromRGB(splitToRGB(start)));
  ColorHLS enHLS = rgbToHLS(fromRGB(splitToRGB(end)));

  float stepH = (enHLS.hue        - stHLS.hue)        / palLen;
  float stepL = (enHLS.luminance  - stHLS.luminance)  / palLen;
  float stepS = (enHLS.saturation - stHLS.saturation) / palLen;

  dstPalette[0] = background;
  ColorHLS temp;
  for(int i = 1; i < static_cast<int>(N); i++) {
    temp.hue        = stHLS.hue        + stepH * (i - 1);
    temp.luminance  = stHLS.luminance  + stepL * (i - 1);
    temp.saturation = stHLS.saturation + stepS * (i - 1);
    dstPalette[i] = mergeRGB(toRGB(hlsToRGB(temp)));
  }
}

#endif
