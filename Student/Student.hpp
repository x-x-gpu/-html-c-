#pragma once
#include "../BaseEntity.hpp"
#include <string>
#include <nlohmann/json.hpp>
using std::string ;
using std::vector ;

class score_of_student
{
    public :

        string course_id;
        double course_value; // 学分
        double score;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(score_of_student, course_id, course_value, score)
};

class Student : virtual public BaseEntity 
{
    private :

        string name;
        string age;
        string className;

        vector <score_of_student> scores;
        double gpa;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Student, id, updatedBy, name, age, className, scores, gpa)

    public :
        
        Student () ;
        Student ( const string& id, const string& name, const string& age, const string& className, const string& updatedBy ) ;
        string getName() const ;
        string getAge() const ;
        string getClassName() const ;
        double getGPA() const ;
        void setName( const string& name ) ;
        void setAge( const string& age ) ;
        void setClassName( const string& className ) ;
        void setScores( const vector<score_of_student>& scores ) ;
        void deleteScore( const string& course_id ) ;
        void ExchangeOrAddScore( const score_of_student& score ) ;
        void calcGPA() ;
        void setGPA( double gpa ) ;
} ;