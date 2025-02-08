#include "bvhExporter.hpp"

#include <cstring>
#include <fstream>

#include "bvhParser.hpp"

#define INDENT ("  ")

// forward declearation of helper functions --------------------
void writeJointsRecursive(std::vector<std::shared_ptr<Animator::Joint>> joints,
                          ikura::GroupID currentJointID,
                          std::shared_ptr<Motion> motion,
                          std::ofstream &targetFile, uint32_t currentLevel,
                          bool writeAllPositionChannels);
void writeIndents(std::ofstream &targetFile, uint32_t level);
/// This function inserts pre-space if `channelsToExport` owns `channel`.
/// e.g. writes " Xposition" to targetFile.
void writeChannelStrIfOwned(std::ofstream &targetFile,
                            std::set<ChannelEnum> channelsToExport,
                            ChannelEnum channel);

void exportLoopRangeToBvhFile(const std::shared_ptr<Animator> animator,
                              std::filesystem::path destFile,
                              bool exportAllPositionChennel) {

    // vector<vector<Motion>>
    auto motion = animator->getMotion();
    std::ofstream targetFile(destFile);

    auto joints = animator->getJoints();

    // Header Section
    targetFile << TOKEN_TOP << std::endl;
    writeJointsRecursive(joints, 0, motion, targetFile, 0,
                         exportAllPositionChennel);

    // Data Section
    auto loopStart = animator->getLoopStartFrameIndex();
    auto loopEnd = animator->getLoopEndFrameIndex();
    auto loopLength = loopEnd - loopStart + 1;

    targetFile << TOKEN_MOTION << std::endl;
    targetFile << TOKEN_FRAMES << " " << loopLength << std::endl;
    targetFile << TOKEN_FRAME_TIME << " " << motion->frameRate << std::endl;

    for (size_t frameIdx = loopStart; frameIdx <= loopEnd; frameIdx++) {
        for (size_t i = 0; i < motion->channelDescriptionOrder.size(); i++) {
            if (i > 0) {
                targetFile << " ";
            }

            auto id = motion->channelDescriptionOrder[i].joindId;
            bool exportPositionChannelValue = false;
            if (id == 0 || exportAllPositionChennel) {
                exportPositionChannelValue = true;
            }

            switch (motion->channelDescriptionOrder[i].channel) {
            case ChannelEnum::Xposition:
                if (exportPositionChannelValue) {
                    targetFile << motion->jointMotions[id]
                                      ->jointStates[frameIdx]
                                      ->pos.x;
                }
                break;
            case ChannelEnum::Yposition:
                if (exportPositionChannelValue) {
                    targetFile << motion->jointMotions[id]
                                      ->jointStates[frameIdx]
                                      ->pos.y;
                }
                break;
            case ChannelEnum::Zposition:
                if (exportPositionChannelValue) {
                    targetFile << motion->jointMotions[id]
                                      ->jointStates[frameIdx]
                                      ->pos.z;
                }
                break;
            case ChannelEnum::Xrotation:
                targetFile
                    << motion->jointMotions[id]->jointStates[frameIdx]->rot.x;
                break;
            case ChannelEnum::Yrotation:
                targetFile
                    << motion->jointMotions[id]->jointStates[frameIdx]->rot.y;
                break;
            case ChannelEnum::Zrotation:
                targetFile
                    << motion->jointMotions[id]->jointStates[frameIdx]->rot.z;
                break;
            }
        }

        targetFile << std::endl;
    }
}

void writeJointsRecursive(std::vector<std::shared_ptr<Animator::Joint>> joints,
                          ikura::GroupID currentJointID,
                          std::shared_ptr<Motion> motion,
                          std::ofstream &targetFile, uint32_t currentLevel,
                          bool writeAllPositionChannels) {

    auto currentJoint = joints[currentJointID];
    auto jointMotion = motion->jointMotions[currentJointID];

    // Joint Name
    std::string jointDeclaration;
    if (currentJointID == 0) {
        jointDeclaration = TOKEN_ROOT;
    } else if (currentJoint->getIsEdge()) {
        jointDeclaration = TOKEN_END_SITE;
    } else {
        jointDeclaration = TOKEN_JOINT;
    }
    writeIndents(targetFile, currentLevel);
    targetFile << jointDeclaration << " " << currentJoint->getName()
               << std::endl;

    // Beggin '{'
    writeIndents(targetFile, currentLevel);
    targetFile << TOKEN_BEGGIN_BRACKET << std::endl;

    currentLevel += 1;

    // Offset
    auto pos = currentJoint->getPos();
    writeIndents(targetFile, currentLevel);
    targetFile << TOKEN_OFFSET << " ";
    targetFile << pos.x << " " << pos.y << " " << pos.z << std::endl;

    // Channels (non-EndSize only)
    if (!currentJoint->getIsEdge()) {
        // erase non-Root && ?position channels if `writeAllPositionChannels` is
        // true
        std::set<ChannelEnum> channelsToExport;
        if (currentJointID == 0 || writeAllPositionChannels) {
            channelsToExport = jointMotion->ownedChannels;
        } else {
            channelsToExport = jointMotion->ownedChannels;
            channelsToExport.erase(ChannelEnum::Xposition);
            channelsToExport.erase(ChannelEnum::Yposition);
            channelsToExport.erase(ChannelEnum::Zposition);
        }

        writeIndents(targetFile, currentLevel);
        targetFile << TOKEN_CHANNELS << " ";
        targetFile << channelsToExport.size();
        // write "?position" if current joint owns these channels
        writeChannelStrIfOwned(targetFile, channelsToExport,
                               ChannelEnum::Xposition);
        writeChannelStrIfOwned(targetFile, channelsToExport,
                               ChannelEnum::Yposition);
        writeChannelStrIfOwned(targetFile, channelsToExport,
                               ChannelEnum::Zposition);
        // write "?rotation" in rotation order
        for (const auto &r : motion->rotationOrder) {
            writeChannelStrIfOwned(targetFile, channelsToExport,
                                   convertRotationAxisEnumToChannelEnum(r));
        }
        targetFile << std::endl;
    }

    // Child Joint (non-EndSite only)
    if (!currentJoint->getIsEdge()) {
        auto closestChildIDs = currentJoint->getClosestChildIDs();
        for (const auto &id : closestChildIDs) {
            auto childID = joints[id]->getID();
            writeJointsRecursive(joints, childID, motion, targetFile,
                                 currentLevel, writeAllPositionChannels);
        }
    }

    // End '}'
    writeIndents(targetFile, currentLevel - 1);
    targetFile << TOKEN_END_BRACKET << std::endl;
}

void writeIndents(std::ofstream &targetFile, uint32_t level) {
    for (uint32_t i = 0; i < level; i++) {
        targetFile << INDENT;
    }
}

void writeChannelStrIfOwned(std::ofstream &targetFile,
                            std::set<ChannelEnum> channelsToExport,
                            ChannelEnum channel) {

    if (channelsToExport.find(channel) != channelsToExport.end()) {
        targetFile << " ";
        targetFile << convertChannelEnumToStr(channel);
    }
}
