# 项目类接口简介

本项目采用分层架构，以下简要介绍各层的核心类及其接口（方法和功能）。所有类均使用C++实现，支持虚函数、继承和STL容器。

## 实体层 (Entity Layer)

### BaseEntity
- **继承关系**: 无（虚基类）。
- **目的**: 提供基础实体功能，包括ID管理和拷贝操作。
- **关键接口**:
  - `BaseEntity()`: 默认构造函数。
  - `BaseEntity(const BaseEntity&)`: 复制构造函数。
  - `operator=(const BaseEntity&)`: 赋值运算符重载。
  - `virtual ~BaseEntity()`: 虚析构函数。
  - `virtual std::string toJson() const`: 序列化为JSON。
  - `virtual void fromJson(const std::string&)`: 从JSON反序列化。
  - `int getId() const`: 获取ID。
  - `void setId(int)`: 设置ID。

### User
- **目的**: 表示用户实体，包含登录信息。
- **关键接口**:
  - `User()`: 默认构造函数。
  - `std::string getUsername() const`: 获取用户名。
  - `void setUsername(const std::string&)`: 设置用户名。
  - `std::string getPassword() const`: 获取密码（哈希存储）。
  - `void setPassword(const std::string&)`: 设置密码。
  - `std::string getRole() const`: 获取角色（e.g., admin, student）。
  - `void setRole(const std::string&)`: 设置角色。
  - 继承BaseEntity的所有虚方法。

### Student
- **继承关系**: 虚继承自BaseEntity。
- **目的**: 表示学生实体，包含个人信息和成绩。
- **关键接口**:
  - `Student()`: 默认构造函数。
  - `std::string getName() const`: 获取姓名。
  - `void setName(const std::string&)`: 设置姓名。
  - `std::string getStudentId() const`: 获取学号。
  - `void setStudentId(const std::string&)`: 设置学号。
  - `std::vector<double> getGrades() const`: 获取成绩列表。
  - `void addGrade(double)`: 添加成绩。
  - `double getAverageGrade() const`: 计算平均成绩。
  - 继承BaseEntity的所有虚方法。

### Course
- **继承关系**: 虚继承自BaseEntity。
- **目的**: 表示课程实体，包含课程信息和学生列表。
- **关键接口**:
  - `Course()`: 默认构造函数。
  - `std::string getCourseName() const`: 获取课程名。
  - `void setCourseName(const std::string&)`: 设置课程名。
  - `std::string getTeacher() const`: 获取教师名。
  - `void setTeacher(const std::string&)`: 设置教师名。
  - `std::vector<int> getStudentIds() const`: 获取选课学生ID列表。
  - `void addStudent(int)`: 添加学生ID。
  - `void removeStudent(int)`: 移除学生ID。
  - 继承BaseEntity的所有虚方法。

## 数据访问层 (DAO Layer)

### BaseDAO（存在于设计理论中）
- **模板类**: 泛型DAO基类，T为实体类型。
- **目的**: 提供CRUD操作和文件持久化。
- **关键接口**:
  - `BaseDAO(const std::string& filename)`: 构造函数，指定JSON文件。
  - `virtual ~BaseDAO()`: 虚析构函数。
  - `virtual T* findById(int id)`: 根据ID查找实体。
  - `virtual std::vector<T*> findAll()`: 获取所有实体。
  - `virtual void save(T* entity)`: 保存实体。
  - `virtual void update(T* entity)`: 更新实体。
  - `virtual void remove(int id)`: 删除实体。
  - `virtual void loadFromFile()`: 从JSON文件加载数据。
  - `virtual void saveToFile()`: 保存数据到JSON文件。

### UserDAO
- **继承关系**: 继承自BaseDAO<User>。
- **目的**: 用户数据访问。
- **关键接口**: 继承BaseDAO的所有方法，无额外接口。

### StudentDAO
- **继承关系**: 继承自BaseDAO<Student>。
- **目的**: 学生数据访问。
- **关键接口**: 继承BaseDAO的所有方法，无额外接口。

### CourseDAO
- **继承关系**: 继承自BaseDAO<Course>。
- **目的**: 课程数据访问。
- **关键接口**: 继承BaseDAO的所有方法，无额外接口。

## 服务层 (Service Layer)

### BaseService<T, D> （存在于设计理论中）
- **模板类**: 泛型服务基类，T为实体，D为DAO。
- **目的**: 封装业务逻辑，调用DAO。
- **关键接口**:
  - `BaseService(D* dao)`: 构造函数，注入DAO指针。
  - `virtual ~BaseService()`: 虚析构函数。
  - `virtual T* getById(int id)`: 获取实体。
  - `virtual std::vector<T*> getAll()`: 获取所有实体。
  - `virtual void create(T* entity)`: 创建实体。
  - `virtual void update(T* entity)`: 更新实体。
  - `virtual void deleteById(int id)`: 删除实体。
  - `virtual bool validate(T* entity)`: 验证实体数据。

