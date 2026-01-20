// Version 2
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <iostream>
#include <string>
#include <vector>
#include "httplib.h"
#include <nlohmann/json.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using json = nlohmann::json;

// 检查服务器是否在线
bool checkServer(httplib::Client& cli) {
    if (auto res = cli.Get("/health_check")) {
        return true;
    }
    cerr << "\033[31m[错误] 无法连接到服务器，请确保 back_end.exe 已启动！\033[0m" << endl;
    return false;
}

// 辅助函数：格式化显示 JSON 响应
void printResponse(const httplib::Result& res) {
    if (res) {
        if (res->status == 200) {
            cout << "\033[32m[成功]\033[0m ";
        } else {
            cout << "\033[31m[失败]\033[0m (" << res->status << ") ";
        }
        // 尝试解析 JSON 使得显示更可读，如果不是 JSON 则直接输出
        try {
            auto j = json::parse(res->body);
            cout << j.dump(4) << endl;
        } catch (...) {
            cout << res->body << endl;
        }
    } else {
        cerr << "\033[31m[错误] 请求发送失败 (无响应)\033[0m" << endl;
    }
}

void showLoginMenu() {
    cout << "\n========= 用户登录 =========" << endl;
    cout << "1. 用户注册" << endl;
    cout << "2. 用户登录" << endl;
    cout << "0. 退出" << endl;
    cout << "请选择: ";
}

void showMenu() {
    cout << "\n========= 学生管理系统 (C++ 客户端) =========" << endl;
    cout << "1. 列出所有学生 (GET)" << endl;
    cout << "2. 添加学生 (POST)" << endl;
    cout << "3. 删除学生 (DELETE)" << endl;
    cout << "4. 查找学生 (GET by ID)" << endl;
    cout << "5. 修改学生信息 (PUT)" << endl;       
    cout << "6. 从 CSV 文件导入 (POST)" << endl;   
    cout << "7. 导出到 CSV 文件 (POST)" << endl;   
    cout << "8. 按 GPA 排序列出学生 (GET)" << endl;

    cout << "--------- 课程管理 ---------" << endl;
    cout << "9. 列出所有课程 (GET)" << endl;
    cout << "10. 添加课程(不含成绩) (POST)" << endl;
    cout << "11. 删除课程 (DELETE)" << endl;
    cout << "12. 修改课程(不含成绩) (PUT)" << endl;
    cout << "13. 查找课程 (GET by ID)" << endl;
    cout << "14. 导入某课程成绩CSV (POST)" << endl;
    cout << "15. 导出某课程成绩CSV (POST)" << endl;

    cout << "--------- 用户管理 ---------" << endl;
    cout << "16. 更改密码 (PUT)" << endl;

    cout << "0. 退出" << endl;
    cout << "---------------------------------------------" << endl;
    cout << "请选择操作: ";
}

