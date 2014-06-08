#include <boost/gom/global_object_manager.hpp>

#include <boost/shared_ptr.hpp>

#include <iostream>

class GlobalObject
{
public:
    static inline int initializationOrder()
    {
        return boost::gom::GlobalObjectManager::initializationOrder() + 
               boost::gom::GlobalObjectManager::initializationOrderStep();
    }

    virtual int id()
    {
        return 0;
    }
};

typedef GlobalObject* GlobalObjectRPtr;
GlobalObjectRPtr gObject = boost::gom::GlobalObjectManager::rawPointer(gObject);

class GlobalObjectMock : public GlobalObject
{
public:
    static inline int initializationOrder()
    {
        return GlobalObject::initializationOrder() - 1;
    }

    static void initilize()
    {
        gObject = new GlobalObjectMock();
    }

    virtual int id()
    {
        return 1;
    }
};


int main()
{
    // mocking from main
    boost::gom::GlobalObjectManager::genericRegister(GlobalObjectMock::initializationOrder(), &GlobalObjectMock::initilize, NULL);

    boost::gom::GlobalObjectManager gom;

    return 0;
}