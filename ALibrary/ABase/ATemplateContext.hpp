#ifndef INCLUDED__ATemplateContext_HPP__
#define INCLUDED__ATemplateContext_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ABasePtrContainer.hpp"
#include "AXmlDocument.hpp"
#include "AOutputBuffer.hpp"

/*!
Template context that is passed along to template node

objects - Contains ABase* types
model - XML model used by handlers 
output - destination of the result of the handler
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
  ATemplateContext(ABasePtrContainer& objects, AXmlDocument& model, AOutputBuffer& output);
  
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
  Use the output buffer

  @return AOutputBuffer for output
  */
  AOutputBuffer& useOutput();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  //! Objects available to the template nodes
  ABasePtrContainer& m_Objects;  
  
  //! Output buffer
  AOutputBuffer& m_Output;
  
  //! XML data model
  AXmlDocument& m_Model;
};

#endif //INCLUDED__ATemplateContext_HPP__
