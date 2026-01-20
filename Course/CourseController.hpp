#include "Course.hpp"
#include "CourseDAO.hpp"
#include "CourseService.hpp"
#include "../httplib.h"
#include "../Result.hpp"
#include "../http_helper.hpp"
#include <nlohmann/json.hpp>

class CourseController
{
    private:
        CourseService* service;

    public:
        CourseController() = delete;
        ~CourseController() = default;
        CourseController(CourseService* svc) ;
        void RegisterRoutes(httplib::Server& svr) ;
} ;