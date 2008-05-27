/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__templateASingletonPattern_HPP__
#define INCLUDED__templateASingletonPattern_HPP__

/*!
 ASingletonPattern
   to use just create a global object of this type

 ASingletonPattern<MyObject> singleObject;
 singleObject.get().myMethodInMyObject();
*/

template<class T>
class ASingletonPattern
{
  public:
    /*!
    Delaying creation allows a non-initialized singleton to exist
    You must be careful when initializing it at runtime
    by providing synchronization
    Delayed creation allows us to create a singleton for a socket
    library and not initialize it until at least one socket is created
    */
    ASingletonPattern(bool boolDelayCreation = false)
    {
      if (!boolDelayCreation)
        get();
    }
    
    /*!
    Gets an instance of the singleton object and initializes it
    if it was never created
    */
    T &get();
};

template <class T> T& ASingletonPattern<T>::get()
{ 
  static T one;
  return one;
}

#endif
