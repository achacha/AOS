#ifndef INCLUDED__templateACache_HPP__
#define INCLUDED__templateACache_HPP__

#include "apiABase.hpp"
#include "ACacheInterface.hpp"
#include "AXmlElement.hpp"

/*!
Maps key -> value (_key class must have operator< implemented)
To use emit AEmittable and AXmlEmittable must be implemented on _key class
*/
template <class _key, class _value>
class ACache : public ACacheInterface, public std::map<_key, _value>
{
public:
  /*!
  ctor
  */
  ACache()
  {
  }
  
  /*!
  dtor
  */
  virtual ~ACache() {}

  /*!
  Iterate keys and call emit _key type MUST be AXmlEmittable
  */
  virtual void emit(AXmlElement& target) const
  {
    AXmlElement& base = target.addElement("ACache").addAttribute("_key", typeid(_key).name()).addAttribute("_value", typeid(_value).name());

    const AString elementName("key", 3);
    for(const_iterator cit = begin(); cit != end(); ++cit)
    {
      (*cit).first.emit(base.addElement(elementName));
    }
  }

  /*!
  Iterate keys and call emit _key type MUST be AEmittable
  */
  virtual void emit(AOutputBuffer& target) const
  {
    //a_Only key is show in emit
    for(const_iterator cit = begin(); cit != end(); ++cit)
    {
      (*cit).first.emit(target);
      target.append(AString::sstr_EOL);
    }
  }

  /*!
  Cache management function, should be called by cache manager thread/loop
  */
  virtual void manage()
  {
    //a_Nothing to do here, this is a simple "infinite" cache
  }

private:
  size_t m_MaxItems;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const
  {
    ADebugDumpable::indent(os, indent) << "(ACache<" << typeid(_key).name() << "," << typeid(_value).name() << "> @ " << std::hex << this << std::dec << ") {" << std::endl;
    ADebugDumpable::indent(os, indent+1) << "m_MaxItems=" << m_MaxItems << std::endl;
    ADebugDumpable::indent(os, indent+1) << "map={" << std::endl;
    for(const_iterator cit = begin(); cit != end(); ++cit)
    {
      ADebugDumpable::indent(os, indent+2) << "key={" << std::endl;
      (*cit).first.debugDump(os, indent+3);
      ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
      ADebugDumpable::indent(os, indent+2) << "value={" << std::endl;
      (*cit).second.debugDump(os, indent+3);
      ADebugDumpable::indent(os, indent+2) << "}" << std::endl;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
    ADebugDumpable::indent(os, indent) << "}" << std::endl;
  }
#endif
};

#endif // INCLUDED__templateACache_HPP__
