#include "CourseService.hpp"

CourseService::CourseService ( CourseDAO *dao , StudentService *stuSvc ) 
    : courseDAO( dao ), studentService( stuSvc ) 
{   
    
}

CourseService::~CourseService () {}

Res<void> CourseService::UndoScores ( const string& id )
{
    double courseValue = this->FindCourseById_Value( id ).data.getValue() ;
    auto scores = this->FindCourseById( id ).data->getStudentScores() ;
    for ( const auto& ss : scores ) {
        Student *student = studentService->FindStudentById( ss.student_id ).data ;

        if ( student == nullptr ) {
            return Res<void>::error( "未找到学生 " + ss.student_id + " ，撤销成绩失败。" ) ;
        }

        student->deleteScore( id ) ;
        student->calcGPA() ;
    }

    return Res<void>::success() ;
}

Res<void> CourseService::ExchangeOrAddScores ( const string& id, const vector<student_score>& scores )
{
    double courseValue = this->FindCourseById_Value( id ).data.getValue() ;
    for ( const auto& ss : scores ) {
        Student *student = studentService->FindStudentById( ss.student_id ).data ;

        if ( student == nullptr ) {
            return Res<void>::error( "未找到学生 " + ss.student_id + " ，更新成绩失败。" ) ;
        }

        score_of_student scoreEntry ;
        scoreEntry.course_id = id ;
        scoreEntry.score = ss.score ;
        scoreEntry.course_value = courseValue ;

        student->ExchangeOrAddScore( scoreEntry ) ;
        student->calcGPA() ;
    }

    return Res<void>::success() ;
}

Res<void> CourseService::RegisterCourse_ExceptScores( const Course& course )
{
    if ( courseDAO->GetCourseById( course.getId() ) != nullptr ) {
        return Res<void>::error( "课程ID已存在，无法注册相同ID的课程。" ) ;
    }
    courseDAO->AddCourse( course ) ;
    return Res<void>::success() ;
}

Res<void> CourseService::RemoveCourse( const string& id )
{
    if ( courseDAO->GetCourseById( id ) == nullptr ) {
        return Res<void>::error( "未找到指定ID的课程，删除失败。" ) ;
    }
    auto res = this->UndoScores( id ) ;
    courseDAO->DeleteCourse( id ) ;
    return Res<void>::success() ;
}

Res<void> CourseService::ModifyCourse_ExceptScores( const Course& updatedCourse )
{
    if ( courseDAO->GetCourseById( updatedCourse.getId() ) == nullptr ) {
        return Res<void>::error( "未找到指定ID的课程，修改失败。" ) ;
    }

    courseDAO->UpdateCoursePartial( updatedCourse ) ;
    return Res<void>::success() ;
}

Res<Course*> CourseService::FindCourseById( const string& id )
{
    Course *course = courseDAO->GetCourseById( id ) ;
    if ( course == nullptr ) {
        return Res<Course*>::error( "未找到指定ID的课程。" ) ;
    }
    return Res<Course*>::success( course ) ;
}

Res<Course> CourseService::FindCourseById_Value( const string& id )
{
    Course *coursePtr = courseDAO->GetCourseById( id ) ;
    if ( coursePtr == nullptr ) {
        return Res<Course>::error( "未找到指定ID的课程。" ) ;
    }
    return Res<Course>::success( *coursePtr ) ;
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

Res<void> CourseService::LoadScoreFromCSV( const string& id, const string& filename )
{
    Course *course = courseDAO->GetCourseById( id ) ;
    if ( course == nullptr ) {
        return Res<void>::error( "未找到指定ID的课程，加载成绩失败。" ) ;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        return Res<void>::error("无法打开 CSV 文件，请检查路径是否正确。");
    }

    std::string line;

    // 读取表头
    if (!std::getline(file, line)) {
        return Res<void>::error("CSV 文件为空。");
    }

    // 初始化新的成绩向量，直接覆盖原有数据
    std::vector<student_score> scores;
    scores.clear();

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        auto cols = splitCSV(line);
        if (cols.size() < 2) {
            return Res<void>::error("CSV 文件格式错误，缺少必要的列。");
        }

        const std::string stuId = cols[0];
        const std::string scoreStr = cols[1];
        if (stuId.empty() || scoreStr.empty()) {
            return Res<void>::error("CSV 文件格式错误，学号或分数为空。");
        }

        double score = 0.0;
        try {
            score = std::stod(scoreStr);
        } catch (...) {
            return Res<void>::error("CSV 文件格式错误，分数不是有效的数字。");
        }

        student_score ss;
        ss.student_id = stuId;
        ss.score = score;
        scores.push_back(ss);
    }

    std::sort (scores.begin(), scores.end(), []( const student_score& a, const student_score& b ) {
        return a.score < b.score ;
    } ) ;

    file.close();
    course->setStudentScores(scores);
    this->ExchangeOrAddScores( id, scores ) ;
    return Res<void>::success();
}

Res<void> CourseService::ExportScoreToCSV( const string& id, const string& filename ) const
{
    Course *course = courseDAO->GetCourseById( id ) ;
    if ( course == nullptr ) {
        return Res<void>::error( "未找到指定ID的课程，导出成绩失败。" ) ;
    }

    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".csv") {
        return Res<void>::error("导出文件必须以 .csv 结尾。");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        return Res<void>::error("无法创建导出文件。");
    }

    // UTF-8 BOM
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    file.write(reinterpret_cast<char*>(bom), 3);

    file << "学号,分数\n";
    auto scores = course->getStudentScores();
    for (const auto& ss : scores) {
        file << ss.student_id << "," << ss.score << "\n";
    }

    file.close();
    return Res<void>::success("成绩已成功导出为 CSV 格式。");
}

Res<vector<Course>> CourseService::FindAllCourses() const
{
    vector<Course> courses = courseDAO->GetAllCourses() ;
    auto coursesCopy = vector<Course>( courses.size() ) ;
    for ( size_t i = 0 ; i < courses.size() ; i++ ) {
        coursesCopy[i] = Course( courses[i].getId(), courses[i].getUpdatedBy() ) ;
        coursesCopy[i].setCourseName( courses[i].getCourseName() ) ;
        coursesCopy[i].setValue( courses[i].getValue() ) ;
        coursesCopy[i].setTeacherName( courses[i].getTeacherName() ) ;

        student_score countEntry;
        countEntry.student_id = "__count__";
        countEntry.score = static_cast<double>( courses[i].getStudentScores().size() );
        coursesCopy[i].setStudentScores( std::vector<student_score>{ countEntry } );
    }
    return Res<vector<Course>>::success( coursesCopy ) ;
}