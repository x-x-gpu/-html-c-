#include "Crypto.hpp"
#include "User.hpp" 

using std::string ;

User :: User() 
    : hashedUsername(""), hashedPassword("")
{
}

User :: ~ User () 
{
}

User :: User( const string& username, const string& password )
    : hashedUsername( hashstr(username) ), hashedPassword( hashstr(password) )
{
}

string User :: getUsername() const 
{
    return hashedUsername ;
}

string User :: getHashedPassword() const 
{
    return hashedPassword ;
}

bool User :: checkPassword( const string& pwd ) const 
{
    return hashedPassword == hashstr(pwd) ;
}

void User :: setUsername( const string& new_username ) 
{
    hashedUsername = hashstr(new_username) ;
}

void User :: setPassword( const string& new_password ) 
{
    hashedPassword = hashstr(new_password) ;
}

void User :: setHashedUser( const string& hashed_username, const string& hashed_password ) 
{
    hashedUsername = hashed_username ;
    hashedPassword = hashed_password ;
}
