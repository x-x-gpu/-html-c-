#include "Student.hpp"

Student :: Student () 
    : BaseEntity ( "", "" ), name(""), age(""), className(""), gpa(0.0), scores()
{
}

Student :: Student ( const string& id, const string& name, const string& age, const string& className, const string& updatedBy ) 
    : BaseEntity ( id, updatedBy ), name(name), age(age), className(className), gpa(0.0), scores()
{
}

string Student :: getName() const 
{
    return name ;
}

string Student :: getAge() const 
{
    return age ;
}

string Student :: getClassName() const 
{
    return className ;
}

void Student :: deleteScore( const string& course_id ) 
{
    for ( size_t i = 0 ; i < scores.size() ; i++ ) {
        if ( scores[i].course_id == course_id ) {
            scores.erase( scores.begin() + i ) ;
            return ;
        }
    }
}

void Student :: ExchangeOrAddScore( const score_of_student& score ) 
{
    for ( size_t i = 0 ; i < scores.size() ; i++ ) {
        if ( scores[i].course_id == score.course_id ) {
            scores[i] = score ;
            return ;
        }
    }
    scores.push_back( score ) ;
}

double Student :: getGPA() const 
{
    return gpa ;
}

void Student :: setName( const string& name ) 
{
    this->name = name ;
}

void Student :: setAge( const string& age ) 
{
    this->age = age ;
}

void Student :: setClassName( const string& className ) 
{
    this->className = className ;
}

void Student :: setScores( const vector<score_of_student>& scores ) 
{
    this->scores = scores ;
}

void Student :: calcGPA() 
{
    double totalScore = 0.0 ;
    double totalCredits = 0.0 ;

    for ( const auto& sc : scores ) {
        double deal_score = (std::max (sc.score - 60.0, 0.0)) / 10.0 + 1.0;
        totalScore += deal_score * sc.course_value ;
        totalCredits += sc.course_value ;
    }

    this->gpa = totalScore / totalCredits ;
}

void Student :: setGPA( double gpa ) 
{
    this->gpa = gpa ;
}