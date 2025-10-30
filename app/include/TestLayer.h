#pragma once

#include "Core/Layer.h"

class TestLayer : public RE::Layer {
public:
 TestLayer(){}

 void OnAttach() override{
    TraceLog(LOG_INFO,"Attach");
 }

 void OnUpdate(float dt){
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();
 }
};