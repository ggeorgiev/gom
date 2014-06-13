///////////////////////////////////////////////////////////////////////////////
// global_object_manager.hpp header file
//
// Copyright 2014 George Georgiev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// See http://www.boost.org/libs/gom for documentation
//
// Credits:
//  George Georgiev - for the initial inspiration and implementation

#ifndef BOOST_GLOBALOBJECTMANAGER
#define BOOST_GLOBALOBJECTMANAGER

// MS compatible compilers support #pragma once
#if defined(_MSC_VER)
# pragma once
#endif

#include "boost/function.hpp"
#include "boost/bind.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <map>
#include <vector>

#include <iostream>

namespace boost { namespace gom {

class GlobalObjectManager
{
public:
    typedef boost::function<bool(void)> InitializeFunction;
    typedef boost::function<bool(int, const char**)> ArgInitializeFunction;
    typedef boost::function<void(void)> UninitializeFunction;

    struct InitializeRecord
    {
        InitializeFunction    voidInitializeFn;
        ArgInitializeFunction argInitializeFn;

        UninitializeFunction  uninitializeFn;
    };


    GlobalObjectManager()
    {
        callTheInitializers(0, NULL);
    }

    GlobalObjectManager(int argc, const char **argv)
    {
        callTheInitializers(argc, argv);
    }

    ~GlobalObjectManager()
    {
        callTheUninitializers();
    }

    // Every global object should use initializationOrder + 1 at least
    // The recommended step is 2. This way mock object could be provided
    // with initializationOrder() - 1 value

    // It is function instead of static const value, because gcc compiler (probably others)
    // static const values are also global objects. Using one static value to initialize
    // another is not reliable, because the initialization order could be wrong and variable
    // could be used before to be initialized with a random effect over the end result
    static inline int initializationOrder()
    {
        return 1;
    }

    // The recommended way the order value for a particular object to be determined is:
    // the max of all global objects it depends on + initializationOrderStep().
    // The initializationOrderStep is 2 instead the usual suspect 1, because this will
    // give possibility for test frameworks to register mock objects with initializationOrder - 1.
    // This way without possibility of changing the initialization order it will
    // provide a mock object instead the original one.
    static inline int initializationOrderStep()
    {
        return 2;
    }


    // Generic registration of global object method. It requires initializationOrder value
    // every object with lower initializationOrder value will be initialized before
    // all objects with high initializationOrder. If two or more objects have the same
    // initializationOrder they will be initialized in a "random" order. The initializer method
    // pointer is required. Uninitializer method pointer is optional. The objects will
    // be uninitialized in reversed order. There is guarantee even for objects with the same
    // initializationOrder will be uninitialized in exact reversed initialize order.
    static bool genericRegister(int initializationOrder,
                                InitializeFunction initializer,
                                UninitializeFunction uninitializer)
    {
        add(initializationOrder, initializer, NULL, uninitializer);
        return true;
    }

    static bool genericArgRegister(int initializationOrder,
                                   ArgInitializeFunction initializer,
                                   UninitializeFunction uninitializer)
    {
        add(initializationOrder, NULL, initializer, uninitializer);
        return true;
    }



    // Generic registration of global object method. It requires initializationOrder value
    // every object with lower initializationOrder value will be initialized before
    // all objects with high initializationOrder. If two or more objects have the same
    // initializationOrder they will be initialized in a "random" order. The initializer method
    // pointer is required. Uninitializer method pointer is optional. The objects will
    // be uninitialized in reversed order. Note there is no guarantee that object with the same
    // initializationOrder will uninitialized in exact reversed initialize order. The order
    // should be considered "random"
    static void* genericPointerRegister(int initializationOrder,
                                        InitializeFunction initializer,
                                        UninitializeFunction uninitializer)
    {
        add(initializationOrder, initializer, NULL, uninitializer);
        return NULL;
    }


