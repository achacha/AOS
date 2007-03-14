#ifndef INCLUDED__AINIProfile_HPP__
#define INCLUDED__AINIProfile_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AString.hpp"
#include "ANameValuePair.hpp"
#include "templateAutoPtr.hpp"

class AFilename;

//a_Non exported struct's
struct _INILeaf;
struct _ININode;

class ABASE_API AINIProfile : public ADebugDumpable
{  
public:
  AINIProfile();                              //a_Creates an empty profile
  virtual ~AINIProfile();
  
  /*!
  Automatically parses the file, closes the file/handle after it is read
  */
  AINIProfile(const AString& filename);
  AINIProfile(const AFilename& filename);
      
  /*!
  Access to the values of a given [key/name]
  */
  bool exists(const AString& key, const AString& name) const;
  bool getValue(const AString& key, const AString& name, AOutputBuffer& value) const;
  bool isValue(const AString& key, const AString& name, const AString& value) const;

  /*!
  Setting methods with option to force write
  After set you should call writeNow() to update the file
  */
  void setValue(const AString& key, const AString& name, const AString& value, bool boolWrite = false);
  
  /*!
  Removes the Name/Value pair (may choose to delay writes if many are done)
  Only removes one value from the key
  */
  void removeValue(const AString& key, const AString& name, bool boolWrite = false);
  
  /*!
  Removes entire key and all it's leafs (name/value pairs) (may choose to delay writes if many are done)
  In reality it removes all leaves and when this is save empty branches are pruned
  */
  void clearKey(const AString& key, bool boolWrite = false);
  
  /*!
  Prunes the entire key and all it's child and leaf nodes
  */
  void removeKey(const AString& key, bool boolWrite = false);

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  Forward declare an iterator class
  */
  class ABASE_API Iterator;
  class ABASE_API WindingIterator;
  class ABASE_API UnwindingIterator;

  /*!
  Gets the iterator for a given key (Default is the root)
  Initially it is at the begining and is empty, must use Iterator::next() to start the iteration
  Example:
    AINIProfile::Iterator it = myINI.getWindingIterator("SomeKey/SubKey");
    AString str;
    while ( it.next() )
    {
      it.emit(str);
      std::cout << str << std::endl;
    }
  */
  Iterator getIterator(const AString& key = AString::sstr_Empty) const;                   //a_next() iterates only within the given key
  WindingIterator getWindingIterator(const AString& key = AString::sstr_Empty) const;     //a_next() iterates into subkeys as well and not to parent keys
  UnwindingIterator getUnwindingIterator(const AString& key = AString::sstr_Empty) const; //a_next() iterates through all keys and moves up to parent until root is reached

  /*!
  Access to filename
  */
  void setFilename(const AString&);
  void setFilename(const AFilename&);
  const AString& getFilename() const;

  /*!
  Forces a save if any of the writes were delayed
  */
  void writeNow();
  void toAFile(AFile&) const;

  /*!
  Force a parse of an INI file (usually done on construction if a filename was provided)
  Will add to the current structure unless the user calls clear()
  Calling reset then parse will allow dynamic reparses of a changed profile
  */
  void parse();
  
  /*!
  Clear all
  */
  void clear();

private:
  //a_Cannot copy this class
  AINIProfile(const AINIProfile&) {}
  AINIProfile& operator=(const AINIProfile&) { return *this; }

  //a_Filename of this INI
  AString mstr_Filename;
 
  //a_Extracts the key from the compound key at given offset
  size_t _getNextKey(const AString& str, AString& key, size_t uStart) const;

  //a_The root of all evil...  :)
  _ININode* mp_Root;

  //a_Finds/makes a node for the given key
  //a_If create flag is set a new node will be created
  //a_If no create flag, then a NULL is possible if no such node exists
  _ININode* _findKeyNode(const AString& str) const;
  _ININode* _makeKeyNode(const AString& str);

  //a_Write method (NULL defaults to root)
  void _writeINI(_ININode* pNode = NULL) const;
  void _writeINIRecursiveLoop(_ININode* , AOutputBuffer&) const;

  //a_Peer helpers
  _ININode* _addPeer(_ININode* pStart, const AString& name);
  _ININode* _findPeer(_ININode* pStart, const AString& name) const;
  
  //a_Adds a leaf to the node, and overwrites the value if already exists (optionally it will not overwrite)
  _INILeaf* _addLeaf(_ININode* pNode, const AString& str, bool boolOverwrite = true);

public:
  //a_Iterator for the name/value pairs for a given key and go into subkeys as well
  class ABASE_API IteratorBase
  {
  public:
    IteratorBase() : mpnode_Base(NULL), mpnode_Current(NULL), mpleaf_Current(NULL) {}
    IteratorBase(_ININode* p) : mpnode_Base(p), mpnode_Current(p), mpleaf_Current(NULL) {}
    virtual ~IteratorBase() {}

    //a_The key that the iterator was creator for
    void getBaseKey(AString&) const;              //a_Base key (Full name 'parent/parent/this')
    void getCurrentKey(AString&) const;           //a_Current key (Full name 'parent/parent/this')
    void getRelativeKey(AString&) const;          //a_Current key relative to base (Full name 'parent/this')
    void getCurrentKeyName(AString&) const;       //a_Current key (Only the name of 'this' key, no parent names)

