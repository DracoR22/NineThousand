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
        // Initialize PhysX foundation and physics instance
        _gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _gAllocator, _gErrorCallback);
        _gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_gFoundation, PxTolerancesScale(), true);

        // Create a CPU dispatcher for multithreading
        _gDispatcher = PxDefaultCpuDispatcherCreate(2);

        // Create a PhysX scene with gravity
        PxSceneDesc sceneDesc(_gPhysics->getTolerancesScale()); 
        sceneDesc.gravity = PxVec3(0.0f, -0.05f, 0.0f); // -9.81 earths gravity
        sceneDesc.cpuDispatcher = _gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        _gScene = _gPhysics->createScene(sceneDesc);

        // Create a default material for the simulation
        _gMaterial = _gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // Static friction, dynamic friction, restitution
    }


    PxRigidDynamic* CreateDynamicActor(const PxVec3& position, const PxVec3& halfExtents, PxReal mass) {
        PxTransform transform(position);
        PxRigidDynamic* cubeActor = _gPhysics->createRigidDynamic(transform);

        PxShape* shape = _gPhysics->createShape(PxBoxGeometry(halfExtents), *_gMaterial);
        cubeActor->attachShape(*shape);
        shape->release();

        PxRigidBodyExt::updateMassAndInertia(*cubeActor, mass);
        _gScene->addActor(*cubeActor);

        printf("Cube actor created at position: x=%f, y=%f, z=%f\n",
            position.x, position.y, position.z);

        _cubeActor = cubeActor;

        return cubeActor;
    }

    PxRigidStatic* CreateStaticActor(const PxVec3& position, const PxVec3& halfExtents) {
        PxTransform transform(position);

        PxRigidStatic* staticActor = _gPhysics->createRigidStatic(transform);

        // Attach a shape with box geometry to the actor
        PxShape* shape = _gPhysics->createShape(PxBoxGeometry(halfExtents), *_gMaterial);
        staticActor->attachShape(*shape);
        shape->release();

        _gScene->addActor(*staticActor);

        printf("Static actor (wall) created at position: x=%f, y=%f, z=%f\n", position.x, position.y, position.z);

        return staticActor;
    }

    void Simulate(double dt) {
        _gScene->simulate(dt);
        _gScene->fetchResults(true);
    }

    void UpdateModelFromPhysics(Model& model, const physx::PxRigidActor* actor) {
        if (!actor) return;

        const physx::PxTransform& transform = actor->getGlobalPose();
        glm::vec3 position(transform.p.x, transform.p.y, transform.p.z);
        glm::quat rotationQuat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
        glm::mat4 rotation = glm::mat4_cast(rotationQuat);

        model.setPosition(position);
        model.setRotation(rotation);
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