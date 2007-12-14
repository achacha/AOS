#include "pchABase.hpp"
#include "AContentType_TextHtml.hpp"
#include "AFile.hpp"
#include "AException.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AException.hpp"

const AString AContentType_TextHtml::CONTENT_TYPE("text/html");

#ifdef __DEBUG_DUMP__
void AContentType_TextHtml::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  
  AContentTypeInterface::debugDump(os, indent+1);

  if (m__pheHead)
  {
    ADebugDumpable::indent(os, indent+1) << "*m__pheHead={" << std::endl;
    m__pheHead->debugDump(os, indent+2);
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }
  else
    ADebugDumpable::indent(os, indent+1) << "m__pheHead=NULL" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m__boolFormatted=" << m__boolFormatted << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__boolNeedParsing=" << m__boolNeedParsing << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m__boolExtendedErrorLogging=" << m__boolExtendedErrorLogging << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AContentType_TextHtml::AContentType_TextHtml() :
  AContentTypeInterface(CONTENT_TYPE),
  m__pheHead(NULL),
  m__boolExtendedErrorLogging(false),
  m__boolFormatted(false),
  m__boolNeedParsing(false)
{ 
}

AContentType_TextHtml::~AContentType_TextHtml()
{
  try
  {
    //a_Reset hierarchy and associated elements, delete observers, etc
    __resetHierarchy();
  }
  catch(...) {}
}

void AContentType_TextHtml::__resetHierarchy()
{
  //a_cleanup the AFormObserver vector
  size_t u4X;
  for (u4X = 0; u4X < m__vectorForms.size(); u4X++)
    pDelete(m__vectorForms[u4X]);
  m__vectorForms.clear();

  //a_cleanup the AHrefObserver vector
  for (u4X = 0; u4X < m__vectorHrefs.size(); u4X++)
    pDelete(m__vectorHrefs[u4X]);
  m__vectorHrefs.clear();

  //a_Clean up the old hierarchy
  pDelete(m__pheHead);
}

void AContentType_TextHtml::parse()
{
  if (!m_Data.isNotEof())
    ATHROW(this, AException::ProgrammingError);

  //a_Reset object
  __resetHierarchy();

  m__pheHead = new AElement_HTML(NULL);   //a_The head has no parent but has an error visitor
  size_t pos = 0;
  if (m__boolExtendedErrorLogging)
    m__pheHead->setExtendedErrorLogging();
  m__pheHead->parse(m_Data, pos);

  m__boolNeedParsing = false;
}

void AContentType_TextHtml::clear()
{
  //a_Cleanup
  __resetHierarchy();

  AContentTypeInterface::clear();
}

void AContentType_TextHtml::lint()
{
  if (m__boolNeedParsing)
    parse();
  
  //a_Checks if the age is empty
  if (m__pheHead == NULL)
    ATHROW_EX(this, AException::InvalidData, ASWNL("lint: Empty or unparsed page"));

  //a_<HTML> should be the first thing a browser sees
  if ((m__pheHead->getType().compareNoCase("html")) && (m__pheHead->getType().compareNoCase("!doctype")))
    ATHROW_EX(this, AException::InvalidData, ASWNL("lint: <html> or <!doctype> should be the first element the browser sees"));

  AElement_HTML *pheElement = NULL, *pheChild;
  
  //a_Check to make sure <script> contents are enclosed in a comment type
  while (pheElement = m__pheHead->findType("script", pheElement))
  {
    pheChild = pheElement->nextChildElement(pheElement);
    if (pheChild)
    {
      //a_Next should be a comment
      if (pheChild->getType() != "!--")
        ATHROW_EX(this, AException::InvalidData, ASWNL("lint: First element after <script> should be <!-- to hide script body"));
    }
    else
      ATHROW_EX(this, AException::InvalidData, ASWNL("lint: <script> found with no content"));
  }

  //a_Mandatory tags
  if (m__pheHead->findType("html", NULL) == NULL)
    ATHROW_EX(this, AException::InvalidData, ASWNL("lint: <html> tag not found"));
  if (m__pheHead->findType("body", NULL) == NULL)
    ATHROW_EX(this, AException::InvalidData, ASWNL("lint: <body> tag not found"));
}

