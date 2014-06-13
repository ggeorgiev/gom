#include <boost/gom/global_object_manager.hpp>

#include <boost/detail/lightweight_test.hpp>

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
    virtual int id()
    {
        return 1;
    }
};


int main()
{
    // mocking from main
    boost::gom::GlobalObjectManager::mockRawPointer<GlobalObject, GlobalObjectMock>(gObject);

    boost::gom::GlobalObjectManager gom;

    BOOST_TEST(gObject->id() == 1);

    return boost::report_errors();
}