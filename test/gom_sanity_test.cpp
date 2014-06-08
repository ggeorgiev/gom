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
};

typedef GlobalObject* GlobalObjectRPtr;
GlobalObjectRPtr gObjectRPtr = boost::gom::GlobalObjectManager::rawPointer(gObjectRPtr);

typedef boost::shared_ptr<GlobalObject> GlobalObjectSPtr;
GlobalObjectSPtr gObjectSPtr = boost::gom::GlobalObjectManager::sharedPointer(gObjectSPtr);

int main()
{
    BOOST_TEST(gObjectRPtr == NULL);
    BOOST_TEST(gObjectSPtr == NULL);

    {
        boost::gom::GlobalObjectManager gom;

        BOOST_TEST(gObjectRPtr != NULL);
        BOOST_TEST(gObjectSPtr != NULL);
    }

    BOOST_TEST(gObjectRPtr == NULL);
    BOOST_TEST(gObjectSPtr == NULL);

    return boost::report_errors();
}