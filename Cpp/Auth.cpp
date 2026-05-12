include "Auth.hpp"
#include <Windows.h>
#include <string>
#include <vector>
#include <curl/curl.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Normaliz.lib")

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    out->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

static std::string httpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CruuzAuthClient/1.0");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return response;
}

static std::string urlEnc(const std::string& val) {
    CURL* curl = curl_easy_init();
    if (!curl) return val;
    char* enc = curl_easy_escape(curl, val.c_str(), (int)val.size());
    std::string res(enc);
    curl_free(enc);
    curl_easy_cleanup(curl);
    return res;
}

static std::string jval(const std::string& json, const std::string& key) {
    std::string p1 = "\"" + key + "\":\"";
    size_t pos = json.find(p1);
    if (pos != std::string::npos) {
        pos += p1.size();
        size_t end = json.find('"', pos);
        return (end != std::string::npos) ? json.substr(pos, end - pos) : "";
    }
    std::string p2 = "\"" + key + "\":";
    pos = json.find(p2);
    if (pos != std::string::npos) {
        pos += p2.size();
        while (pos < json.size() && json[pos] == ' ') pos++;
        size_t end = json.find_first_of(",}\n\r", pos);
        std::string v = json.substr(pos, (end != std::string::npos) ? end - pos : std::string::npos);
        if (!v.empty() && v.front() == '"') v = v.substr(1);
        if (!v.empty() && v.back() == '"') v.pop_back();
        return v;
    }
    return "";
}

static std::string jobj(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":{";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos += search.size() - 1;
    int depth = 0; size_t start = pos;
    for (size_t i = pos; i < json.size(); ++i) {
        if (json[i] == '{') depth++;
        else if (json[i] == '}') { if (--depth == 0) return json.substr(start, i - start + 1); }
    }
    return "";
}

static std::string jfirstArr(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":[";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos = json.find('{', pos);
    if (pos == std::string::npos) return "";
    int depth = 0; size_t start = pos;
    for (size_t i = pos; i < json.size(); ++i) {
        if (json[i] == '{') depth++;
        else if (json[i] == '}') { if (--depth == 0) return json.substr(start, i - start + 1); }
    }
    return "";
}

static std::string getHWID() {
    DWORD serial = 0;
    GetVolumeInformationA("C:\\", nullptr, 0, &serial, nullptr, nullptr, nullptr, 0);
    char pc[128] = "PC"; DWORD len = sizeof(pc); GetComputerNameA(pc, &len);
    char buf[256]; sprintf_s(buf, "HWID-%lu-%s", (unsigned long)serial, pc);
    return std::string(buf);
}

