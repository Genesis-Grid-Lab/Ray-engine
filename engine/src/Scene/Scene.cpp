#include "repch.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"

namespace RE {

    Scene::Scene(){
        m_EditorCam.position = { 10.0f, 10.0f, 10.0f }; // Camera position
        m_EditorCam.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
        m_EditorCam.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
        m_EditorCam.fovy = 45.0f;                                // Camera field-of-view Y
        m_EditorCam.projection = CAMERA_PERSPECTIVE;             // Camera projection type
    }

    Scene::~Scene(){}

    Entity Scene::CreateEntity(const std::string& name)
    {
        return CreateEntityWithUUID(UUID(), name);
    }

    Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        // entity.AddComponent<IDComponent>(uuid);
        auto &id = entity.AddComponent<IDComponent>();
        id.ID = uuid;
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntityNow(Entity entity) { m_Registry.destroy(entity); }

    void Scene::DestroyEntity(Entity entity){
        // Queue for destruction; actual registry destroy happens in
        // FlushEntityDestruction()
        m_DestroyQueue.push_back((entt::entity)entity);
    }

    void Scene::FlushEntityDestruction()
    {
        if (m_DestroyQueue.empty()) return;    
        for (auto e : m_DestroyQueue)
        if (m_Registry.valid(e))
        m_Registry.destroy(e);

        m_DestroyQueue.clear();
    }

    void Scene::OnRuntimeStart(){

    }

    void Scene::OnRuntimeStop(){

    }

    void Scene::PhysicsUpdate(float dt){

    }

    void Scene::OnUpdate(float dt){
        UpdateCamera(&m_EditorCam, CAMERA_FREE);
        
        ClearBackground(RAYWHITE);

        BeginMode3D(m_EditorCam);                
        {
            ViewEntity<Entity, Camera3DComponent>([this](auto entity, auto& comp) {
                DrawCube(comp.Camera.position, 1.0f, 1.0f, 1.0f, SKYBLUE);
            });
            ViewEntity<Entity, CubeComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawCube(transform.Translation, transform.Scale.x, transform.Scale.y, transform.Scale.z, comp.color);
            });   
            
            ViewEntity<Entity, SphereComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawSphere(transform.Translation, 1.0f, comp.color);
            });

            ViewEntity<Entity, PlaneComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawPlane(transform.Translation, {transform.Scale.x, transform.Scale.y}, comp.color);
            });

            ViewEntity<Entity, ModelComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawModelEx(comp.model, transform.Translation, transform.Rotation, 1.0f, transform.Scale, comp.color);
            });

            DrawGrid(10, 1.0f);
        }
        EndMode3D();       

        FlushEntityDestruction();
    }
    void Scene::OnUpdateRuntime(float dt){
        ViewEntity<Entity, Camera3DComponent>([this](auto entity, auto& comp) {             
            if (comp.Primary) {
                UpdateCamera(&comp.Camera, CAMERA_FIRST_PERSON);
                m_RuntimeCam = &comp.Camera;
            }
        });

                
        ClearBackground(RAYWHITE);

        PhysicsUpdate(dt);

        if(m_RuntimeCam){
            BeginMode3D(*m_RuntimeCam);

            ViewEntity<Entity, CubeComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawCube(transform.Translation, transform.Scale.x, transform.Scale.y, transform.Scale.z, comp.color);
            });

            ViewEntity<Entity, SphereComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawSphere(transform.Translation, 1.0f, comp.color);
            });

            ViewEntity<Entity, PlaneComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawPlane(transform.Translation, {transform.Scale.x, transform.Scale.y}, comp.color);
            });  
            
            ViewEntity<Entity, ModelComponent>([this](auto entity, auto& comp) {
                auto& transform = entity.template GetComponent<TransformComponent>();
                DrawModelEx(comp.model, transform.Translation, transform.Rotation, 1.0f, transform.Scale, comp.color);
            });


            EndMode3D();
        }
        else{
            DrawText("NO PRIMARY CAM", 40, 80, 10, RED);
        }            

        FlushEntityDestruction();
    }

    template<typename T>
    void  Scene::OnComponentAdded(Entity entity, T& component)
  {
    // static_assert(false);
  }

  template<>
    void  Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
  {
  }

  template<>
    void  Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
  {
  }

  template <>
  void Scene::OnComponentAdded<ModelComponent>(Entity entity,
                                               ModelComponent &component)
  {}     

  template <>
  void Scene::OnComponentAdded<Camera3DComponent>(Entity entity, Camera3DComponent& component)
  {}

  template <>
  void Scene::OnComponentAdded<CubeComponent>(Entity entity, CubeComponent& component)
  {}

  template <>
  void Scene::OnComponentAdded<SphereComponent>(Entity entity, SphereComponent& component)
  {}

  template <>
  void Scene::OnComponentAdded<PlaneComponent>(Entity entity, PlaneComponent& component)
  {}
}