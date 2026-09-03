#pragma once

#include <string>
#include <vector>
#include "Activity.h"

class JsonParser {
    public:
    static std::vector<Activity> parseActivity(const std::string& data);
};