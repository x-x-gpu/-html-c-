#pragma once
#include "Student.hpp"
#include <vector>
#include <map>
using std::vector ;
using std::map ;
using std::string ;

// Cache

class StudentDAO
{
    private :

        vector<Student> students ;
        map <string, size_t> studentIndexMap ; // Quickly find student index by ID
        string filename ;

    public :

        StudentDAO () ;
        StudentDAO (const string& filename ) ;
        ~ StudentDAO () ;
        void setFilename( const string& filename )  ;
        string getFilename() const ;
        void LoadFromFile( const string& filename ) ;
        void SaveToFile( const string& filename ) const ;
        void AddStudent( const Student& student ) ;
        void DeleteStudent( const string& id ) ;
        void UpdateStudent( const string& id, const Student& updatedStudent ) ;
        Student* GetStudentById( const string& id ) ;
        vector<Student> GetAllStudents() const ;
} ;
