#include "StudentDAO.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json ;
using std::swap ;

StudentDAO :: StudentDAO () 
    : students(), studentIndexMap(), filename("")
{
}

StudentDAO :: StudentDAO (const string& filename ) 
    : students(), studentIndexMap(), filename(filename)
{
    LoadFromFile( filename ) ;
}

void StudentDAO :: setFilename( const string& filename )  
{
    this->filename = filename ;
}

string StudentDAO :: getFilename() const 
{
    return this->filename ;
}

StudentDAO :: ~ StudentDAO () 
{
    if ( !this->filename.empty() )
        SaveToFile( this->filename ) ;
}

void StudentDAO :: LoadFromFile  ( const string& filename ) 
{
    std::ifstream inFile(filename);
    if (!inFile) {
        throw std::runtime_error("无法打开文件");
    }

    json j_in;
    inFile >> j_in;

    students = j_in.get<std::vector<Student>>();

    // Rebuild the index map
    studentIndexMap.clear();
    for (size_t i = 0; i < students.size(); ++i) {
        studentIndexMap[students[i].getId()] = i;
    }

    inFile.close();
}

void StudentDAO :: SaveToFile  ( const string& filename ) const
{
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("无法打开文件");
    }

    json j_out = students;
    outFile << j_out.dump(4);

    outFile.close();
}

void StudentDAO :: AddStudent  ( const Student& student ) 
{
    students.push_back( student ) ;
    studentIndexMap[ student.getId() ] = students.size() - 1 ;
}

// Typical swap-and-pop deletion for efficiency

void StudentDAO :: DeleteStudent  ( const string& id ) 
{
    swap (students[studentIndexMap[id]], students.back());
    studentIndexMap[students[studentIndexMap[id]].getId()] = studentIndexMap[id];
    students.pop_back();
    studentIndexMap.erase(id);
}

void StudentDAO :: UpdateStudent  ( const string& id, const Student& updatedStudent ) 
{
    auto it = studentIndexMap.find( id ) ;
    students[ it->second ] = updatedStudent ;
}

Student* StudentDAO :: GetStudentById  ( const string& id ) 
{
    auto it = studentIndexMap.find( id ) ;
    if ( it == studentIndexMap.end() ) {
        return nullptr ;
    }
    return &students[ it->second ] ;
}

vector<Student> StudentDAO :: GetAllStudents() const 
{
    return students ;
}