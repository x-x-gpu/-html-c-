const API_BASE = "http://127.0.0.1:8080";
let currentUser = null;
let studentScoreChart = null;

// 页面加载完成后执行
document.addEventListener("DOMContentLoaded", () => {
    loadCurrentUser();
    const hasApp = document.querySelector(".app");
    const hasLoginForm = document.getElementById("login-form");
    const hasRegisterForm = document.getElementById("register-form");

    if (hasApp) {
        if (!currentUser) {
            window.location.href = "login.html";
            return;
        }
        setupNavigation();
        if (document.getElementById("status-indicator")) {
            checkServerStatus();
            // 每5秒检查一次服务器状态
            setInterval(checkServerStatus, 5000);
        }
        updateUserChip();
        // 初始加载数据
        fetchAllStudents();
        fetchAllCourses();
    }

    if (hasLoginForm && currentUser) {
        window.location.href = "index.html";
    }
});

function loadCurrentUser() {
    currentUser = localStorage.getItem("currentUser") || sessionStorage.getItem("currentUser");
}

function ensureLoggedIn() {
    if (currentUser) return true;
    safeShowToast("请先登录");
    window.location.href = "login.html";
    return false;
}

function safeShowToast(message) {
    const toast = document.getElementById("toast");
    if (toast) {
        showToast(message);
    } else {
        alert(message);
    }
}

function setupNavigation() {
    const navItems = document.querySelectorAll(".nav-item");
    navItems.forEach(item => {
        item.addEventListener("click", (e) => {
            e.preventDefault();
            navItems.forEach(n => n.classList.remove("active"));
            item.classList.add("active");
            const targetId = item.getAttribute("data-target");
            setActiveSection(targetId);
        });
    });
}

function setActiveSection(sectionId) {
    const sections = document.querySelectorAll(".page-section");
    sections.forEach(s => s.classList.add("hidden"));
    const target = document.getElementById(sectionId);
    if (target) target.classList.remove("hidden");
}

// === 1. 服务器状态检查 (对应 checkServer) ===
async function checkServerStatus() {
    const indicator = document.getElementById("status-indicator");
    try {
        const response = await fetch(`${API_BASE}/health_check`);
        if (response.ok) {
            indicator.textContent = "在线";
            indicator.className = "online";
            return true;
        } else {
            throw new Error("Status not 200");
        }
    } catch (error) {
        indicator.textContent = "离线 (请启动后端)";
        indicator.className = "offline";
        return false;
    }
}

// === 2. 获取所有学生 (对应 GET /students) ===
async function fetchAllStudents() {
    if (!ensureLoggedIn()) return;
    showSpinner(true);
    try {
        const response = await fetch(`${API_BASE}/students/get_all`);
        const result = await response.json();
        
        // C++ 后端结构可能是 {data: [...]} 或直接 [...]
        let list = [];
        if (result.data) {
            list = result.data;
        } else if (Array.isArray(result)) {
            list = result;
        }
        
        renderStudentTable(list);
    } catch (error) {
        console.error("Fetch error:", error);
        showToast("获取列表失败，请检查后端", "error");
    } finally {
        showSpinner(false);
    }
}

async function fetchAllStudentsSorted() {
    if (!ensureLoggedIn()) return;
    showSpinner(true);
    try {
        const response = await fetch(`${API_BASE}/students/get_all_and_sort_by_gpa`);
        const result = await response.json();

        let list = [];
        if (result.data) {
            list = result.data;
        } else if (Array.isArray(result)) {
            list = result;
        }

        renderStudentTable(list);
    } catch (error) {
        showToast("获取排序失败", "error");
    } finally {
        showSpinner(false);
    }
}

// === 3. 渲染学生表格 ===
function renderStudentTable(students) {
    const tbody = document.getElementById("student-table-body");
    const noData = document.getElementById("no-data-msg");
    tbody.innerHTML = "";

    if (!students || students.length === 0) {
        noData.classList.remove("hidden");
        return;
    }
    noData.classList.add("hidden");

    students.forEach(s => {
        const tr = document.createElement("tr");
        const gpa = typeof s.gpa === "number" ? s.gpa.toFixed(2) : (s.gpa ?? "-");
        tr.innerHTML = `
            <td>${s.id}</td>
            <td>${s.name}</td>
            <td>${s.age}</td>
            <td>${s.className}</td>
            <td>${gpa}</td>
            <td>${s.updatedBy || '-'}</td>
            <td>
                <button class="btn btn-small btn-info" onclick="prepareStudentEdit('${s.id}', '${s.name}', '${s.age}', '${s.className}')">编辑</button>
                <button class="btn btn-small btn-secondary" onclick="showStudentDetail('${s.id}')">详细</button>
                <button class="btn btn-small btn-danger" onclick="deleteStudent('${s.id}')">删除</button>
            </td>
        `;
        tbody.appendChild(tr);
    });
}

