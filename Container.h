/**
@file Container.h
@brief Intrusive Container Documentation and Base Class.

A File with some general documentation information for the various @ref IntrusiveContainers "container classes", no code here. 

 @warning Most of the use of this library has been with ContainerNoSafety, so there may be errors in the other versions.

@copyright  (c) 2014-2024 Richard Damon
@parblock
MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
@endparblock

@ingroup IntrusiveContainers

*/
#ifndef CONTAINER_H
#define CONTAINER_H

#define CONTAINER_VERSION_MAJOR 1
#define CONTAINER_VERSION_MINOR 0
#define CONTAINER_VERSION_BUILD 0
#define CONTAINER_VERSION_NUMBER 0x10000
#define CONTAINER_VERSION_STRING "1.0.0"


#ifdef DOXYGEN
/**
 * @def CONTAINER_OS
 * OS Selection for Thread Safety Selection
 * 
 * One of:
 * + #CONTAINER_OS_USER
 * + #CONTAINER_OS_FREERTOS
 * + #CONTAINER_OS_CMSIS
 * + #CONTAINER_OS_CMSIS2
 * 
 * @ingroup IntrusiveContainers
 */

#ifndef CONTAINER_OS
#define CONTAINER_OS CONTINER_OS_FREERTOS
#endif
#endif

/**
 * @def CONTAINER_OS_USER
 * User Defined Safety Layer
 * 
 * The User Application needs to define the member functions for Container\<s> and ContainerNode\<s>
 * for each supported ContainerThreadSafety level it will support except ContainerNoSafety
 * which will still have the default no locking.
 * 
 * @ingroup IntrusiveContainers
 */
#define CONTAINER_OS_USER       0
/**
 * @def CONTAINER_OS_FREERTOS
 * FreeRTOS based Safety layer
 * 
 * @ingroup IntrusiveContainers  
 */   
#define CONTAINER_OS_FREERTOS   1

/**
 * @def CONTAINER_OS_CMSIS
 * CMSIS based Safety layer
 * 
 * @todo Implement CMSIS Container Safety layer
 * 
 * @ingroup IntrusiveContainers  
 */   
#define CONTAINER_OS_CMSIS      2

/**
 * @def CONTAINER_OS_CMSIS2
 * CMSIS version 2 based Safety layer
 * 
 * @todo Implement CMSIS2 Container Safety layer
 * 
 * @ingroup IntrusiveContainers  
 */   
#define CONTAINER_OS_CMSIS2     3

// See if OS Not defined, and try to detect OS being used, assumes RTOS base header already included
#ifndef CONTAINER_OS
#  ifdef INC_FREERTOS_H
#  define CONTAINER_OS CONTAINER_OS_FREERTOS
#  endif
#endif

#ifndef CONTAINER_OS
// Only fall back to CMSIS if we can't detect the actual OS
  #ifdef _CMSIS_OS_H
    #define CONTAINER_OS CONTAINER_OS_CMSIS
  #endif
  #ifdef CMSIS_OS2_H_
    #define CONTAINER_OS CONTAINER_OS_CMSIS2
  #endif
#endif

#ifndef CONTAINER_OS
// Couldn't detect, either warn or apply a global default
#warning CONTAINER_OS not defined and can not be detected
#endif


