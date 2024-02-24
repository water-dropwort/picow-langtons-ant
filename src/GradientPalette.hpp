#ifndef GRADIENTPALETTE_HPP_
#define GRADIENTPALETTE_HPP_

#include <cstdint>
#include <array>

struct ColorRGB {
  uint16_t red;
  uint16_t green;
  uint16_t blue;
};

struct ColorStep {
  float red;
  float green;
  float blue;
};

ColorRGB splitToRGB(uint16_t inputColor) {
  ColorRGB out;
  out.red   = (inputColor >> 11) & 0x1F;
  out.green = (inputColor >>  5) & 0x3F;
  out.blue  = (inputColor >>  0) & 0x1F;
  return out;
}

ColorStep calcGradStep(ColorRGB st,
                       ColorRGB en,
                       uint16_t step)
{
  ColorStep out;
  const float fstep = (float)step;
  out.red   = (float)(en.red   - st.red)   / fstep;
  out.green = (float)(en.green - st.green) / fstep;
  out.blue  = (float)(en.blue  - st.blue)  / fstep;
  return out;
}

uint16_t mergeRGB(ColorRGB color) {
  return ((color.red & 0x1F) << 11)
    | ((color.green & 0x3F) << 5)
    | (color.blue & 0x1F);
}

template <std::size_t N>
void makeGradientPalette(const uint16_t background,
                         const uint16_t start,
                         const uint16_t mid,
                         const uint16_t end,
                         std::array<uint16_t, N>& dstPalette)
{
  const int iSt = 1;
  const uint16_t stepCountStMd = (N - 1) / 2;
  const int iMd = iSt + stepCountStMd;
  const int iEn = (N - 1);
  const uint16_t stepCountMdEn = iEn - iMd;

  const ColorRGB st = splitToRGB(start);
  const ColorRGB md = splitToRGB(mid);
  const ColorRGB en = splitToRGB(end);
  const ColorStep stepStMd
    = calcGradStep(st, md, stepCountStMd);
  const ColorStep stepMdEn
    = calcGradStep(md, en, stepCountMdEn);

  dstPalette[0] = background;

  ColorRGB temp;

  // Start ~ Mid
  for(int i = iSt; i < iMd; i++) {
    temp.red
      = st.red   + (int16_t)(stepStMd.red   * (float)(i-iSt));
    temp.green
      = st.green + (int16_t)(stepStMd.green * (float)(i-iSt));
    temp.blue
      = st.blue  + (int16_t)(stepStMd.blue  * (float)(i-iSt));
    dstPalette[i] = mergeRGB(temp);
  }
  // Mid ~ End
  for(int i = iMd; i <= iEn; i++) {
    temp.red
      = md.red   + (int16_t)(stepMdEn.red   * (float)(i-iMd));
    temp.green
      = md.green + (int16_t)(stepMdEn.green * (float)(i-iMd));
    temp.blue
      = md.blue  + (int16_t)(stepMdEn.blue  * (float)(i-iMd));
    dstPalette[i] = mergeRGB(temp);
  }
  dstPalette[iEn] = end;
}

#endif
