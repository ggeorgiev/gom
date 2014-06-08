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
};

typedef GlobalObject* GlobalObjectRPtr;
GlobalObjectRPtr gObjectRPtr = boost::gom::GlobalObjectManager::rawPointer(gObjectRPtr);

typedef boost::shared_ptr<GlobalObject> GlobalObjectSPtr;
GlobalObjectSPtr gObjectSPtr = boost::gom::GlobalObjectManager::sharedPointer(gObjectSPtr);

int main()
{
    if (gObjectRPtr != NULL)
        return 1;
    if (gObjectSPtr != NULL)
        return 1;

    {
        boost::gom::GlobalObjectManager gom;

        if (gObjectRPtr == NULL)
            return 1;
        if (gObjectSPtr == NULL)
            return 1;
    }

    if (gObjectRPtr != NULL)
        return 1;
    if (gObjectSPtr != NULL)
        return 1;

    return 0;
}