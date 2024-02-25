#ifndef RENDEREDGRID_HPP_
#define RENDEREDGRID_HPP_

#include <Adafruit_GFX.h>
#include <array>
#include "IGrid.hpp"
#include "Consts.hpp"
#ifdef GRAD_RGB_PALETTE
#include "GradientPalette.hpp"
#else
#include "HLSGradientPalette.hpp"
#endif

template <size_t Width, size_t Height>
class RenderedGrid : public IGrid {
public:
  RenderedGrid(Adafruit_GFX& initializedGfx)
    : m_gfx(initializedGfx)
    , m_width(Width)
    , m_height(Height)
  {
#ifdef GRAD_RGB_PALETTE
    makeGradientPalette(COLOR_BACKGROUND,
                        COLOR_PALETTE_START,
                        COLOR_PALETTE_MID,
                        COLOR_PALETTE_END,
                        m_palette);
#else
    makeHLSGradientPalette(COLOR_BACKGROUND,
                           COLOR_PALETTE_START,
                           COLOR_PALETTE_END,
                           m_palette);
#endif
  }

  int16_t width() override { return m_width; }

  int16_t height() override { return m_height; }

  void updateState(Coord coord,
                   uint8_t state) override
  {
    m_cells[coord.x][coord.y] = state;
    m_gfx.drawPixel(coord.x, coord.y, m_palette[state]);
  }

  uint8_t state(Coord coord) override {
    return m_cells[coord.x][coord.y];
  }

  void clearStates() override {
    for(int i = 0; i < m_width; i++) {
      for(int j = 0; j < m_height; j++) {
        m_cells[i][j] = 0;
      }
    }
    m_gfx.fillScreen(m_palette[0]);
  }

  void setColorToPalette(int index, uint16_t color) {
    m_palette[index] = color;
  }

  uint16_t getColorFromPalette(int index) {
    return m_palette[index];
  }
private:
  Adafruit_GFX& m_gfx;
  const int16_t m_width;
  const int16_t m_height;
  std::array<std::array<uint8_t, Height>, Width> m_cells;
  std::array<uint16_t, MAX_RULE_LENGTH> m_palette;
};

#endif