AElement_HTML *AContentType_TextHtml::findType(const AString& strType, AElement_HTML *pheStartFrom) const
{ 
  if (m__pheHead)
    return m__pheHead->findType(strType, pheStartFrom);
  else
    return NULL;
}

void AContentType_TextHtml::parseForms(const AUrl& url)
{
  AElement_HTML *pFormElement = NULL;
  AElementObserver_FORM *pNewObserver;
  while (pFormElement = findType("form", pFormElement))
  {
    pNewObserver = new AElementObserver_FORM(pFormElement, &url);
    m__vectorForms.push_back(pNewObserver);
  }
}

void AContentType_TextHtml::parseHrefs(const AUrl& url)
{
  AElement_HTML *pHrefElement = NULL;
  AElementObserver_HREF *pNewObserver;
  while (pHrefElement = findType("a", pHrefElement))
  {
    //a_Make sure that HREF exists, otherwise this is just a regular anchor
    if (pHrefElement->isName("href"))
    {
      pNewObserver = new AElementObserver_HREF(pHrefElement, &url);

      //a_Add only HREFs that are not mailto:
      if (pNewObserver->getSubmissionUrl().getProtocol() != "mailto:")
        m__vectorHrefs.push_back(pNewObserver);
			else
				pDelete(pNewObserver);
    }
  }

  //a_Now the MAP based AREA type from a client side map
  pHrefElement = NULL;
  while (pHrefElement = findType("area", pHrefElement))
  {
    //a_Make sure that HREF exists, otherwise this is just a regular anchor
    if (pHrefElement->isName("href"))
    {
      pNewObserver = new AElementObserver_HREF(pHrefElement, &url);

      //a_Add only HREFs that are not mailto:
      if (pNewObserver->getSubmissionUrl().getProtocol() != "mailto:")
        m__vectorHrefs.push_back(pNewObserver);
			else
				pDelete(pNewObserver);
    }
  }

}

void AContentType_TextHtml::parseSrcs(const AUrl& url)
{
  AElement_HTML *pSrcElement = NULL;
  AElementObserver_SRC *pNewObserver;
  
  //a_Start with FRAMEs
  while (pSrcElement = findType("frame", pSrcElement))
  {
    if (pSrcElement->isName("src"))
    {
      pNewObserver = new AElementObserver_SRC(pSrcElement, &url);
      m__vectorSrcs.push_back(pNewObserver);
    }
  }

  //a_IMG also has an SRC that can be used to call back
  while (pSrcElement = findType("img", pSrcElement))
  {
    if (pSrcElement->isName("src"))
    {
      pNewObserver = new AElementObserver_SRC(pSrcElement, &url);
      m__vectorSrcs.push_back(pNewObserver);
    }
  }

  //a_EMBED also has an SRC that contains sometimes useful data (like SWF files and such)
  while (pSrcElement = findType("embed", pSrcElement))
  {
    if (pSrcElement->isName("src"))
    {
      pNewObserver = new AElementObserver_SRC(pSrcElement, &url);
      m__vectorSrcs.push_back(pNewObserver);
    }
  }
}

size_t AContentType_TextHtml::getTargetCount() const
{
  return m__vectorForms.size() + m__vectorHrefs.size() + m__vectorSrcs.size();
}

