/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__ATemplateContext_HPP__
#define INCLUDED__ATemplateContext_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ABasePtrContainer.hpp"
#include "AXmlDocument.hpp"
#include "AEventVisitor.hpp"

/*!
Template context that is passed along to template node

Objects: Contain ABase* types
Model: XML model used by handlers
Visitor: For event logging
*/
class ABASE_API ATemplateContext : public ADebugDumpable
{
public:
  /*!
  Construct the context
  Objects passed by reference are NOT OWNED NOR DELETED

  @param objects available to the node handlers
  @param model XML data
  @param visitor for event logging
  */
  ATemplateContext(ABasePtrContainer& objects, AXmlDocument& model, AEventVisitor& visitor);
  
  /*!
  */
  virtual ~ATemplateContext();

  /*!
  Use the objects
  
  @return ABasePtrContainer
  */
  ABasePtrContainer& useObjects();

  /*!
  Use the model object

  @return AXmlDocument model
  */
  AXmlDocument& useModel();
  
  /*!
  Use the event visitor

  @return AEventVisitor
  */
  AEventVisitor& useEventVisitor();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! Objects available to the template nodes
  ABasePtrContainer& m_Objects;  
  
  //! XML data model
  AXmlDocument& m_Model;

  //! Event visitor
  AEventVisitor& m_EventVisitor;
};

#endif //INCLUDED__ATemplateContext_HPP__
