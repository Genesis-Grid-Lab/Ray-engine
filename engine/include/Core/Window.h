#pragma once

#include "Config.h"

namespace RE {
  class Window {
  public:
    Window(const int width, const int height, const char* title) {
      InitWindow(width, height, title);
    }
    ~Window(){
      CloseWindow();
    }    
  };
}