const AUrl AContentType_TextHtml::getRandomTargetUrl(size_t iFormThreshold, size_t iHrefThreshold, size_t iMaxThreshold)
{
  AUrl urlRet;

  //a_There were no targets found, or parsing not preformed
  if (m__vectorForms.size() + m__vectorHrefs.size() + m__vectorSrcs.size() <= 0)
    ATHROW(this, AException::ProgrammingError);

  ARandomNumberGenerator& rng = ARandomNumberGenerator::get();
  size_t iRandomType = rng.nextRange(iMaxThreshold);
  if (iRandomType > iHrefThreshold)
  {
    //a_SRC
    if (m__vectorSrcs.size() > 0)
      return m__vectorSrcs[rng.nextRange(m__vectorSrcs.size())]->getSubmissionUrl();
  }
  else if (iRandomType > iFormThreshold)
  {
    //a_HREF
    if (m__vectorHrefs.size() > 0)
      return m__vectorHrefs[rng.nextRange(m__vectorHrefs.size())]->getSubmissionUrl();
  }

  //a_Default is to use the form (this method has affinity for FORMs
  if (m__vectorForms.size() > 0)
    return m__vectorForms[rng.nextRange(m__vectorForms.size())]->getSubmissionUrl();
  else
  {
    //a_Fallthough case
    if (m__vectorHrefs.size() > 0)
      return m__vectorHrefs[rng.nextRange(m__vectorHrefs.size())]->getSubmissionUrl();
    else if (m__vectorSrcs.size() > 0)
      return m__vectorSrcs[rng.nextRange(m__vectorSrcs.size())]->getSubmissionUrl();
  }

  //a_We should not be here
  ATHROW(this, AException::ProgrammingError);
}

void AContentType_TextHtml::parseAll(const AUrl& url)
{
  //a_Parse HTML
  parse();
  
  //aParse FORMs
  parseForms(url);
  
  //a_Parse HREFs
  parseHrefs(url);
  
  //a_Parse SRCs
  parseSrcs(url);
}

void AContentType_TextHtml::toAFile(AFile& aFile) const
{
  //a_Parse is needed before write can occur
  if (m__boolNeedParsing)
    ATHROW(this, AException::ProgrammingError);

  int iDepth = -1;

  //a_If formatted, enable formatting
  if (m__boolFormatted) iDepth = 0;

  if (m__pheHead)
	{
	  AString strOut = m__pheHead->begin(iDepth);
	  aFile.write(strOut);
	  if (m__pheHead->isSingular() == false)
    {
      strOut = m__pheHead->end(iDepth);
      aFile.write(strOut);
    }
  }
}

void AContentType_TextHtml::fromAFile(AFile& aFile)
{
  __resetHierarchy();

  AContentTypeInterface::fromAFile(aFile);

  //a_Set the flag to false, parent calls the virtual parse() method by default
  m__boolNeedParsing = false;
}

void AContentType_TextHtml::emit(AOutputBuffer& target) const
{
  //a_If not formatted just dump back the buffer without formatting
  if (m__boolNeedParsing || !m__boolFormatted)
    target.append(m_Data);
  else
  {
    if (m__pheHead)
	  {
		  target.append(m__pheHead->begin(0));
		  if (!m__pheHead->isSingular())
			  target.append(m__pheHead->end(0));
	  }
  }
}

AElement_HTML *AContentType_TextHtml::getHead() const
{
  return m__pheHead; 
}

size_t AContentType_TextHtml::getFormCount() const 
{
  return m__vectorForms.size();
}

AElementObserver_FORM &AContentType_TextHtml::getFormObserver(size_t i) const 
{
  return *m__vectorForms[i]; 
}

size_t AContentType_TextHtml::getHrefCount() const
{ 
  return m__vectorHrefs.size(); 
}

AElementObserver_HREF &AContentType_TextHtml::getHrefObserver(size_t i) const 
{ 
  return *m__vectorHrefs[i];
}

size_t AContentType_TextHtml::getSrcCount() const 
{ 
  return m__vectorSrcs.size();
}

AElementObserver_SRC &AContentType_TextHtml::getSrcObserver(size_t i) const 
{
  return *m__vectorSrcs[i];
}

void AContentType_TextHtml::setFormatted(bool boolFlag) 
{ 
  m__boolFormatted = boolFlag;
}

void AContentType_TextHtml::setExtendedErrorLogging(bool boolFlag)
{
  m__boolExtendedErrorLogging = boolFlag; 
}
