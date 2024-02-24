#ifndef IGRID_HPP_
#define IGRID_HPP_

#include <cstdint>
#include "Coord.hpp"

class IGrid {
public:
  virtual int16_t width();
  virtual int16_t height();
  virtual void updateState(Coord coord, uint8_t state);
  virtual uint8_t state(Coord coord);
  virtual void clearStates();
};

#endif
