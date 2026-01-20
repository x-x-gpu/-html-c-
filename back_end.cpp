#include "Student/Student.hpp"
#include "Student/StudentDAO.hpp"
#include "Student/StudentService.hpp"
#include "Student/StudentController.hpp"
#include "Course/Course.hpp"
#include "Course/CourseDAO.hpp"
#include "Course/CourseService.hpp"
#include "Course/CourseController.hpp"
#include "User/User.hpp"
#include "User/UserDAO.hpp"
#include "User/UserService.hpp"
#include "User/UserController.hpp"

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "httplib.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <thread>
#include <atomic>
#include <chrono>
#include <conio.h>

// Controller统一注册路由

void RegisterAll(httplib::Server& /*svr*/) {}

template <typename Head, typename... Tail>
void RegisterAll(httplib::Server& svr, Head& head, Tail&... tail) 
{
    head.RegisterRoutes(svr);
    RegisterAll(svr, tail...);
}

// DAO统一加载和保存数据

void LoadFileAll () {}

template <typename Head, typename... Tail>
void LoadFileAll ( Head& head, Tail&... tail )
{
    if (std::ifstream(head.getFilename()))
        head.LoadFromFile(head.getFilename()) ;
    else
        std::cerr << "Data file " << head.getFilename() << " not found, starting with empty database." << std::endl;
    LoadFileAll( tail... ) ;
}

void SaveFileAll () {}

template <typename Head, typename... Tail>
void SaveFileAll ( Head& head, Tail&... tail )
{
    head.SaveToFile(head.getFilename()) ;
    SaveFileAll( tail... ) ;
}

// 加载httplib路由头+调用RegisterAll

template <typename... Controllers>
void ConfigureSVR(httplib::Server& svr, Controllers&... controllers) {
    // 处理CORPS问题（不做这个除了GET都用不了）
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With"}
    });

    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 200;
    });

    // 在线检查（前端检查状态）
    svr.Get("/health_check", [](const auto&, auto& res) {
        res.set_content("ok", "text/plain");
        std::cout << "[Health Check] ok" << std::endl;
    });

    RegisterAll(svr, controllers...);
}

#include <windows.h> 
#include <fstream>
#include <iostream>
#include <csignal>
#include <atomic>

static httplib::Server* global_svr_ptr = nullptr;

void signalHandler(int signum)
{
    std::cout << "Ctrl + C, 正在停止服务器..." << std::endl;
    if (global_svr_ptr) {
        global_svr_ptr->stop(); 
    }
}

int start_main() {
    // 处理Ctrl+C正常终止的问题
    std::signal(SIGINT, signalHandler);

    // 冷启动（弃用sqlite，那就从文件启动）
    const std::string studentDataFile = "D:\\c++_OOP\\HW\\core\\DataStore\\students.json";

    StudentDAO student_dao;
    student_dao.setFilename(studentDataFile);

    const std::string courseDataFile = "D:\\c++_OOP\\HW\\core\\DataStore\\course.json";

    CourseDAO course_dao;
    course_dao.setFilename(courseDataFile);

    const std::string userDataFile = "D:\\c++_OOP\\HW\\core\\DataStore\\users.json";
    UserDAO user_dao;
    user_dao.setFilename(userDataFile);
    
    LoadFileAll( student_dao, course_dao, user_dao ) ;

    StudentService student_service(&student_dao);
    StudentController student_controller(&student_service);

    CourseService course_service(&course_dao, &student_service);
    CourseController course_controller(&course_service);

    UserService user_service(&user_dao);
    UserController user_controller(&user_service);

    httplib::Server svr;
    global_svr_ptr = &svr;  

    ConfigureSVR(svr, student_controller, course_controller, user_controller);
    std::cout << "JSON 服务器正在监听端口 8080..." << std::endl;
    std::cout << "按 'q' 键退出服务器。" << std::endl;

    // 在单独的线程中运行服务器
    std::thread server_thread([&svr]() {
        svr.listen("0.0.0.0", 8080);
    });

    // 键盘检测循环
    while (true) {
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q') {
                std::cout << "检测到 'q' 键，正在停止服务器..." << std::endl;
                svr.stop();
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 设置频率
    }

    // 阻塞，svr.stop()只是给了一个信号，需要等待线程结束
    server_thread.join();

    SaveFileAll( student_dao, course_dao, user_dao ) ;
    std::cout << "服务器已停止，数据已保存。" << std::endl;
    return 0;
}

int main() {
    // 调整控制台为UTF-8编码，解决乱码问题
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 启用虚拟终端处理以支持ANSI颜色，如果不用这个那就只能用Terminal
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    return start_main();
}
