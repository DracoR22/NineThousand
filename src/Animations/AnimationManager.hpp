#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <glm/glm.hpp>
#include "Animator.hpp"

class AnimationManager {
private:
    Animator* animator = nullptr;
    std::thread animationThread;
    std::mutex boneMutex;
    bool isRunning = false;

public:
    AnimationManager() = default;
    ~AnimationManager() { Stop(); }

    void SetAnimator(Animator* anim) {
        animator = anim;
    }

    void Start() {
        if (!animator || isRunning) return;
        isRunning = true;
        animationThread = std::thread(&AnimationManager::UpdateLoop, this);
    }

    void Stop() {
        if (isRunning) {
            isRunning = false;
            if (animationThread.joinable())
                animationThread.join();
        }
    }

    void UpdateLoop(float deltaTime) {
        while (isRunning) {
            if (animator) {
                std::lock_guard<std::mutex> lock(boneMutex);
                animator->UpdateAnimation(deltaTime);  
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    std::vector<glm::mat4> GetBoneMatrices() {
        std::lock_guard<std::mutex> lock(boneMutex);
        return animator ? animator->GetFinalBoneMatrices() : std::vector<glm::mat4>{};
    }
};