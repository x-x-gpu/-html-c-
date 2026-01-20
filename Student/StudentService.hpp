#pragma once
#include "Student.hpp"
#include "StudentDAO.hpp"
#include "../Result.hpp"

class StudentService
{
    private :

        StudentDAO *studentDAO;

    public :

        StudentService () = delete ;
        StudentService ( StudentDAO *dao ) ;
        ~ StudentService () ;   

        Res<void> LoadStudentsFromCSV( const string& filename ) ;
        Res<void> ExportStudentsToCSV( const string& filename ) const ;
        Res<void> RegisterStudent( const Student& student ) ;
        Res<void> RemoveStudent( const string& id ) ;
        Res<void> ModifyStudent( const Student& updatedStudent ) ;
        Res<Student*> FindStudentById( const string& id ) ;
        Res<Student> FindStudentById_Value( const string& id ) ;
        Res<vector<Student>> ListAllStudents() const ;
        Res<vector<Student>> ListStudentsByFunc(bool (*compareFunc)(const Student&, const Student&)) const ;
        // Res<vector<Student>> ListTopkStudents ( size_t k ) const ;
} ;