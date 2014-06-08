#include <boost/gom/global_object_manager.hpp>

#include <boost/shared_ptr.hpp>

#include <iostream>

class DependeeGlobalObject
{
public:
    static inline int initializationOrder()
    {
        return boost::gom::GlobalObjectManager::initializationOrder() + 
               boost::gom::GlobalObjectManager::initializationOrderStep();
    }

    void touch()
    {
    }
};

typedef DependeeGlobalObject* DependeeGlobalObjectRPtr;
DependeeGlobalObjectRPtr gDependeeObject = boost::gom::GlobalObjectManager::rawPointer(gDependeeObject);

class DependerGlobalObject
{
public:
    DependerGlobalObject()
    {
        // If the global object manager works correctly - when we get here
        // the pointer must be already initialized
        gDependeeObject->touch();
    }
    
    ~DependerGlobalObject()
    {
        // If the global object manager works correctly - when we get here
        // the pointer will be still initialized
        gDependeeObject->touch();
    }

    static inline int initializationOrder()
    {
        return DependeeGlobalObject::initializationOrder() + 
               boost::gom::GlobalObjectManager::initializationOrderStep();
    }
};

typedef DependerGlobalObject* DependerGlobalObjectRPtr;
DependerGlobalObjectRPtr gDependerObject = boost::gom::GlobalObjectManager::rawPointer(gDependerObject);

int main()
{
    boost::gom::GlobalObjectManager gom;

    return 0;
}