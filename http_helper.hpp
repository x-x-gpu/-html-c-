#pragma once
#include "httplib.h"
#include <nlohmann/json.hpp>
#include "Result.hpp"

struct IdReq {
    std::string id;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(IdReq, id)
};

struct FilePathDto {
    std::string path;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FilePathDto, path)
};

struct CourseIdToFilenameDto {
    std::string course_id;
    std::string filename;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CourseIdToFilenameDto, course_id, filename)
};

struct UserAndPasswordDto {
    std::string username;
    std::string password;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UserAndPasswordDto, username, password)
};

struct UserPasswordAndAdminDto {
    std::string username;
    std::string password;
    std::string adminPassword;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(UserPasswordAndAdminDto, username, password, adminPassword)
};

inline void send_json(httplib::Response& res, const nlohmann::json& j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(4), "application/json; charset=utf-8");
}

inline void send_error(httplib::Response& res, const std::string& msg, int status = 400) {
    send_json(res, Res<void>::error(msg), status);
}

template <typename T>
inline void reply(httplib::Response& res, const Res<T>& result) {
    send_json(res, result, result.ok ? 200 : 400); 
}

template <typename Func>
inline auto SafeHandler (Func func) {
    return [func](const httplib::Request& req, httplib::Response& res) {
        try {
            func(req, res);
        } catch (const nlohmann::json::exception& e) {
            // 专门捕获 JSON 解析错误
            send_error(res, std::string("JSON Parse Error: ") + e.what(), 400);
        } catch (const std::exception& e) {
            // 捕获标准错误
            send_error(res, e.what(), 500);
        } catch (...) {
            // 捕获未知错误
            send_error(res, "Unknown Internal Error", 500);
        }
    };
};

template <typename T>
inline T parse_body(const httplib::Request& req) {
    return nlohmann::json::parse(req.body).get<T>();
}