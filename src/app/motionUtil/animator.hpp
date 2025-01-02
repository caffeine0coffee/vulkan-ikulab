#pragma once

#include <array>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <ikura/ikura.hpp>

#include "../context/ui.hpp"
#include "./common.hpp"

#define MAX_ANIMATION_SPEED 10.0f
#define MIN_ANIMATION_SPEED (1.0f / 128.0f)

class Animator {
    std::shared_ptr<Motion> motion;
    std::shared_ptr<UI> ui;
    uint32_t numOfFrames;
    std::string sourceFilePath;

    // loop range is [ start : end ]
    // e.g. start=2, end=5 -> 2 3 4 5 2 3 4 5 ....
    uint32_t loopStartFrameIndex;
    uint32_t loopEndFrameIndex;
    float loopDurationTime;

    float frameRate;
    float animationTime;
    float animationSpeed;

    bool animationStopped;
    bool loopEnabled;

  public:
    class Joint {
        ikura::GroupID id;
        std::string name;
        glm::vec3 pos;
        // [parent, grand parent, ..., root]
        std::vector<ikura::GroupID> parentIDs;
        // [childA, childB, ...]
        std::vector<ikura::GroupID> closestChildIDs;
        bool isEdge;

        static ikura::GroupID currentID;

      public:
        Joint(std::string name, ikura::GroupID id, glm::vec3 pos,
              std::vector<ikura::GroupID> parentIDs, bool isEdge)
            : name(name), id(id), pos(pos), parentIDs(parentIDs),
              isEdge(isEdge) {}

        ikura::GroupID getID() const;
        glm::vec3 getPos() const;
        std::vector<ikura::GroupID> getParentIDs() const;
        std::string getName() const;
        bool getIsEdge() const;
        const std::vector<ikura::GroupID> &getClosestChildIDs() const;

        void setClosestChildIDs(std::vector<ikura::GroupID> childIDs);

        void showInfo();
    };

    Animator(std::shared_ptr<UI> ui);

    void initFromBVH(std::string filePath);
    void
    generateBones(std::vector<std::shared_ptr<ikura::shapes::Shape>> &bones);
    std::array<glm::mat4, ikura::NUM_OF_MODEL_MATRIX> generateModelMatrices();
    void updateAnimator(float deltaTime);

    uint32_t getNumOfJoints() const;
    uint32_t getNumOfFrames() const;
    float getFrameRate() const;
    uint32_t getLoopStartFrameIndex() const;
    uint32_t getLoopEndFrameIndex() const;
    uint32_t getCurrentFrameIndex() const;
    float getAnimationTime() const;
    float getAnimationSpeed() const;
    std::string getSourceFilePath();
    const std::shared_ptr<Motion> &getMotion() const;
    const std::vector<std::shared_ptr<Animator::Joint>> &getJoints() const;

    void setRotationOrder(std::array<RotationAxisEnum, 3> rotationOrder);

    bool isAnimationStopped() const;
    void stopAnimation();
    void resumeAnimation();

    void setLoopEnabled(bool enabled);
    void enableLoop();
    void disableLoop();

    void updateLoopRange(uint32_t _loopStartFrameIndex,
                         uint32_t _loopEndFrameIndex);
    void seekAnimation(uint32_t frameIndex);
    void incrementFrameIndex(int inc);
    void setAnimationSpeed(float speed);

    void showSkeltonInfo();
    void showMotionInfo();

  private:
    std::vector<std::shared_ptr<Animator::Joint>> joints;
};
