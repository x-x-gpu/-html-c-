#pragma once
#include "Course.hpp"
#include <vector>
#include <map>
using std::vector ;
using std::map ;
using std::string ;

// Cache

class CourseDAO
{
    private :

        vector<Course> courses ;
        map <string, size_t> courseIndexMap ; // Quickly find course index by ID
        string filename ;

    public :

        CourseDAO () ;
        CourseDAO (const string& filename ) ;
        ~ CourseDAO () ;
        void setFilename( const string& filename )  ;
        string getFilename() const ;
        void LoadFromFile  ( const string& filename ) ;
        void SaveToFile  ( const string& filename ) const ;
        void AddCourse( const Course& course ) ;
        void DeleteCourse( const string& id ) ;
        void UpdateCourse( const Course& course ) ;
        void UpdateCoursePartial( const Course& updatedFields ) ; // 这个函数不更改scores，其他字段全部覆盖
        Course* GetCourseById( const string& id ) ;
        vector<Course>& GetAllCourses() ;
} ;
