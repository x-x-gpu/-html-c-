#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <nlohmann/json.hpp>
using std::string;

class BaseEntity 
{
    protected :

        string id;
        time_t createdAt;
        time_t updatedAt;
        string updatedBy;

    public :

        static int objectCount;

        BaseEntity () ;
        BaseEntity ( const string& id, const string& updatedBy ) ;
        BaseEntity ( const BaseEntity& other ) ;
        BaseEntity& operator= ( const BaseEntity& other ) ;
        virtual ~ BaseEntity () ; // 这也算虚函数吧

        friend void printBaseEntity(const BaseEntity& entity);

        string getId() const ;
        time_t getCreatedAt() const ;
        time_t getUpdatedAt() const ;
        string getUpdatedBy() const ;
        void setUpdatedInfo( const string& updatedBy ) ;

        static int getObjectCount();
} ;