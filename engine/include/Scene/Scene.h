#pragma once


namespace RE {

enum class SceneState {
    Edit = 0,
    Play = 1
};

class Scene{
public:
 Scene();
 ~Scene();

 void OnRuntimeStart();
 void OnRuntimeStop();
 void PhysicsUpdate(float dt);

 void OnUpdate(float dt);
 void OnUpdateRuntime(float dt);
}
}