#pragma once

#include "Core/Layer.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Core/ImGuiHelper.h"

class TestLayer : public RE::Layer {
public:
 TestLayer(){}

 void OnAttach() override{
    TraceLog(LOG_TRACE,"Attach");
    MainScene = RE::CreateRef<RE::Scene>();

    auto cam = MainScene->CreateEntity("cam");
    auto& cc = cam.AddComponent<RE::Camera3DComponent>();
    cc.Primary = true;      
    cc.Camera.position = {1, 1, 1}; // Camera position
    cc.Camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    cc.Camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    cc.Camera.fovy = 45.0f;                                // Camera field-of-view Y
    cc.Camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type  

    auto floor = MainScene->CreateEntity("floor");
    // auto& planeComp = floor.AddComponent<RE::PlaneComponent>();
    // planeComp.color = RED;
    auto& floorTC = floor.GetComponent<RE::TransformComponent>();
    floorTC.Scale = {30, 30, 0};

    auto cube = MainScene->CreateEntity("cube");
    auto& CubeComp = cube.AddComponent<RE::CubeComponent>();
    CubeComp.color = BLUE;
    auto& cubeTC = cube.GetComponent<RE::TransformComponent>();
    cubeTC.Translation = {3.0f, 0.0f, 2.0f};

    auto sphere = MainScene->CreateEntity("sphere");
    auto& SphereComp = sphere.AddComponent<RE::SphereComponent>();
    SphereComp.color = GREEN;
    auto& sphereTC = sphere.GetComponent<RE::TransformComponent>();
    sphereTC.Translation = {2.0f, 2.0f, 0.0f};

    auto model = MainScene->CreateEntity("model");
    auto& modelComp = model.AddComponent<RE::ModelComponent>();
    modelComp.model = LoadModel("Resources/sponza/source/glTF/Sponza.gltf");    
    modelComp.color = WHITE;
    auto& modelTC = model.GetComponent<RE::TransformComponent>();
    // modelTC.Translation = {2.0f, 5.0f, 1.0f};

    manEntt = MainScene->CreateEntity("man");
    auto& manComp = manEntt.AddComponent<RE::ModelComponent>();    
    manComp.model = LoadModel("Resources/FinalBaseMesh/FinalBaseMesh.obj");
    manComp.color = WHITE;
    auto& manTC = manEntt.GetComponent<RE::TransformComponent>();
    // manTC.Translation = {.0f, 5.0f, 1.0f};
    manTC.Scale = {0.05f, 0.05f, 0.05f};
  }

 void OnUpdate(float dt){
    switch(m_SceneState){
    case RE::SceneState::Edit:
        MainScene->OnUpdate(dt);
        break;
    case RE::SceneState::Play:
        MainScene->OnUpdateRuntime(dt);
        break;
    }

    if(IsKeyPressed(KEY_TAB)){
        if(m_SceneState == RE::SceneState::Edit)
            OnScenePlay();
        else if (m_SceneState == RE::SceneState::Play)
            OnSceneStop();
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        DisableCursor();
    }
    else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        EnableCursor();
    }
 }

 void OnImGuiRender() override{
    #ifdef IMGUI_HAS_DOCK
        ImGui::DockSpaceOverViewport(0,  NULL, ImGuiDockNodeFlags_PassthruCentralNode); // set ImGuiDockNodeFlags_PassthruCentralNode so that we can see the raylib contents behind the dockspace
    #endif
    auto& manTC = manEntt.GetComponent<RE::TransformComponent>();
    ImGui::Begin("Propreties");
    ImGui::Text("Camera");
    ImGui::Separator();
    ImGui::Text("Man");
    DrawVec3Control("Man Pos", manTC.Translation);
    DrawVec3Control("Man Scale", manTC.Scale);
    ImGui::End();
 }

private:
 void OnScenePlay(){
    m_SceneState = RE::SceneState::Play;
    MainScene->OnRuntimeStart();
 }

 void OnSceneStop(){
    if(m_SceneState != RE::SceneState::Play){
        return;
    }

    m_SceneState = RE::SceneState::Edit;
    MainScene->OnRuntimeStop();
 }

private:
 RE::Ref<RE::Scene> MainScene;
 RE::SceneState m_SceneState = RE::SceneState::Edit;

 RE::Entity manEntt;
};