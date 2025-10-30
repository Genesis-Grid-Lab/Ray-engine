#include "Core/Window.h"

int main() {

  RE::Window mWin(200, 200, "Testing");

  while(!WindowShouldClose()){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();
  }
  return 0;  
}
