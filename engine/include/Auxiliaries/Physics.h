#pragma once

#include "Core/Config.h"
#include <vector>
#include <memory>
#include <functional>

// Forward declare Bullet types to avoid leaking heavy headers in user headers
struct btBroadphaseInterface;
struct btDefaultCollisionConfiguration;
struct btCollisionDispatcher;
struct btSequentialImpulseConstraintSolver;
struct btDiscreteDynamicsWorld;
struct btCollisionShape;
struct btRigidBody;
struct btTransform;

namespace RE {

 struct RaycastHit {
    bool hit = false;
    float dist = 0.0f;
    // world space hit position and normal
    float hitX = 0.0f, hitY = 0.0f, hitZ = 0.0f;
    float normalX = 0.0f, normalY = 0.0f, normalZ = 0.0f;
    // pointer to the rigid body hit (may be nullptr)
    void* body = nullptr;
};

class Physics {
public:
    virtual ~Physics() = default;
    virtual void Init() = 0;
    virtual void Step(float ts, int maxSubSteps = 1, float fixedStep = 1.0f/60.0f) = 0;
    virtual void Shutdown() = 0;
};

// Physics3D: a small RAII-friendly wrapper around Bullet's dynamics world.
//
// Responsibilities:
//  - Initialize and shutdown Bullet world in correct order.
//  - Provide simple API to create/destroy rigid bodies (pass ownership pointers).
//  - Provide raycast helper.
//  - Keep all Bullet objects owned by this class (so user doesn't have to).
class Physics3D : public Physics {
public:
    Physics3D();
    ~Physics3D() override;

    // initialize Bullet structures; call after creating OpenGL/window if any debug drawer wants GL.
    void Init() override;

    // step simulation
    void Step(float ts, int maxSubSteps = 1, float fixedStep = 1.0f/60.0f) override;

    // shutdown and free all resources; best called before destroying GL/context.
    void Shutdown() override;

    // set/get gravity
    void SetGravity(float x, float y, float z);
    void GetGravity(float &x, float &y, float &z) const;

    // Control simulation
    // Start / resume stepping
    void Start();

    // Stop / pause stepping (Step() becomes a no-op until Start() called)
    void Stop();

    // Returns true if simulation is running (i.e. Step() will advance)
    bool IsRunning() const;

    // Reset simulation: destroy current world and reinitialize Bullet.
    // Removes all bodies and shapes owned by the Physics3D wrapper.
    // After Reset() the world is empty and you must re-create any ground/objects.
    // Reset() is safe to call while stopped or running; the wrapper will stop then re-init.
    void Reset();

    // Create a rigid body and register it with the world.
    // - shape: pointer to btCollisionShape (ownership can be transferred or kept; we provide helper to own shapes)
    // - mass: mass in kg; use 0.0f for static bodies
    // - startTransform: array of 7 floats: pos xyz, quat x y z w (or pass identity)
    // Returns a void* handle to the created btRigidBody (caller treats as opaque). Use RemoveRigidBody to destroy.
    void* AddRigidBody(btCollisionShape* shape, float mass, const Vector3& pos, const Vector3& rotation);

    // Remove and destroy a rigid body previously created by AddRigidBody.
    // If `destroyShape` is true the collision shape will also be deleted if it is owned by this wrapper.
    void RemoveRigidBody(void* bodyHandle, bool destroyShape = true);

    // Convenience helpers: create common shapes (ownership transferred to Physics3D)
    btCollisionShape* CreateBoxShape(float hx, float hy, float hz);   // half extents
    btCollisionShape* CreateSphereShape(float radius);
  btCollisionShape *CreateCylinderShape(float hx, float hy, float hz);
  // Create an infinite static plane collision shape.
  // `normalX/Y/Z` is the plane normal (doesn't need to be unit-length but should be).
  // `planeConstant` is the plane constant `c` in plane equation: n·x + c = 0.
  // Example: horizontal ground at y = 0 -> normal=(0,1,0), planeConstant = 0
  // Returns btCollisionShape* owned by Physics3D (will be deleted in Shutdown()).
  btCollisionShape* CreatePlaneShape(float normalX, float normalY, float normalZ, float planeConstant);


    // Raycast from `from` to `to` in world coords
    RaycastHit Raycast(const float from[3], const float to[3]);

    // Debug: toggle internal debug drawer (if available); can be extended to draw with your renderer
    void EnableDebugDrawer(bool enable);

private:
    // disallow copy
    Physics3D(const Physics3D&) = delete;
    Physics3D& operator=(const Physics3D&) = delete;

    // internal helpers
    btTransform ToBtTransform(const Vector3& pos, const float tr[4]) const;
    void FromBtTransform(const btTransform& t, float outTransform[7]) const;

private:
    // Bullet main objects (opaque here; defined in cpp)
    btBroadphaseInterface* m_broadphase = nullptr;
    btDefaultCollisionConfiguration* m_collisionConfig = nullptr;
    btCollisionDispatcher* m_dispatcher = nullptr;
    btSequentialImpulseConstraintSolver* m_solver = nullptr;
    btDiscreteDynamicsWorld* m_dynamicsWorld = nullptr;

    // owned shapes and bodies to make lifetime management simple
    std::vector<btCollisionShape*> m_ownedShapes;
    std::vector<btRigidBody*> m_ownedBodies;

    bool m_initialized = false;
    bool m_running = true;
};

} // namespace RE