// === 学生详情（含成绩与饼图） ===
async function showStudentDetail(id) {
    if (!ensureLoggedIn()) return;

    try {
        const response = await fetch(`${API_BASE}/students/get`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id })
        });
        const result = await response.json();
        const student = result?.data ?? result;

        if (!student || !student.id) {
            showToast("未找到该学生");
            return;
        }

        // 基本信息
        const info = document.getElementById("student-detail-info");
        const gpaText = typeof student.gpa === "number" ? student.gpa.toFixed(2) : (student.gpa ?? "-");
        info.innerHTML = `
            <div>姓名：${student.name ?? "-"}</div>
            <div>ID：${student.id ?? "-"}</div>
            <div>年龄：${student.age ?? "-"}</div>
            <div>班级：${student.className ?? "-"}</div>
            <div>GPA：${gpaText}</div>
        `;

        // 成绩明细
        const scores = Array.isArray(student.scores) ? student.scores : [];
        const detail = document.getElementById("student-detail-scores");
        if (scores.length === 0) {
            detail.innerHTML = `<div>暂无成绩</div>`;
        } else {
            detail.innerHTML = `
                <table class="detail-table">
                    <thead>
                        <tr>
                            <th>课程ID</th>
                            <th>学分</th>
                            <th>成绩</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${scores.map(s => `
                            <tr>
                                <td>${s.course_id ?? "-"}</td>
                                <td>${s.course_value ?? "-"}</td>
                                <td>${s.score ?? "-"}</td>
                            </tr>
                        `).join("")}
                    </tbody>
                </table>
            `;
        }

        // 饼图数据分桶
        const buckets = { "70-80": 0, "80-90": 0, "90-100": 0 };
        scores.forEach(s => {
            const v = Number(s.score);
            if (v >= 70 && v < 80) buckets["70-80"]++;
            else if (v >= 80 && v < 90) buckets["80-90"]++;
            else if (v >= 90 && v <= 100) buckets["90-100"]++;
        });

        // 绘制饼图
        const ctx = document.getElementById("student-score-chart");
        if (studentScoreChart) studentScoreChart.destroy();
        studentScoreChart = new Chart(ctx, {
            type: "pie",
            data: {
                labels: ["70-80", "80-90", "90-100"],
                datasets: [{
                    data: [buckets["70-80"], buckets["80-90"], buckets["90-100"]],
                    backgroundColor: ["#FF9F80", "#7BC8FF", "#F28BFF"],
                    borderColor: "#ffffff",
                    borderWidth: 2,
                    hoverOffset: 8
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                layout: {
                    padding: { top: 8, bottom: 8, left: 8, right: 8 }
                },
                plugins: {
                    legend: {
                        position: "bottom",
                        labels: {
                            boxWidth: 12,
                            boxHeight: 12
                        }
                    },
                    title: {
                        display: true,
                        text: "成绩分布图"
                    },
                    tooltip: {
                        enabled: true
                    }
                }
            }
        });

        document.getElementById("student-detail-modal").style.display = "block";
    } catch (error) {
        showToast("获取详情失败");
    }
}

function closeStudentDetailModal() {
    document.getElementById("student-detail-modal").style.display = "none";
}

// === 4. 查找学生 (对应 POST /students/get) ===
async function findStudent() {
    if (!ensureLoggedIn()) return;
    const id = document.getElementById("search-input").value.trim();
    if (!id) {
        fetchAllStudents(); // 如果为空，显示所有
        return;
    }

    try {
        const response = await fetch(`${API_BASE}/students/get`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id: id })
        });

        const result = await response.json();

        // 兼容多种返回结构：
        // 1) { data: {...} }
        // 2) { data: [...] }
        // 3) {...} (直接对象)
        let list = [];
        if (result && result.data) {
            if (Array.isArray(result.data)) {
                list = result.data;
            } else {
                list = [result.data];
            }
        } else if (result && result.id) {
            list = [result];
        }

        if (list.length > 0) {
            renderStudentTable(list);
        } else {
            renderStudentTable([]);
            showToast("未找到该ID的学生");
        }
    } catch (error) {
        showToast("查找失败");
    }
}

