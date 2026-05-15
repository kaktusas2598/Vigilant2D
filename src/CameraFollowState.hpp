#pragma once

#include <string>

struct CameraFollowState {
    bool followEntity = false;
    std::string targetEntityId;
};
