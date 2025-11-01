#include "BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "raylib.h"
#include "rlgl.h"
#include "repch.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Auxiliaries/rayext.h"
#include "Core/Application.h"
#include "Core/UUID.h"

namespace RE {

  Scene::Scene(){
    m_EditorCam.position = { 10.0f, 10.0f, 10.0f }; // Camera position
    m_EditorCam.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    m_EditorCam.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    m_EditorCam.fovy = 45.0f;                                // Camera field-of-view Y
    m_EditorCam.projection = CAMERA_PERSPECTIVE; // Camera projection type

    m_Physics3D.Init();

    testPos = {0, 3, 0};
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
    TraceLog(LOG_INFO, "Physics start");

    ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto &comp) {
      auto& transform = entity.template GetComponent<TransformComponent>();
      auto& rigidShape = comp.shape;
      if(rigidShape.box){
	if(rigidShape.Dirty || !rigidShape.btShape){
	  rigidShape.btShape = m_Physics3D.CreateBoxShape(rigidShape.boxSize.x, rigidShape.boxSize.y, rigidShape.boxSize.z);
	}
      }
      if(rigidShape.sphere){
        if (rigidShape.Dirty || !rigidShape.btShape) {
	  rigidShape.btShape = m_Physics3D.CreateSphereShape(rigidShape.radius);
	}
      }

      if(rigidShape.plane){
        if (rigidShape.Dirty || !rigidShape.btShape) {
	  rigidShape.btShape = m_Physics3D.CreatePlaneShape(rigidShape.planeSize.x, rigidShape.planeSize.y, rigidShape.planeSize.z, 0);
	}          
      }

      switch (comp.type) {
      case BodyType::Static:
        comp.body = m_Physics3D.AddRigidBody(
            rigidShape.btShape, 0, transform.Translation, transform.Rotation);
        break;
      case BodyType::Dynamic:
        comp.body = m_Physics3D.AddRigidBody(
            rigidShape.btShape, 1, transform.Translation, transform.Rotation);
        break;
      case BodyType::Kinematic:
	break;
      }

      comp.savedTranslation = transform.Translation;
      comp.savedRotation = transform.Rotation;
      comp.savedScale = transform.Scale;
    });

    m_Physics3D.Start();
  }

  void Scene::OnRuntimeStop(){
    TraceLog(LOG_INFO, "Physics stop");
    m_Physics3D.Stop();
    m_Physics3D.Reset();

    ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto &comp) {
      auto &transform = entity.template GetComponent<TransformComponent>();
      transform.Translation = comp.savedTranslation;
      transform.Rotation = comp.savedRotation;
      transform.Scale = comp.savedScale;
    });
  }

  void Scene::PhysicsUpdate(float dt){
    m_Physics3D.Step(dt);

    ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto &comp) {
      auto &transform = entity.template GetComponent<TransformComponent>();
      btTransform trans;
      static_cast<btRigidBody*>(comp.body)->getMotionState()->getWorldTransform(trans);

      transform.Translation = {float(trans.getOrigin().getX()),
                               float(trans.getOrigin().getY()),
                               float(trans.getOrigin().getZ())};

      btQuaternion quat = trans.getRotation();

      transform.Rotation = {
	float(quat.getAxis().getX()),
	float(quat.getAxis().getY()),
	float(quat.getAxis().getZ())
      };
      
    });



  }

  void Scene::OnUpdate(float dt) {

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
      inView = true;
      DisableCursor();
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
      inView = false;
      EnableCursor();
    }

    if(inView)
      UpdateCamera(&m_EditorCam, CAMERA_FREE);
        
    ClearBackground(RED);

    BeginMode3D(m_EditorCam);
    {     
      
      ViewEntity<Entity, Camera3DComponent>([this](auto entity, auto &comp) {
	DrawCameraFrustum(comp.Camera, 0.1f, 2.0f, SKYBLUE);
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

      ViewEntity<Entity, ModelComponent>([this](auto entity, auto &comp) {
	auto &transform =
	  entity.template GetComponent<TransformComponent>();
	DrawModelEx(comp.model->Data, transform.Translation, transform.Rotation,
		    1.0f, transform.Scale, comp.color);
      });

      ViewEntity<Entity, AnimationComponent>([this](auto entity, auto &comp) {
        auto &transform = entity.template GetComponent<TransformComponent>();
        if (entity.template HasComponent<ModelComponent>()) {
          auto &model = entity.template GetComponent<ModelComponent>().model;
          unsigned int animIndex = 0;
          // if (comp.playingAnim) {            
	  //   ModelAnimation anim = comp.playingAnim[animIndex];
	  //   comp.animCurrentFrame = (comp.animCurrentFrame + 1)%anim.frameCount;
	  //   UpdateModelAnimation(model, anim,comp.animCurrentFrame);
	  // }          
	}
      });

      ViewEntity<Entity, RigidbodyComponent>([this](auto entity, auto &comp) {
	auto& transform = entity.template GetComponent<TransformComponent>();
	auto& rigidShape = comp.shape;
	if(rigidShape.box){
	  if(rigidShape.Dirty || !rigidShape.btShape){
	    rigidShape.btShape = m_Physics3D.CreateBoxShape(rigidShape.boxSize.x, rigidShape.boxSize.y, rigidShape.boxSize.z);
          }
	  const auto& shapeSize = static_cast<btBoxShape*>(rigidShape.btShape)->getHalfExtentsWithMargin();
	  DrawCubeWiresV(transform.Translation, {shapeSize.x(), shapeSize.y(), shapeSize.z()}, MAROON);
        }
        if (rigidShape.sphere) {
          if (rigidShape.Dirty || !rigidShape.btShape) {
	    rigidShape.btShape = m_Physics3D.CreateSphereShape(rigidShape.radius);
          }
          const auto &shapeRadius =
              static_cast<btSphereShape *>(rigidShape.btShape)->getRadius();
	  DrawSphereWires(transform.Translation, shapeRadius, 4, 4, MAROON);
        }

	if(rigidShape.plane){
	  if (rigidShape.Dirty || !rigidShape.btShape) {
	    rigidShape.btShape = m_Physics3D.CreatePlaneShape(rigidShape.planeSize.x, rigidShape.planeSize.y, rigidShape.planeSize.z, 0);
          }
          const auto &shapeSize =
              static_cast<btStaticPlaneShape *>(rigidShape.btShape)
                  ->getPlaneNormal();
	  DrawPlane(transform.Translation, {shapeSize.x(), shapeSize.z()}, MAROON);
	}
      });

      ViewEntity<Entity, SkyboxComponent>([this](auto entity, auto &comp) {

        rlDisableBackfaceCulling();     // make inside faces visible
        rlDisableDepthMask();           // so skybox is always behind everything
        DrawModel(comp.skybox->Data, m_EditorCam.position, 1.0f, WHITE);
        rlEnableBackfaceCulling();
        rlEnableDepthMask();
      });

      DrawGrid(10, 1.0f);
    }
    EndMode3D();

    DrawFPS(10,10);

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
	DrawModelEx(comp.model->Data, transform.Translation, transform.Rotation, 1.0f, transform.Scale, comp.color);
      });

      ViewEntity<Entity, SkyboxComponent>([&](auto entity, auto &comp) {

        rlDisableBackfaceCulling();     // make inside faces visible
        rlDisableDepthMask();           // so skybox is always behind everything
        DrawModel(comp.skybox->Data, m_RuntimeCam->position, 1.0f, WHITE);
        rlEnableBackfaceCulling();
        rlEnableDepthMask();
      });

      DrawCube(testPos, 1, 1, 1, RED);

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
                                               ModelComponent &component) {}

  template <>
  void Scene::OnComponentAdded<AnimationComponent>(Entity entity,
						   AnimationComponent &component) {}

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
  void Scene::OnComponentAdded<PlaneComponent>(Entity entity,
                                               PlaneComponent &component) {}

  template <>
  void Scene::OnComponentAdded<SkyboxComponent>(Entity entity,
                                                SkyboxComponent &component) {}

  template <>
  void Scene::OnComponentAdded<RigidbodyComponent>(Entity entity, RigidbodyComponent& component)
  {}
}
