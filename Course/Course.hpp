#pragma once
#include "../BaseEntity.hpp"
#include <string>
#include <nlohmann/json.hpp>
#include <vector>
using std::string ;
using std::vector ;

class student_score 
{
    public :

        string student_id;
        double score;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(student_score, student_id, score)
} ;

class Course : virtual public BaseEntity
{
    private :

        string course_name;
        string teacher_name;
        double value; // 学分
        std::vector<student_score> student_scores;

    public :

        Course () ;
        Course ( const string& id, const string& updatedBy ) ;
        string getCourseName() const ;
        double getValue() const ;   
        string getTeacherName() const ;
        const std::vector<student_score>& getStudentScores() const ;
        void setCourseName( const string& course_name ) ;
        void setValue( double value ) ;
        void setTeacherName( const string& teacher_name ) ;
        void setStudentScores( const std::vector<student_score>& student_scores ) ;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Course, id, updatedBy, course_name, value, teacher_name, student_scores)
} ;