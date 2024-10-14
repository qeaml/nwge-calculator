#include <nwge/engine.hpp>
#include "Calculator.hpp"

s32 main([[maybe_unused]] s32 argc, [[maybe_unused]] CStr *argv) {
  nwge::startPtr(createCalculator(), {
    .appName = "Nwge Calculator",
  });
  return 0;
}
