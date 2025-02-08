#include "bvhParser.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

// Forward declearation of helper functions ----------
bool isValidChannel(std::string str);

void BVHParser::parseJoints(bool isJointTokenRead,
                            ClosestChildMap &closestChildMap) {
    std::string input, jointName;
    uint32_t numOfChannels;
    glm::vec3 pos;
    bool isRoot = false;
    bool isEndSite = false;
    bool rotationOrderPredicted = false;
    std::vector<RotationAxisEnum> predictedRotationOrder;

    jointIDStack.push_back(currentID);

    auto throwTooManyJointsError = [&] {
        std::string msg;
        msg += "Too many Joints in '";
        msg += filePath;
        msg += "'.\n";
        msg += "The max number of Joints in ikura is: ";
        msg += std::to_string(ikura::NUM_OF_MODEL_MATRIX);
        msg += ".";

        throw std::runtime_error(msg);
    };

    // Root / Joint / End Site definition
    if (!isJointTokenRead) {
        *inputStream >> input;
        bool isInvalidToken = false;
        if (isRootDefined) {
            if (input == TOKEN_ROOT) {
                throw parse_failed_error("Multiple Root definition detected.",
                                         inputStream);
            } else if (input == TOKEN_END_SITE_END) {
                *inputStream >> input;
                if (input == TOKEN_END_SITE_SITE) {
                    isEndSite = true;
                } else {
                    isInvalidToken = true;
                }
            } else if (input != TOKEN_JOINT) {
                isInvalidToken = true;
            }
        } else {
            if (input == TOKEN_ROOT) {
                isRootDefined = true;
                isRoot = true;
            } else if (input == TOKEN_JOINT || input == TOKEN_END_SITE_END) {
                throw parse_failed_error("Root is not defined.", inputStream);
            } else {
                isInvalidToken = true;
            }
        }

        if (isInvalidToken) {
            std::string msg;
            msg += "'";
            msg += TOKEN_ROOT;
            msg += "', '";
            msg += TOKEN_JOINT;
            msg += "' or '";
            msg += TOKEN_END_SITE;
            msg += "' are expected.";
            throw parse_failed_error(msg, inputStream);
        }
    }

    // joint name
    if (!isEndSite) {
        *inputStream >> jointName;
    }

    // Beggin bracket
    *inputStream >> input;
    if (input != TOKEN_BEGGIN_BRACKET) {
        throw parse_failed_error("Beggin bracket is expected.", inputStream);
    }

    // Offset token
    *inputStream >> input;
    if (input != TOKEN_OFFSET) {
        std::string msg;
        msg += "'";
        msg += TOKEN_OFFSET;
        msg += "' is expected.";
        throw parse_failed_error(msg, inputStream);
    }

    // offset values
    try {
        *inputStream >> pos.x >> pos.y >> pos.z;
    } catch (std::exception e) {
        throw parse_failed_error(
            "Invalid Position format. 3 position value are expected.",
            inputStream);
    }

    if (!isEndSite) {
        // Channels token
        *inputStream >> input;
        if (input != TOKEN_CHANNELS) {
            std::string msg;
            msg += "'";
            msg += TOKEN_CHANNELS;
            msg += "' is expected.";
            throw parse_failed_error(msg, inputStream);
        }

        // number of channels
        try {
            *inputStream >> numOfChannels;
        } catch (std::exception e) {
            std::string msg;
            throw parse_failed_error(
                "Number of channels (int value) is expected.", inputStream);
        }

        // register channels
        for (uint32_t i = 0; i < numOfChannels; i++) {
            *inputStream >> input;
            if (!isValidChannel(input)) {
                std::string msg;
                msg += "Invalid Channel name at #";
                msg += i;
                msg += ".";
                throw parse_failed_error(msg, inputStream);
            }

            auto channel = convertStrToChannelEnum(input);

            ChannelJointCorrespondance correspondance{};
            correspondance.joindId = currentID;
            correspondance.channel = channel;
            motion->channelDescriptionOrder.push_back(correspondance);

            // Perdict rotation order
            if (!rotationOrderPredicted) {
                RotationAxisEnum rotation;
                if (convertChannelEnumToRotationAxisEnum(channel, rotation)) {
                    predictedRotationOrder.push_back(rotation);
                    if (predictedRotationOrder.size() >= 3) {
                        for (size_t i = 0; i < 3; i++) {
                            motion->rotationOrder[i] =
                                predictedRotationOrder[i];
                        }
                        rotationOrderPredicted = true;
                    }
                }
            }
        }

        currentID++;
        if (currentID > ikura::NUM_OF_MODEL_MATRIX) {
            throwTooManyJointsError();
        }
        parseJoints(false, closestChildMap);
    }

    // End bracket / Joint token
    while (true) {
        *inputStream >> input;
        if (input == TOKEN_JOINT) {
            currentID++;
            if (currentID > ikura::NUM_OF_MODEL_MATRIX) {
                throwTooManyJointsError();
            }
            parseJoints(true, closestChildMap);
        } else if (input == TOKEN_END_BRACKET && !inputStream->eof()) {
            break;
        } else {
            std::string msg;
            msg += "End bracket or '";
            msg += TOKEN_JOINT;
            msg += "' are expected.";
            throw parse_failed_error(msg, inputStream);
        }
    }

    // pop Joint ID stack
    ikura::GroupID id = jointIDStack.back();
    jointIDStack.pop_back();

    // Create Joint Object
    skelton.push_back(std::move(std::make_unique<Animator::Joint>(
        jointName, id, pos, jointIDStack, isEndSite)));

    // Register Closest Parent ID
    if (!jointIDStack.empty()) {
        ikura::GroupID closestParentID = jointIDStack.back();
        closestChildMap[closestParentID].push_back(id);
    }
}

