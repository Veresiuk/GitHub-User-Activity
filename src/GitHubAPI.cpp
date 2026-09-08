#include "GitHubAPI.h"
#include <windows.h>
#include <winhttp.h>
#include <stdexcept>
#include <vector>
#include "JsonParser.h"


std::string GitHubAPI::fetchData(const std::string& username){

    HINTERNET session = WinHttpOpen(
        L"github-activity",
        0,
        0,
        0,
        0

    );

    if (session == NULL) {
        throw std::runtime_error("Failed to open WinHTTP session");

    }

    HINTERNET connection = WinHttpConnect(
        session,
        L"api.github.com",
        443,
        0

    );

    if (connection == NULL) {
        throw std::runtime_error("Could not connect to API");

    }

    std::wstring wideUsername(username.begin(), username.end());

    std::wstring path = L"/users/" + wideUsername + L"/events";

    HINTERNET request = WinHttpOpenRequest(
        connection,
        L"GET",
        path.c_str(),
        nullptr,
        nullptr,
        nullptr,
        WINHTTP_FLAG_SECURE
    );

    if (request == NULL) {
        throw std::runtime_error("Failed to open HTTP request");
    }

    BOOL result = WinHttpSendRequest(
        request,
        nullptr,
        0,
        nullptr,
        0,
        0,
        0

    );

    if (result == FALSE) {
        throw std::runtime_error("Failed to send HTTP request");

    }

    BOOL response = WinHttpReceiveResponse(
        request,
        nullptr
    );

    if (response == FALSE) {
        throw std::runtime_error("Failed to receive response");
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);

    BOOL headerResult = WinHttpQueryHeaders(
        request,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &statusCode,
        &statusCodeSize,
        WINHTTP_NO_HEADER_INDEX
    );

    if (headerResult == FALSE) {
        throw std::runtime_error("Failed to get HTTP status");
    }

    if (statusCode == 404) {
        throw std::runtime_error("GitHub user not found");
    }

    if (statusCode != 200) {
         throw std::runtime_error("GitHub API request failed");
    }

    std::string data;

    while (true) {
        
    DWORD size;

    BOOL dataAvailable = WinHttpQueryDataAvailable(
        request,
        &size
    );

    if (dataAvailable == FALSE) {
        throw std::runtime_error("Failed to get available data size");
    
    }

    if (size == 0) {
        break;
    }

    std::vector<char> buffer(size);

    DWORD bytesRead = 0;

    BOOL dataRead = WinHttpReadData(
        request,
        buffer.data(),
        size,
        &bytesRead

    );

    if (dataRead == FALSE) {
        throw std::runtime_error("Failed to read response data");
    }

    data.append(buffer.data(), bytesRead);

    }   

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    return data;

}


int GitHubAPI::getCommitCount(
    const std::string& repo,
    const std::string& before,
    const std::string& head
) {

    HINTERNET session = WinHttpOpen(
        L"github-activity",
        0,
        0,
        0,
        0
    );

    if (session == NULL) {
        throw std::runtime_error("Failed to open WinHTTP session");
    }

    HINTERNET connection = WinHttpConnect(
        session,
        L"api.github.com",
        443,
        0
    );

    if (connection == NULL) {
        WinHttpCloseHandle(session);
        throw std::runtime_error("Could not connect to API");
    }

    std::wstring wideRepo(repo.begin(), repo.end());
    std::wstring wideBefore(before.begin(), before.end());
    std::wstring wideHead(head.begin(), head.end());

    std::wstring path =
        L"/repos/" +
        wideRepo +
        L"/compare/" +
        wideBefore +
        L"..." +
        wideHead;

    HINTERNET request = WinHttpOpenRequest(
        connection,
        L"GET",
        path.c_str(),
        nullptr,
        nullptr,
        nullptr,
        WINHTTP_FLAG_SECURE
    );

    if (request == NULL) {
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);

        throw std::runtime_error("Failed to open compare request");
    }

    BOOL result = WinHttpSendRequest(
        request,
        nullptr,
        0,
        nullptr,
        0,
        0,
        0
    );

    if (result == FALSE) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);

        throw std::runtime_error("Failed to send compare request");
    }

    BOOL response = WinHttpReceiveResponse(
        request,
        nullptr
    );

    if (response == FALSE) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);

        throw std::runtime_error("Failed to receive compare response");
    }

    std::string data;

    while (true) {

        DWORD size = 0;

        BOOL dataAvailable = WinHttpQueryDataAvailable(
            request,
            &size
        );

        if (dataAvailable == FALSE) {
            throw std::runtime_error(
                "Failed to get compare data size"
            );
        }

        if (size == 0) {
            break;
        }

        std::vector<char> buffer(size);

        DWORD bytesRead = 0;

        BOOL dataRead = WinHttpReadData(
            request,
            buffer.data(),
            size,
            &bytesRead
        );

        if (dataRead == FALSE) {
            throw std::runtime_error(
                "Failed to read compare response"
            );
        }

        data.append(buffer.data(), bytesRead);
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    size_t commitsPosition =
        data.find("\"total_commits\"");

    if (commitsPosition == std::string::npos) {
        return 0;
    }

    size_t colonPosition =
        data.find(":", commitsPosition);

    if (colonPosition == std::string::npos) {
        return 0;
    }

    size_t numberStart =
        data.find_first_of("0123456789", colonPosition);

    if (numberStart == std::string::npos) {
        return 0;
    }

    size_t numberEnd =
        data.find_first_not_of("0123456789", numberStart);

    return std::stoi(
        data.substr(
            numberStart,
            numberEnd - numberStart
        )
    );
}

std::vector<Activity> GitHubAPI::getUserActivity(const std::string& username) {

    std::string data = fetchData(username);

    std::vector<Activity> result = JsonParser::parseActivity(data);

    for (Activity& activity : result) {

        if (activity.type == "PushEvent") {

            if (!activity.before.empty() &&
                !activity.head.empty()) {

                activity.commits = getCommitCount(
                    activity.description,
                    activity.before,
                    activity.head
                );
            }
        }
    }

    return result;

}
