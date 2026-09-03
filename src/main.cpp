#include "GitHubAPI.h"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Usage: github-activity <username>" << std::endl;
        return 1;
    }

    std::string username = argv[1];

    GitHubAPI api;

    try {
        std::vector<Activity> activities = api.getUserActivity(username);

        if (activities.empty()) {
            std::cout << "No activity found." << std::endl;
            return 0;
        }

        for (const Activity& activity : activities) {

            std::cout << "- " << activity.action;

            if (activity.commits > 0) {

                std::cout << " " << activity.commits << " commits";

            }

            std::cout << " to " << activity.description << std::endl;

        }

    }
    
    catch (const std::exception& error) {

            std::cout << "Error: " << error.what() << std::endl;
            return 1;

        }

    return 0;
}