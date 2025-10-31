#pragma once

#include "Core/UUID.h"
#include <entt/entt.hpp>

namespace RE {

class Entity;

enum class SceneState {
    Edit = 0,
    Play = 1
};

class Scene{
public:
 Scene();
 ~Scene();

 Entity CreateEntity(const std::string &name = std::string());
 Entity CreateEntityWithUUID(UUID uuid,
                             const std::string& name = std::string());

 void DestroyEntity(Entity entity);
 void DestroyEntityNow(Entity entity);
 void FlushEntityDestruction();

 void OnRuntimeStart();
 void OnRuntimeStop();
 void PhysicsUpdate(float dt);

 void OnUpdate(float dt);
 void OnUpdateRuntime(float dt);

 template<typename Entt, typename Comp, typename Task>
    void ViewEntity(Task&& task){
      // E_CORE_ASSERT(std::is_base_of<Entity, Entt>::value, "error viewing entt");
      m_Registry.view<Comp>().each([this, &task] 
				   (const auto entity, auto& comp) 
      { 
	// task(std::move(Entt(&m_Registry, entity)), comp);
	task(std::move(Entt(entity, this)), comp);
      });
  }
private:
  template <typename T> void OnComponentAdded(Entity entity, T &component);

private:
 entt::registry m_Registry;
 std::vector<entt::entity> m_DestroyQueue;
 Camera3D m_EditorCam;
 Camera3D *m_RuntimeCam = nullptr;
 friend class Entity;
};
}