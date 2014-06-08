#include "shared_lib.h"

#include <boost/gom/global_object_manager.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/detail/lightweight_test.hpp>

class ExeGlobalObject
{
public:
    static inline int initializationOrder()
    {
        return boost::gom::GlobalObjectManager::initializationOrder() + 
               boost::gom::GlobalObjectManager::initializationOrderStep();
    }
};

typedef ExeGlobalObject* ExeGlobalObjectRPtr;
ExeGlobalObjectRPtr gExeObject = boost::gom::GlobalObjectManager::rawPointer(gExeObject);

int main()
{
    boost::gom::GlobalObjectManager gom;

    BOOST_TEST(gExeObject != NULL);
    BOOST_TEST(gDllObject != NULL);

    return boost::report_errors();
}