// === 5. 删除学生 (对应 DELETE /students/delete) ===
async function deleteStudent(id) {
    if (!ensureLoggedIn()) return;
    if (!confirm(`确定要删除学生 ID: ${id} 吗?`)) return;

    try {
        const response = await fetch(`${API_BASE}/students/delete`, {
            method: "DELETE", // 虽然 HTTP 标准 DELETE 不建议带 body，但你的 C++ 后端是这样写的
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id: id })
        });

        if (response.ok) {
            showToast("删除成功");
            fetchAllStudents(); // 刷新列表
        } else {
            showToast("删除失败");
        }
    } catch (error) {
        showToast("请求错误");
    }
}

// === 6. 学生添加/修改 表单处理 ===
function openStudentModal(mode, id = '', name = '', age = '', className = '') {
    const modal = document.getElementById("student-modal");
    const title = document.getElementById("student-modal-title");
    const formMode = document.getElementById("student-form-mode");
    const idInput = document.getElementById("student-form-id");

    modal.style.display = "block";
    formMode.value = mode;

    if (mode === 'add') {
        title.textContent = "添加新学生";
        document.getElementById("student-form").reset();
        idInput.disabled = false; // 添加时可以输入ID
    } else {
        title.textContent = "修改学生信息";
        idInput.value = id;
        idInput.disabled = true; // 修改时ID不可变
        document.getElementById("student-form-name").value = name;
        document.getElementById("student-form-age").value = age;
        document.getElementById("student-form-class").value = className;
    }
}

function closeStudentModal() {
    document.getElementById("student-modal").style.display = "none";
}

function prepareStudentEdit(id, name, age, className) {
    openStudentModal('edit', id, name, age, className);
}

// 提交表单逻辑
async function handleStudentSubmit(event) {
    event.preventDefault();
    if (!ensureLoggedIn()) return;
    const mode = document.getElementById("student-form-mode").value;
    
    const studentData = {
        id: document.getElementById("student-form-id").value,
        name: document.getElementById("student-form-name").value,
        age: document.getElementById("student-form-age").value,
        className: document.getElementById("student-form-class").value,
        updatedBy: currentUser // 标记来源
    };

    let url = "";
    let method = "";

    if (mode === 'add') {
        url = `${API_BASE}/students/register`;
        method = "POST";
        studentData.gpa = 0.0;
        studentData.scores = [];
    } else {
        // C++ 代码中 PUT 也是发送完整 JSON
        url = `${API_BASE}/students/modify`;
        method = "PUT";
    }

    try {
        const response = await fetch(url, {
            method: method,
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(studentData)
        });

        if (response.ok) {
            showToast(mode === 'add' ? "添加成功" : "修改成功");
            closeStudentModal();
            fetchAllStudents();
        } else {
            showToast("操作失败");
        }
    } catch (error) {
        showToast("服务器连接错误");
    }
}

// === 7. CSV 导入/导出 ===
function toggleCsvPanel() {
    if (!ensureLoggedIn()) return;
    const panel = document.getElementById("csv-panel");
    panel.classList.toggle("hidden");
}

async function importCSV() {
    if (!ensureLoggedIn()) return;
    const path = document.getElementById("csv-path").value;
    if (!path) { showToast("请输入文件路径"); return; }

    try {
        await fetch(`${API_BASE}/students/import`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ path: path })
        });
        showToast("导入请求已发送");
        fetchAllStudents();
    } catch (e) { showToast("导入失败"); }
}

async function exportCSV() {
    if (!ensureLoggedIn()) return;
    const path = document.getElementById("csv-path").value;
    if (!path) { showToast("请输入导出路径"); return; }

    try {
        await fetch(`${API_BASE}/students/export`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ path: path })
        });
        showToast("导出请求已发送");
    } catch (e) { showToast("导出失败"); }
}

// === 8. 课程管理 ===
async function fetchAllCourses() {
    if (!ensureLoggedIn()) return;
    showCourseSpinner(true);
    try {
        const response = await fetch(`${API_BASE}/courses/get_all`);
        const result = await response.json();

        let list = [];
        if (result.data) {
            list = result.data;
        } else if (Array.isArray(result)) {
            list = result;
        }

        renderCourseTable(list);
    } catch (error) {
        showToast("获取课程失败", "error");
    } finally {
        showCourseSpinner(false);
    }
}

