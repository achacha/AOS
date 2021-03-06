/*
Written by Alex Chachanashvili

$Id: ASync_Semaphore.hpp 301 2009-09-14 22:36:18Z achacha $
*/
#ifndef INCLUDED__ASync_Semaphore_HPP__
#define INCLUDED__ASync_Semaphore_HPP__

#include "apiAPlatform.hpp"
#include "ASynchronization.hpp"
#include "AString.hpp"

#ifdef __LINUX__
#include <semaphore.h>
#endif

class APLATFORM_API ASync_Semaphore : public ASynchronization
{
#if defined(__WINDOWS__)
  public: typedef HANDLE Handle;
#elif defined(__LINUX__)
  public: typedef sem_t * Handle;
#endif

  public :
    ASync_Semaphore(const AString& strName, size_t maxCount = 1, ASynchronization::eInitialState i = UNLOCKED);
    virtual ~ASync_Semaphore();

    virtual void lock();
    virtual bool trylock();
    virtual void unlock();

    size_t getMaxCount() const { return m_maxCount; }
    const AString& getName()   { return mstr_Name; }

    //a_Clone self
    virtual ASynchronization *clone() const { return new ASync_Semaphore(mstr_Name, m_maxCount); }

protected:
    size_t m_maxCount;
    Handle m_handle;
    AString mstr_Name;

  private :
    ASync_Semaphore(const ASync_Semaphore&);
    ASync_Semaphore& operator=(const ASync_Semaphore&);
};

#endif
