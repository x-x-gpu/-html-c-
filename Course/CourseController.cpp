#include "CourseController.hpp"

CourseController::CourseController(CourseService* svc) : service(svc) {}

void CourseController::RegisterRoutes(httplib::Server& svr) 
{
    // 1. Health Check 在主程序中注册

    // 2. POST: 注册课程（不含成绩）
    svr.Post("/courses/register_except_scores", SafeHandler([&](const auto& req, auto& res) {
        auto course = parse_body<Course>(req);
        reply(res, service->RegisterCourse_ExceptScores(course));
    }));

    // 3. DELETE: 删除课程
    svr.Delete("/courses/delete", SafeHandler([&](const auto& req, auto& res) {
        auto id = parse_body<IdReq>(req);
        reply(res, service->RemoveCourse(id.id));
    }));

    // 4. PUT: 修改课程（不含成绩） 
    svr.Put("/courses/modify_except_scores", SafeHandler([&](const auto& req, auto& res) {
        auto course = parse_body<Course>(req);
        reply(res, service->ModifyCourse_ExceptScores(course));
    }));

    // 5. GET(POST用于统一json解析): 获取单个课程
    svr.Post("/courses/get", SafeHandler([&](const auto& req, auto& res) {
        auto id = parse_body<IdReq>(req);
        reply(res, service->FindCourseById_Value(id.id));
    }));

    // 6. 导入/导出某门课的成绩 
    svr.Post("/courses/load_scores_from_csv", SafeHandler([&](const auto& req, auto& res) {
        auto loadReq = parse_body<CourseIdToFilenameDto>(req);
        reply(res, service->LoadScoreFromCSV(loadReq.course_id, loadReq.filename));
    }));

    svr.Post("/courses/export_scores_to_csv", SafeHandler([&](const auto& req, auto& res) {
        auto exportReq = parse_body<CourseIdToFilenameDto>(req);
        reply(res, service->ExportScoreToCSV(exportReq.course_id, exportReq.filename));
    }));

    // 7. 获取所有课程的简略信息
    svr.Get("/courses/get_all", SafeHandler([&](const auto& req, auto& res) {
        auto result = service->FindAllCourses();
        reply(res, result);
    }));
}