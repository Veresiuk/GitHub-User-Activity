#pragma once

#include <string>

struct Activity {

    std::string type;
    std::string description;
    std::string action;
    std::string createdAt;

    std::string before;
    std::string head;
    
    int commits;

};