void BVHParser::parseMotion() {
    std::string input;

    // Frames: token
    *inputStream >> input;
    if (input != TOKEN_FRAMES) {
        std::string msg;
        msg += "'";
        msg += TOKEN_FRAMES;
        msg += "' is expected";
        throw parse_failed_error(msg, inputStream);
    }

    // frames value
    try {
        *inputStream >> motion->numOfFrames;
    } catch (std::exception e) {
        throw parse_failed_error("Number of frames (int value) is expected.",
                                 inputStream);
    }

    // allocate JointMotions
    for (size_t jointIdx = 0; jointIdx < skelton.size(); jointIdx++) {
        // auto jm = std::make_shared<JointMotion>();
        motion->jointMotions.push_back(std::make_shared<JointMotion>());
    }

    // Frame Time: token
    *inputStream >> input;
    if (input != TOKEN_FRAME_TIME_FRAME) {
        std::string msg;
        msg += "'";
        msg += TOKEN_FRAME_TIME;
        msg += "' is expected.";
        throw parse_failed_error(msg, inputStream);
    }
    *inputStream >> input;
    if (input != TOKEN_FRAME_TIME_TIME) {
        std::string msg;
        msg += "'";
        msg += TOKEN_FRAME_TIME;
        msg += "' is expected.";
        throw parse_failed_error(msg, inputStream);
    }

    // frame time value
    try {
        *inputStream >> motion->frameRate;
    } catch (std::exception e) {
        throw parse_failed_error("Frame rate (float value) is expected.",
                                 inputStream);
    }
    // flush new line
    std::getline(*inputStream, input);

    std::stringstream strStream;
    uint32_t numOfChannels =
        static_cast<uint32_t>(motion->channelDescriptionOrder.size());
    float value;
    for (uint32_t frame = 0; frame < motion->numOfFrames; frame++) {
        strStream.clear();
        std::getline(*inputStream, input);
        if (frame < motion->numOfFrames - 1 && inputStream->eof()) {
            std::string msg;
            msg += "The number of frames is smaller than the Hierarchy section "
                   "specification (";
            msg += motion->numOfFrames;
            msg += ").";
            throw parse_failed_error(msg, inputStream);
        }

        strStream << input;

        // allocate JointState for current frame
        for (size_t jointIdx = 0; jointIdx < skelton.size(); jointIdx++) {
            motion->jointMotions[jointIdx]->jointStates.push_back(
                std::make_shared<JointState>());
        }

        for (uint32_t i = 0; i < numOfChannels; i++) {
            strStream >> value;
            if (i < numOfChannels - 1 && strStream.eof()) {
                std::string msg;
                msg += "The number of channel values is smaller than the "
                       "Hierarchy section specification (";
                msg += numOfChannels;
                msg += ").";
                throw parse_failed_error(msg, inputStream);
            }

            auto id = motion->channelDescriptionOrder[i].joindId;
            auto channel = motion->channelDescriptionOrder[i].channel;
            switch (channel) {
            case ChannelEnum::Xposition:
                motion->jointMotions[id]->jointStates[frame]->pos.x = value;
                break;
            case ChannelEnum::Yposition:
                motion->jointMotions[id]->jointStates[frame]->pos.y = value;
                break;
            case ChannelEnum::Zposition:
                motion->jointMotions[id]->jointStates[frame]->pos.z = value;
                break;

            case ChannelEnum::Xrotation:
                motion->jointMotions[id]->jointStates[frame]->rot.x = value;
                break;
            case ChannelEnum::Yrotation:
                motion->jointMotions[id]->jointStates[frame]->rot.y = value;
                break;
            case ChannelEnum::Zrotation:
                motion->jointMotions[id]->jointStates[frame]->rot.z = value;
                break;
            }

            motion->jointMotions[id]->ownedChannels.insert(channel);
        }
    }
}

void BVHParser::parseBVH() {
    motion = std::make_shared<Motion>();
    ClosestChildMap closestChildMap;

    try {
        // Hierarchy section definition
        std::string input;
        *inputStream >> input;
        if (input != TOKEN_TOP) {
            std::string msg;
            msg += "Top of .bvh file should be '";
            msg += TOKEN_TOP;
            msg += "'.";
            throw parse_failed_error(msg, inputStream);
        }
        parseJoints(false, closestChildMap);

        // sort by ID
        std::sort(skelton.begin(), skelton.end(),
                  [](const std::shared_ptr<Animator::Joint> &a,
                     const std::shared_ptr<Animator::Joint> &b) {
                      return a->getID() < b->getID();
                  });

        // register closest child id
        for (auto &joint : skelton) {
            joint->setClosestChildIDs(closestChildMap[joint->getID()]);
        }

        // Motion section definition
        *inputStream >> input;
        if (input != TOKEN_MOTION) {
            std::string msg;
            msg += "Start of motion section should be '";
            msg += TOKEN_MOTION;
            msg += "'.";
            throw parse_failed_error(msg, inputStream);
        }
        parseMotion();
    } catch (parse_failed_error e) {
        std::cerr << e.what() << std::endl;
        std::cerr << e.where() << std::endl;
        exit(1);
    }
}

BVHParser::BVHParser(std::string filePath) {
    this->filePath = filePath;
    inputStream = std::make_unique<std::ifstream>(filePath);

    if (!inputStream->is_open()) {
        std::string msg;
        msg += "failed to open .bvh file '";
        msg += filePath;
        msg += "'.";
        throw std::runtime_error(msg);
    }
}

bool isValidChannel(std::string str) {
    return (str == "Xposition" || str == "Yposition" || str == "Zposition" ||
            str == "Xrotation" || str == "Yrotation" || str == "Zrotation");
}
