#include "common.hpp"

ChannelEnum convertStrToChannelEnum(std::string str) {
    if (str == "Xposition")
        return Xposition;
    else if (str == "Yposition")
        return Yposition;
    else if (str == "Zposition")
        return Zposition;
    else if (str == "Xrotation")
        return Xrotation;
    else if (str == "Yrotation")
        return Yrotation;
    else if (str == "Zrotation")
        return Zrotation;

    std::string msg;
    msg += "Invalid channel '";
    msg += str;
    msg += "'.";
    throw std::runtime_error(msg);
}

std::string convertChannelEnumToStr(ChannelEnum channel) {
    switch (channel) {
    case ChannelEnum::Xposition:
        return "Xposition";
    case ChannelEnum::Yposition:
        return "Yposition";
    case ChannelEnum::Zposition:
        return "Zposition";
    case ChannelEnum::Xrotation:
        return "Xrotation";
    case ChannelEnum::Yrotation:
        return "Yrotation";
    case ChannelEnum::Zrotation:
        return "Zrotation";
    }

    std::string msg;
    msg += "Cannot convert ChannelEnum to String. ";
    msg += "Unknown ChannelEnum value detected.";
    throw std::runtime_error(msg);
}

bool convertChannelEnumToRotationAxisEnum(ChannelEnum channel,
                                          RotationAxisEnum &rotation) {
    switch (channel) {
    case ChannelEnum::Xrotation:
        rotation = RotationAxisEnum::X;
        return true;
    case ChannelEnum::Yrotation:
        rotation = RotationAxisEnum::Y;
        return true;
    case ChannelEnum::Zrotation:
        rotation = RotationAxisEnum::Z;
        return true;
    default:
        return false;
    }
}

ChannelEnum convertRotationAxisEnumToChannelEnum(RotationAxisEnum rotation) {
    switch (rotation) {
    case RotationAxisEnum::X:
        return ChannelEnum::Xrotation;
    case RotationAxisEnum::Y:
        return ChannelEnum::Yrotation;
    case RotationAxisEnum::Z:
        return ChannelEnum::Zrotation;
    }

    std::string msg;
    msg += "Cannot convert RotationAxisEnum to Channel String. ";
    msg += "Unknown RotationAxisEnum value detected.";
    throw std::runtime_error(msg);
}

std::string convertRotationAxisEnumToChannelStr(RotationAxisEnum rotation) {
    switch (rotation) {
    case RotationAxisEnum::X:
        return convertChannelEnumToStr(ChannelEnum::Xrotation);
    case RotationAxisEnum::Y:
        return convertChannelEnumToStr(ChannelEnum::Yrotation);
    case RotationAxisEnum::Z:
        return convertChannelEnumToStr(ChannelEnum::Zrotation);
    }

    std::string msg;
    msg += "Cannot convert RotationAxisEnum to Channel String. ";
    msg += "Unknown RotationAxisEnum value detected.";
    throw std::runtime_error(msg);
}

std::string convertRotationAxisEnumToRotationOrderStr(
    std::array<RotationAxisEnum, 3> rotationOrder) {

    std::string orderStr;
    for (size_t i = 0; i < 3; i++) {
        if (i > 0) {
            orderStr += "-";
        }
        switch (rotationOrder[i]) {
        case RotationAxisEnum::X:
            orderStr += "X";
            break;
        case RotationAxisEnum::Y:
            orderStr += "Y";
            break;
        case RotationAxisEnum::Z:
            orderStr += "Z";
            break;
        }
    }

    return orderStr;
}

std::array<RotationAxisEnum, 3> convertStrToRotationOrder(std::string str) {
    const auto throwInvalidStrFormatError = [&]() {
        std::string msg;
        msg += "Invalid string format: '";
        msg += str;
        msg += "'. ";
        msg += "str must be 3 upper-case axes separated by '-'. ";
        msg += "For example, str is 'Z-X-Y'.";
        throw std::runtime_error(msg);
    };

    if (str.length() != 5) {
        throwInvalidStrFormatError();
    }

    std::array<char, 3> axesChar = {str[0], str[2], str[4]};
    std::array<RotationAxisEnum, 3> rotationOrder;

    for (size_t i = 0; i < 3; i++) {
        switch (axesChar[i]) {
        case 'X':
            rotationOrder[i] = RotationAxisEnum::X;
            break;
        case 'Y':
            rotationOrder[i] = RotationAxisEnum::Y;
            break;
        case 'Z':
            rotationOrder[i] = RotationAxisEnum::Z;
            break;
        default:
            throwInvalidStrFormatError();
        }
    }

    return rotationOrder;
}
