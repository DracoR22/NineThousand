#include "Physics.h"

namespace Physics {
    using namespace physx;

    PxDefaultAllocator g_allocator;
    PxDefaultErrorCallback g_errorCallback;
    PxFoundation* g_foundation = nullptr;
    PxPhysics* g_physics = nullptr;
    PxDefaultCpuDispatcher* g_dispatcher = nullptr;
    PxScene* g_scene = nullptr;
    PxMaterial* g_material = nullptr;
    PxMaterial* g_characterMaterial = nullptr;
    PxController* g_controller = nullptr;
    PxControllerManager* g_controllerManager = nullptr;

    PxRigidDynamic* g_cubeActor = nullptr;
    std::unordered_map<uint64_t, RigidStatic> g_rigidStatic;
    std::unordered_map<uint64_t, RigidDynamic> g_rigidDynamic;

    float g_ControllerVerticalVelocity = 0.0f;

    void Init() {
        g_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_allocator, g_errorCallback);
        g_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_foundation, PxTolerancesScale(), true);

        // Create a CPU dispatcher for multithreading
        g_dispatcher = PxDefaultCpuDispatcherCreate(2);

        PxSceneDesc sceneDesc(g_physics->getTolerancesScale()); 
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // -9.81 earths gravity
        sceneDesc.cpuDispatcher = g_dispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        g_scene = g_physics->createScene(sceneDesc);

        g_material = g_physics->createMaterial(0.5f, 0.5f, 0.0f);
        g_characterMaterial = g_physics->createMaterial(0.5f, 0.5f, 0.0f);

        // create a ground plane
        PxRigidStatic* groundPlane = nullptr;
        groundPlane = PxCreatePlane(*g_physics, PxPlane(0.0f, 1.0f, 0.0f, 0.0f), *g_material);
        g_scene->addActor(*groundPlane);
    }

    void Simulate(double deltaTime) {
        const double fixedTimestep = 1.0 / 420; // 420 fps
        static double accumulatedTime = 0.0;

        accumulatedTime += deltaTime;

        while (accumulatedTime >= fixedTimestep) {
            g_scene->simulate(fixedTimestep);
            g_scene->fetchResults(true);
            accumulatedTime -= fixedTimestep;
        }
    }

    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass) {
        PxTransform transform(position);
        PxRigidDynamic* cubeActor = g_physics->createRigidDynamic(transform);

        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_material);
        cubeActor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*cubeActor, mass);
        g_scene->addActor(*cubeActor);

        g_cubeActor = cubeActor;

        return cubeActor;
    }

    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents) {
        PxTransform transform(position);

        PxRigidStatic* staticActor = g_physics->createRigidStatic(transform);

        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_material);
        staticActor->attachShape(*shape);
        shape->release();

        g_scene->addActor(*staticActor);

        return staticActor;
    }

    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass) {
        PxTransform transform(position);

        PxRigidDynamic* capsuleActor = g_physics->createRigidDynamic(transform);

        PxShape* shape = g_physics->createShape(PxCapsuleGeometry(radius, halfHeight), *g_material);
        capsuleActor->attachShape(*shape);
  
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*capsuleActor, mass);

        g_scene->addActor(*capsuleActor);

        return capsuleActor;
    }

    void CreateCharacterController() {
        g_controllerManager = PxCreateControllerManager(*g_scene);

        physx::PxCapsuleControllerDesc desc;
        desc.height = 1.8f;       
        desc.radius = 0.3f;         
        desc.material = g_physics->createMaterial(0.5f, 0.5f, 0.0f);
        desc.position = PxExtendedVec3(0, 5, 0); 
        desc.slopeLimit = 0.707f;    
        desc.stepOffset = 0.5f;      
        desc.upDirection = PxVec3(0, 1, 0);

       g_controller = g_controllerManager->createController(desc);
    }

    void MoveCharacterController(const glm::vec3& direction, float deltaTime) {

        g_ControllerVerticalVelocity -= 0.81f * deltaTime;
        

        physx::PxVec3 displacement(direction.x, g_ControllerVerticalVelocity, direction.z);
        PxControllerCollisionFlags flags = g_controller->move(displacement, 0.0f, deltaTime, nullptr);

        if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
           g_ControllerVerticalVelocity = 0.0f;
        }
    }

    void UpdateCharacterControllerVerticalVelocity() {
        float jumpVelocity = 0.15f; 
        g_ControllerVerticalVelocity = jumpVelocity;
    }

    PxExtendedVec3 GetCharacterControllerPosition() {
        return g_controller->getPosition();
    }

    uint64_t CreateRigidDynamicBox(PhysicsTransformData transform, const PxVec3& halfExtents, PxReal mass) {
        PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
        PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        PxTransform pxTransform(pxPos, pxRot);

        PxRigidDynamic* pxRigidDynamic = g_physics->createRigidDynamic(pxTransform);

        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_material);
        pxRigidDynamic->attachShape(*shape);
        shape->release();

       /* PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, mass);*/
        g_scene->addActor(*pxRigidDynamic);

        // create rigid dynamic
        uint64_t physicsId = Utils::GenerateUniqueID();
        RigidDynamic& rigidDynamic = g_rigidDynamic[physicsId];
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.UpdateMassAndInertia(mass);

        return physicsId;
    }

    RigidDynamic* GetRigidDynamicById(uint64_t id) {
        auto it = g_rigidDynamic.find(id);
        if (it != g_rigidDynamic.end()) {
            return &it->second;
        }
        return nullptr;
    }

    uint64_t CreateRigidStaticBox(PhysicsTransformData transform, const PxVec3& halfExtents) {
        PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
        PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        PxTransform pxTransform(pxPos, pxRot);

        PxRigidStatic* pxRigidStatic = g_physics->createRigidStatic(pxTransform);

        // create shape
        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_material);
        pxRigidStatic->attachShape(*shape);
        shape->release();

        g_scene->addActor(*pxRigidStatic);

        // create rigid static
        uint64_t physicsId = Utils::GenerateUniqueID();
        RigidStatic& rigidStatic = g_rigidStatic[physicsId];

        rigidStatic.SetPxRigidStatic(pxRigidStatic);

        return physicsId;
    }

    uint64_t CreateRigidStaticConvexMeshFromVertices(const std::vector<glm::vec3>& vertices, const PhysicsTransformData& transform) {
        // Convert vertices to PxVec3
        std::vector<PxVec3> pxVertices; 
        pxVertices.reserve(vertices.size());
        for (const glm::vec3& v : vertices) {
            pxVertices.emplace_back(v.x, v.y, v.z);
        }

        PxConvexMeshDesc convexDesc;
        convexDesc.points.count = static_cast<PxU32>(pxVertices.size());
        convexDesc.points.stride = sizeof(PxVec3);
        convexDesc.points.data = pxVertices.data();
        convexDesc.flags = PxConvexFlag::eSHIFT_VERTICES | PxConvexFlag::eCOMPUTE_CONVEX;

        PxTolerancesScale scale;
        PxCookingParams params(scale);

        PxDefaultMemoryOutputStream writeBuffer;
        PxConvexMeshCookingResult::Enum result;
        if (!PxCookConvexMesh(params, convexDesc, writeBuffer, &result)) {
            std::cout << "Convex mesh cooking failed!\n";
            return 0;
        }

        PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        PxConvexMesh* convexMesh = g_physics->createConvexMesh(readBuffer);
        PxConvexMeshGeometryFlags flags(~PxConvexMeshGeometryFlag::eTIGHT_BOUNDS);
        PxConvexMeshGeometry geometry(convexMesh, PxMeshScale(PxVec3(1.0f)), flags);

        PxShape* pxShape = g_physics->createShape(geometry, *g_material);

        // create PxRigidStatic
        PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
        PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        PxTransform pxTransform(pxPos, pxRot);

        PxRigidStatic* pxRigidStatic = g_physics->createRigidStatic(pxTransform);
        pxRigidStatic->attachShape(*pxShape);
        pxShape->release();
        g_scene->addActor(*pxRigidStatic);

        // store to RigidStatic
        uint64_t physicsId = Utils::GenerateUniqueID();
        RigidStatic& rigidStatic = g_rigidStatic[physicsId];
        rigidStatic.SetPxRigidStatic(pxRigidStatic);

        return physicsId;
    }

    RigidStatic* GetRigidStaticById(uint64_t id) {
        auto it = g_rigidStatic.find(id);
        if (it != g_rigidStatic.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::unordered_map<uint64_t, RigidStatic>& GetRigidStaticsMap() {
        return g_rigidStatic;
    }

    glm::vec3 PxVec3toGlmVec3(PxVec3 vec) {
        return { vec.x, vec.y, vec.z };
    }

    glm::quat PxQuatToGlmQuat(PxQuat quat) {
        return { quat.x, quat.y, quat.z, quat.w };
    }

    PxScene* GetScene() {
        return g_scene;
    }

    PhysicsTransformData GetActorTransform(const physx::PxRigidActor* actor) {
        PhysicsTransformData transformData;

        if (!actor) return transformData;

        const physx::PxTransform& transform = actor->getGlobalPose();
        transformData.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
        transformData.rotation = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

        return transformData;
    }

    void CleanupPhysX() {
        g_rigidDynamic.clear();
        g_rigidStatic.clear();
      /*  g_cubeActor->release();*/
        g_controller->release();
        g_controllerManager->release();
        g_scene->release();
        g_dispatcher->release();
        g_material->release();
        g_physics->release();
        g_foundation->release();
    }
}