#include "CourseDAO.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json ;
#include <fstream>

using std::swap ;

CourseDAO::CourseDAO () : courses(), courseIndexMap(), filename("") 
{
}

CourseDAO::CourseDAO (const string& filename ) : courses(), courseIndexMap(), filename(filename) 
{
    LoadFromFile( filename ) ;
}

CourseDAO::~CourseDAO () 
{
    if ( !filename.empty() )
        SaveToFile( filename ) ;
}

void CourseDAO::setFilename( const string& filename )  
{
    this->filename = filename ;
}

string CourseDAO::getFilename() const 
{
    return this->filename ;
}

void CourseDAO::LoadFromFile  ( const string& filename ) 
{
    std::ifstream inFile(filename);
    if (!inFile) {
        throw std::runtime_error("无法打开文件");
    }

    json j_in;
    inFile >> j_in;

    courses = j_in.get<std::vector<Course>>();

    // Rebuild the index map
    courseIndexMap.clear();
    for (size_t i = 0; i < courses.size(); ++i) {
        courseIndexMap[courses[i].getId()] = i;
    }

    inFile.close();
}

void CourseDAO::SaveToFile  ( const string& filename ) const
{
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("无法打开文件");
    }

    json j_out = courses;
    outFile << j_out.dump(4);

    outFile.close();
}

void CourseDAO::AddCourse( const Course& course ) 
{
    courses.push_back(course);
    courseIndexMap[course.getId()] = courses.size() - 1;
}

// Typical swap-and-pop deletion for efficiency

void CourseDAO::DeleteCourse( const string& id ) 
{
    swap (courses[courseIndexMap[id]], courses.back());
    courseIndexMap[courses[courseIndexMap[id]].getId()] = courseIndexMap[id];
    courses.pop_back();
    courseIndexMap.erase(id);
}

void CourseDAO::UpdateCourse( const Course& course ) 
{
    auto it = courseIndexMap.find(course.getId());
    courses[it->second] = course;
}

void CourseDAO::UpdateCoursePartial( const Course& updatedFields ) 
{
    auto it = courseIndexMap.find(updatedFields.getId());
    Course& existingCourse = courses[it->second];

    existingCourse.setCourseName(updatedFields.getCourseName());
    existingCourse.setTeacherName(updatedFields.getTeacherName());
    existingCourse.setValue(updatedFields.getValue());
    if (!updatedFields.getUpdatedBy().empty()) {
        existingCourse.setUpdatedInfo(updatedFields.getUpdatedBy());
    }
}

Course* CourseDAO::GetCourseById( const string& id ) 
{
    auto it = courseIndexMap.find(id);
    if (it == courseIndexMap.end()) {
        return nullptr;
    }
    return &courses[it->second];
}

vector<Course>& CourseDAO::GetAllCourses()
{
    return courses;
}
