#ifndef INCLUDED__ALock_HPP__
#define INCLUDED__ALock_HPP__

#include "apiABase.hpp"

class ASynchronization;

class ABASE_API ALock
{
  public:
    //! Lock mode
    enum eLockMode
    {
      UNLOCKED = 0x0,
      LOCKED   = 0x1
    };
  
  public:
    //! ctor   
    ALock();
    
    //! copy ctor
    ALock(const ALock& that);
    
    //! copy operator
    ALock &operator=(const ALock& that);
    
    //! dtor
    ~ALock();


    /*!
    ctor to lock ASynchronization objects
    They get unlocked when this object is destroyed
    */
    ALock(ASynchronization& synchronization, eLockMode mode = ALock::LOCKED);
    
    /*!
    ctor to lock ASynchronization pointer
    They get unlocked when this object is destroyed
    */
    ALock(ASynchronization* psynchronization, eLockMode mode = ALock::LOCKED);   //a_Will not lock if object is NULL

    /*!
    Lock contained synchronization
    Can be unlocked explicitly or implicitly with dtor
    */
    void lock();

    /*!
    Try and lock synchronization
    Returns true is lock is successful
    Can be unlocked explicitly or implicitly with dtor
    */
    bool trylock();
    
    /*!
    Explicit unlock
    */
    void unlock();

  private:
    //! Current state
    eLockMode me__LockMode;

    /*!
    ASynchronization based object
    The pointer is NOT deleted in dtor
    */
    ASynchronization* mp__Synchronization;
};

#endif // INCLUDED__ALock_HPP__

