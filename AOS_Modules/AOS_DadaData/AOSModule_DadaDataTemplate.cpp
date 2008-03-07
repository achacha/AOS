#include "pchAOS_DadaData.hpp"
#include "AOSModule_DadaDataTemplate.hpp"
#include "ADadaDataHolder.hpp"
#include "AFile_AString.hpp"
#include "AWordUtility.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AXmlElement.hpp"
#include "AAttributes.hpp"
#include "AFile_Physical.hpp"

const AString& AOSModule_DadaDataTemplate::getClass() const
{
  static const AString CLASS("DadaDataTemplate");
  return CLASS;
}

void AOSModule_DadaDataTemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModule_DadaDataTemplate @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Templates={" << std::endl;
  TEMPLATES::const_iterator citT = m_Templates.begin();
  while (citT != m_Templates.end())
  {
    ADebugDumpable::indent(os, indent+2) << citT->first << " size=" << u4(citT->second.size()) << std::endl;
    ++citT;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  AOSModuleInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSModule_DadaDataTemplate::AOSModule_DadaDataTemplate(AOSServices& services) :
  AOSModuleInterface(services)
{
}

void AOSModule_DadaDataTemplate::adminEmitXml(
  AXmlElement& eBase, 
  const AHTTPRequestHeader& request
)
{
  AOSModuleInterface::adminEmitXml(eBase, request);
}

void AOSModule_DadaDataTemplate::init()
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
        m_Services.useLog().add(ARope("AOS_DadaData: Missing file: ")+filename, ALog::WARNING);
    }
    else
      m_Services.useLog().add(ASWNL("AOS_DadaData: AOS_DadaData/template missing 'name' attribute"), ALog::FAILURE);

    ++it;
  }
}

AOSContext::ReturnCode AOSModule_DadaDataTemplate::execute(AOSContext& context, const AXmlElement& params)
{
  AString templateName;
  if (!context.useRequestParameterPairs().get(ASW("templateName",12), templateName))
  {
    context.addError(getClass(), ASWNL("Please specify 'templateName' parameter."));
    return AOSContext::RETURN_ERROR;
  }

  AString strSet;
  if (!context.useRequestParameterPairs().get(ASW("dataset",7), strSet))
  {
    context.addError(getClass(), ASWNL("Please specify 'dataset' parameter."));
    return AOSContext::RETURN_ERROR;
  }

  ADadaDataHolder *pddh = m_Objects.useAsPtr<ADadaDataHolder>(strSet);
  if (!pddh)
  {
    context.addError(getClass(), AString("AOSModule_DadaDataTemplate: ADadaDataHolder not found at: ")+strSet);
    return AOSContext::RETURN_ERROR;
  }
  
  VARIABLEMAP globals;
  TEMPLATES::iterator it = m_Templates.find(templateName);
  if (it != m_Templates.end())
  {
    VECTOR_AString& templateLines = (*it).second;
    for (size_t i=0; i<templateLines.size(); ++i)
    {
      _generateLine(pddh, globals, templateLines.at(i), context.useModel());
    }
  }
  else
  {
    context.addError(getClass(), ARope("Not found templateName: ")+templateName);
    return AOSContext::RETURN_ERROR;
  }
  
  return AOSContext::RETURN_OK;
}

void AOSModule_DadaDataTemplate::deinit()
{
}

void AOSModule_DadaDataTemplate::_generateLine(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& format, AXmlElement& element)
{
  static const AString delimStart("{");
  static const AString delimEnd("}");
  static const AString DADA("dada",4);
  static const AString LINE("line",4);
  
  if (format.isEmpty())
    return;
  
  AFile_AString file(format);
  size_t pos = 0;
  size_t readresult;
  char c = '\x0';
  AString target(1024, 1024), strType(64,128);
  AAttributes attributes;
  AXmlElement& eDada = element.overwriteElement(DADA);
  while (AConstant::npos != file.readUntil(target, delimStart, true, true))
  {
    file.read(c);
    readresult = file.readUntil(strType, delimEnd, true, true);
    if (0 == readresult)
    {
      file.readUntilEOF(target);
      eDada.addElement(LINE).addData(target).addAttributes(attributes);
      target.clear();
      attributes.clear();
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
            //a_Process the tag {%type:tar,tag,...}
            _appendWordType(pddh, globals, strType, target);
          break;

          case '$':
            //a_ {$variable}
            _appendVariable(pddh, globals, strType, target);
          break;

          case '&':
            //a_ {&name:value}
            _parseAttributes(strType, attributes);
          break;

          default:
            target.append("{ERROR:", 7);
            target.append(strType);
            target.append('}');
            m_Services.useLog().append(AString("AOSModule_DadaDataTemplate::_generateLine: unknown type:")+strType);
          break;
        }
        strType.clear(); 
      }
    }
  }
  file.readUntilEOF(target);
  eDada.addElement(LINE).addData(target).addAttributes(attributes);
  target.clear();
  attributes.clear();
}

void AOSModule_DadaDataTemplate::_appendVariable(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& strType, AOutputBuffer& target)
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
        str.use(0) = toupper(str.at(0));
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

void AOSModule_DadaDataTemplate::_appendWordType(ADadaDataHolder *pddh, VARIABLEMAP& globals, const AString& strType, AOutputBuffer& target)
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
//      TRACE1("Checking control: %s\n", (*it).c_str());
      
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
        str.use(0) = toupper(str.at(0));
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

void AOSModule_DadaDataTemplate::_parseAttributes(const AString& data, AAttributes& attributes)
{
  size_t pos = data.find(':');
  if (AConstant::npos != pos)
  {
    AString name, value;
    data.peek(name, 0, pos);
    data.peek(value, pos+1);
    attributes.insert(name, value);
  }
  else
  {
    attributes.insert(data);
  }
}
