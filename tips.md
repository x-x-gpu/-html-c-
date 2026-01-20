1. HTTP动词

GET: 获取资源（查询学生列表或单个学生）。

POST: 创建资源（注册新学生或导入数据）。

PUT: 更新资源（修改学生信息）。

DELETE: 删除资源。

2. 业务逻辑的分层 (Service & DAO)

Controller (接口层)：即这些 svr.Get/Post 函数，负责解析 HTTP 请求和封装响应。

Service (业务逻辑层)：即 service.ListAllStudents()，负责具体的业务规则。

DAO (数据持久层)：即 dao.SaveToFile()，负责把数据存入磁盘。

3. 路由简化
   
目前代码中每个路由都在手动 j.dump(4) 和设置 Content-Type。写一个辅助函数：

void send_json(httplib::Response& res, const nlohmann::json& j, int status = 200) {
    res.status = status;
    res.set_content(j.dump(4), "application/json; charset=utf-8");
}

这样路由内部就只需要一行：send_json(res, result_data);。

4. cmake 编译
```
# 确保在项目根目录
rmdir -Recurse -Force build   # 删除旧目录
mkdir build 
cd build
# 手动运行配置命令，观察这里是否报错
cmake -G Ninja ..
# 如果配置成功，再运行编译
ninja
```
