#pragma once

#include <string>
#include "Activity.h"
#include <vector>

class GitHubAPI{

    private:
    std::vector<Activity> activities;
    std::string fetchData(const std::string& username);

    int getCommitCount(
        const std::string& repo,
        const std::string& before,
        const std::string& head
    );

    public:
    std::vector<Activity> getUserActivity(const std::string& username);
   

};