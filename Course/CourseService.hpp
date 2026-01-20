#pragma once
#include "Course.hpp"
#include "CourseDAO.hpp"
#include "../Student/StudentService.hpp"
#include "../Result.hpp"
using std::string ;
using std::vector ;
using std::map ;
#include <fstream>

class CourseService
{
    private :

        CourseDAO *courseDAO;
        StudentService *studentService;

    public :

        CourseService () = delete ;
        CourseService ( CourseDAO *dao, StudentService *stuSvc ) ;
        ~ CourseService () ;   

        Res<void> UndoScores ( const string& id ) ;
        Res<void> ExchangeOrAddScores ( const string& id, const vector<student_score>& scores ) ;
        Res<void> RegisterCourse_ExceptScores( const Course& course ) ;
        Res<void> RemoveCourse( const string& id ) ;
        Res<void> ModifyCourse_ExceptScores( const Course& updatedCourse ) ;
        Res<Course*> FindCourseById( const string& id ) ;
        Res<Course> FindCourseById_Value( const string& id ) ;
        Res<void> LoadScoreFromCSV( const string& id, const string& filename ) ;
        Res<void> ExportScoreToCSV( const string& id, const string& filename ) const ;
        Res<vector<Course>> FindAllCourses() const ;
} ;

