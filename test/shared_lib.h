#include <boost/gom/global_object_manager.hpp>

#include <boost/shared_ptr.hpp>

#ifdef EXPORTING
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API DllGlobalObject
{
public:
    void touch()
    {
    }

    static inline int initializationOrder()
    {
        return boost::gom::GlobalObjectManager::initializationOrder() + 
               boost::gom::GlobalObjectManager::initializationOrderStep();
    }
};

typedef DllGlobalObject* DllGlobalObjectRPtr;
extern DLL_API DllGlobalObjectRPtr gDllObject;