#pragma once
#include <string>
#include <map>
#include <curl/curl.h>
#include <stdexcept>

namespace ehttp {

struct Response {
    long status = 0;
    std::string text;
};

class Request {
    std::string url_;
    std::string body_;
    std::map<std::string, std::string> headers_;
    long timeout_ = 0;
    std::string method_ = "GET";

    static size_t handleData(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append(static_cast<char*>(contents), size * nmemb);
        return size * nmemb;
    }

public:
    Request(const std::string& url) : url_(url) {}

    Request& withMethod(const std::string& m) {
        method_ = m;
        return *this;
    }
    Request& withBody(const std::string& b) {
        body_ = b;
        return *this;
    }
    Request& withHeader(const std::string& k, const std::string& v) {
        headers_[k] = v;
        return *this;
    }
    Request& withTimeout(long seconds) {
        timeout_ = seconds;
        return *this;
    }

    Response go() const {
        CURL* session = curl_easy_init();
        if (!session) {
            throw std::runtime_error("Couldn't start a cURL session");
        }

        std::string receivedData;
        long statusCode = 0;

        curl_easy_setopt(session, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, handleData);
        curl_easy_setopt(session, CURLOPT_WRITEDATA, &receivedData);

        if (!body_.empty()) {
            curl_easy_setopt(session, CURLOPT_POSTFIELDS, body_.c_str());
        }
        if (timeout_ > 0) {
            curl_easy_setopt(session, CURLOPT_TIMEOUT, timeout_);
        }

        curl_slist* headerList = nullptr;
        for (auto const& [key, value] : headers_) {
            std::string headerString = key + ": " + value;
            headerList = curl_slist_append(headerList, headerString.c_str());
        }
        if (headerList) {
            curl_easy_setopt(session, CURLOPT_HTTPHEADER, headerList);
        }

        if (method_ != "GET" && method_ != "POST") {
            curl_easy_setopt(session, CURLOPT_CUSTOMREQUEST, method_.c_str());
        }

        CURLcode result = curl_easy_perform(session);
        if (result != CURLE_OK) {
            if (headerList) curl_slist_free_all(headerList);
            curl_easy_cleanup(session);
            throw std::runtime_error(curl_easy_strerror(result));
        }

        curl_easy_getinfo(session, CURLINFO_RESPONSE_CODE, &statusCode);
        curl_easy_cleanup(session);
        if (headerList) curl_slist_free_all(headerList);

        return {statusCode, receivedData};
    }
};

inline Response Get(const std::string& url) {
    return Request(url).withMethod("GET").go();
}
inline Request Post(const std::string& url) {
    return Request(url).withMethod("POST");
}
inline Request Put(const std::string& url) {
    return Request(url).withMethod("PUT");
}
inline Request Patch(const std::string& url) {
    return Request(url).withMethod("PATCH");
}
inline Request Delete(const std::string& url) {
    return Request(url).withMethod("DELETE");
}

} // ehttp
