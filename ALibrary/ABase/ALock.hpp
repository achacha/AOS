#ifndef __ALock_HPP__
#define __ALock_HPP__

#include "apiABase.hpp"

class ASynchronization;

class ABASE_API ALock
{
  public:
    enum eLockMode
    {
      UNLOCKED = 0x0,
      LOCKED   = 0x1
    };
  
  public:
    ALock();
    ALock(const ALock& that);
    ALock &operator=(const ALock& that);
    ~ALock();


    /**
     * Constructor to lock ASynchronization objects
     *  They get unlocked when this object is destroyed
    **/
    ALock(ASynchronization& synchronization, eLockMode mode = ALock::LOCKED);
    ALock(ASynchronization* psynchronization, eLockMode mode = ALock::LOCKED);   //a_Will not lock if object is NULL

    /**
     * Locking/Unlocking methods
    **/
    void lock();
    bool trylock();
    void unlock();

  private:
    /**
     * Lock mode
    **/
    eLockMode me__LockMode;

    /**
     * ASynchronization based object
    **/
    ASynchronization* mp__Synchronization;
};

#endif
