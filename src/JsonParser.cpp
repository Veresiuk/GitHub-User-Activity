#include "JsonParser.h"

std::vector<Activity> JsonParser::parseActivity(const std::string& data) {
    std::vector<Activity> result;

    size_t position = data.find("\"type\"");


    while (position != std::string::npos) {

        size_t nextEvent = data.find("},{", position + 1);

        size_t eventEnd;

        if (nextEvent != std::string::npos) {
            eventEnd = nextEvent;
        }
        else {
            eventEnd = data.size();
        }

        std::string eventData = data.substr(
            position,
            eventEnd - position
        );
    
        
    size_t typeStart = eventData.find("\"type\"");

    if (typeStart == std::string::npos) {
        break;
    }

    size_t start = eventData.find("\"", typeStart + 6);

    if (start == std::string::npos) {
        break;
    }

    size_t end = eventData.find("\"", start + 1);

    if (end == std::string::npos) {
        break;
    }

    std::string type = eventData.substr(start + 1, end - start - 1);

    size_t repoPosition = eventData.find("\"repo\"", end);

    if(repoPosition == std::string::npos){
        break;
    }

    size_t namePosition = eventData.find("\"name\"", repoPosition);

    if (namePosition == std::string::npos) {
        break;
    }

    size_t nameStart = eventData.find("\"", namePosition + 6);

    if (nameStart == std::string::npos) {
        break;
    }

    size_t nameEnd = eventData.find("\"", nameStart + 1);

    if (nameEnd == std::string::npos) {
        break;
    }

    std::string repoName = eventData.substr(nameStart + 1, nameEnd - nameStart - 1);

    size_t datePosition = eventData.find("\"created_at\"", end);

    if (datePosition == std::string::npos) {
        break;
    }

    size_t dateStart = eventData.find("\"", datePosition + 13);

    if (dateStart == std::string::npos) {
        break;

    }

    size_t dateEnd = eventData.find("\"", dateStart + 1);

    if (dateEnd == std::string::npos) {
        break;

    }

    std::string createAt = eventData.substr(dateStart + 1, dateEnd - dateStart - 1);

    std::string before;
    std::string head;

    if (type == "PushEvent") {

        size_t beforePosition = eventData.find("\"before\"");

        if (beforePosition != std::string::npos) {

            size_t beforeStart = eventData.find("\"", beforePosition + 8);

            if (beforeStart != std::string::npos) {

                size_t beforeEnd = eventData.find("\"", beforeStart + 1);

                if (beforeEnd != std::string::npos) {

                    before = eventData.substr(beforeStart + 1, beforeEnd - beforeStart - 1);
                }
            }
        }

        size_t headPosition = eventData.find("\"head\"");

        if (headPosition != std::string::npos) {

            size_t headStart = eventData.find("\"", headPosition + 6);

            if (headStart != std::string::npos) {

                size_t headEnd = eventData.find("\"", headStart + 1);

                if (headEnd != std::string::npos) {

                    head = eventData.substr (headStart + 1, headEnd - headStart - 1);

                }
            }
        }

    }

    int commits = 0;
    if (type == "PushEvent") {

        size_t commitsPosition = eventData.find("\"commits\"", end);

        if (commitsPosition != std::string::npos) {

            size_t commitsStart = eventData.find("[", commitsPosition);
            size_t commitsEnd = eventData.find("]", commitsStart);

            if (commitsStart != std::string::npos && commitsEnd != std::string::npos) {
            

            size_t shaPosition = eventData.find("\"sha\"", commitsStart);

            while (shaPosition != std::string::npos && shaPosition < commitsEnd) {
                
                commits++;

                shaPosition = eventData.find("\"sha\"", shaPosition + 1);

            }

            }
            

        }

    }

    std::string action;

    switch (type[0]) {

    case 'P':
    if (type == "PushEvent") {
        action = "Pushed";
    }
    else {
        action = "Pull request";
    }
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

    activity.before = before;
    activity.head = head;

    result.push_back(activity);

    if (nextEvent != std::string::npos) {
        position = data.find("\"type\"", nextEvent);
    }
    else {
        position = std::string::npos;
    }


    }

    return result;

}