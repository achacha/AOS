#ifndef INCLUDED__ATextOdometer_HPP__
#define INCLUDED__ATextOdometer_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"

/**
 * Given a fixed size it will allow modifications and roll over when
 *  maximum size is reached.  Size is always maintained and zeros are
 *  added in front as a padding.
 *
 *  eg. if size is 3 and count is 998 and 5 is added result is 003
 *      in essence this is an odometer
 *
 * The subset is a string it uses for digits (from left (lowest) to right (highest))
 *  default subset is "0123456789" for a numeric odometer
 *  you can use "0123456789ABCDEF" for a hexadecimal odometer, etc...
**/
class ABASE_API ATextOdometer : public ADebugDumpable
{
  public:
    ATextOdometer(size_t iSize = 4, const AString& strSubset = AConstant::CHARSET_BASE10);
    ATextOdometer(const AString& initial, size_t iSize = 4, const AString& strSubset = AConstant::CHARSET_BASE10);
    ATextOdometer(const ATextOdometer&);
    virtual ~ATextOdometer();

    /*!
    Pre increment
    */
    const ATextOdometer &operator ++(void);
    
    /*!
    Pre decrement
    */
    const ATextOdometer &operator --(void);

    /*!
    Post increment
    */
    const ATextOdometer  operator ++(int);
    
    /*!
    Post decrement
    */
    const ATextOdometer  operator --(int);

    /*!
    Gets the current odometer
    */
    inline const AString& get() const;
    
    /*!
    AEmittable
    */
    virtual void emit(AOutputBuffer&) const;

    /*!
    Equals
    */
    bool operator ==(const ATextOdometer&) const;

    /*!
    Not equals
    */
    bool operator !=(const ATextOdometer&) const;

    /*!
    Size in digits
    */
    size_t getSize() const;
    
    /*!
    A way to reset the odometer (iSize of AConstant::npos leaves the default size)
    */
    void clear(size_t size = AConstant::npos);
    
    /*!
    Gets the current subset
    */
    const AString& getSubset() const;

    /*!
    Sets a new subset and resets the odometer
    */
    void setSubset(const AString& strSubset);
    
    /*!
    Sets the initial value and returns it's reference for convenience
    */
    const AString& setInitial(const AString& strInitial);
    
    /*!
    Sets random initial value
    */
    const AString& setRandomInitial();

private:
    //a_Subset
    AString mstr_Subset;
    
    //a_Current odometer
    AString mstr_Odometer;

    /**
     * Modifiers
    **/
    void __inc();
    void __dec();

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__ATextOdometer_HPP__