### UserService
- **继承关系**: 继承自BaseService<User, UserDAO>。
- **目的**: 用户业务逻辑，包括登录验证。
- **关键接口**:
  - `bool login(const std::string& username, const std::string& password)`: 用户登录。
  - `void registerUser(User* user)`: 用户注册。
  - 继承BaseService的所有方法。

### StudentService
- **继承关系**: 继承自BaseService<Student, StudentDAO>。
- **目的**: 学生业务逻辑，包括成绩管理。
- **关键接口**:
  - `void enrollCourse(int studentId, int courseId)`: 学生选课。
  - `void dropCourse(int studentId, int courseId)`: 学生退课。
  - `std::vector<Course*> getCourses(int studentId)`: 获取学生课程。
  - 继承BaseService的所有方法。

### CourseService
- **继承关系**: 继承自BaseService<Course, CourseDAO>。
- **目的**: 课程业务逻辑，包括学生管理。
- **关键接口**:
  - `void addStudent(int courseId, int studentId)`: 添加学生到课程。
  - `void removeStudent(int courseId, int studentId)`: 从课程移除学生。
  - `std::vector<Student*> getStudents(int courseId)`: 获取课程学生。
  - 继承BaseService的所有方法。

## 控制器层 (Controller Layer)

### BaseController （存在于设计理论中）
- **目的**: 基础控制器，提供HTTP响应处理。
- **关键接口**:
  - `BaseController()`: 默认构造函数。
  - `virtual ~BaseController()`: 虚析构函数。
  - `virtual std::string handleRequest(const std::string& method, const std::string& path, const std::string& body)`: 处理HTTP请求，返回JSON响应。
  - `virtual void registerRoutes()`: 注册路由（纯虚函数）。

### UserController
- **继承关系**: 继承自BaseController。
- **目的**: 处理用户相关HTTP请求。
- **关键接口**:
  - `UserController(UserService* service)`: 构造函数，注入服务。
  - `std::string login(const std::string& body)`: 处理登录请求。
  - `std::string registerUser(const std::string& body)`: 处理注册请求。
  - `void registerRoutes()`: 注册用户路由（e.g., /login, /register）。

### StudentController
- **继承关系**: 继承自BaseController。
- **目的**: 处理学生相关HTTP请求。
- **关键接口**:
  - `StudentController(StudentService* service)`: 构造函数，注入服务。
  - `std::string getStudents()`: 获取所有学生。
  - `std::string createStudent(const std::string& body)`: 创建学生。
  - `std::string updateStudent(int id, const std::string& body)`: 更新学生。
  - `std::string deleteStudent(int id)`: 删除学生。
  - `void registerRoutes()`: 注册学生路由（e.g., /students）。

### CourseController
- **继承关系**: 继承自BaseController。
- **目的**: 处理课程相关HTTP请求。
- **关键接口**:
  - `CourseController(CourseService* service)`: 构造函数，注入服务。
  - `std::string getCourses()`: 获取所有课程。
  - `std::string createCourse(const std::string& body)`: 创建课程。
  - `std::string updateCourse(int id, const std::string& body)`: 更新课程。
  - `std::string deleteCourse(int id)`: 删除课程。
  - `void registerRoutes()`: 注册课程路由（e.g., /courses）。

## 其他辅助类

### Res<T> (Result Template)
- **模板类**: 表示操作结果，支持成功/失败状态和数据封装。
- **关键接口**:
  - `Res(bool ok, std::string msg, T data)`: 构造函数。
  - `static Res<T> success(T d, std::string m = "Success")`: 创建成功结果。
  - `static Res<T> error(std::string m)`: 创建错误结果。
  - `bool ok`: 成功标志。
  - `std::string msg`: 消息。
  - `T data`: 数据（Res<void> 特化无此字段）。
- **特化**: Res<void> 用于无数据结果，仅包含 ok 和 msg。

### HttpHelper (Functions in http_helper.hpp)
- **目的**: HTTP请求/响应辅助工具，提供JSON发送、错误处理和安全包装。
- **关键接口**:
  - `void send_json(httplib::Response& res, const nlohmann::json& j, int status = 200)`: 发送JSON响应。
  - `void send_error(httplib::Response& res, const std::string& msg, int status = 400)`: 发送错误响应。
  - `void reply(httplib::Response& res, const Res<T>& result)`: 根据Res结果回复响应。
  - `auto SafeHandler(Func func)`: 安全处理函数，捕获异常并发送错误响应。
  - `T parse_body(const httplib::Request& req)`: 解析请求体为指定类型。
- **DTO结构体**: IdReq, FilePathDto, CourseIdToFilenameDto, UserAndPasswordDto, UserPasswordAndAdminDto，用于请求数据绑定。
