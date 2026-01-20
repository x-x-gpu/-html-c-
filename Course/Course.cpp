#include "Course.hpp"

Course::Course () : BaseEntity() , course_name(""), value(0.0), teacher_name(""), student_scores() {}

Course::Course ( const string& id, const string& updatedBy ) : BaseEntity( id, updatedBy ) , course_name(""), value(0.0), teacher_name(""), student_scores() {}

string Course::getCourseName() const 
{
    return course_name ;
}

double Course::getValue() const 
{
    return value ;
}

string Course::getTeacherName() const 
{
    return teacher_name ;
}

const std::vector<student_score>& Course::getStudentScores() const
{
    return student_scores ;
}

void Course::setCourseName( const string& course_name ) 
{
    this->course_name = course_name ;
}

void Course::setValue( double value ) 
{
    this->value = value ;
}

void Course::setTeacherName( const string& teacher_name ) 
{
    this->teacher_name = teacher_name ;
}

void Course::setStudentScores( const std::vector<student_score>& student_scores ) 
{
    this->student_scores = student_scores ;
}

