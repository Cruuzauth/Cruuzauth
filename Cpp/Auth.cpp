#include "Auth.hpp"
#include <windows.h>
#include <wininet.h>
#include <sstream>

#pragma comment(lib, "wininet.lib")

using namespace CruuzAuth;

api::api(std::string name, std::string ownerid, std::string secret, std::string version, std::string url)
    : name(name), ownerid(ownerid), secret(secret), version(version), url(url) {}

static bool contains_success_true(const std::string& body) {
    return body.find("\"success\":true") != std::string::npos || body.find("\"success\": true") != std::string::npos;
}

void api::init() {
    std::string res = request("type=init&name=" + name + "&ownerid=" + ownerid + "&secret=" + secret + "&ver=" + version);
    response.success = contains_success_true(res);
    response.message = response.success ? "Success" : res;
    response.raw = res;
}

void api::license(std::string key) {
    std::string res = request("type=license&name=" + name + "&ownerid=" + ownerid + "&secret=" + secret + "&key=" + key);
    response.success = contains_success_true(res);
    response.message = response.success ? "Success" : res;
    response.raw = res;
}

void api::login(std::string username, std::string password) {
    std::string res = request("type=login&name=" + name + "&ownerid=" + ownerid + "&secret=" + secret + "&username=" + username + "&password=" + password);
    response.success = contains_success_true(res);
    response.message = response.success ? "Success" : res;
    response.raw = res;
}

void api::regstr(std::string username, std::string password, std::string key) {
    std::string res = request("type=register&name=" + name + "&ownerid=" + ownerid + "&secret=" + secret + "&username=" + username + "&password=" + password + "&key=" + key);
    response.success = contains_success_true(res);
    response.message = response.success ? "Success" : res;
    response.raw = res;
}

std::vector<unsigned char> api::download(std::string fileid) {
    std::string res = request("type=download&name=" + name + "&ownerid=" + ownerid + "&secret=" + secret + "&fileid=" + fileid);
    return std::vector<unsigned char>(res.begin(), res.end());
}

std::string api::request(std::string data) {
    HINTERNET hInternet = InternetOpenA("CruuzAuthClient", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return "";

    URL_COMPONENTSA urlComp{};
    char host[256]{};
    char path[1024]{};
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = sizeof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = sizeof(path);

    InternetCrackUrlA(url.c_str(), 0, 0, &urlComp);

    HINTERNET hConnect = InternetConnectA(hInternet, host, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) { InternetCloseHandle(hInternet); return ""; }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path[0] ? path : "/api/", NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) { InternetCloseHandle(hConnect); InternetCloseHandle(hInternet); return ""; }

    std::string headers = "Content-Type: application/x-www-form-urlencoded\r\n";
    HttpSendRequestA(hRequest, headers.c_str(), (DWORD)headers.length(), (LPVOID)data.c_str(), (DWORD)data.length());

    std::string responseBody;
    char buffer[4096];
    DWORD bytesRead = 0;

    while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        responseBody.append(buffer, bytesRead);
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return responseBody;
}