/**
 * Thread Safety selection for container.
 *
 * + ContainerNoSafety: No safety provided by container, all needs to be supplied
 * by the application. Suitable for containers built at startup so have no safety
 * issues or containers externally managed by the application
 *  
 * + ContainerMutexSafe: Will use a Mutex to protect the container. 
 *
 * + ContainerTaskSafe: Critical Sections will be used around updates and internal
 * searches such that the container is safe to be updated by a task, and possibly
 * read by an ISR. Global locks are used
 *
 * + ContainerISRSafe: Critical Sections will be used around updates and internal
 * searches such that the container is safe to be updated by either a task or
 * by an ISR. Note, this may cause longer critical sections.
 * 
 * Note, it is presumed that while the containers may be shared between tasks / interrupts, 
 * that a given node have its root connections be changed by two different tasks / interrupts.
 *
 * TODO: May want to look at implementing a R/W lock version for TaskSafe updates
 * that has minimal global critical sections.
 * 
 * | ContainerThreadSafety | ISR?       | Read                 | Write               |
 * | ---                   | ---        | ---                  | ---                 |
 * | #ContainerNoSafety     | User Def   |                     |                     |
 * | #ContainerReadWrite    | No Access  | Container Shared    | Container Exclusive |
 * | #ContainerMutexSafe    | No Access  | Container Exclusive | Container Exclusive |
 * | #ContainerTaskOnly     | No Access  | Exclusive Section   | Exclusive Section   |
 * | #ContainerTaskSafe     | Read Only  | Exclusive Section   | Critical Section    |
 * | #ContainerISRSafe      | Read/Write | Critical Section    | Critical Section    |
 * 
 * + Critical Section: Interrupts will be disabled to prevent an ISR for interfacing
 * + Exclusive Section: Processor Lock held to not let other tasks interfere
 * + Container Exclusive: A Lock in the container held, keeping other tasks out of the Container
 * + Container Shared: A Read lock in the container is held, keeping other tasks from changing the Container.
 *
 * @warning Most of the use of this library has been with ContainerNoSafety, so there may be errors in the other versions.
 *
 * @ingroup IntrusiveContainers
 */
enum ContainerThreadSafety {
    ContainerNoSafety,  ///< Thread Safety (if needed) provided by the application
#ifdef CONTAINER_OS     // Only define "safe" versions if we have an OS defined
    ContainerReadWrite, ///< Container will include a Read/Write lock to protect updates in tasks
    ContainerMutexSafe, ///< Container will include a Mutex to protect updates in Tasks, but not ISRs
    ContainerTaskOnly,  ///< Container will suspend the scheduler as no need to protect from ISR usage
    ContainerTaskSafe,  ///< Container will include Critical Sections to protect updates in tasks, but not ISRs
    ContainerISRSafe,   ///< Container will include Critical Sections to protect updates in ISR and Tasks
#endif
};

template<ContainerThreadSafety s> class ContainerNode;

/**
 * Base Container Class
 * 
 * Provides the base locking ability for the container.
 * 
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * 
 * @note Most of the testing of this library has been done with ContainerNoSafety
 * 
 * @ingroup IntrusiveContainers
 */
template<ContainerThreadSafety s>
class Container {
    friend class ContainerNode<s>;
protected:
    Container() {}
    ~Container() {}

    virtual bool check() const = 0;

    /**
     * Obtain a read lock 
     * 
     * While a read lock is held on a container, the container can not be changed.
     * 
     * Read locks will not be given while a write lock is held.
     * 
     * @param upgradable: If called with upgradeable, then the lock can be upgradable to a write lock,
     * and no other upgradable read lock will be granted. 
     * (Note, the code returned will need to encode the upgradable bit)
     *
     * @returns code to give to readUnlock()
     */
    unsigned readLock(bool upgradable) const;

    /**
     * Release the read lock that was held.
     * 
     * @param code from writeLock();
     */
    void readUnlock(unsigned code) const;

    /**
     * Obtain a write lock
     * 
     * While a write is held, no other read or write lock will be created.
     * 
     * @param upgrade: operation already has a read lock that was made with the upgradeable flag
     */
    unsigned writeLock(bool upgrade) const;
    /**
     * Release write lock
     * 
     * If was from an upgraded read lock, we return to an upgradable read lock
     * 
     * @param code the value returned from the previous call to writeLock()
     */
    void writeUnlock(unsigned code) const;

private:
    Container(Container const &) = delete;      ///< We are not copyable
    void operator=(Container const &) = delete; ///< We are not assignable;
};

