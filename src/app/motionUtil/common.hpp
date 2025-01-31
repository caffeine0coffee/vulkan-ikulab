#pragma once

#include <array>
#include <set>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <ikura/ikura.hpp>

enum ChannelEnum {
    Xposition,
    Yposition,
    Zposition,
    Xrotation,
    Yrotation,
    Zrotation
};

enum RotationAxisEnum { X, Y, Z };

struct JointState {
    glm::vec3 pos = {};
    glm::vec3 rot = {};
};

struct JointMotion {
    std::vector<std::shared_ptr<JointState>> jointStates;
    std::set<ChannelEnum> ownedChannels;
};

struct ChannelJointCorrespondance {
    ikura::GroupID joindId;
    ChannelEnum channel;
};

struct Motion {
    std::vector<std::shared_ptr<JointMotion>> jointMotions;
    std::array<RotationAxisEnum, 3> rotationOrder;
    std::vector<ChannelJointCorrespondance> channelDescriptionOrder;
    size_t numOfFrames;
    float frameRate;
};

ChannelEnum convertStrToChannelEnum(std::string str);
/// Return true if `channel` can be converted RotationAxisEnum.
/// Return false if `channel` cannot be converted RotationAxisEnum.
/// Conversion result will be stored in `rotation`.
bool convertChannelEnumToRotationAxisEnum(ChannelEnum channel,
                                          RotationAxisEnum &rotation);
ChannelEnum convertRotationAxisEnumToChannelEnum(RotationAxisEnum rotation);
std::string convertChannelEnumToStr(ChannelEnum channel);
std::string convertRotationAxisEnumToChannelStr(RotationAxisEnum rotation);
std::string convertRotationAxisEnumToRotationOrderStr(
    std::array<RotationAxisEnum, 3> rotationOrder);
/// Describe 3 upper-case axes separated by '-'.
/// For example, str is "Z-X-Y".
std::array<RotationAxisEnum, 3> convertStrToRotationOrder(std::string str);
