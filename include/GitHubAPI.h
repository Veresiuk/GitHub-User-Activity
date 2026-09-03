#pragma once

#include <string>
#include "Activity.h"
#include <vector>

class GitHubAPI{

    private:
    std::vector<Activity> activities;
    std::string fetchData(const std::string& username);

    public:
    std::vector<Activity> getUserActivity(const std::string& username);
   

};