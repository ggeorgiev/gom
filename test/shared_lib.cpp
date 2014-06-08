#include "shared_lib.h"

DLL_API DllGlobalObjectRPtr gDllObject = boost::gom::GlobalObjectManager::rawPointer(gDllObject);

boost::gom::GlobalObjectManagerSPtr gGlobalObjectManager;

#include "windows.h"

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            gGlobalObjectManager = boost::make_shared<boost::gom::GlobalObjectManager>();
            break;

        case DLL_THREAD_ATTACH:
         // Do thread-specific initialization.
            break;

        case DLL_THREAD_DETACH:
         // Do thread-specific cleanup.
            break;

        case DLL_PROCESS_DETACH:
            gGlobalObjectManager.reset();
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}