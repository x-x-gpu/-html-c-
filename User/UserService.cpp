#include "Crypto.hpp"
#include "User.hpp"
#include "UserDAO.hpp"
#include "UserService.hpp"
#include "../Result.hpp"
#include <string>

UserService::UserService ( UserDAO* dao ) 
    : userDAO( dao ) 
{

}

bool UserService::authenticate ( const User& user, const std::string& password ) 
{
    return user.checkPassword( password ) ;
}

Res<void> UserService::addUser (const std::string& newUsername, const std::string& newPassword, const std::string& adminPassword) {
    if (!authenticate(User("admin", "admin"), adminPassword)) {
        return Res<void>::error("Admin authentication failed");
    }
    if (userDAO->userExists(newUsername)) {
        return Res<void>::error("User already exists");
    }
    User newUser(newUsername, newPassword);
    userDAO->saveUser(newUser);
    return Res<void>::success();
}

Res<void> UserService::ChangePassword (const std::string& username, const std::string& newPassword, const std::string& adminPassword) 
{
    if (!authenticate(User("admin", "admin"), adminPassword)) {
        return Res<void>::error("Admin authentication failed");
    }
    if (!userDAO->userExists(username)) {
        return Res<void>::error("User does not exist");
    }
    User user(username, newPassword); 
    userDAO->saveUser(user);
    return Res<void>::success();
}

Res<void> UserService::Login (const std::string& username, const std::string& password) {
    if (!userDAO->userExists(username)) {
        return Res<void>::error("User does not exist");
    }
    User user = userDAO->loadUser(username);
    if (!authenticate(user, password)) {
        return Res<void>::error("Authentication failed");
    }
    return Res<void>::success();
}