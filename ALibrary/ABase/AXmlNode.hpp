#ifndef INCLUDED__AXmlNode_HPP__
#define INCLUDED__AXmlNode_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "AAttributes.hpp"
#include "AXmlEmittable.hpp"

/*!
Base class that represents an XML node from which AElement, AData and such are derived
Pure virtual class not meant to be instanciated
*/
class ABASE_API AXmlNode : public ADebugDumpable, public AXmlEmittable
{
public:
  typedef std::list<AXmlNode *> NodeContainer;
  typedef std::list<const AXmlNode *> ConstNodeContainer;

  AXmlNode(AXmlNode *pParent = NULL);
  AXmlNode(const AString& name, AXmlNode *pParent = NULL);
  AXmlNode(const AAttributes& attrs, AXmlNode *pParent = NULL);
  AXmlNode(const AString& name, const AAttributes& attrs, AXmlNode *pParent = NULL);
  virtual ~AXmlNode();

  /*!
  Attributes
  */
  AXmlNode& addAttribute(const AString& name, const AString& value = AString::sstr_Empty);
  AXmlNode& addAttributes(const AAttributes& attrs);
  inline const AAttributes& getAttributes() const;
  inline AAttributes& useAttributes();

  /*!
  Adds a comment
  Returns this node
  */
  AXmlNode& addComment(const AString&);

  /*!
  Name of this node
  */
  const AString& getName() const;
  AString& useName();

  /*!
  Find path based on this node as root
  Returns NULL if not found
  */
  AXmlNode *findNode(const AString& xpath);
  const AXmlNode *findNode(const AString& xpath) const;

  /*!
  AEmittable and other output methods
   indent >= 0 will make it human-readable by adding indent and CRLF
   emit will include current node, emitContent will not
  */
  virtual void emit(AOutputBuffer&) const;
  virtual void emit(AOutputBuffer&, int indent) const;
  /*!
   Emit as JavaScript Object Notation (JSON)
  */
  virtual void emitJSON(AOutputBuffer&, int indent = -1) const;
  
  /*!
  Emit content only, unecoded raw data
  */
  virtual void emitContent(AOutputBuffer&) const;

  /*!
  Emit Xml from path
  returns false if path not found
  */
  bool emitFromPath(const AString& path, AOutputBuffer&, int indent = -1) const;

  /*!
  AXmlEmittable
  */
  virtual void emit(AXmlElement&) const;

  /*!
  Clear the object and release sub-elements
  NOTE: Does NOT clear the name of the object
  */
  virtual void clear();

  /*!
  Adding content
  This object will OWN and release the pointer passed to it
  */
  void addContentNode(AXmlNode *pNode);

  /*!
  Content
  List of AXmlNode objects contained in this one
  */
  const AXmlNode::NodeContainer& getContentContainer() const;

  /*!
  Set/Get parent node if any, NULL if none
  */
  AXmlNode *getParent() const { return mp_Parent; }
  void setParent(AXmlNode *pParent) { mp_Parent = pParent; }

  //a_Clone of self used in deep copy
  virtual AXmlNode* clone() const = 0;

protected:
  //a_Name
  AString m_Name;

  //a_Content container
  NodeContainer m_Content;

  //a_Attributes
  AAttributes m_Attributes;

  //a_Parent node (NULL if none)
  AXmlNode *mp_Parent;

  //a_Recursive search function
  AXmlNode *_get(LIST_AString& xparts) const;

  //a_Indents with 2 spaces per indent
  inline void _indent(AOutputBuffer&, int) const;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AXmlNode_HPP__
