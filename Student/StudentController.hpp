#pragma once
#include "../httplib.h"
#include "../Result.hpp"
#include "../http_helper.hpp"
#include "Student.hpp"
#include "StudentDAO.hpp"
#include "StudentService.hpp"
#include <nlohmann/json.hpp>

class StudentController
{
    private:
        StudentService* service;

    public:
        StudentController() = delete;
        ~StudentController() = default;
        StudentController(StudentService* svc) ;
        void RegisterRoutes(httplib::Server& svr) ;
} ;

