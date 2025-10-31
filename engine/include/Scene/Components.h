#pragma once

#include "Core/UUID.h"
#include "raylib.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace RE {

    struct IDComponent
  {
    UUID ID;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
  };

  struct TagComponent
  {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag)
      : Tag(tag) {}
  };

  struct TransformComponent
  {
    Vector3 Translation = { 0.0f, 0.0f, 0.0f };
    Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
    Vector3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const Vector3& translation)
      : Translation(translation) {}

    // glm::mat4 GetTransform() const
    // {
    //   glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

    //   return glm::translate(glm::mat4(1.0f), Translation)
	// * rotation
	// * glm::scale(glm::mat4(1.0f), Scale);
    // }

    // float GetRadius() const
    // {
    //   // Assuming you're transforming a unit sphere
    //   return glm::compMax(Scale) * 0.5f; // max(Scale.x, Scale.y, Scale.z)
    // }
  };

  struct  ModelComponent{
    Model model;
    Color color;
    BoundingBox box;
    ModelComponent() = default;
    ModelComponent(const ModelComponent&) = default;
  };

  struct AnimationComponent {
    ModelAnimation *playingAnim = nullptr;
    int animsCount = 0;
    unsigned int animIndex = 0;
    unsigned int animCurrentFrame = 0;
    void AddAnimation(const std::string &name, ModelAnimation *animation) {
      animations[name] = animation;
    }

    void PlayAnimation(const std::string &name) {
      playingAnim = animations[name];
    }
    AnimationComponent() = default;
    AnimationComponent(const AnimationComponent&) = default;
  private:
    std::map<std::string, ModelAnimation*> animations;
  };

  struct Camera3DComponent
  {
    Camera3D Camera = {0};
    bool Primary = false; // TODO: think about moving to Scene
    bool FixedAspectRatio = false;

    Camera3DComponent() = default;
    Camera3DComponent(const Camera3DComponent&) = default;
  };

  struct CubeComponent{
    Color color;
    CubeComponent() = default;
    CubeComponent(const CubeComponent&) = default;
  };

  struct SphereComponent{
    Color color;
    SphereComponent() = default;
    SphereComponent(const SphereComponent&) = default;
  };

  struct PlaneComponent {
    Color color;
    PlaneComponent() = default;
    PlaneComponent(const PlaneComponent&) = default;
  };

  struct SkyboxComponent {
    Model skybox;
    SkyboxComponent() = default;
    SkyboxComponent(const SkyboxComponent&) = default;
  };
}