function renderCourseTable(courses) {
    const tbody = document.getElementById("course-table-body");
    const noData = document.getElementById("course-no-data-msg");
    tbody.innerHTML = "";

    if (!courses || courses.length === 0) {
        noData.classList.remove("hidden");
        return;
    }
    noData.classList.add("hidden");

    courses.forEach(raw => {
        const c = normalizeCourse(raw);
        const tr = document.createElement("tr");
        const scoresCount = getCourseScoreCount(c.student_scores);
        tr.innerHTML = `
            <td>${c.id}</td>
            <td>${c.course_name}</td>
            <td>${c.teacher_name}</td>
            <td>${c.value ?? '-'}</td>
            <td>${scoresCount}</td>
            <td>${c.updatedBy || '-'}</td>
            <td>
                <button class="btn btn-small btn-info" onclick="prepareCourseEdit('${c.id}', '${c.course_name}', '${c.teacher_name}', '${c.value}')">编辑</button>
                <button class="btn btn-small btn-secondary" onclick="showCourseDetail('${c.id}')">详细</button>
                <button class="btn btn-small btn-danger" onclick="deleteCourse('${c.id}')">删除</button>
            </td>
        `;
        tbody.appendChild(tr);
    });
}

function normalizeCourse(course) {
    const scores = normalizeScores(
        course.student_scores ?? course.scores ?? course.studentScores ?? course.score_list
    );
    return {
        id: course.id ?? course.course_id ?? course.courseId ?? "",
        course_name: course.course_name ?? course.courseName ?? "",
        teacher_name: course.teacher_name ?? course.teacherName ?? "",
        value: course.value ?? course.credit ?? "-",
        student_scores: scores,
        updatedBy: course.updatedBy ?? course.updated_by ?? course.updatedby ?? "-"
    };
}

function normalizeScores(rawScores) {
    if (!rawScores) return [];
    if (Array.isArray(rawScores)) return rawScores;
    if (Array.isArray(rawScores.data)) return rawScores.data;
    if (typeof rawScores === "object") return Object.values(rawScores);
    return [];
}

function getCourseScoreCount(scores) {
    if (!Array.isArray(scores)) return 0;
    if (scores.length === 0) return 0;
    const first = scores[0];
    if (first && typeof first.score === "number" && (first.student_id === "__count__" || first.student_id === "count")) {
        return Math.max(0, Math.floor(first.score));
    }
    return scores.length;
}

async function findCourse() {
    if (!ensureLoggedIn()) return;
    const id = document.getElementById("course-search-input").value.trim();
    if (!id) {
        fetchAllCourses();
        return;
    }

    try {
        const response = await fetch(`${API_BASE}/courses/get`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id: id })
        });
        const result = await response.json();

        let list = [];
        if (result && result.data) {
            if (Array.isArray(result.data)) {
                list = result.data;
            } else {
                list = [result.data];
            }
        } else if (result && result.id) {
            list = [result];
        }

        if (list.length > 0) {
            renderCourseTable(list);
        } else {
            renderCourseTable([]);
            showToast("未找到该课程");
        }
    } catch (error) {
        showToast("查找课程失败");
    }
}

function openCourseModal(mode, id = '', name = '', teacher = '', value = '') {
    const modal = document.getElementById("course-modal");
    const title = document.getElementById("course-modal-title");
    const formMode = document.getElementById("course-form-mode");
    const idInput = document.getElementById("course-form-id");

    modal.style.display = "block";
    formMode.value = mode;

    if (mode === 'add') {
        title.textContent = "添加课程";
        document.getElementById("course-form").reset();
        idInput.disabled = false;
    } else {
        title.textContent = "修改课程";
        idInput.value = id;
        idInput.disabled = true;
        document.getElementById("course-form-name").value = name;
        document.getElementById("course-form-teacher").value = teacher;
        document.getElementById("course-form-value").value = value;
    }
}

function closeCourseModal() {
    document.getElementById("course-modal").style.display = "none";
}

function prepareCourseEdit(id, name, teacher, value) {
    openCourseModal('edit', id, name, teacher, value);
}

