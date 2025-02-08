#include <algorithm>
#include <cmath>
#include <iostream>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "animator.hpp"
#include "bvhParser.hpp"

// ----------------------------------------
// Animator::Joint
// ----------------------------------------

ikura::GroupID Animator::Joint::getID() const { return id; }

glm::vec3 Animator::Joint::getPos() const { return pos; }

std::vector<ikura::GroupID> Animator::Joint::getParentIDs() const {
    return parentIDs;
}

std::string Animator::Joint::getName() const { return name; }

bool Animator::Joint::getIsEdge() const { return isEdge; }

void Animator::Joint::setClosestChildIDs(std::vector<ikura::GroupID> childIDs) {
    closestChildIDs = childIDs;
}

// ----------------------------------------
// Animator
// ----------------------------------------

void Animator::updateAnimator(float deltaTime) {
    if (animationStopped) {
        return;
    }

    animationTime += deltaTime * animationSpeed;

    float loopStartTime;
    float loopEndTime;
    if (loopEnabled) {
        loopStartTime = loopStartFrameIndex * frameRate;
        loopEndTime = loopEndFrameIndex * frameRate;
        animationTime =
            fmod((animationTime - loopStartTime), loopDurationTime) +
            loopStartTime;
    } else {
        loopStartTime = 0;
        loopEndTime = numOfFrames * frameRate;
        animationTime = fmod(animationTime, numOfFrames * frameRate);
    }
}

Animator::Animator(std::shared_ptr<UI> ui) { this->ui = ui; }

void Animator::initFromBVH(std::string filePath) {
    BVHParser parser(filePath);
    parser.parseBVH();
    assert(parser.getSkentonData().size() <= ikura::NUM_OF_MODEL_MATRIX);

    joints = parser.getSkentonData();
    motion = parser.getMotion();
    numOfFrames = motion->numOfFrames;
    frameRate = motion->frameRate;

    // set default rotation order for ui
    {
        std::string rotationOrderStr =
            convertRotationAxisEnumToRotationOrderStr(motion->rotationOrder);

        size_t arrSize = sizeof(ui->config.rotationOrderComboItems) /
                         sizeof(ui->config.rotationOrderComboItems[0]);

        for (size_t i = 0; i < arrSize; i++) {
            if (strcmp(ui->config.rotationOrderComboItems[i],
                       rotationOrderStr.c_str()) == 0) {
                ui->config.rotationOrderIndex = i;
                break;
            }
        }
    }

    loopStartFrameIndex = 0;
    loopEndFrameIndex = numOfFrames - 1;
    loopDurationTime = frameRate * numOfFrames;

    animationTime = 0;
    animationSpeed = 1.0;
    animationStopped = false;

    sourceFilePath = filePath;
}

void Animator::generateBones(
    std::vector<std::shared_ptr<ikura::shapes::Shape>> &bones) {
    assert(joints.size() <= ikura::NUM_OF_MODEL_MATRIX);
    bones.clear();
    bones.resize(joints.size());

    uint32_t baseIndex = 0;
    for (ikura::GroupID id = 0; id < joints.size(); id++) {
        if (joints[id]->getParentIDs().empty()) {
            // Root Joint
            bones[id] = std::make_shared<ikura::shapes::SingleColorCube>(
                2.0, 2.0, 2.0, glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(1.0, 0.0, 0.0), id);
        } else {
            float length = glm::length(joints[id]->getPos());
            bones[id] =
                std::make_shared<ikura::shapes::OctahedronBone>(length, id);
        }
        bones[id]->setBaseIndex(baseIndex);
        baseIndex += bones[id]->getVertices().size();
    }
}

