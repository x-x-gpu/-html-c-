#include "BaseEntity.hpp"

int BaseEntity::objectCount = 0;

void printBaseEntity(const BaseEntity& entity) {
    std::cout << "ID: " << entity.id << ", UpdatedBy: " << entity.updatedBy << std::endl;
}

BaseEntity::BaseEntity() : id(""), createdAt(0), updatedAt(0), updatedBy("") 
{
    objectCount++;
}
    
BaseEntity::BaseEntity ( const string& id, const string& updatedBy ) : id(id), updatedBy(updatedBy) 
{
    createdAt = time( nullptr ) ;
    updatedAt = createdAt ;
    objectCount++;
}

BaseEntity::BaseEntity ( const BaseEntity& other ) : id(other.id), createdAt(other.createdAt), updatedAt(other.updatedAt), updatedBy(other.updatedBy) 
{
    objectCount++;
}

BaseEntity& BaseEntity::operator= ( const BaseEntity& other ) 
{
    if (this != &other) 
    {
        id = other.id;
        createdAt = other.createdAt;
        updatedAt = other.updatedAt;
        updatedBy = other.updatedBy;
    }
    return *this;
}

BaseEntity::~BaseEntity() 
{
    objectCount--;
}

string BaseEntity :: getId() const 
{
    return id ;
}

time_t BaseEntity :: getCreatedAt() const 
{
    return createdAt ;
}

time_t BaseEntity :: getUpdatedAt() const 
{
    return updatedAt ;
}

string BaseEntity :: getUpdatedBy() const 
{
    return updatedBy ;
}

void BaseEntity :: setUpdatedInfo( const string& updatedBy ) 
{
    this->updatedBy = updatedBy ;
    this->updatedAt = time( nullptr ) ;
}

int BaseEntity::getObjectCount() {
    return objectCount;
}