async function handleCourseSubmit(event) {
    event.preventDefault();
    if (!ensureLoggedIn()) return;
    const mode = document.getElementById("course-form-mode").value;

    const courseData = {
        id: document.getElementById("course-form-id").value,
        course_name: document.getElementById("course-form-name").value,
        teacher_name: document.getElementById("course-form-teacher").value,
        value: parseFloat(document.getElementById("course-form-value").value),
        updatedBy: currentUser
    };

    let url = "";
    let method = "";

    if (mode === 'add') {
        url = `${API_BASE}/courses/register_except_scores`;
        method = "POST";
        courseData.student_scores = [];
    } else {
        url = `${API_BASE}/courses/modify_except_scores`;
        method = "PUT";
        courseData.student_scores = [];
    }

    try {
        const response = await fetch(url, {
            method: method,
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(courseData)
        });

        if (response.ok) {
            showToast(mode === 'add' ? "添加课程成功" : "修改课程成功");
            closeCourseModal();
            fetchAllCourses();
        } else {
            showToast("课程操作失败");
        }
    } catch (error) {
        showToast("课程请求失败");
    }
}

async function deleteCourse(id) {
    if (!ensureLoggedIn()) return;
    if (!confirm(`确定要删除课程 ID: ${id} 吗?`)) return;

    try {
        const response = await fetch(`${API_BASE}/courses/delete`, {
            method: "DELETE",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id: id })
        });

        if (response.ok) {
            showToast("删除课程成功");
            fetchAllCourses();
        } else {
            showToast("删除课程失败");
        }
    } catch (error) {
        showToast("课程删除请求失败");
    }
}

function toggleCourseCsvPanel() {
    if (!ensureLoggedIn()) return;
    const panel = document.getElementById("course-csv-panel");
    panel.classList.toggle("hidden");
}

async function importCourseScoresCSV() {
    if (!ensureLoggedIn()) return;
    const courseId = document.getElementById("course-csv-id").value.trim();
    const path = document.getElementById("course-csv-path").value.trim();
    if (!courseId || !path) { showToast("请输入课程ID和CSV路径"); return; }

    try {
        const response = await fetch(`${API_BASE}/courses/load_scores_from_csv`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ course_id: courseId, filename: path })
        });
        await handleCourseScoreResponse(response, "导入");
    } catch (error) {
        showToast("导入成绩失败");
    }
}

async function exportCourseScoresCSV() {
    if (!ensureLoggedIn()) return;
    const courseId = document.getElementById("course-csv-id").value.trim();
    const path = document.getElementById("course-csv-path").value.trim();
    if (!courseId || !path) { showToast("请输入课程ID和导出路径"); return; }

    try {
        const response = await fetch(`${API_BASE}/courses/export_scores_to_csv`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ course_id: courseId, filename: path })
        });
        await handleCourseScoreResponse(response, "导出");
    } catch (error) {
        showToast("导出成绩失败");
    }
}

async function handleCourseScoreResponse(response, actionLabel) {
    let message = "";
    try {
        const data = await response.clone().json();
        message = data.message || data.msg || "";
    } catch (_) {
        message = "";
    }

    if (response.ok) {
        showToast(message || `${actionLabel}成绩成功`);
        fetchAllCourses();
    } else {
        showToast(message || `${actionLabel}成绩失败`);
    }
}

// === 9. 用户管理 ===
async function handleLoginPage(event) {
    event.preventDefault();
    const username = document.getElementById("login-username").value.trim();
    const password = document.getElementById("login-password").value.trim();
    const remember = document.getElementById("login-remember")?.checked;

    try {
        const response = await fetch(`${API_BASE}/users/login`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username, password })
        });

        if (response.ok) {
            currentUser = username;
            sessionStorage.removeItem("currentUser");
            localStorage.removeItem("currentUser");
            if (remember) {
                localStorage.setItem("currentUser", username);
            } else {
                sessionStorage.setItem("currentUser", username);
            }
            window.location.href = "index.html";
        } else {
            safeShowToast("登录失败");
        }
    } catch (error) {
        safeShowToast("登录请求失败");
    }
}

function handleLogout() {
    sessionStorage.removeItem("currentUser");
    localStorage.removeItem("currentUser");
    currentUser = null;
    window.location.href = "login.html";
}

async function handleRegisterPage(event) {
    event.preventDefault();
    const username = document.getElementById("register-username").value.trim();
    const password = document.getElementById("register-password").value.trim();
    const adminPassword = document.getElementById("register-admin-password").value.trim();

    try {
        const response = await fetch(`${API_BASE}/users/register`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username, password, adminPassword })
        });

        if (response.ok) {
            safeShowToast("注册成功，可直接登录");
            document.getElementById("register-form").reset();
            window.location.href = "login.html";
        } else {
            safeShowToast("注册失败");
        }
    } catch (error) {
        safeShowToast("注册请求失败");
    }
}

