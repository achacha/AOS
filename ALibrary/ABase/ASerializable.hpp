#ifndef __ASerializable_HPP__
#define __ASerializable_HPP__

#include "apiABase.hpp"

class AFile;

/*!
  Required methods

-------------------START: HPP cup/past----------------------
public:
  virtual void toAFile(AFile&) const;
  virtual void fromAFile(AFile&);
-------------------STOP: HPP cup/past-----------------------
*/
class ABASE_API ASerializable
{
  public:
    virtual ~ASerializable() {}

    /*!
    Serialization/archival methods to an AFile type

    Given:
    AFile *pFile = is something valid

    myObject.toAFile(*pFile);    // this saves the object state

    Then:
    AFile *pFile = points to an AFile object where myObject was serialized to

    myObject.fromAFile(*pFile);  // this restores the object state


    Example:
    ANameValuePair nvpOriginal("name", "value");
    AFile_AString strfile;        // AFile object that stores data in an AString (for example)
    nvp.toAFile(strfile);         // save to AFile type
    strfile.reset();              // reset the read/write positions
    nvp.clear();                  // clear the name/value pair
    nvp.fromAFile(strfile);       // restore it
    */
    virtual void toAFile(AFile& aFile) const = 0;
    virtual void fromAFile(AFile& aFile) = 0;
};

#endif



