#include "Physics.h"

namespace Physics {
    using namespace physx;

    PxDefaultAllocator g_allocator;
    PxDefaultErrorCallback g_errorCallback;
    PxFoundation* g_foundation = nullptr;
    PxPhysics* g_physics = nullptr;
    PxDefaultCpuDispatcher* g_dispatcher = nullptr;
    PxScene* g_scene = nullptr;
    PxMaterial* g_defaultMaterial = nullptr;
    PxControllerManager* g_controllerManager = nullptr;

    PxRigidDynamic* g_cubeActor = nullptr;

    std::unordered_map<uint64_t, RigidStatic> g_rigidStatics;
    std::unordered_map<uint64_t, RigidDynamic> g_rigidDynamics;
    std::unordered_map<uint64_t, CharacterController> g_charaterControllers;

    double g_fixedDeltaTime = 1.0 / 60.0;
    double g_accumulatedTime = 0.0;

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

        g_defaultMaterial = g_physics->createMaterial(0.5f, 0.5f, 0.6f);

        // initialize character controller manager
        g_controllerManager = PxCreateControllerManager(*g_scene);

        // create a ground plane
        PxRigidStatic* groundPlane = nullptr;
        groundPlane = PxCreatePlane(*g_physics, PxPlane(0.0f, 1.0f, 0.0f, 0.0f), *g_defaultMaterial);
        g_scene->addActor(*groundPlane);
    }

    void BeginFrame() {
        // remove marked rigid dynamics
        if (!g_rigidDynamics.empty()) {
            std::vector<uint64_t> idsToRemove;

            // Gather marked actors
            for (const auto& [id, rigidDynamic] : g_rigidDynamics) {
                if (rigidDynamic.IsMarkedForRemoval()) {
                    idsToRemove.push_back(id);
                }
            }

            // Remove them
            for (uint64_t id : idsToRemove) {
                Physics::RemoveRigidDynamic(id);
            }
        }
    }

    void Simulate(double deltaTime) {
        g_accumulatedTime += deltaTime;

        while (g_accumulatedTime >= g_fixedDeltaTime) {
            g_accumulatedTime -= g_fixedDeltaTime;
            g_scene->simulate(g_fixedDeltaTime);
            g_scene->fetchResults(true);
        }
    }

    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass) {
        PxTransform transform(position);
        PxRigidDynamic* cubeActor = g_physics->createRigidDynamic(transform);

        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_defaultMaterial);
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

        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_defaultMaterial);
        staticActor->attachShape(*shape);
        shape->release();

        g_scene->addActor(*staticActor);

        return staticActor;
    }

    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass) {
        PxTransform transform(position);

        PxRigidDynamic* capsuleActor = g_physics->createRigidDynamic(transform);

        PxShape* shape = g_physics->createShape(PxCapsuleGeometry(radius, halfHeight), *g_defaultMaterial);
        capsuleActor->attachShape(*shape);

        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*capsuleActor, mass);

        g_scene->addActor(*capsuleActor);

        return capsuleActor;
    }

    /*void CreateCharacterController() {
        g_controllerManager = PxCreateControllerManager(*g_scene);

        physx::PxCapsuleControllerDesc desc;
        desc.height = 1.8f;
        desc.radius = 0.3f;
        desc.material = g_physics->createMaterial(0.5f, 0.5f, 0.0f);
        desc.position = PxExtendedVec3(35.0f, 5.5f, 55.0f);
        desc.slopeLimit = 0.707f;
        desc.stepOffset = 0.5f;
        desc.upDirection = PxVec3(0, 1, 0);

       g_controller = g_controllerManager->createController(desc);
    }*/

    uint64_t CreateCharacterController(const glm::vec3& position, float height) {
        PxCapsuleControllerDesc desc;
        desc.height = height - 2.0f * desc.radius;
        desc.radius = 0.3f;
        desc.material = g_defaultMaterial;
        desc.position = PxExtendedVec3(position.x, position.y, position.z);
        desc.slopeLimit = 0.707f;
        desc.stepOffset = 0.5f;
        desc.upDirection = PxVec3(0, 1, 0);

        PxController* pxController = g_controllerManager->createController(desc);

        uint64_t physicsId = Utils::GenerateUniqueID();
        CharacterController& characterController = g_charaterControllers[physicsId];
        characterController.SetPxController(pxController);

        return physicsId;
    }

    void MoveCharacterController(uint64_t physicsId, const glm::vec3& direction) {
        CharacterController* characterController = GetCharacterControllerById(physicsId);

        if (characterController) {
            characterController->Move(direction);
        }
        else {
            std::cout << "ERROR: Physics::MoveCharacterController() controller with physicsId: " << physicsId << " was nullptr!\n";
        }
    }

    void UpdateCharacterControllerVerticalVelocity(uint64_t physicsId) {
        CharacterController* characterController = GetCharacterControllerById(physicsId);

        if (characterController) {
            characterController->SetVerticalVelocity(0.35f);
        }
        else {
            std::cout << "ERROR: Physics::UpdateCharacterControllerVerticalVelocity() controller with physicsId: " << physicsId << " was nullptr!\n";
        }
    }

    glm::vec3 GetCharacterControllerPosition(uint64_t physicsId) {
        CharacterController* characterController = GetCharacterControllerById(physicsId);

        if (characterController) {
            PxExtendedVec3 position = characterController->GetPxController()->getPosition();
            return glm::vec3(position.x, position.y, position.z);
        }
        else {
            std::cout << "ERROR: Physics::GetCharacterControllerPosition() controller with physicsId: " << physicsId << " was nullptr!\n";
            return glm::vec3(0.0f);
        }
    }

    CharacterController* GetCharacterControllerById(uint64_t physicsId) {
        auto it = g_charaterControllers.find(physicsId);
        if (it != g_charaterControllers.end()) {
            return &it->second;
        }
        return nullptr;
    }

    uint64_t CreateRigidDynamicBox(PhysicsTransformData transform, const glm::vec3& halfExtents, PxReal mass, const glm::vec3 initialForce, const glm::vec3 initialTorque) {
        PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
        PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        PxTransform pxTransform(pxPos, pxRot);

        PxRigidDynamic* pxRigidDynamic = g_physics->createRigidDynamic(pxTransform);

        PxVec3 pxHalfExtents = PxVec3(halfExtents.x, halfExtents.y, halfExtents.z);

        PxTransform shapeOffset(PxVec3(0, pxHalfExtents.y, 0));
        PxShape* shape = g_physics->createShape(PxBoxGeometry(pxHalfExtents), *g_defaultMaterial);
        shape->setLocalPose(shapeOffset);
        pxRigidDynamic->attachShape(*shape);
        shape->release();

        /* PxRigidBodyExt::updateMassAndInertia(*pxRigidDynamic, mass);*/
        g_scene->addActor(*pxRigidDynamic);

        PxVec3 force = PxVec3(initialForce.x, initialForce.y, initialForce.z);
        pxRigidDynamic->addForce(force, PxForceMode::eIMPULSE);

        PxVec3 torque = PxVec3(initialTorque.x, initialTorque.y, initialTorque.z);
        pxRigidDynamic->addTorque(torque, PxForceMode::eIMPULSE);

        //pxRigidDynamic->setMaxAngularVelocity(1.0f);

        // create rigid dynamic
        uint64_t physicsId = Utils::GenerateUniqueID();
        RigidDynamic& rigidDynamic = g_rigidDynamics[physicsId];
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.UpdateMassAndInertia(mass);

        return physicsId;
    }

    RigidDynamic* GetRigidDynamicById(uint64_t id) {
        auto it = g_rigidDynamics.find(id);
        if (it != g_rigidDynamics.end()) {
            return &it->second;
        }
        return nullptr;
    }

    uint64_t CreateRigidDynamicConvexMeshFromVertices(std::vector<Vertex>& vertices, const PhysicsTransformData& transform, float mass, const glm::vec3& scale, const glm::vec3 initialForce, const glm::vec3 initialTorque) {
        // Convert vertices to PxVec3
        std::vector<PxVec3> pxVertices;
        pxVertices.reserve(vertices.size());
        for (Vertex& v : vertices) {
            pxVertices.emplace_back(v.m_Position.x, v.m_Position.y, v.m_Position.z);
        }

        PxConvexMeshDesc convexDesc;
        convexDesc.points.count = static_cast<PxU32>(pxVertices.size());
        convexDesc.points.stride = sizeof(PxVec3);
        convexDesc.points.data = pxVertices.data();
        convexDesc.flags = PxConvexFlag::eSHIFT_VERTICES | PxConvexFlag::eCOMPUTE_CONVEX;

        PxTolerancesScale scaleHint;
        PxCookingParams params(scaleHint);

        PxDefaultMemoryOutputStream writeBuffer;
        PxConvexMeshCookingResult::Enum result;
        if (!PxCookConvexMesh(params, convexDesc, writeBuffer, &result)) {
            std::cout << "Convex mesh cooking failed!\n";
            return 0;
        }

        PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
        PxConvexMesh* convexMesh = g_physics->createConvexMesh(readBuffer);
        PxConvexMeshGeometryFlags flags(~PxConvexMeshGeometryFlag::eTIGHT_BOUNDS);
        // PxConvexMeshGeometry geometry(convexMesh, PxMeshScale(PxVec3(1.0f)), flags);

        PxMeshScale pxScale(PxVec3(scale.x, scale.y, scale.z));
        PxConvexMeshGeometry geometry(convexMesh, pxScale, flags);

        PxShape* pxShape = g_physics->createShape(geometry, *g_defaultMaterial);

        // create rigid dynamic
        PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
        PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        PxTransform pxTransform(pxPos, pxRot);
        PxRigidDynamic* pxRigidDynamic = g_physics->createRigidDynamic(pxTransform);

        pxRigidDynamic->attachShape(*pxShape);
        pxShape->release();
        g_scene->addActor(*pxRigidDynamic);


        PxVec3 force = PxVec3(initialForce.x, initialForce.y, initialForce.z);
        pxRigidDynamic->addForce(force, PxForceMode::eIMPULSE);

        PxVec3 torque = PxVec3(initialTorque.x, initialTorque.y, initialTorque.z);
        pxRigidDynamic->addTorque(torque);

        // create rigid dynamic
        uint64_t physicsId = Utils::GenerateUniqueID();
        RigidDynamic& rigidDynamic = g_rigidDynamics[physicsId];
        rigidDynamic.SetPxRigidDynamic(pxRigidDynamic);
        rigidDynamic.UpdateMassAndInertia(mass);

        return physicsId;
    }

    void RemoveRigidDynamic(uint64_t id) {
        RigidDynamic* rigidDynamic = GetRigidDynamicById(id);

        if (rigidDynamic) {
            PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();

            if (pxRigidDynamic->userData) {
                pxRigidDynamic->userData = nullptr;
            }
            if (pxRigidDynamic->getScene() != nullptr) {
                g_scene->removeActor(*pxRigidDynamic);
            }

            pxRigidDynamic->release();

            // Remove from container
            g_rigidDynamics.erase(id);
        }
    }

    void MarkRigidDynamicForRemoval(uint64_t id) {
        RigidDynamic* rigidDynamic = GetRigidDynamicById(id);

        if (rigidDynamic) {
            rigidDynamic->MarkForRemoval();
        }
    }

    void SetRigidDynamicGlobalPose(uint64_t id, glm::mat4 transformMatrix) {
        RigidDynamic* rigidDynamic = GetRigidDynamicById(id);

        if (!rigidDynamic) return;

        PxRigidDynamic* pxRigidDynamic = rigidDynamic->GetPxRigidDynamic();

        if (!pxRigidDynamic) return;

        PxMat44 pxMatrix = GlmMat4ToPxMat44(transformMatrix);
        PxTransform pxTransform = PxTransform(pxMatrix);
        pxRigidDynamic->setGlobalPose(pxTransform);
    }

    uint64_t CreateRigidStaticBox(PhysicsTransformData transform, const PxVec3& halfExtents) {
        PxVec3 pxPos(transform.position.x, transform.position.y, transform.position.z);
        PxQuat pxRot(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
        PxTransform pxTransform(pxPos, pxRot);

        PxRigidStatic* pxRigidStatic = g_physics->createRigidStatic(pxTransform);

        // create shape
        PxShape* shape = g_physics->createShape(PxBoxGeometry(halfExtents), *g_defaultMaterial);
        pxRigidStatic->attachShape(*shape);
        shape->release();

        g_scene->addActor(*pxRigidStatic);

        // create rigid static
        uint64_t physicsId = Utils::GenerateUniqueID();
        RigidStatic& rigidStatic = g_rigidStatics[physicsId];

        rigidStatic.SetPxRigidStatic(pxRigidStatic);

        return physicsId;
    }

    uint64_t CreateRigidStaticConvexMeshFromVertices(std::vector<Vertex>& vertices, const PhysicsTransformData& transform) {
        // Convert vertices to PxVec3
        std::vector<PxVec3> pxVertices; 
        pxVertices.reserve(vertices.size());
        for (Vertex& v : vertices) {
            pxVertices.emplace_back(v.m_Position.x, v.m_Position.y, v.m_Position.z);
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

        PxShape* pxShape = g_physics->createShape(geometry, *g_defaultMaterial);

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
        RigidStatic& rigidStatic = g_rigidStatics[physicsId];
        rigidStatic.SetPxRigidStatic(pxRigidStatic);

        return physicsId;
    }

    RigidStatic* GetRigidStaticById(uint64_t id) {
        auto it = g_rigidStatics.find(id);
        if (it != g_rigidStatics.end()) {
            return &it->second;
        }
        return nullptr;
    }

    std::unordered_map<uint64_t, RigidStatic>& GetRigidStaticsMap() {
        return g_rigidStatics;
    }

    void SetRigidStaticGlobalPose(uint64_t id, glm::mat4 transformMatrix) {
        RigidStatic* rigidStatic = GetRigidStaticById(id);

        if (!rigidStatic) return;

        PxRigidStatic* pxRigidStatic = rigidStatic->GetPxRigidStatic();
        
        if (!pxRigidStatic) return;

        PxMat44 pxMatrix = GlmMat4ToPxMat44(transformMatrix);
        PxTransform pxTransform = PxTransform(pxMatrix);
        pxRigidStatic->setGlobalPose(pxTransform);
    }

    glm::vec3 PxVec3toGlmVec3(PxVec3 vec) {
        return { vec.x, vec.y, vec.z };
    }

    glm::quat PxQuatToGlmQuat(PxQuat quat) {
        return { quat.x, quat.y, quat.z, quat.w };
    }

    PxMat44 GlmMat4ToPxMat44(glm::mat4 glmMatrix) {
        PxMat44 matrix;
        std::copy(glm::value_ptr(glmMatrix),
            glm::value_ptr(glmMatrix) + 16,
            reinterpret_cast<float*>(&matrix));
        return matrix;
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

    double GetInterpolationAlpha() {
        return g_accumulatedTime / g_fixedDeltaTime;
    }

    void CleanupPhysX() {
        g_rigidDynamics.clear();
        g_rigidStatics.clear();
        g_charaterControllers.clear();
     
        g_controllerManager->release();
        g_scene->release();
        g_dispatcher->release();
        g_defaultMaterial->release();
        g_physics->release();
        g_foundation->release();
    }
}