/*
Written by Alex Chachanashvili

$Id: AOSModule_DadaTemplateProperties.cpp 160 2009-07-03 13:42:03Z achacha $
*/
#include "pchAOS_DadaData.hpp"
#include "AOSModule_DadaTemplateProperties.hpp"
#include "ADadaDataHolder.hpp"
#include "AFile_AString.hpp"
#include "AWordUtility.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AFile_Physical.hpp"

const AString AOSModule_DadaTemplateProperties::CLASS("Dada.Properties");

const AString& AOSModule_DadaTemplateProperties::getClass() const
{
  return CLASS;
}

void AOSModule_DadaTemplateProperties::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModule_DadaTemplateProperties @ " << std::hex << this << std::dec << ") {" << std::endl;

  AOSModuleInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSModule_DadaTemplateProperties::AOSModule_DadaTemplateProperties(AOSServices& services) :
  AOSModuleInterface(services)
{
}

void AOSModule_DadaTemplateProperties::adminEmitXml(
  AXmlElement& eBase, 
  const AHTTPRequestHeader& request
)
{
  AOSModuleInterface::adminEmitXml(eBase, request);
}

AOSContext::ReturnCode AOSModule_DadaTemplateProperties::execute(AOSContext& context, const AXmlElement& params)
{
  AString dataset;
  const AXmlElement *pDada = params.findElement(AOS_DadaData_Constants::S_DATASET);
  if (NULL != pDada)
  {
    //a_Found element which contains overrides
    const AXmlElement *p = pDada->findElement(AOS_DadaData_Constants::S_QUERYOVERRIDE);
    if (NULL != p)
    {
      //a_Should check query parameters for override
      AString name;
      p->emitContent(name);
      context.useRequestParameterPairs().get(name, dataset);
    }

    //a_Try default since dataset is not yet set
    if (dataset.isEmpty())
    {
      p = pDada->findElement(AOS_DadaData_Constants::S_DEFAULT);
      if (NULL != p)
      {
        p->emitContent(dataset);
      }
    }
  }

  //a_Set the dataset
  if (!dataset.isEmpty())
  {
    context.useModel().setString(AOS_DadaData_Constants::DATASETPATH, dataset);
  }
  else
  {
    //a_No dataset found
    context.useEventVisitor().addEvent(getClass() + ": Could not determine 'dataset'", AEventVisitor::EL_ERROR);
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}
