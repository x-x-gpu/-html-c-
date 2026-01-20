# 学生管理系统 — 使用说明

这是一份编译与使用front_end.exe的使用手册，网页端基本上按键一目了然，就不提供手册了。

**使用时请先运行start.bat**
**注意超级管理员账号是admin，密码是admin**

1) 前提要求
- C++17 支持的编译器（g++, clang, MSVC）。
- 依赖头文件：httplib.h（cpp-httplib）、nlohmann/json.hpp。
- Windows: 需要链接 Winsock 库（ws2_32）。

2) 编译示例
- MinGW / g++ (Windows):
  g++ front_end.cpp -o front_end.exe -std=c++17 -lws2_32
- MSVC:
  cl /std:c++17 front_end.cpp ws2_32.lib
- Linux (若需编译，注意移除/适配 Winsock 代码):
  g++ front_end.cpp -o front_end -std=c++17

将 httplib.h 与 nlohmann/json.hpp 放在编译器能找到的 include 目录或与源文件同目录。

httplib.h已经放了，json建议用[vcpkg](https://www.cnblogs.com/kenall/p/18469767)在powershell里面装好。

3) 后端
- 后端服务运行在 http://127.0.0.1:8080 并暴露相应路由（参见下文）。
- 启动后端（例如 back_end.exe），确认 /health_check 或根路由可访问。

4) 运行客户端
- 在后端启动并可访问后，运行：
  front_end.exe
- 首次运行会出现登录/注册菜单。所有操作会先检测后端在线状态。

5) 登录与注册
- 注册需输入管理员密码（admin），注册路由：POST /users/register
- 登录路由：POST /users/login
- 登录成功后进入主菜单，后续请求携带由后端管理的权限/会话。

6) 客户端菜单 -> 主要 API 路由（前端实现调用）
- 用户：
  - 注册: POST /users/register
  - 登录: POST /users/login
  - 改密码: PUT /users/change_password
- 学生：
  - 列表: GET /students/get_all
  - 添加: POST /students/register
  - 删除: DELETE /students/delete
  - 获取: POST /students/get
  - 修改: PUT /students/modify
  - CSV 导入: POST /students/import  (body: { "path": "..." })
  - CSV 导出: POST /students/export  (body: { "path": "..." })
  - 按 GPA 排序: GET /students/get_all_and_sort_by_gpa
- 课程：
  - 列表: GET /courses/get_all
  - 添加(不含成绩): POST /courses/register_except_scores
  - 删除: DELETE /courses/delete
  - 修改(不含成绩): PUT /courses/modify_except_scores
  - 获取: POST /courses/get
  - 导入成绩 CSV: POST /courses/load_scores_from_csv (body: { "course_id","filename" })
  - 导出成绩 CSV: POST /courses/export_scores_to_csv (body: { "course_id","filename" })

7) 请求格式
- 所有请求为 JSON（Content-Type: application/json）。
- 常用 DTO 示例：
  - 登录: { "username": "...", "password": "..." }
  - 注册: { "username": "...", "password": "...", "adminPassword": "..." }
  - 操作 ID: { "id": "..." }
  - 文件路径: { "path": "C:\\path\\to\\file.csv" }
  - 课程文件: { "course_id": "CS101", "filename": "file.csv" }

8) 响应格式
- 后端统一使用 Res<T> 结构：{ "ok": bool, "msg": string, "data": ... }（void 类型无 data）。
- ok=true 表示成功；失败时查看 msg。

9) 常见问题
- 无法连接：确认后端已启动并监听 127.0.0.1:8080，防火墙放通端口。
- JSON 解析失败：检查请求体是否为合法 JSON。
- 权限/管理员密码错误：确认后端管理员密码配置或联系维护者。

10) 建议
- 二次开发/调试时使用 curl 或 Postman 验证单个接口。
- 将后端日志输出打开以便排查错误。
- 用ninja编译快很多，也能用断点。

