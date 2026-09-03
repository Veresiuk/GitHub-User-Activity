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

std::vector<Activity> GitHubAPI::getUserActivity(const std::string& username) {

    std::string data = fetchData(username);

    std::vector<Activity> result = JsonParser::parseActivity(data);
    return result;

}
