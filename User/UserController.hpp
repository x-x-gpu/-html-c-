#pragma once
#include "UserService.hpp"
#include "../httplib.h"
#include "../http_helper.hpp"
#include "../Result.hpp"

class UserController 
{
    private:
        UserService userService;

    public:
        UserController() = delete;
        ~UserController() = default;
        UserController(UserService* svc) ;
        void RegisterRoutes(httplib::Server& svr) ;
} ;