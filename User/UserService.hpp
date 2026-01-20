#pragma once
#include "User.hpp"
#include "UserDAO.hpp"
#include "../Result.hpp"
#include <string>

class UserService 
{
    private:
        UserDAO *userDAO;

    public :
        UserService () = delete ;
        UserService ( UserDAO* dao ) ;
        ~ UserService () = default ;
        bool authenticate( const User& user, const std::string& password ) ;
        Res<void> addUser(const std::string& newUsername, const std::string& newPassword, const std::string& adminPassword) ;
        Res<void> ChangePassword(const std::string& username, const std::string& newPassword, const std::string& adminPassword) ;
        Res<void> Login (const std::string& username, const std::string& password) ;
} ;