#include "repch.h"
#include "Auxiliaries/Physics.h"
#include <btBulletDynamicsCommon.h>
#include <stdexcept>
#include <cstring> // memcpy
#include <iostream>

namespace RE {

  // --- Helpers to convert between simple float arrays and btTransform ----------------
  btTransform Physics3D::ToBtTransform(const Vector3& pos, const float tr[4]) const {
    btTransform t;
    t.setIdentity();
    // position
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    // quaternion (x,y,z,w)
    btQuaternion q(tr[0], tr[1], tr[2], tr[3]);
    t.setRotation(q);
    return t;
  }

  void Physics3D::FromBtTransform(const btTransform& t, float outTransform[7]) const {
    btVector3 o = t.getOrigin();
    btQuaternion q = t.getRotation();
    outTransform[0] = o.x();
    outTransform[1] = o.y();
    outTransform[2] = o.z();
    outTransform[3] = q.x();
    outTransform[4] = q.y();
    outTransform[5] = q.z();
    outTransform[6] = q.w();
  }

  // --- Constructor / destructor ---------------------------------------------------
  Physics3D::Physics3D() = default;

  Physics3D::~Physics3D() {
    // best-effort: Shutdown if still initialized
    if (m_initialized) {
      try { Shutdown(); } catch(...) {}
    }
  }

  // --- Init -----------------------------------------------------------------------
  void Physics3D::Init() {
    if (m_initialized) return;

    // Broadphase
    m_broadphase = new btDbvtBroadphase();

    // Collision configuration and dispatcher
    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);

    // Solver
    m_solver = new btSequentialImpulseConstraintSolver();

