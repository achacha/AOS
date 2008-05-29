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

/*!
Template context that is passed along to template node

objects - Contains ABase* types
model - XML model used by handlers 
*/
class ABASE_API ATemplateContext : public ADebugDumpable
{
public:
  /*!
  Construct the context

  @param objects available to the node handlers
  @param model XML data
  @param output buffer
  */
  ATemplateContext(ABasePtrContainer& objects, AXmlDocument& model);
  
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
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //! Objects available to the template nodes
  ABasePtrContainer& m_Objects;  
  
  //! XML data model
  AXmlDocument& m_Model;
};

#endif //INCLUDED__ATemplateContext_HPP__