int main() {
    // 检测WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WinSock 初始化失败！" << endl;
        return 1;
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 启用虚拟终端处理以支持ANSI颜色，和后端一样的
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // 建立客户端连接
    httplib::Client cli("http://127.0.0.1:8080");
    cli.set_connection_timeout(1, 0); 

    // 登录循环
    std::string user = "";
    bool loggedIn = false;
    while (!loggedIn) {
        showLoginMenu();
        int loginChoice;
        if (!(cin >> loginChoice)) {
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }
        if (loginChoice == 0) {
            return 0; 
        }
        // 每次操作前检查服务器
        if (!checkServer(cli)) continue;
        if (loginChoice == 1) {
            // 注册
            string username, password, adminPassword;
            cout << ">>> 用户注册" << endl;
            cout << "用户名: "; cin >> username;
            cout << "密码: "; cin >> password;
            cout << "管理员密码: "; cin >> adminPassword;
            json reqBody = {
                {"username", username},
                {"password", password},
                {"adminPassword", adminPassword}
            };
            auto res = cli.Post("/users/register", reqBody.dump(), "application/json");
            printResponse(res);
        } else if (loginChoice == 2) {
            // 登录
            string username, password;
            cout << ">>> 用户登录" << endl;
            cout << "用户名: "; cin >> username;
            cout << "密码: "; cin >> password;
            json reqBody = {
                {"username", username},
                {"password", password}
            };
            auto res = cli.Post("/users/login", reqBody.dump(), "application/json");
            if (res && res->status == 200) {
                cout << "\033[32m[成功] 登录成功！\033[0m" << endl;
                loggedIn = true;
                user = username;
            } else {
                printResponse(res);
            }
        } else {
            cout << "无效的选择，请重试。" << endl;
        }
    }

    // 主操作循环

    int choice;
    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }

        if (choice == 0) break;

        // 每次操作前检查服务器
        if (!checkServer(cli)) continue;

        if (choice == 1) {
            // === 1. 列出所有 ===
            auto res = cli.Get("/students/get_all");
            if (res && res->status == 200) {
                try {
                    auto j = json::parse(res->body);
                    if (j.contains("data")) {
                        cout << "\n学生列表:\n" << j["data"].dump(4) << endl;
                    } else {
                        cout << "\n原始响应:\n" << j.dump(4) << endl;
                    }
                } catch (...) {
                    cout << res->body << endl;
                }
            } else {
                printResponse(res);
            }
        }
        else if (choice == 2) {
            // === 2. 添加学生 ===
            string id, name, age, className;
            cout << ">>> 添加新学生" << endl;
            cout << "ID: "; cin >> id;
            cout << "姓名: "; cin >> name;
            cout << "年龄: "; cin >> age;
            cout << "班级: "; cin >> className;

            json student = {
                {"id", id}, 
                {"name", name}, 
                {"age", age}, 
                {"className", className}, 
                {"updatedBy", user},
                {"gpa", 0.0},
                {"scores", json::array()}
            };
            auto res = cli.Post("/students/register", student.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 3) {
            // === 3. 删除学生 ===
            string id;
            cout << ">>> 删除学生" << endl;
            cout << "请输入要删除的ID: "; cin >> id;
            json reqBody = {{"id", id}};
            auto res = cli.Delete("/students/delete", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 4) {
            // === 4. 查找学生 ===
            string id;
            cout << ">>> 查找学生" << endl;
            cout << "请输入要查找的ID: "; cin >> id;
            json reqBody = {{"id", id}};
            auto res = cli.Post("/students/get", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 5) {
            // === 5. 修改学生 (PUT) ===
            // 后端路由: /students/:id
            string targetId, name, age, className;
            cout << ">>> 修改学生信息" << endl;
            cout << "请输入目标学生ID: "; cin >> targetId;
            
            cout << "请输入新姓名: "; cin >> name;
            cout << "请输入新年龄: "; cin >> age;
            cout << "请输入新班级: "; cin >> className;

            json student = {
                {"id", targetId}, 
                {"name", name}, 
                {"age", age}, 
                {"className", className}, 
                {"updatedBy", user}
            };

            auto res = cli.Put("/students/modify", student.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 6) {
            // === 6. CSV 导入 (POST) ===
            string path;
            cout << ">>> 从 CSV 导入" << endl;
            cout << "请输入CSV文件绝对路径 (例如 D:\\data.csv): "; 
            getline (cin, path);

            json reqBody = {{"path", path}};
            auto res = cli.Post("/students/import", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 7) {
            // === 7. CSV 导出 (POST) ===
            string path;
            cout << ">>> 导出到 CSV" << endl;
            cout << "请输入导出目标路径 (例如 D:\\backup.csv): "; 
            cin >> path;

            json reqBody = {{"path", path}};
            auto res = cli.Post("/students/export", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 8) {
            // === 8. GPA 排序 ===
            auto res = cli.Get("/students/get_all_and_sort_by_gpa");
            printResponse(res);
        }
        else if (choice == 9) {
            // === 9. 列出所有课程 ===
            auto res = cli.Get("/courses/get_all");
            printResponse(res);
        }
        else if (choice == 10) {
            // === 10. 添加课程(不含成绩) ===
            string id, course_name, teacher_name;
            double value;
            cout << ">>> 添加课程(不含成绩)" << endl;
            cout << "课程ID: "; cin >> id;
            cout << "课程名: "; cin >> course_name;
            cout << "教师名: "; cin >> teacher_name;
            cout << "学分: "; cin >> value;

            json course = {
                {"id", id},
                {"course_name", course_name},
                {"teacher_name", teacher_name},
                {"value", value},
                {"student_scores", json::array()},
                {"updatedBy", user}
            };
            auto res = cli.Post("/courses/register_except_scores", course.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 11) {
            // === 11. 删除课程 ===
            string id;
            cout << ">>> 删除课程" << endl;
            cout << "请输入课程ID: "; cin >> id;
            json reqBody = {{"id", id}};
            auto res = cli.Delete("/courses/delete", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 12) {
            // === 12. 修改课程(不含成绩) ===
            string id, course_name, teacher_name;
            double value;
            cout << ">>> 修改课程(不含成绩)" << endl;
            cout << "课程ID: "; cin >> id;
            cout << "课程名: "; cin >> course_name;
            cout << "教师名: "; cin >> teacher_name;
            cout << "学分: "; cin >> value;

            json course = {
                {"id", id},
                {"course_name", course_name},
                {"teacher_name", teacher_name},
                {"value", value},
                {"student_scores", json::array()},
                {"updatedBy", user}
            };
            auto res = cli.Put("/courses/modify_except_scores", course.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 13) {
            // === 13. 查找课程 ===
            string id;
            cout << ">>> 查找课程" << endl;
            cout << "请输入课程ID: "; cin >> id;
            json reqBody = {{"id", id}};
            auto res = cli.Post("/courses/get", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 14) {
            // === 14. 导入某课程成绩CSV ===
            string course_id, filename;
            cout << ">>> 导入课程成绩CSV" << endl;
            cout << "课程ID: "; cin >> course_id;
            cout << "CSV路径: "; cin >> filename;

            json reqBody = {{"course_id", course_id}, {"filename", filename}};
            auto res = cli.Post("/courses/load_scores_from_csv", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 15) {
            // === 15. 导出某课程成绩CSV ===
            string course_id, filename;
            cout << ">>> 导出课程成绩CSV" << endl;
            cout << "课程ID: "; cin >> course_id;
            cout << "导出路径: "; cin >> filename;

            json reqBody = {{"course_id", course_id}, {"filename", filename}};
            auto res = cli.Post("/courses/export_scores_to_csv", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else if (choice == 16) {
            // === 16. 更改密码 ===
            string username, newPassword, adminPassword;
            cout << ">>> 更改密码" << endl;
            cout << "用户名: "; cin >> username;
            cout << "新密码: "; cin >> newPassword;
            cout << "管理员密码: "; cin >> adminPassword;

            json reqBody = {
                {"username", username},
                {"password", newPassword},
                {"adminPassword", adminPassword}
            };
            auto res = cli.Put("/users/change_password", reqBody.dump(), "application/json");
            printResponse(res);
        }
        else {
            cout << "无效的选择，请重试。" << endl;
        }
    }
    return 0;
}