std::array<glm::mat4, ikura::NUM_OF_MODEL_MATRIX>
Animator::generateModelMatrices() {
    uint32_t frameIndex = getCurrentFrameIndex();

    // calculate current motion
    std::vector<JointState> currentJointStates;
    for (ikura::GroupID id = 0; id < joints.size(); id++) {
        JointState js{};
        js.pos = motion->jointMotions[id]->jointStates[frameIndex]->pos;
        js.rot = motion->jointMotions[id]->jointStates[frameIndex]->rot;
        currentJointStates.push_back(js);
    }

    // generate result matrices
    std::array<glm::mat4, ikura::NUM_OF_MODEL_MATRIX> result;

    for (ikura::GroupID id = 0; id < joints.size(); id++) {
        result[id] = glm::mat4(1.0);
        // convert "right-hand Y-up" to "right-hand Z-up"
        result[id] *= glm::rotate(glm::mat4(1.0), glm::radians(90.0f),
                                  glm::vec3(1.0, 0.0, 0.0));

        const std::vector<ikura::GroupID> &parentIDs =
            joints[id]->getParentIDs();

        for (size_t parentIdx = 0; parentIdx < parentIDs.size(); parentIdx++) {
            ikura::GroupID pID = parentIDs[parentIdx];
            // Move to each parent's position
            if (pID == 0) {
                // Motion position
                result[id] *=
                    glm::translate(glm::mat4(1.0), currentJointStates[pID].pos);
            } else {
                // Joint offset
                result[id] *=
                    glm::translate(glm::mat4(1.0), joints[pID]->getPos());
            }

            // Motion rotation
            const auto multiplyRotateMat = [&](RotationAxisEnum axis) {
                glm::vec3 axisVec3;
                float radians;
                switch (axis) {
                case RotationAxisEnum::X:
                    radians = glm::radians(currentJointStates[pID].rot.x);
                    axisVec3 = glm::vec3(1.0, 0.0, 0.0);
                    break;
                case RotationAxisEnum::Y:
                    radians = glm::radians(currentJointStates[pID].rot.y);
                    axisVec3 = glm::vec3(0.0, 1.0, 0.0);
                    break;
                case RotationAxisEnum::Z:
                    radians = glm::radians(currentJointStates[pID].rot.z);
                    axisVec3 = glm::vec3(0.0, 0.0, 1.0);
                    break;
                }
                result[id] *= glm::rotate(glm::mat4(1.0), radians, axisVec3);
            };
            std::for_each(motion->rotationOrder.begin(),
                          motion->rotationOrder.end(), multiplyRotateMat);
        }

        // Move to current joint's position
        result[id] *= glm::translate(glm::mat4(1.0),
                                     id != 0 ? joints[id]->getPos()
                                             : currentJointStates[id].pos);

        // rotate current joint object to turn to parent
        glm::vec3 pos = glm::normalize(joints[id]->getPos());
        glm::vec3 orig = glm::vec3(1.0, 0.0, 0.0);
        glm::vec3 cross = glm::normalize(glm::cross(pos, orig));
        if (glm::length(cross) > 0) {
            result[id] *= glm::rotate(
                glm::mat4(1.0),
                glm::pi<float>() - glm::acos(glm::dot(pos, orig)), cross);
        } else if (pos.x > 0) {
            result[id] *= glm::rotate(glm::mat4(1.0), glm::radians(180.0f),
                                      glm::vec3(0.0, 1.0, 0.0));
        }
    }

    return result;
}

uint32_t Animator::getNumOfJoints() const { return joints.size(); }

uint32_t Animator::getNumOfFrames() const { return numOfFrames; }

float Animator::getFrameRate() const { return frameRate; }

uint32_t Animator::getLoopStartFrameIndex() const {
    return loopStartFrameIndex;
}

uint32_t Animator::getLoopEndFrameIndex() const { return loopEndFrameIndex; }

uint32_t Animator::getCurrentFrameIndex() const {
    return std::floor(animationTime / frameRate);
}

float Animator::getAnimationTime() const { return animationTime; }

float Animator::getAnimationSpeed() const { return animationSpeed; }

std::string Animator::getSourceFilePath() { return sourceFilePath; }

const std::shared_ptr<Motion> &Animator::getMotion() const { return motion; }
const std::vector<std::shared_ptr<Animator::Joint>> &
Animator::getJoints() const {
    return joints;
}
const std::vector<ikura::GroupID> &Animator::Joint::getClosestChildIDs() const {
    return closestChildIDs;
}

