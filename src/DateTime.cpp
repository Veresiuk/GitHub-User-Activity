#include "DateTime.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string getCurrentDateTime() {

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};

    localtime_s(&localTime, &currentTime);

    std::ostringstream output;

    output << std::put_time(
        &localTime,
        "%Y-%m-%d %H:%M:%S"
    );

    return output.str();
}