async function handleChangePassword(event) {
    event.preventDefault();
    if (!ensureLoggedIn()) return;
    const username = document.getElementById("change-username").value.trim();
    const password = document.getElementById("change-password").value.trim();
    const adminPassword = document.getElementById("change-admin-password").value.trim();

    try {
        const response = await fetch(`${API_BASE}/users/change_password`, {
            method: "PUT",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username, password, adminPassword })
        });

        if (response.ok) {
            showToast("密码修改成功");
            document.getElementById("change-password-form").reset();
        } else {
            showToast("密码修改失败");
        }
    } catch (error) {
        showToast("密码修改请求失败");
    }
}

function updateUserChip() {
    const chip = document.getElementById("user-chip-name");
    if (chip) {
        chip.textContent = currentUser ? currentUser : "未登录";
    }
}

// === 工具函数 ===
function showSpinner(show) {
    const spinner = document.getElementById("loading-spinner");
    if (show) spinner.classList.remove("hidden");
    else spinner.classList.add("hidden");
}

function showCourseSpinner(show) {
    const spinner = document.getElementById("course-loading-spinner");
    if (show) spinner.classList.remove("hidden");
    else spinner.classList.add("hidden");
}

function showToast(message) {
    const toast = document.getElementById("toast");
    toast.textContent = message;
    toast.className = "toast show";
    setTimeout(() => { toast.className = toast.className.replace("show", ""); }, 3000);
}

// 点击模态框外部关闭
window.onclick = function(event) {
    const studentModal = document.getElementById("student-modal");
    const courseModal = document.getElementById("course-modal");
    const detailModal = document.getElementById("student-detail-modal");
    const courseDetailModal = document.getElementById("course-detail-modal");
    if (event.target == studentModal) closeStudentModal();
    if (event.target == courseModal) closeCourseModal();
    if (event.target == detailModal) closeStudentDetailModal();
    if (event.target == courseDetailModal) closeCourseDetailModal();
}

// === 课程详情相关 ===
function ensureCourseDetailModal() {
    let modal = document.getElementById("course-detail-modal");
    if (modal) return modal;

    modal = document.createElement("div");
    modal.id = "course-detail-modal";
    modal.className = "modal";
    modal.innerHTML = `
        <div class="modal-content wide">
            <span class="close" onclick="closeCourseDetailModal()">&times;</span>
            <h2>课程详细信息</h2>
            <div id="course-detail-info" class="detail-info"></div>
            <h3>成绩明细</h3>
            <div id="course-detail-scores" class="detail-scores"></div>
        </div>
    `;
    document.body.appendChild(modal);
    return modal;
}

async function showCourseDetail(id) {
    if (!ensureLoggedIn()) return;

    try {
        const response = await fetch(`${API_BASE}/courses/get`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id })
        });
        const result = await response.json();
        const course = normalizeCourse(result?.data ?? result);

        if (!course || !course.id) {
            showToast("未找到该课程");
            return;
        }

        ensureCourseDetailModal();

        const info = document.getElementById("course-detail-info");
        info.innerHTML = `
            <div>课程ID：${course.id ?? "-"}</div>
            <div>课程名：${course.course_name ?? "-"}</div>
            <div>教师：${course.teacher_name ?? "-"}</div>
            <div>学分：${course.value ?? "-"}</div>
        `;

        const scores = normalizeScores(course.student_scores);
        const detail = document.getElementById("course-detail-scores");
        if (!scores || scores.length === 0) {
            detail.innerHTML = `<div>暂无成绩</div>`;
        } else {
            detail.innerHTML = `
                <table class="detail-table">
                    <thead>
                        <tr>
                            <th>学生ID</th>
                            <th>成绩</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${scores.map(s => `
                            <tr>
                                <td>${s.student_id ?? s.id ?? "-"}</td>
                                <td>${s.score ?? "-"}</td>
                            </tr>
                        `).join("")}
                    </tbody>
                </table>
            `;
        }

        document.getElementById("course-detail-modal").style.display = "block";
    } catch (error) {
        showToast("获取课程详情失败");
    }
}

function closeCourseDetailModal() {
    const modal = document.getElementById("course-detail-modal");
    if (modal) modal.style.display = "none";
}