void Animator::setRotationOrder(std::array<RotationAxisEnum, 3> rotationOrder) {
    std::cout << "aaa";
    motion->rotationOrder = rotationOrder;
}

bool Animator::isAnimationStopped() const { return animationStopped; }

void Animator::stopAnimation() { animationStopped = true; }

void Animator::resumeAnimation() { animationStopped = false; }

void Animator::setLoopEnabled(bool enabled) { loopEnabled = enabled; }

void Animator::enableLoop() { loopEnabled = true; }

void Animator::disableLoop() { loopEnabled = false; }

void Animator::updateLoopRange(uint32_t _loopStartFrameIndex,
                               uint32_t _loopEndFrameIndex) {

    loopStartFrameIndex = std::clamp(_loopStartFrameIndex, 0U, numOfFrames - 1);
    loopEndFrameIndex = std::clamp(_loopEndFrameIndex, 0U, numOfFrames - 1);

    loopDurationTime = (loopEndFrameIndex - loopStartFrameIndex) * frameRate;

    if (loopEnabled) {
        uint32_t clampedFrameIndex = std::clamp(
            getCurrentFrameIndex(), loopStartFrameIndex, loopEndFrameIndex);
        if (clampedFrameIndex != getCurrentFrameIndex()) {
            animationTime = clampedFrameIndex * frameRate;
        }
    }
}

void Animator::seekAnimation(uint32_t frameIndex) {
    if (loopEnabled) {
        frameIndex =
            std::clamp(frameIndex, loopStartFrameIndex, loopEndFrameIndex);
    }

    animationTime = frameIndex * frameRate;
}

void Animator::incrementFrameIndex(int inc) {
    uint32_t currentFrameIndex = getCurrentFrameIndex();

    // if (currentFrameIndex + inc < 0), but considering unsigned int
    if (inc < 0 && currentFrameIndex < -inc) {
        seekAnimation(0);
    } else {
        uint32_t newFrameIndex =
            std::clamp(currentFrameIndex + inc, 0U, numOfFrames - 1);
        seekAnimation(newFrameIndex);
    }
}

void Animator::setAnimationSpeed(float speed) { animationSpeed = speed; }

void Animator::Joint::showInfo() {
    std::cout << id << ": " << name << std::ends;
    std::cout << (isEdge ? " (Edge)" : " ") << std::endl;
    std::cout << "\tPosition: ( " << pos.x << ", " << pos.y << ", " << pos.z
              << " )" << std::endl;
    std::cout << "\tParents: ( " << std::ends;
    std::for_each(parentIDs.begin(), parentIDs.end(), [](ikura::GroupID id) {
        std::cout << id << ", " << std::ends;
    });
    std::cout << ")" << std::endl;
}

void Animator::showSkeltonInfo() {
    std::for_each(
        joints.begin(), joints.end(),
        [](const std::shared_ptr<Joint> &joint) { joint->showInfo(); });
    std::cout << "Number of Joints:" << joints.size() << std::endl;
}

void Animator::showMotionInfo() {
    for (uint32_t frame = 0; frame < numOfFrames; frame++) {
        std::cout << "frame:" << frame << std::endl;
        for (uint32_t joint = 0; joint < joints.size(); joint++) {
            std::cout << "(("
                      << motion->jointMotions[joint]->jointStates[frame]->pos.x
                      << ","
                      << motion->jointMotions[joint]->jointStates[frame]->pos.y
                      << ","
                      << motion->jointMotions[joint]->jointStates[frame]->pos.z
                      << "), ("
                      << motion->jointMotions[joint]->jointStates[frame]->rot.x
                      << ","
                      << motion->jointMotions[joint]->jointStates[frame]->rot.y
                      << ","
                      << motion->jointMotions[joint]->jointStates[frame]->rot.z
                      << ")), " << std::ends;
        }
        std::cout << std::endl;
    }
}
