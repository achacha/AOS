#ifndef INCLUDED__ASemaphore_HPP__
#define INCLUDED__ASemaphore_HPP__

#include "apiABase.hpp"
#include "ASynchronization.hpp"
#include "AString.hpp"

class ABASE_API ASemaphore : public ASynchronization
{
#if defined(__WINDOWS__)
  public: typedef HANDLE Handle;
#endif

  public :
    ASemaphore(const AString& strName, size_t maxCount = 1, ASynchronization::eInitialState i = UNLOCKED);
    virtual ~ASemaphore();

    virtual void lock();
    virtual bool trylock();
    virtual void unlock();

    size_t getMaxCount() const { return m_maxCount; }
    const AString& getName()   { return mstr_Name; }

    //a_Clone self
    virtual ASynchronization *clone() const { return new ASemaphore(mstr_Name, m_maxCount); }

protected:
    Handle m_handle;
    size_t m_maxCount;
    AString mstr_Name;

  private :
    ASemaphore(const ASemaphore&);
    ASemaphore& operator=(const ASemaphore&);
};

#endif
