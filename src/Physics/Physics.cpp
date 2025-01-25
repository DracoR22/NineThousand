#include "Physics.h"

namespace Physics {
    using namespace physx;
    PxDefaultAllocator _gAllocator;
    PxDefaultErrorCallback _gErrorCallback;
    PxFoundation* _gFoundation = nullptr;
    PxPhysics* _gPhysics = nullptr;
    PxDefaultCpuDispatcher* _gDispatcher = nullptr;
    PxScene* _gScene = nullptr;
    PxMaterial* _gMaterial = nullptr;

    PxRigidDynamic* _cubeActor = nullptr;

    void InitPhysx() {
        _gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _gAllocator, _gErrorCallback);
        _gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_gFoundation, PxTolerancesScale(), true);

        // Create a CPU dispatcher for multithreading
        _gDispatcher = PxDefaultCpuDispatcherCreate(2);

        PxSceneDesc sceneDesc(_gPhysics->getTolerancesScale()); 
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // -9.81 earths gravity
        sceneDesc.cpuDispatcher = _gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        _gScene = _gPhysics->createScene(sceneDesc);

        _gMaterial = _gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // Static friction, dynamic friction, restitution
    }


    PxRigidDynamic* CreateDynamicBox(const PxVec3& position, const PxVec3& halfExtents, PxReal mass) {
        PxTransform transform(position);
        PxRigidDynamic* cubeActor = _gPhysics->createRigidDynamic(transform);

        PxShape* shape = _gPhysics->createShape(PxBoxGeometry(halfExtents), *_gMaterial);
        cubeActor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*cubeActor, mass);
        _gScene->addActor(*cubeActor);

        _cubeActor = cubeActor;

        return cubeActor;
    }

    PxRigidStatic* CreateStaticBox(const PxVec3& position, const PxVec3& halfExtents) {
        PxTransform transform(position);

        PxRigidStatic* staticActor = _gPhysics->createRigidStatic(transform);

        PxShape* shape = _gPhysics->createShape(PxBoxGeometry(halfExtents), *_gMaterial);
        staticActor->attachShape(*shape);
        shape->release();

        _gScene->addActor(*staticActor);

        return staticActor;
    }

    PxRigidDynamic* CreateDynamicCapsule(const PxVec3& position, PxReal halfHeight, PxReal radius, PxReal mass) {
        PxTransform transform(position);

        PxRigidDynamic* capsuleActor = _gPhysics->createRigidDynamic(transform);

        PxShape* shape = _gPhysics->createShape(PxCapsuleGeometry(radius, halfHeight), *_gMaterial);
        capsuleActor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*capsuleActor, mass);

        _gScene->addActor(*capsuleActor);

        return capsuleActor;
    }

    void Simulate(double deltaTime) {
        const double fixedTimestep = 1.0 / 60.0; // 60 fps
        static double accumulatedTime = 0.0;

        accumulatedTime += deltaTime;

        while (accumulatedTime >= fixedTimestep) {
            _gScene->simulate(fixedTimestep);
            _gScene->fetchResults(true);
            accumulatedTime -= fixedTimestep;
        }
    }

    PhysicsTransformData GetTransformFromPhysics(const physx::PxRigidActor* actor) {
        PhysicsTransformData transformData;

        if (!actor) return transformData;

        const physx::PxTransform& transform = actor->getGlobalPose();
        transformData.position = glm::vec3(transform.p.x, transform.p.y, transform.p.z);
        transformData.rotation = glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

        return transformData;
    }

    void CleanupPhysX() {
        _cubeActor->release();
        _gScene->release();
        _gDispatcher->release();
        _gMaterial->release();
        _gPhysics->release();
        _gFoundation->release();
    }
}