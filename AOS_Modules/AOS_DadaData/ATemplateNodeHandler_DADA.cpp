/*
Written by Alex Chachanashvili

$Id: ATemplateNodeHandler_DADA.cpp 239 2008-09-02 20:52:54Z achacha $
*/
#include "pchAOS_DadaData.hpp"
#include "ATemplateNodeHandler_DADA.hpp"
#include "AXmlDocument.hpp"
#include "AXmlElement.hpp"
#include "ATemplate.hpp"
#include "ABasePtrContainer.hpp"
#include "AFile_Physical.hpp"
#include "AWordUtility.hpp"
#include "ARandomNumberGenerator.hpp"

const AString ATemplateNodeHandler_DADA::S_DELIM_START("{",1);
const AString ATemplateNodeHandler_DADA::S_DELIM_END("}",1);

const AString ATemplateNodeHandler_DADA::TAGNAME("DADA",4);

void ATemplateNodeHandler_DADA::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << "@ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent) << "TAGNAME=" << getTagName() << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

ATemplateNodeHandler_DADA::ATemplateNodeHandler_DADA(AOSServices& services) :
  m_Services(services)
{
}

ATemplateNodeHandler_DADA::~ATemplateNodeHandler_DADA()
{
}

void ATemplateNodeHandler_DADA::init()
{
  AXmlElement::CONST_CONTAINER nodes;

  m_Services.useConfiguration().getConfigRoot().find("AOS_DadaData/dataset", nodes);
  AXmlElement::CONST_CONTAINER::iterator it;
  for (it = nodes.begin(); it != nodes.end(); ++it)
  {
    AString strSet;
    (*it)->getAttributes().get(ASW("name",4), strSet);
    if (strSet.isEmpty())
      ATHROW_EX(*it, AException::InvalidData, ASWNL("AOS_DadaData/dataset missing 'name' parameter"));

    ADadaDataHolder *pddh = new ADadaDataHolder();
    pddh->readData(m_Services, *it);
    m_Objects.insert(strSet, pddh, true);
  }

  nodes.clear();
  m_Services.useConfiguration().getConfigRoot().find(ASW("AOS_DadaData/template",21), nodes);
  it = nodes.begin();
  while (it != nodes.end())
  {
    AString str;
    (*it)->emitContent(str);
    
    AString strName;
    if ((*it)->getAttributes().get(ASW("name",4), strName))
    {
      AFilename filename(m_Services.useConfiguration().getAosBaseDataDirectory(), str, false);
      if (AFileSystem::exists(filename))
      {
        AFile_Physical file(filename, ASW("r", 1));
        file.open();

        str.clear();
        while (AConstant::npos != file.readLine(str))
        {
          if ('#' != str.at(0, '\x0'))
          {
            m_Templates[strName].push_back(str);
          }
          str.clear();
        }
      }
      else
        m_Services.useLog().add(ARope("AOS_DadaData: Missing file: ")+filename, ALog::EVENT_WARNING);
    }
    else
      m_Services.useLog().add(ASWNL("AOS_DadaData: AOS_DadaData/template missing 'name' attribute"), ALog::EVENT_FAILURE);

    ++it;
  }
}

void ATemplateNodeHandler_DADA::deinit()
{
}

const AString& ATemplateNodeHandler_DADA::getTagName() const
{
  return ATemplateNodeHandler_DADA::TAGNAME;
}

ATemplateNode *ATemplateNodeHandler_DADA::create(AFile& file)
{
  ATemplateNodeHandler_DADA::Node *pNode = new ATemplateNodeHandler_DADA::Node(this);
  pNode->fromAFile(file);
  return pNode;
}

ATemplateNodeHandler *ATemplateNodeHandler_DADA::clone()
{ 
  return new ATemplateNodeHandler_DADA(m_Services);
}

///////////////////////////////////////// ATemplateNodeHandler_DADA::Node ///////////////////////////////////////////////////////////////////////////

