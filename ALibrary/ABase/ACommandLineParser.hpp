/*
Written by Alex Chachanashvili

$Id: ABitArray.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED_ACommandLineParser_HPP_
#define INCLUDED_ACommandLineParser_HPP_

#include "ADebugDumpable.hpp"
#include "AString.hpp"

/*!
Class to parse command line arguements

Assumes argv[0] is always the name of the current executable

Given:
"filename of this executable"
"-f"
"somefile.txt"
"foo"
"-d"
"100"
"bar"

NamedPairs:
f:somefile.txt
d:100

AnonList:
foo
bar

*/
class ABASE_API ACommandLineParser : public ADebugDumpable
{
public:
  /*!
  Parse command line arguements

  @param argc count
  @param argv values
  */
  ACommandLineParser(int argc, char **argv, AString delimiter = AConstant::ASTRING_MINUS);

  /*!
  @return size of named and anon values
  */
  size_t size() const;

  /*!
  Check if name/value pair was found on command prompt

  @param name to look for
  @return true if found
  */
  bool existsNamedValue(const AString& name) const;

  /*!
  Check if name/value pair was found on command prompt
  Append to value if found

  @param name to look for
  @param value to append result to
  @return true if found
  */
  bool getNamedValue(const AString& name, AOutputBuffer& value) const;

  /*!
  Check if name/value pair was found on command prompt
  Appends defaultValue if not found

  @param name to look for
  @param value to append result to
  @return true if found or false if not found and default used
  */
  bool getNamedValue(const AString& name, AOutputBuffer& value, const AString& defaultValue) const;

  /*!
  Check if anon value exists

  @param name to look for
  @return true if found
  */
  bool existsAnonValue(const AString& name) const;

  /*!
  Filename specified in argv[0]
  */
  AString& useExecutableName();

  /*!
  Access the map of name:value pairs specified
  */
  MAP_AString_AString& useNamedPairs();

  /*!
  Access the list of anon values specified that do not have a delimited name associated
  */
  LIST_AString& useAnonValues();

  /*!
  Filename specified in argv[0]
  */
  const AString& getExecutableName() const;

  /*!
  Access the map of name:value pairs specified
  */
  const MAP_AString_AString& getNamedPairs() const;

  /*!
  Access the list of anon values specified that do not have a delimited name associated
  */
  const LIST_AString& getAnonValues() const;

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  //! Anon value listed, space separated
  void emitAnonValues(AOutputBuffer& target) const;

  //! Named Parameters, space separated, name with delimiter
  void emitNamedParamaters(AOutputBuffer& target) const;
  
  //! Filename only
  void emitFilename(AOutputBuffer& target) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  // No default ctor
  ACommandLineParser();
  
  // executable for reference
  AString m_ExecutableName;
  
  // Delimiter that denotes a name
  AString m_Delimiter;

  // name/value pairs
  MAP_AString_AString m_NamedPairs;

  // values without name
  LIST_AString m_AnonValues;
};

#endif // INCLUDED_ACommandLineParser_HPP_