    // Dynamics world
    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfig);

    // sensible default gravity (y-down)
    m_dynamicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));

    m_initialized = true;
  }

  // --- Step -----------------------------------------------------------------------
  void Physics3D::Step(float ts, int maxSubSteps, float fixedStep) {
    if (!m_initialized) return;

    // don't advance simulation while paused
    if (!m_running) return;

    // clamp inputs
    if (ts <= 0.0f) return;
    if (maxSubSteps < 0) maxSubSteps = 1;
    if (fixedStep <= 0.0f) fixedStep = 1.0f / 60.0f;

    m_dynamicsWorld->stepSimulation(ts, maxSubSteps, fixedStep);
  }

  // --- Shutdown -------------------------------------------------------------------
  void Physics3D::Shutdown() {
    if (!m_initialized) return;

    // remove and delete rigid bodies (reverse order to be safe)
    for (auto it = m_ownedBodies.rbegin(); it != m_ownedBodies.rend(); ++it) {
      btRigidBody* body = *it;
      if (!body) continue;
      // remove from world if present
      if (m_dynamicsWorld->getCollisionObjectArray().findLinearSearch(body) != -1) {
	m_dynamicsWorld->removeRigidBody(body);
      }
      // delete motion state and body
      btMotionState* ms = body->getMotionState();
      delete body;
      if (ms) delete ms;
    }
    m_ownedBodies.clear();

    // delete shapes
    for (auto s : m_ownedShapes) {
      delete s;
    }
    m_ownedShapes.clear();

    // delete world objects
    delete m_dynamicsWorld;
    m_dynamicsWorld = nullptr;
    delete m_solver;
    m_solver = nullptr;
    delete m_dispatcher;
    m_dispatcher = nullptr;
    delete m_collisionConfig;
    m_collisionConfig = nullptr;
    delete m_broadphase;
    m_broadphase = nullptr;

    m_initialized = false;
  }

  // Start simulation (allow Step to advance)
  void Physics3D::Start() {
    if (!m_initialized) {
      // If world isn't initialized, init it
      Init();
    }
    m_running = true;
  }

  // Stop simulation (Step will no-op)
  void Physics3D::Stop() {
    m_running = false;
  }

  bool Physics3D::IsRunning() const {
    return m_running;
  }

  // Reset simulation: teardown and re-init. Removes all bodies and shapes owned by wrapper.
  void Physics3D::Reset() {
    // If not initialized, simply re-init
    if (!m_initialized) {
      Init();
      m_running = false;
      return;
    }

    // Temporarily disable running while we reset
    bool wasRunning = m_running;
    m_running = false;

    // Shutdown clears bodies/shapes/world
    Shutdown();

    // Reinitialize bullet objects
    Init();

    // Keep simulation stopped after Reset by default (safer), restore previous running state if desired:
    // If you want to auto-resume, uncomment the following line:
    // m_running = wasRunning;
  }

  // --- Gravity helpers -----------------------------------------------------------
  void Physics3D::SetGravity(float x, float y, float z) {
    if (!m_initialized) return;
    m_dynamicsWorld->setGravity(btVector3(x,y,z));
  }
  void Physics3D::GetGravity(float &x, float &y, float &z) const {
    if (!m_initialized) { x = y = z = 0.0f; return; }
    btVector3 g = m_dynamicsWorld->getGravity();
    x = g.x(); y = g.y(); z = g.z();
  }

  // --- Shape factory helpers -----------------------------------------------------
  btCollisionShape* Physics3D::CreateBoxShape(float hx, float hy, float hz) {
    btCollisionShape* s = new btBoxShape(btVector3(hx, hy, hz));
    m_ownedShapes.push_back(s);
    return s;
  }
  btCollisionShape* Physics3D::CreateSphereShape(float radius) {
    btCollisionShape* s = new btSphereShape(radius);
    m_ownedShapes.push_back(s);
    return s;
  }
  btCollisionShape* Physics3D::CreateCylinderShape(float hx, float hy, float hz) {
    btCollisionShape* s = new btCylinderShape(btVector3(hx, hy, hz));
    m_ownedShapes.push_back(s);
    return s;
  }

  btCollisionShape* Physics3D::CreatePlaneShape(float normalX, float normalY, float normalZ, float planeConstant) {
    // btStaticPlaneShape takes a btVector3 normal and a float plane constant c where plane eq is n·x + c = 0.
    btVector3 normal(normalX, normalY, normalZ);
    // Optionally normalize the normal so planeConstant remains meaningful
    if (normal.length2() > 0.0f) normal.normalize();

    btCollisionShape* s = new btStaticPlaneShape(normal, planeConstant);
    // register for cleanup
    m_ownedShapes.push_back(s);

    // optionally set margin to 0 for infinite plane (choice)
    s->setMargin(0.0f);

    return s;
  }


  // --- Add / Remove rigid body ---------------------------------------------------
  void* Physics3D::AddRigidBody(btCollisionShape* shape, float mass,const Vector3& pos, const Vector3& rotation) {
    if (!m_initialized) return nullptr;
    if (!shape) return nullptr;

    // calculate local inertia
    btVector3 localInertia(0,0,0);
    if (mass > 0.0f) shape->calculateLocalInertia(mass, localInertia);

    // transform
    // btTransform start = ToBtTransform(pos,startTransform);
    btTransform start;
    start.setIdentity();
    start.setOrigin(btVector3(pos.x, pos.y, pos.z));
    start.setRotation(btQuaternion(btScalar(rotation.z), btScalar(rotation.y), btScalar(rotation.x)));

    // motion state
    btDefaultMotionState* motion = new btDefaultMotionState(start);

    // construction
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    // add to world
    m_dynamicsWorld->addRigidBody(body);

    // track for cleanup
    m_ownedBodies.push_back(body);

    return static_cast<void*>(body);
  }

  void Physics3D::RemoveRigidBody(void* bodyHandle, bool destroyShape) {
    if (!m_initialized) return;
    if (!bodyHandle) return;
    btRigidBody* body = static_cast<btRigidBody*>(bodyHandle);

    // remove from world
    m_dynamicsWorld->removeRigidBody(body);

    // remove from owned list
    auto it = std::find(m_ownedBodies.begin(), m_ownedBodies.end(), body);
    if (it != m_ownedBodies.end()) {
      // delete motion state
      btMotionState* ms = body->getMotionState();
      delete body;
      if (ms) delete ms;
      m_ownedBodies.erase(it);
    } else {
      // if not found, still delete
      btMotionState* ms = body->getMotionState();
      delete body;
      if (ms) delete ms;
    }

    // optionally delete shape if owned (crude: search owned shapes list and delete)
    if (destroyShape) {
      // try to find shape pointer in owned shapes and delete if found
      // This assumes shapes were created via Create* helpers above.
      auto sit = std::find(m_ownedShapes.begin(), m_ownedShapes.end(), body->getCollisionShape());
      if (sit != m_ownedShapes.end()) {
	delete *sit;
	m_ownedShapes.erase(sit);
      }
    }
  }

  // --- Raycast -------------------------------------------------------------------
  RaycastHit Physics3D::Raycast(const float from[3], const float to[3]) {
    RaycastHit out;
    if (!m_initialized) return out;

    btVector3 btFrom(from[0], from[1], from[2]);
    btVector3 btTo(to[0], to[1], to[2]);

    btCollisionWorld::ClosestRayResultCallback cb(btFrom, btTo);
    m_dynamicsWorld->rayTest(btFrom, btTo, cb);

    if (cb.hasHit()) {
      out.hit = true;
      out.dist = cb.m_closestHitFraction; // fraction along segment
      const btVector3 &pt = cb.m_hitPointWorld;
      const btVector3 &n = cb.m_hitNormalWorld;
      out.hitX = pt.x(); out.hitY = pt.y(); out.hitZ = pt.z();
      out.normalX = n.x(); out.normalY = n.y(); out.normalZ = n.z();
      out.body = static_cast<void*>(cb.m_collisionObject ? const_cast<btCollisionObject*>(cb.m_collisionObject) : nullptr);
    }
    return out;
  }

  // --- Debug drawer stub (optional) ------------------------------------------------
  // You can implement a Bullet debug drawer that forwards draw lines into your renderer.
  // For now EnableDebugDrawer toggles no-op; implement if you want to visualize collision shapes.
  void Physics3D::EnableDebugDrawer(bool enable) {
    (void)enable;
    // TODO: implement btIDebugDraw subclass and assign to m_dynamicsWorld->setDebugDrawer(...)
  }

} // namespace RE
