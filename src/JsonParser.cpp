#include "JsonParser.h"
#include <iostream>

std::vector<Activity> JsonParser::parseActivity(const std::string& data) {
    std::vector<Activity> result;

    size_t position = data.find("\"type\"");

    if (position == std::string::npos) {
        return result;
    }

    while (position != std::string::npos) {
        

    size_t start = data.find("\"", position + 7);

    if (start == std::string::npos) {
        break;
    }

    size_t end = data.find("\"", start + 1);

    if (end == std::string::npos) {
        break;
    }

    std::string type = data.substr(start + 1, end - start - 1);

    size_t repoPosition = data.find("\"repo\"", end);

    if(repoPosition == std::string::npos){
        break;
    }

    size_t namePosition = data.find("\"name\"", repoPosition);

    if (namePosition == std::string::npos) {
        break;
    }

    size_t nameStart = data.find("\"", namePosition + 6);

    if (nameStart == std::string::npos) {
        break;
    }

    size_t nameEnd = data.find("\"", nameStart + 1);

    if (nameEnd == std::string::npos) {
        break;
    }

    std::string repoName = data.substr(nameStart + 1, nameEnd - nameStart - 1);

    size_t datePosition = data.find("\"created_at\"", end);

    if (datePosition == std::string::npos) {
        break;
    }

    size_t dateStart = data.find("\"", datePosition + 13);

    if (dateStart == std::string::npos) {
        break;

    }

    size_t dateEnd = data.find("\"", dateStart + 1);

    if (dateEnd == std::string::npos) {
        break;

    }

    std::string createAt = data.substr(dateStart + 1, dateEnd - dateStart - 1);

    std::cout << "DEBUG DATE: " << createAt << std::endl;

    int commits = 0;
    if (type == "PushEvent") {

        size_t commitsPosition = data.find("\"commits\"", end);

        if (commitsPosition != std::string::npos) {

            size_t commitsStart = data.find("[", commitsPosition);
            size_t commitsEnd = data.find("]", commitsStart);

            size_t shaPosition = data.find("\"sha\"", commitsStart);

            while (shaPosition != std::string::npos && shaPosition < commitsEnd) {
                
                commits++;

                shaPosition = data.find("\"sha\"", shaPosition + 1);

            }
            

        }

    }

    std::string action;

    switch (type[0]) {

    case 'P':
        action = "Pushed";
        break;

    case 'W':
        action = "Starred";
        break;

    case 'I':
        action = type == "IssuesEvent"
            ? "Opened an issue"
            : "Commented on an issue";
        break;

    case 'R':
        action = "Created a pull request";
        break;

    case 'C':
        action = "Created";
        break;

    case 'D':
        action = "Deleted";
        break;

    case 'F':
        action = "Forked a repository";
        break;

    default:
        action = "Unknown action";

    }
    
    Activity activity;
    activity.type = type;
    activity.description = repoName;
    activity.action = action;
    activity.commits = commits;
    activity.createdAt = createAt;

    result.push_back(activity);

    size_t nextEvent = data.find("},{\"id\"", nameEnd);

    if (nextEvent != std::string::npos) {
        position = data.find("\"type\"", nextEvent);
    }
    else {
        position = std::string::npos;
    }


    }

    return result;

}