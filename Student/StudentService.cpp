#include "Student.hpp"
#include "StudentDAO.hpp"
#include "StudentService.hpp"
#include "../Result.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

StudentService :: StudentService ( StudentDAO *dao ) 
    : studentDAO( dao ) 
{   
    
}

StudentService :: ~ StudentService () 
{
    
}

Res<void> StudentService :: RegisterStudent( const Student& student ) 
{
    // check if student with same ID already exists
    if ( studentDAO->GetStudentById( student.getId() ) != nullptr ) {
        return Res<void>::error( "学生ID已存在，无法注册相同ID的学生。" ) ;
    }   
    studentDAO->AddStudent( student ) ;
    return Res<void>::success() ;
}

Res<void> StudentService :: RemoveStudent( const string& id ) 
{
    if ( studentDAO->GetStudentById( id ) == nullptr ) {
        return Res<void>::error( "未找到指定ID的学生，删除失败。" ) ;
    }
    studentDAO->DeleteStudent( id ) ;
    return Res<void>::success() ;
}

Res<void> StudentService :: ModifyStudent( const Student& updatedStudent ) 
{
    if ( studentDAO->GetStudentById( updatedStudent.getId() ) == nullptr ) {
        return Res<void>::error( "未找到指定ID的学生，修改失败。" ) ;
    }
    studentDAO->UpdateStudent( updatedStudent.getId(), updatedStudent ) ;
    return Res<void>::success() ;
}

Res<Student*> StudentService :: FindStudentById( const string& id ) 
{
    Student *student = studentDAO->GetStudentById( id ) ;
    if ( student == nullptr ) {
        return Res<Student*>::error( "未找到指定ID的学生。" ) ;
    }
    return Res<Student*>::success( student ) ;
}

Res<Student> StudentService :: FindStudentById_Value( const string& id ) 
{
    Student *student = studentDAO->GetStudentById( id ) ;
    if ( student == nullptr ) {
        return Res<Student>::error( "未找到指定ID的学生。" ) ;
    }
    return Res<Student>::success( *student ) ;
}

Res<vector<Student>> StudentService :: ListAllStudents() const 
{
    try {
        vector<Student> students = studentDAO->GetAllStudents() ;
        return Res<vector<Student>>::success( students ) ;
    } catch ( const std::exception& e ) {
        return Res<vector<Student>>::error( e.what() ) ;
    }
}

Res<vector<Student>> StudentService::ListStudentsByFunc(bool (*compareFunc)(const Student&, const Student&)) const 
{
    try {
        vector<Student> allStudents = studentDAO->GetAllStudents();
        std::sort(allStudents.begin(), allStudents.end(), compareFunc); 
        return Res<vector<Student>>::success(allStudents);
    } catch (const std::exception& e) {
        return Res<vector<Student>>::error(e.what());
    }
}

static std::string trim_copy(const std::string& s) {
    const char* ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

static vector<string> splitCSV(const string& line) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(line);
    while (std::getline(tokenStream, token, ',')) {
        tokens.push_back(trim_copy(token));
    }
    return tokens;
}

Res<void> StudentService::LoadStudentsFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return Res<void>::error("无法打开 CSV 文件，请检查路径是否正确。");
    }

    string line;
    int addCount = 0;
    int coverCount = 0;
    int skipCount = 0;

    // 检查一下表头
    if (!getline(file, line)) return Res<void>::error("CSV 文件为空。");

    while (getline(file, line)) {
        if (line.empty()) continue;
        
        vector<string> cols = splitCSV(line);
        if (cols.size() < 4) {
            skipCount++;
            continue;
        }

        // UpdateBy以后要跟User联动
        Student temp(cols[0], cols[1], cols[2], cols[3], "system");

        if (studentDAO->GetStudentById(temp.getId()) != nullptr) 
        {
            studentDAO->UpdateStudent( temp.getId(), temp ) ;
            coverCount++;
        } else {
            studentDAO->AddStudent(temp);
            addCount++;
        }
    }

    file.close();

    string resultMsg = "导入完成：加入 " + to_string(addCount) + " 条，数据不全 " + to_string(skipCount) + " 条，" + "覆盖 " + to_string(coverCount) + " 条。";
    return Res<void>::success(resultMsg);
}

Res<void> StudentService::ExportStudentsToCSV(const string& filename) const {
    // 校验文件名
    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".csv") {
        return Res<void>::error("导出文件必须以 .csv 结尾。");
    }

    ofstream file(filename);
    if (!file.is_open()) {
        return Res<void>::error("无法创建导出文件。");
    }

    // 写入 UTF-8 编码覆盖头，防止使用默认GBK编码的Excel打开时乱码
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    file.write((char*)bom, 3);

    file << "ID,姓名,年龄,班级\n";

    vector<Student> all = studentDAO->GetAllStudents();
    for (const auto& s : all) {
        file << s.getId() << ","
             << s.getName() << ","
             << s.getAge() << ","
             << s.getClassName() << "\n";
    }

    file.close();
    return Res<void>::success("数据已成功导出为 CSV 格式。");
}