/**
 * Base Container Node Class
 * 
 * Used to provide the Thread Safety Primitives for a Container Node
 * 
 * @tparam s The ContainerThreadSafety value to define the thread safety model of the Container
 * Note, the Nodes must match the safety level of the container. 
 *
 * @ingroup IntrusiveContainers
 */
template<ContainerThreadSafety s>
class ContainerNode {
    typedef Container<s> C;     ///< The type of the Container that we are part of.
protected:
    ContainerNode(C* root = nullptr) { setRoot(root); }
    ~ContainerNode() {}

    virtual bool check() const = 0;
    /**
     * Set our Container
     * 
     * Used to allow to Node to record what Container it is in.
     * Used only if safety method need resources from the Container, like a Mutex
     * 
     * If an operation changes the root of a node, then it needs to save the 
     * original root to exit the critical section on that container that it entered before the 
     * operation.
     */
    void setRoot(C* root) { (void)root; }


    unsigned readLock(bool upgradable) const;


    void readUnlock(unsigned code) const;

   
    unsigned writeLock(bool upgrade) const;

    void writeUnlock(unsigned code) const;

protected:
    ContainerNode(ContainerNode const&) = delete;
    void operator=(ContainerNode const&) = delete;
};

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline unsigned Container<ContainerNoSafety>::writeLock(bool upgrade) const {
    return upgrade;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline void Container<ContainerNoSafety>::writeUnlock(unsigned save) const {
    (void) save;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline unsigned Container<ContainerNoSafety>::readLock(bool upgradable) const {
    return upgradable;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline void Container<ContainerNoSafety>::readUnlock(unsigned save) const {
    (void) save;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline unsigned ContainerNode<ContainerNoSafety>::writeLock(bool upgrade) const {
    return upgrade;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline void ContainerNode<ContainerNoSafety>::writeUnlock(unsigned save) const {
    (void) save;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline unsigned ContainerNode<ContainerNoSafety>::readLock(bool upgradable) const {
    return upgradable;
}

/**
 * Under ContainerNoSafety application handles all the needed restrictions.
 */
template<>
inline void ContainerNode<ContainerNoSafety>::readUnlock(unsigned save) const {
    (void) save;
}

/*****************************************************************************/
// FreeRTOS implementations

#if CONTAINER_OS == CONTAINER_OS_FREERTOS
#include <FreeRTOS.h>
#include <task.h>

#ifdef configUSE_RECURSIVE_MUTEXES
// Only if Mutexes are configured
// TODO see if we can work with a pair of non-recurvise mutexes for search/alter
// This likely will need an "upgrade" flag to writeLock
#include <semphr.h>

// Define alternate version for Mutex Safety that includes the Mutex
template<>
class Container<ContainerMutexSafe> {
    friend class ContainerNode<ContainerMutexSafe>;
protected:
    Container() {
    #if configSUPPORT_STATIC_ALLOCATION
        m_mutex = xSemaphoreCreateRecursiveMutexStatic(&m_mutex_buffer);
    #else
        m_mutex = xSemaphoreCreateRecursiveMutex();
    #endif
    }
    ~Container() {
        vSemaphoreDelete(m_mutex);
    }

    /**
     * Enter possibly long Read-Only Critical Section
     *
     * @returns code to give to readUnlock()
     */
    unsigned readLock() const;

    /**
     * Leave Read Only Critical Section
     * @param code from writeLock();
     */
    void readUnlock(unsigned code) const;

    /**
     * Enter Write Critical Section
     */
    unsigned writeLock() const;
    /**
     * Leave
     * @param code the value returned from the previous call to writeLock()
     */
    void writeUnlock(unsigned code) const;

    SemaphoreHandle_t m_mutex;
#if configSUPPORT_STATIC_ALLOCATION
    StaticSemaphore_t m_mutex_buffer;
#endif // configSUPPORT_STATIC_ALLOCATION

private:
    Container(Container const &) = delete;      ///< We are not copyable
    void operator=(Container const &) = delete; ///< We are not assignable;
};

// Define alternate version for Mutex Safety that includes the Mutex
template<>
class ContainerNode<ContainerMutexSafe> {
    typedef Container<ContainerMutexSafe> C;
protected:
    ContainerNode(C* root) {
        setRoot(root);
    }
    ~ContainerNode() {
    }

    /**
     * Set our Container
     * 
     * Used to allow to Node to record what Containier it is in. 
     * Used only if safety method need resources from the Container, like a Mutex
     */
    void setRoot(C* root) { m_root = root; }

    /**
     * Enter possibly long Read-Only Critical Section
     *
     * @returns code to give to readUnlock()
     */
    unsigned readLock() const { return m_root->readLock(); }

    /**
     * Leave Read Only Critical Section
     * @param code from writeLock();
     */
    void readUnlock(unsigned code) const { m_root->readUnlock(code); }

    /**
     * Enter Write Critical Section
     */
    unsigned writeLock() const { return m_root->writeLock(); }
    /**
     * Leave
     * @param code the value returned from the previous call to writeLock()
     */
    void writeUnlock(unsigned code) const { m_root->writeUnlock(code); }


private:    
    ContainerNode(ContainerNode const &) = delete;      ///< We are not copyable
    void operator=(ContainerNode const &) = delete;     ///< We are not assignable;

    C*  m_root = nullptr;
};
#endif // configUSE_RECURSIVE_MUTEXES

/**********************************/
// ContainerTaskOnly: Container only used at TaskLevel

template<>
inline unsigned Container<ContainerTaskOnly>::writeLock(bool upgrade) const {
    vTaskSuspendAll();
    return upgrade;
}

template<>
inline void Container<ContainerTaskOnly>::writeUnlock(unsigned save) const {
    (void)save;
    xTaskResumeAll();
}

// Search may be longer, so leave interrupts enabled but ISRs can't change things
template<>
inline unsigned Container<ContainerTaskOnly>::readLock(bool upgradable) const {
    vTaskSuspendAll();
    return upgradable;
}

template<>
inline void Container<ContainerTaskOnly>::readUnlock(unsigned save) const {
    (void)save;
    xTaskResumeAll();
}

/*************************/
// ContainerTaskSafe: Container only changed at Task level, but might be accessed in ISR
// While only changed at task level, some quick operations might use the ordinary critical

template<>
inline unsigned Container<ContainerTaskSafe>::writeLock(bool upgrade) const {
    return taskENTER_CRITICAL_FROM_ISR() | (upgrade << 15);
}

template<>
inline void Container<ContainerTaskSafe>::writeUnlock(unsigned save) const {
    taskEXIT_CRITICAL_FROM_ISR(save & 0xFF);
}

// Search may be longer, so leave interrupts enabled but ISRs can't change things
template<>
inline unsigned Container<ContainerTaskSafe>::readLock(bool upgradable) const {
  
    if(!xPortIsInsideInterrupt()) vTaskSuspendAll();
    return upgradable;
}

template<>
inline void Container<ContainerTaskSafe>::readUnlock(unsigned save) const {
    (void)save;
    if(!xPortIsInsideInterrupt()) xTaskResumeAll();
}

/*****************************************************************************/

template<>
inline unsigned Container<ContainerISRSafe>::writeLock(bool upgrade) const {

    return taskENTER_CRITICAL_FROM_ISR() | (upgrade << 15);
}

template<>
inline void Container<ContainerISRSafe>::writeUnlock(unsigned save) const {
    taskEXIT_CRITICAL_FROM_ISR(save & 0xFF);
}

template<>
inline unsigned Container<ContainerISRSafe>::readLock(bool upgradable) const {
    return taskENTER_CRITICAL_FROM_ISR() | (upgradable << 15);
}

template<>
inline void Container<ContainerISRSafe>::readUnlock(unsigned save) const {
    taskEXIT_CRITICAL_FROM_ISR(save & 0xFF);
}
#endif // CONTAINER_OS == CONTAINER_OS_FREERTOS

#endif