    static void* genericArgPointerRegister(int initializationOrder,
                                           ArgInitializeFunction initializer,
                                           UninitializeFunction uninitializer)
    {
        add(initializationOrder, NULL, initializer, uninitializer);
        return NULL;
    }

    // Provides default initialize method for global object hold with a raw pointer.
    template <class T, class Mock = T>
    static bool initializeRawPointer(T** ppPointer)
    {
        // Initialize pointer only if it is not already
        // This could be used from test framework to initialize the code with
        // mock object with initializationOrder() - 1 value
        if (!*ppPointer)
        {
            *ppPointer = new Mock();
            return true;
        }
        return false;
    }

    // Provides default uninitialize method for global object hold with a raw pointer.
    template <class T>
    static void uninitializeRawPointer(T** ppPointer)
    {
        delete *ppPointer;
        *ppPointer = NULL;
    }

    // Register default initialize and uninitialize methods for global object hold
    // with a raw pointer. Note the raw pointer is not necessary to be used directly.
    // There still could accessor instance method to provide access to the object.
    template <class T>
    static T* rawPointer(int initializationOrder, T*& pPointer)
    {
        return (T*)genericPointerRegister(initializationOrder,
                                          boost::bind(initializeRawPointer<T>, &pPointer),
                                          boost::bind(uninitializeRawPointer<T>, &pPointer));
    }

    // Register default initialize and uninitialize methods for global object hold
    // with a raw pointer. Note the raw pointer is not necessary to be used directly.
    // There still could accessor instance method to provide access to the object.
    // The class needs to provide it initializationOrder() method.
    template <class T>
    static T* rawPointer(T*& pPointer)
    {
        return rawPointer(T::initializationOrder(), pPointer);
    }

    // Register mock object to replace the default initialize and uninitialize methods 
    // for global object hold with a raw pointer. 
    template <class T, class Mock>
    static void mockRawPointer(T*& pPointer)
    {
        genericPointerRegister(T::initializationOrder() - 1,
                               boost::bind(initializeRawPointer<T, Mock>, &pPointer),
                               boost::bind(uninitializeRawPointer<Mock>, (Mock**)&pPointer));
    }


    // Provides default initialize method for global object hold with a shared pointer.
    template <class T>
    static bool initializeSharedPointer(boost::shared_ptr<T>* ppPointer)
    {
        // Initialize pointer only if it is not already
        // This could be used from test framework to initialize the code with
        // mock object with initializationOrder() - 1 value
        if (!*ppPointer)
            *ppPointer = boost::make_shared<T>();
        return true;
    }

    // Provides default uninitialize method for global object hold with a shared pointer.
    template <class T>
    static void uninitializeSharedPointer(boost::shared_ptr<T>* ppPointer)
    {
        ppPointer->reset();
    }

    // Register default initialize and custom uninitialize methods for global object hold
    // with a shared pointer. Note the raw pointer is not necessary to be used directly.
    // There still could accessor instance method to provide access to the object.
    template <class T>
    static boost::shared_ptr<T> sharedPointer(int initializationOrder, boost::shared_ptr<T>& pPointer, UninitializeFunction uninitializer)
    {
        genericPointerRegister(initializationOrder,
                               boost::bind(initializeSharedPointer<T>, &pPointer),
                               uninitializer);
        return boost::shared_ptr<T>();
    }

    // Register default initialize and uninitialize methods for global object hold
    // with a shared pointer. Note the raw pointer is not necessary to be used directly.
    // There still could accessor instance method to provide access to the object.
    template <class T>
    static boost::shared_ptr<T> sharedPointer(int initializationOrder, boost::shared_ptr<T>& pPointer)
    {
        return sharedPointer(initializationOrder, 
                             pPointer,
                             boost::bind(uninitializeSharedPointer<T>, &pPointer));
    }

    // Register default initialize and custom uninitialize methods for global object hold
    // with a shared pointer. Note the raw pointer is not necessary to be used directly.
    // There still could accessor instance method to provide access to the object.
    // The class needs to provide it initializationOrder() method.
    template <class T>
    static boost::shared_ptr<T> sharedPointer(boost::shared_ptr<T>& pPointer, UninitializeFunction uninitializer)
    {
        return sharedPointer(T::initializationOrder(), pPointer, uninitializer);
    }