namespace CruuzAuth
{
    std::vector<unsigned char> api::download(std::string fileid)
    {
        std::string urlStr = this->url + "download.php?fileid=" + urlEnc(fileid);
        std::string resp = httpGet(urlStr);

        return std::vector<unsigned char>(resp.begin(), resp.end());
    }
    void api::init() {
        std::string urlStr = this->url + "?type=init&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&ver=" + urlEnc(this->version);
        std::string resp = httpGet(urlStr);
        this->response.success = (jval(resp, "success") == "true");
        this->response.message = jval(resp, "message");

        if (!this->response.success && !this->response.message.empty()) {
            std::string title = "@MG - Error";
            MessageBoxA(NULL, this->response.message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
        }

        if (this->response.success) {
            this->sessionid = jval(resp, "sessionid");
            std::string ai = jobj(resp, "appinfo");
            this->app_data.numUsers = jval(ai, "numUsers");
            this->app_data.numOnlineUsers = jval(ai, "numOnlineUsers");
            this->app_data.numKeys = jval(ai, "numKeys");
            this->app_data.version = jval(ai, "version");
            this->app_data.customerPanelLink = jval(ai, "customerPanelLink");
        }
        else {
            std::string errorMsg = this->response.message;
            if (errorMsg.find("not found") != std::string::npos ||
                errorMsg.find("invalid") != std::string::npos ||
                errorMsg.find("Application not found") != std::string::npos) {
                
            }
            else if (errorMsg.find("paused") != std::string::npos) {
                
            }
            else {
                
            }
            ExitProcess(1);
        }


    }

    void api::license(std::string key)
    {
        std::string hwid = getHWID();
        std::string urlStr = this->url + "?type=license&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&key=" + urlEnc(key) + "&hwid=" + urlEnc(hwid) + "&sessionid=" + urlEnc(this->sessionid);
        std::string resp = httpGet(urlStr);
        this->response.success = (jval(resp, "success") == "true");
        this->response.message = jval(resp, "message");

        if (this->response.success) {
            std::string info = jobj(resp, "info");
            this->user_data.username = jval(info, "username");
            this->user_data.ip = jval(info, "ip");
            this->user_data.hwid = jval(info, "hwid");
            this->user_data.createdate = jval(info, "createdate");
            this->user_data.lastlogin = jval(info, "lastlogin");

            this->user_data.subscriptions.clear();
            std::string subObj = jfirstArr(info, "subscriptions");
            if (!subObj.empty())
            {
                std::string subName = jval(subObj, "name");
                if (!subName.empty())
                {
                    subscriptions_class sub;
                    sub.name = subName;
                    sub.expiry = jval(subObj, "expiry");
                    this->user_data.subscriptions.push_back(sub);
                }
                this->espira.expiry = jval(subObj, "expiry");
            }

        }
    }

    void api::check() 
    {
        std::string urlStr = this->url + "?type=check&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&sessionid=" + urlEnc(this->sessionid);
        std::string resp = httpGet(urlStr);
        this->response.success = (jval(resp, "success") == "true");
        this->response.message = jval(resp, "message");
    }

    std::string api::var(std::string varid)
    {
        std::string urlStr = this->url + "?type=var&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&varid=" + urlEnc(varid) + "&simple=1";
        std::string resp = httpGet(urlStr);

        if (!resp.empty() && resp.find("{") != 0) {
            return resp;
        }

        urlStr = this->url + "?type=var&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&varid=" + urlEnc(varid);
        resp = httpGet(urlStr);

        bool success = (jval(resp, "success") == "true");
        std::string message = jval(resp, "message");

        if (!success) {
            return "";
        }

        return jval(resp, "data");
    }

    void api::login(std::string username, std::string password)
    {
        std::string hwid = getHWID();
        std::string urlStr = this->url + "?type=login&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&username=" + urlEnc(username) + "&password=" + urlEnc(password) + "&hwid=" + urlEnc(hwid) + "&sessionid=" + urlEnc(this->sessionid);
        std::string resp = httpGet(urlStr);
        this->response.success = (jval(resp, "success") == "true");
        this->response.message = jval(resp, "message");

        if (this->response.success) {
            std::string info = jobj(resp, "info");
            this->user_data.username = jval(info, "username");
            this->user_data.ip = jval(info, "ip");
            this->user_data.hwid = jval(info, "hwid");
            this->user_data.createdate = jval(info, "createdate");
            this->user_data.lastlogin = jval(info, "lastlogin");

            this->user_data.subscriptions.clear();
            std::string subObj = jfirstArr(info, "subscriptions");
            if (!subObj.empty())
            {
                std::string subName = jval(subObj, "name");
                if (!subName.empty())
                {
                    subscriptions_class sub;
                    sub.name = subName;
                    sub.expiry = jval(subObj, "expiry");
                    this->user_data.subscriptions.push_back(sub);
                }
                this->espira.expiry = jval(subObj, "expiry");
            }
        }
    }

    void api::regstr(std::string username, std::string password, std::string key, std::string email)
    {
        std::string hwid = getHWID();
        std::string urlStr = this->url + "?type=register&ownerid=" + urlEnc(this->ownerid) + "&name=" + urlEnc(this->name) + "&secret=" + urlEnc(this->secret) + "&username=" + urlEnc(username) + "&password=" + urlEnc(password) + "&key=" + urlEnc(key) + "&hwid=" + urlEnc(hwid);
        std::string resp = httpGet(urlStr);
        this->response.success = (jval(resp, "success") == "true");
        this->response.message = jval(resp, "message");

        if (this->response.success) {
            std::string info = jobj(resp, "info");
            this->user_data.username = jval(info, "username");
            this->user_data.ip = jval(info, "ip");
            this->user_data.hwid = jval(info, "hwid");
            this->user_data.createdate = jval(info, "createdate");
            this->user_data.lastlogin = jval(info, "lastlogin");

            this->user_data.subscriptions.clear();
            std::string subObj = jfirstArr(info, "subscriptions");
            if (!subObj.empty())
            {
                std::string subName = jval(subObj, "name");
                if (!subName.empty())
                {
                    subscriptions_class sub;
                    sub.name = subName;
                    sub.expiry = jval(subObj, "expiry");
                    this->user_data.subscriptions.push_back(sub);
                }
                this->espira.expiry = jval(subObj, "expiry");
            }
        }
    }
}
