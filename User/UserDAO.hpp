#pragma once
#include "User.hpp"
#include <map>
#include <string>
#include <stdexcept>

class UserDAO 
{
    private:
        string filename;
        std::map<std::string, std::string> userStore; // hashUsername -> hashedPassword

    public :

        UserDAO () ;
        UserDAO( const string& filename ) ;
        void setFilename( const string& new_filename ) ;
        string getFilename() const ;
        void LoadFromFile(const string& filename) ;
        void SaveToFile(const string& filename) const ;
        void saveUser( const User& user ) ;
        bool userExists(const std::string& username) ;
        User loadUser(const std::string& username) ;
} ;