ATemplateNodeHandler_DADA::Node::Node(ATemplateNodeHandler *pHandler) :
  ATemplateNode(pHandler)
{
}

ATemplateNodeHandler_DADA::Node::Node(const ATemplateNodeHandler_DADA::Node& that) :
  ATemplateNode(that)
{
}

ATemplateNodeHandler_DADA::Node::~Node()
{
}

void ATemplateNodeHandler_DADA::Node::process(ATemplateContext& context, AOutputBuffer& output)
{
  AAutoPtr<AEventVisitor::ScopedEvent> scoped;
  if (context.useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
  {
    scoped.reset(new AEventVisitor::ScopedEvent(context.useEventVisitor(), ASWNL("ATemplateNodeHandler_DADA"), m_BlockData, AEventVisitor::EL_DEBUG));
  }

#pragma message("ATemplateNodeHandler_DADA::Node::process: MUST selecte dataset dynamically")

  AString strSet("dada");  //TODO: Find a way to select this (attributes in nodes)
  ADadaDataHolder *pddh = ((ATemplateNodeHandler_DADA *)mp_Handler)->m_Objects.useAsPtr<ADadaDataHolder>(strSet);
  if (!pddh)
  {
    context.useEventVisitor().addEvent(AString("ADadaTemplateProcessor: ADadaDataHolder not found at: ")+strSet, AEventVisitor::EL_ERROR);
    return;
  }
  
  VARIABLEMAP globals;
  _generateLine(pddh, globals, m_BlockData, context, output);
}

void ATemplateNodeHandler_DADA::Node::_generateLine(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& format, ATemplateContext& context, AOutputBuffer& output)
{
  if (format.isEmpty())
    return;
  
  AFile_AString file(format);
  size_t readresult;
  char c = '\x0';
  AString target(1024, 1024), strType(64,128);
  while (AConstant::npos != file.readUntil(target, S_DELIM_START, true, true))
  {
    file.read(c);
    readresult = file.readUntil(strType, S_DELIM_END, true, true);
    if (0 == readresult)
    {
      file.readUntilEOF(target);
      output.append(target);
      target.clear();
      break;
    }

    //a_If not EOF process type
    if (AConstant::npos != readresult)
    {
      if (strType.getSize() > 1)
      {
        switch (c)
        {
          case '%':
            //a_Process the tag {%type:tag,tag,...}
            _appendWordType(pddh, globals, strType, target);
          break;

          case '$':
            //a_ {$variable}
            _appendVariable(pddh, globals, strType, target);
          break;

          default:
            //a_Passthru of unknown tags
            target.append('{');
            target.append(strType);
            target.append('}');
            context.useEventVisitor().addEvent(AString("ADadaTemplateProcessor::_generateLine: unknown type:")+strType, AEventVisitor::EL_WARN);
          break;
        }
        strType.clear(); 
      }
    }
  }
  file.readUntilEOF(target);
  output.append(target);
  target.clear();
}

void ATemplateNodeHandler_DADA::Node::_appendVariable(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& strType, AOutputBuffer& target)
{
  AASSERT(this, strType.getSize() > 0);

  AString strTypeName, strControl;
  LIST_AString listControlNames;
  size_t pos = strType.find(':');
  if (AConstant::npos != pos)
  {
    strType.peek(strTypeName, 0, pos);
    strTypeName.makeLower();

    strType.peek(strControl, pos+1);
    strControl.makeLower();
    strControl.split(listControlNames, ',', AConstant::ASTRING_WHITESPACE);
  }
  else
  {
    strTypeName.assign(strType);
    strTypeName.makeLower();
  }

  //a_Find it in the global lookup
  VARIABLEMAP::iterator it = globals.find(strTypeName);
  if (it != globals.end())
  {
    AString str((*it).second);

    LIST_AString::iterator itN = listControlNames.begin();
    while (itN != listControlNames.end())
    {
      if (!(*itN).compare("article", 7))
      {
        AString strTemp(str);
        if (AConstant::npos == AWordUtility::sstr_Vowels.find(str.at(0)))
        {
          str.assign("a ", 2);
        }
        else
        {
          str.assign("an ", 3);
        }
        str.append(strTemp);
      }

      if (!(*itN).compare("plural", 6))
      {
        AString strTemp;
        AWordUtility::getPlural(str, strTemp);
        str.assign(strTemp);
      }

      if (!(*itN).compare("uppercase", 9))
      {
        str.makeUpper();
      }

      if (!(*itN).compare("lowercase", 9))
      {
        str.makeLower();
      }

      if (!(*itN).compare("proper", 6))
      {
        str.use(0) = (u1)toupper(str.at(0));
      }

      ++itN;
    }

    target.append(str);
  }
  else
  {
    //a_Not found in global map
  }
}

void ATemplateNodeHandler_DADA::Node::_appendWordType(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& strType, AOutputBuffer& target)
{
  //a_First remove control tags "TYPE:controltag1,controltag2,..."
  AString strTypeName, strControl;
  LIST_AString listControlNames;
  size_t pos = strType.find(':');
  if (AConstant::npos != pos)
  {
    strType.peek(strTypeName, 0, pos);
    strTypeName.makeLower();

    strType.peek(strControl, pos+1);
    strControl.makeLower();
    strControl.split(listControlNames, ',', AConstant::ASTRING_WHITESPACE);
  }
  else
  {
    strTypeName.assign(strType);
    strTypeName.makeLower();
  }

  ADadaDataHolder::WORDMAP::iterator it = pddh->useWordMap().find(strTypeName);
  if (pddh->useWordMap().end() != it)
  {
    VECTOR_AString& vec = (*it).second;
    u4 index = ARandomNumberGenerator::get().nextRange((int)vec.size());
    
    AString strSaveVariable;
    AString str(vec.at(index));
    
    //a_Apply control code actions
    LIST_AString::iterator itN = listControlNames.begin();
    while (itN != listControlNames.end())
    {
      if ('$' == (*itN).at(0, '\x0'))
      {
        //a_Add variable to global map
        (*itN).peek(strSaveVariable, 1);
      }
      else if (!strTypeName.compare("verb"))
      {
        //a_Verb only control
        if (!(*itN).compare("present", 7))
        {
          if (AConstant::npos != AWordUtility::sstr_Vowels.find(str.last()))
          {
            //a_Vowel at the end is removed and replaced with "ing"
            str.rremove(1);
          }
          str.append("ing", 3);
        }
        else if (!(*itN).compare("past", 4))
        {
          if (AConstant::npos == AWordUtility::sstr_Vowels.find(str.last()))
          {
            str.append("ed", 2);
          }
          else
          {
            str.append("d", 1);
          }
        }
      }

      if (!(*itN).compare("article", 7))
      {
        AString strTemp(str);
        if (AConstant::npos == AWordUtility::sstr_Vowels.find(str.at(0)))
        {
          str.assign("a ", 2);
        }
        else
        {
          str.assign("an ", 3);
        }
        str.append(strTemp);
      }

      if (!(*itN).compare("plural", 6))
      {
        AString strTemp;
        AWordUtility::getPlural(str, strTemp);
        str.assign(strTemp);
      }

      if (!(*itN).compare("uppercase", 9))
      {
        str.makeUpper();
      }

      if (!(*itN).compare("lowercase", 9))
      {
        str.makeLower();
      }

      if (!(*itN).compare("proper", 6))
      {
        str.use(0) = (u1)toupper(str.at(0));
      }

      ++itN;
    }
    
    target.append(str);

    //a_Save it if save variable was detected
    if (!strSaveVariable.isEmpty())
    {
      globals[strSaveVariable] = str;
      strSaveVariable.clear();
    }
  }
  else
  {
    //a_Unknown type, pass through as is.
    target.append('%');
    target.append(strType);
    target.append('%');
  }
}
