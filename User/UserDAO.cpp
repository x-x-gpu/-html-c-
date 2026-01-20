#include "Crypto.hpp"
#include "User.hpp"
#include "UserDAO.hpp"
#include "../http_helper.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
using json = nlohmann::json ;
using std::string ;
using std::vector ;

UserDAO :: UserDAO () : filename(""), userStore() 
{
    userStore[hashstr("admin")] = hashstr("admin");
}

UserDAO :: UserDAO( const string& filename ) 
    : filename(filename) 
{   
    LoadFromFile (filename) ;
    userStore[hashstr("admin")] = hashstr("admin");
}

void UserDAO :: setFilename( const string& new_filename ) 
{
    filename = new_filename ;
}

string UserDAO :: getFilename() const 
{
    return this->filename ;
}

static std::string trim_copy(const std::string& s) {
    const char* ws = " \t\r\n";
    size_t start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

static vector<string> splitCSV(const string& line) {
    vector<string> tokens;
    string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, ',')) {
        tokens.push_back(trim_copy(token));
    }
    return tokens;
}

void UserDAO :: LoadFromFile( const string& filename )
{
    std::ifstream inFile(filename);
    if (!inFile) {
        throw std::runtime_error("无法打开文件");
    }

    json j_in;
    inFile >> j_in;

    auto dtoList = j_in.get<std::vector<UserAndPasswordDto>>();
    for (const auto& dto : dtoList) {
        userStore[dto.username] = dto.password;
    }
    inFile.close();
}

void UserDAO :: SaveToFile( const string& filename ) const
{
    std::ofstream file( filename ) ;
    if ( ! file.is_open() ) {
        return ;
    }
    std::vector<UserAndPasswordDto> dtoList;
    for ( const auto& pair : userStore ) {
        UserAndPasswordDto dto;
        dto.username = pair.first;
        dto.password = pair.second;
        dtoList.push_back(dto);
    }
    json j_out = dtoList;
    file << j_out.dump(4) ;
    file.close();
}

void UserDAO :: saveUser( const User& user ) 
{
    userStore[ user.getUsername() ] = user.getHashedPassword() ;
}

bool UserDAO :: userExists(const std::string& username) 
{
    std::string hashUser = hashstr(username);
    return userStore.find(hashUser) != userStore.end();
}

User UserDAO :: loadUser(const std::string& username) 
{
    std::string hashUser = hashstr(username);
    auto it = userStore.find(hashUser);
    User user;
    user.setHashedUser(it->first, it->second);
    return user;
}