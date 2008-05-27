/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_XalanXslt.hpp"
#include "ASystemException.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"
#include "AOSServices.hpp"

const AString& AOSOutput_XalanXslt::getClass() const
{
  static const AString CLASS("XalanXslt");
  return CLASS;
}

AOSOutput_XalanXslt::AOSOutput_XalanXslt(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSOutput_XalanXslt::~AOSOutput_XalanXslt()
{
}

void AOSOutput_XalanXslt::init()
{
#pragma message("FIX: AOSOutput_XalanXslt::init: XSLT loading hardcoded")
}

AOSContext::ReturnCode AOSOutput_XalanXslt::execute(AOSContext& context)
{
  AFilename xsltName(m_Services.useConfiguration().getAosBaseDataDirectory());
  AString str;
  if (!context.getOutputParams().emitString(ASW("filename", 8), str))
  {
    context.addError(getClass(), ASWNL("Unable to find '/output/filename' parameter"));
    return AOSContext::RETURN_ERROR;
  }
  xsltName.join(str, false);

//TODO    || context.getRequestQuery().exists(ASW("noCache", 7))

  //a_Emit the context XML document
  ARope rope;
  context.useModelXmlDocument().emit(rope);

  //a_Force XML to file
  if (context.useRequestParameterPairs().exists(ASW("dumpXmlToFile", 13)))
  {
    _dumpToFile(rope);
  }

  try
  {
    //a_Transform
    m_xsl.Transform(rope, xsltName.toAString(), context.useOutputBuffer());
  }
  catch(AException& e)
	{
    _dumpToFile(rope);
    context.addError(getClass(), e);
    return AOSContext::RETURN_ERROR;
	}

  //a_Add content type, length and other useful response data to response header
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html", 9));

  return AOSContext::RETURN_OK;
}

void AOSOutput_XalanXslt::_dumpToFile(ARope& rope)
{
  ATime time;
  AString errorfile("xslt_error_");
  time.emit(errorfile);
  errorfile.append('_');
  ATextGenerator::generateRandomAlphanum(errorfile, 8);
  errorfile.append(ASW(".xml",4));

  AFile_Physical outfile(errorfile, ASW("w",1));
  rope.emit(outfile);
  outfile.close();
}

