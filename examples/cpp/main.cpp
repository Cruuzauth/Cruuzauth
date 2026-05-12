#include <iostream>
#include <Windows.h>
#include "../../Cpp/Auth.hpp"

std::string name = "YOUR_APP_NAME";
std::string ownerid = "YOUR_OWNER_ID";
std::string secret = "YOUR_SECRET";
std::string version = "1.0";
std::string url = "https://cruuzauth.com/api/";

CruuzAuth::api CruuzauthApp(name, ownerid, secret, version, url);

int main() {
    CruuzauthApp.init();

    if (!CruuzauthApp.response.success) {
        std::cout << "Init failed: " << CruuzauthApp.response.message << std::endl;
        Sleep(1500);
        return 0;
    }

    std::string key;
    std::cout << "License: ";
    std::cin >> key;

    CruuzauthApp.license(key);

    if (!CruuzauthApp.response.success) {
        std::cout << "Login failed: " << CruuzauthApp.response.message << std::endl;
        Sleep(1500);
        return 0;
    }

    std::cout << "Access granted." << std::endl;
    Sleep(2000);
    return 0;
}
