#pragma once
#include <string>
#include <vector>

namespace CruuzAuth {
    struct response_data {
        bool success = false;
        std::string message;
        std::string raw;
    };

    class api {
    public:
        std::string name, ownerid, secret, version, url;
        response_data response;

        api(std::string name, std::string ownerid, std::string secret, std::string version, std::string url);

        void init();
        void license(std::string key);
        void login(std::string username, std::string password);
        void regstr(std::string username, std::string password, std::string key);
        std::vector<unsigned char> download(std::string fileid);

    private:
        std::string request(std::string data);
    };
}
