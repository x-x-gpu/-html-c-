#pragma once
#include <string>
#include "Crypto.hpp"
using std::string ;

class User
{
    private :
        std::string hashedUsername ;
        std::string hashedPassword ;

    public :
        User () ;
        User ( const string& username, const string& password ) ;  
        ~ User () ;
        std::string getUsername() const ;
        std::string getHashedPassword() const ;
        bool checkPassword( const std::string& pwd ) const;
        void setUsername( const std::string& new_username ) ;
        void setPassword( const std::string& new_password ) ;
        void setHashedUser( const std::string& hashed_username, const std::string& hashed_password ) ;
} ;