    // Register default initialize and uninitialize methods for global object hold
    // with a shared pointer. Note the raw pointer is not necessary to be used directly.
    // There still could accessor instance method to provide access to the object.
    // The class needs to provide it initializationOrder() method.
    template <class T>
    static boost::shared_ptr<T> sharedPointer(boost::shared_ptr<T>& pPointer)
    {
        return sharedPointer(T::initializationOrder(), pPointer);
    }

    // Provides initialize method for global object hold with a shared pointer to become equal to another
    template <class T>
    static bool initializeSharedPointer(boost::shared_ptr<T>* ppPointer, boost::shared_ptr<T>* ppSource)
    {
        if (!*ppPointer)
            *ppPointer = *ppSource;
        return true;
    }

    // Register initialize and uninitialize methods for global object hold with a shared pointer.
    // That makes it equal to another
    template <class T>
    static boost::shared_ptr<T> copySharedPointer(int initializationOrder, boost::shared_ptr<T>& pPointer, boost::shared_ptr<T>& pSource)
    {
        genericPointerRegister(initializationOrder, 
                               boost::bind(initializeSharedPointer<T>, &pPointer, &pSource),
                               boost::bind(uninitializeSharedPointer<T>, &pPointer));
        return boost::shared_ptr<T>();
    }


private:
    static void add(int initializationOrder,
                    InitializeFunction voidInitializer,
                    ArgInitializeFunction argInitializer,
                    UninitializeFunction uninitializer)
    {
        BOOST_ASSERT(voidInitializer != NULL || argInitializer != NULL);
        BOOST_ASSERT(voidInitializer == NULL || argInitializer == NULL);

        InitializeRecord record;
        record.voidInitializeFn = voidInitializer;
        record.argInitializeFn = argInitializer;
        record.uninitializeFn = uninitializer;

        _initializers().insert(std::multimap<int, InitializeRecord>::value_type(initializationOrder, record));
    }


    void callTheInitializers(int argc, const char** argv)
    {
        // initialize all global objects
        std::multimap<int, InitializeRecord>& initializers = _initializers();

        // TODO: log this:
        // initializers.size() + argInitializers.size() > 0,
        // "callTheInitializers was called with no initilizers."
        // "This mean that it was either already called ... or it does not need to be called at all.");

        std::cout << "The number of global object is :" << initializers.size() << "\n"; 

        for (std::multimap<int, InitializeRecord>::iterator it = initializers.begin();
               it != initializers.end(); ++it)
        {
            InitializeFunction& voidInitializer = it->second.voidInitializeFn;
            ArgInitializeFunction& argInitializer = it->second.argInitializeFn;

            bool init = false;
            if (voidInitializer != NULL)
                init = voidInitializer();
            else if (argInitializer != NULL)
                init = argInitializer(argc, argv);

            if (init)
            {
                UninitializeFunction& uninitializer = it->second.uninitializeFn;
                if (uninitializer != NULL)
                    uninitializers.push_back(uninitializer);
            }
        }

        // This data is useless anymore - not a big deal but let clear it
        initializers.clear();
    }

    void callTheUninitializers()
    {
        // uninitialize all global objects
        for (std::vector<UninitializeFunction>::reverse_iterator it = uninitializers.rbegin(); it != uninitializers.rend(); ++it)
        {
            UninitializeFunction& uninitializer = *it;
            uninitializer();
        }
    }

    static std::multimap<int, InitializeRecord>& _initializers()
    {
        static std::multimap<int, InitializeRecord> initializers;
        return initializers;
    }

    std::vector<UninitializeFunction> uninitializers;
};

typedef boost::shared_ptr<GlobalObjectManager> GlobalObjectManagerSPtr;

}

}

#endif // _GLOBALOBJECTMANAGER_H__
