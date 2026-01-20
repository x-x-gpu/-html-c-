#include "StudentController.hpp"

StudentController::StudentController(StudentService* svc) : service(svc) {}

void StudentController::RegisterRoutes(httplib::Server& svr) 
{
    // 1. Health Check 在主程序中注册

    // 2. POST: 注册学生
    svr.Post("/students/register", SafeHandler([&](const auto& req, auto& res) {
        auto student = parse_body<Student>(req);
        reply(res, service->RegisterStudent(student));
    }));

    // 3. DELETE: 删除学生
    svr.Delete("/students/delete", SafeHandler([&](const auto& req, auto& res) {
        auto id = parse_body<IdReq>(req);
        reply(res, service->RemoveStudent(id.id));
    }));

    // 4. PUT: 修改学生
    svr.Put("/students/modify", SafeHandler([&](const auto& req, auto& res) {
        auto student = parse_body<Student>(req);
        reply(res, service->ModifyStudent(student));
    }));

    // 5. GET(POST用于统一json解析): 获取单个学生
    svr.Post("/students/get", SafeHandler([&](const auto& req, auto& res) {
        auto id = parse_body<IdReq>(req);
        reply(res, service->FindStudentById_Value(id.id));
    }));

    // 6. GET: 获取所有学生
    svr.Get("/students/get_all", SafeHandler([&](const auto&, auto& res) {
        // 这里可以直接把 vector/list 包装进 Res 对象里
        reply(res, service->ListAllStudents());
    }));

    // 7. 导入/导出所有学生数据
    svr.Post("/students/import", SafeHandler([&](const auto& req, auto& res) {
        auto path = parse_body<FilePathDto>(req);
        reply(res, service->LoadStudentsFromCSV(path.path));
    }));

    svr.Post("/students/export", SafeHandler([&](const auto& req, auto& res) {
        auto path = parse_body<FilePathDto>(req);
        reply(res, service->ExportStudentsToCSV(path.path));
    }));

    // 8. 为学生gpa排序
    svr.Get("/students/get_all_and_sort_by_gpa", SafeHandler([&](const auto&, auto& res) {
        auto compareFunc = [](const Student& a, const Student& b) {
            return a.getGPA() > b.getGPA(); // 降序
        };
        reply(res, service->ListStudentsByFunc(compareFunc));
    }));
}