# 核心架构（简明）

遵循分层设计：实体层（Entity）、数据访问层（DAO）、业务层（Service）、交互层（UI/API）。职责单一、**低耦合**、易扩展。

## 核心要点
- 实体层：仅数据模型，所有实体继承 BaseEntity（id、createTime、updateTime、通用方法）。
- DAO 层：仅负责数据读写（内存/文件/DB），继承 BaseDAO 提供通用 CRUD。
- 业务层：封装校验、权限与业务规则，调用 DAO 完成持久化；对外提供服务接口。
- 通信层：调用业务层，封装路由进行通信。
- 交互层：控制台/API，接收请求并调用通信层。

## BaseEntity（摘要）
- 字段：id、createTime、updateTime
- 方法：get/set、equals/hash、toString
- 作用：统一主键与审计字段，减少重复。

## 版本演进（简略）
- v1：内存存储 + 基础 CRUD（Entity、DAO、Service、Console）。
- v2：扩充课程与gpa计算业务。

## 设计原则
- 单一职责、分层清晰：DAO 不含业务逻辑，Service 不直接做 IO。
- 高内聚低耦合：同类功能聚合到同一 Service。
- 可替换存储：切换存储仅改 DAO 实现。
- 接口优先：定义服务接口便于替换实现。

## 扩展建议（要点）
- 接入数据库（MyBatis 等）并抽象 BaseDAO（太麻烦已弃用，相关探索位于json-try/main3.cpp）。
- 引入缓存（Redis）优化高频查询。
- 将统计/导出拆成独立服务。
- 添加单元测试与集成测试覆盖关键逻辑。