    //a_Access/comparisson the name/value pair
    //a_If you need to compare a name use isNameEqualTo() and not .getName() == "blah" (Names are case insensitive!)
    void getName(AString&) const;                //a_The name of the current NV pair
    void getValue(AString&) const;               //a_The value of the current NV pair
    bool isNameEqualTo(const AString&) const; //a_Case insensitive compare 
    
    //a_Mover to the next leaf
    virtual bool next() = 0;         //a_Next entry (within this key)
    
    //a_Key based movers
    //a_Every iterator allows manual intervention to advance to next key whether child, peer, or next leafed
    bool nextKeyWithLeaf();            //a_Advances to the next key that has at least one leaf
    bool nextKeyPeer();                //a_Gets the next peer key (no guarantee there is a leaf here, use next() in that case)
    bool nextKeyChild();               //a_Gets the next child key (no guarantee there is a leaf here, use next() in that case)

    //a_Reposition methods
    void clear();                         //a_Back to base key
    
    //a_Advances the iterator to that name (FALSE if not found and iterator is at last position)
    bool find(const AString& name);

    //a_Checks if at end of list (the name/value pair does not exist for this)
    bool isLast() const;               //a_Checks if the position is at the end

    //a_Displays the name/value pair
    void emit(AOutputBuffer&) const;

  protected:
    //a_Copy another iterator
    void _copy(const IteratorBase& i);

    //a_if (mpnode_Current == mpnode_Base) we are at the begining
    //a_if (mpnode_Current == NULL) we are at the end
    _ININode* mpnode_Base;
    _ININode* mpnode_Current;
    _INILeaf* mpleaf_Current;
  };

  class ABASE_API Iterator : public IteratorBase
  {
  public:
    Iterator(_ININode* p) : IteratorBase(p) {}
    Iterator(const Iterator& i) { _copy(i); }
    Iterator& operator=(const Iterator& i) { if (this != &i) _copy(i); return *this; }
    virtual bool next();           //a_Next entry
  };

  class ABASE_API WindingIterator : public IteratorBase
  {
  public:
    WindingIterator(_ININode* p) : IteratorBase(p) {}
    WindingIterator(const WindingIterator& i) { _copy(i); }
    WindingIterator& operator=(const WindingIterator& i) { if (this != &i) _copy(i); return *this; }
    virtual bool next();           //a_Next entry
  };

  class ABASE_API UnwindingIterator : public IteratorBase
  {
  public:
    UnwindingIterator(_ININode* p) : IteratorBase(p) {}
    UnwindingIterator(const UnwindingIterator& i) { _copy(i); }
    UnwindingIterator& operator=(const UnwindingIterator& i) { if (this != &i) _copy(i); return *this; }
    virtual bool next();           //a_Next entry
  };

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
  void _dumpNode(_ININode* pNode, std::ostream& os, int indent) const;
  void _dumpTree(_ININode* pNode, std::ostream& os, int indent) const;
#endif
};

//a_Examples of how to use the iterators

////a_This example will iterate through all subkeys for a given base key
//  AINIProfile::WindingIterator itR = MyINI.getWindingIterator("MySubBranch/SubDir/SubSubDir");
//  cout << "\r\n[Recursive for MyWeb/Site01]" << endl;
//  AString key, str;
//  while(itR.next())
//  {
//    itR.getCurrentKey(key);
//    itR.emit(str);
//    std::cout << key << " > " << str << std::endl;
//  }

////a_The following example will display all keys with at least one leaf(i.e. name=value pair)
//  AINIProfile::WindingIterator itR = MyINI.getWindingIterator("MySubBranch/SubDir/");
//  cout << "\r\n[Recursive key search for all site names]" << endl;
//  AString str;
//  while(itR.nextKeyWithLeaf())
//  {
//    itR.getCurrentKey(str);
//    std::cout << str << std::endl;
//  }

//a_This example will automatically advance to the next node (or first if never used)
//  AINIProfile::WindingIterator it = myINI.getWindingIterator("MySubBranch");
//  AString key;
//  while ( it.find("myValue") )
//  {
//    it.getCurrentKey(key);
//    std::cout << "Key that contains myValue is " << key << std::endl;
//  }

//a_Example: To itreate through all subkeys of a root key
//  AINIProfile::Iterator it = myINI.getWindingIterator("");
//  AString str;
//  if ( it.nextChild() )
//    do 
//    {
//      it.getCurrentKey( str );
//      std::cout << str << std::endl;
//    } while( it.nextPeer() );

//a_Example: To itreate from a given key up to root (ala HTTP cookie jar)
//  AINIProfile::Iterator it = myINI.getUnwindingIterator("/key0/key1/");
//  AString str;
//  if ( it.nextChild() )
//    do 
//    {
//      it.getCurrentKey( str );
//      std::cout << str << std::endl;
//    } while( it.nextPeer() );

#endif // INCLUDED__AINIProfile_HPP__

