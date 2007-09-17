#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_XalanXslt.hpp"
#include "ASystemException.hpp"
#include "AObjectContainer.hpp"
#include "AFile_Physical.hpp"
#include "ATextGenerator.hpp"
#include "AOSServices.hpp"

const AString& AOSOutput_XalanXslt::getClass() const
{
  static const AString CLASS("XalanXslt");
  return CLASS;
}

AOSOutput_XalanXslt::AOSOutput_XalanXslt(ALog& alog) :
  AOSOutputGeneratorInterface(alog)
{
}

AOSOutput_XalanXslt::~AOSOutput_XalanXslt()
{
}

void AOSOutput_XalanXslt::init(AOSServices& services)
{
#pragma message("FIX: AOSOutput_XalanXslt::init: XSLT loading hardcoded")
}

bool AOSOutput_XalanXslt::execute(AOSOutputContext& context)
{
  AFilename xsltName(context.getConfiguration().getAosBaseDataDirectory());
  AString str;
  if (!context.getOutputParams().emitFromPath(ASW("/params/output/filename", 23), str))
  {
    m_Log.append("AOSOutput_XalanXslt: Unable to find '/params/output/filename' parameter");
    ATHROW_EX(this, AException::InvalidParameter, ASWNL("Xslt requires '/params/output/filename' parameter"));
  }
  xsltName.join(str, false);

#ifdef __DEBUG_DUMP__
//TODO    || context.getRequestQuery().exists(ASW("noCache", 7))
#endif

  //a_Emit the context XML document
  ARope rope;
  context.getOutputXmlDocument().emit(rope);

  //a_Force XML to file
#ifdef __DEBUG_DUMP__
  if (context.getRequestParameterPairs().exists(ASW("dumpXmlToFile", 13)))
  {
    _dumpToFile(rope);
  }
#endif

  try
  {
    //a_Transform
    m_xsl.Transform(rope, xsltName.toAString(), context.useOutputBuffer());
  }
  catch(AException&)
	{
#ifdef __DEBUG_DUMP__
      _dumpToFile(rope);
#endif
    throw;
	}

  //a_Add content type, length and other useful response data to response header
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html", 9));

  return true;
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

