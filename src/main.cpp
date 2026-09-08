#include "GitHubAPI.h"
#include <iostream>
#include <vector>

int main() {

    while (true) {

        std::string username;

        std::cout << "\nEnter GitHub Username (or 'exit' to quit): ";
        std::cin >> username;

        if (username == "exit") {
            break;
        }

        GitHubAPI api;

        try {
        std::vector<Activity> activities = api.getUserActivity(username);

        if (activities.empty()) {

            std::cout << "No activity found." << std::endl;

            continue;
        }

        for (const Activity& activity : activities) {

            std::cout << "- " << activity.action;

            if (activity.commits > 0) {

                std::cout << " " << activity.commits;

                if (activity.commits == 1) {
                    std::cout << " commit";
                }
                else {
                    std::cout << " commit";
                }

            }

            std::cout << " to " << activity.description << " [" << activity.createdAt << "]" << std::endl;

        }

    }
    
    catch (const std::exception& error) {

            std::cout << "Error: " << error.what() << std::endl;

        }
    }

    return 0;
}