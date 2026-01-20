#include "UserController.hpp"

UserController::UserController(UserService* svc) 
    : userService(*svc) 
{

}

void UserController::RegisterRoutes(httplib::Server& svr) 
{
    // 1. POST: 注册用户
    svr.Post("/users/register", SafeHandler([&](const auto& req, auto& res) {
        auto dto = parse_body<UserPasswordAndAdminDto>(req);
        reply(res, userService.addUser(dto.username, dto.password, dto.adminPassword));
    }));

    // 2. POST: 登录用户
    svr.Post("/users/login", SafeHandler([&](const auto& req, auto& res) {
        auto dto = parse_body<UserAndPasswordDto>(req);
        reply(res, userService.Login(dto.username, dto.password));
    }));

    // 3. PUT: 改变密码
    svr.Put("/users/change_password", SafeHandler([&](const auto& req, auto& res) {
        auto dto = parse_body<UserPasswordAndAdminDto>(req);
        reply(res, userService.ChangePassword(dto.username, dto.password/* newPassword */, dto.adminPassword));
    }));
}

