// Version 1

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

using namespace std;
using json = nlohmann::json;

// 检查服务器是否在线的工具函数
bool checkServer(httplib::Client& cli) {
    // 使用简单的 Get 请求根目录或心跳接口
    if (auto res = cli.Get("/health_check")) { // 如果后端没写这个路由，Get("/") 也可以
        return true;
    }
    // 如果返回空值（nullptr），说明底层连接建立失败
    cerr << "\033[31m[错误] 无法连接到服务器，请确保 back_end.exe 已启动！\033[0m" << endl;
    return false;
}

void showMenu() {
    cout << "\n========= 学生管理系统 (C++ 客户端) =========" << endl;
    cout << "1. 列出所有学生" << endl;
    cout << "2. 添加学生" << endl;
    cout << "3. 删除学生 (按ID)" << endl;
    cout << "4. 查找学生 (按ID)" << endl;
    cout << "0. 退出" << endl;
    cout << "请选择操作: ";
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WinSock 初始化失败！" << endl;
        return 1;
    }
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    httplib::Client cli("http://127.0.0.1:8080");
    // 设置较短的连接超时，避免检测服务器时卡顿太久
    cli.set_connection_timeout(1, 0); // 1秒

    int choice;
    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(100, '\n');
            continue;
        }

        if (choice == 0) break;

        // --- 核心逻辑：操作前先检测 ---
        if (!checkServer(cli)) continue; 

        if (choice == 1) {
            auto res = cli.Get("/students");
            if (res && res->status == 200) {
                auto j = json::parse(res->body);
                cout << "\n学生列表:\n" << j["data"].dump(4) << endl;
            }
        } 
        else if (choice == 2) {
            string id, name, age, className;
            cout << "ID: "; cin >> id;
            cout << "姓名: "; cin >> name;
            cout << "年龄: "; cin >> age;
            cout << "班级: "; cin >> className;

            json student = {{"id", id}, {"name", name}, {"age", age}, {"className", className}, {"updatedBy", "system"}};
            auto res = cli.Post("/students", student.dump(), "application/json");
            if (res) cout << "响应: " << res->body << endl;
        }
        else if (choice == 3) {
            string id;
            cout << "要删除的ID: "; cin >> id;
            auto res = cli.Delete(("/students/" + id).c_str());
            if (res) cout << "响应: " << res->body << endl;
        }
        else if (choice == 4) {
            string id;
            cout << "要查找的ID: "; cin >> id;
            auto res = cli.Get(("/students/" + id).c_str());
            if (res) cout << "查询结果: " << res->body << endl;
        }
    }
    return 0;
}
