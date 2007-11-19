#include "pchAOS_DadaData.hpp"
#include "AOSModule_DadaDataTemplate.hpp"
#include "ADadaDataHolder.hpp"
#include "AFile_AString.hpp"
#include "AWordUtility.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AXmlElement.hpp"
#include "AAttributes.hpp"

const AString AOSModule_DadaDataTemplate::PATH_DADADATA("/dadadata", 9);
const AString AOSModule_DadaDataTemplate::PATH_OUTPUT("output/dada", 11);

const AString& AOSModule_DadaDataTemplate::getClass() const
{
  static const AString CLASS("DadaDataTemplate");
  return CLASS;
}

#ifdef __DEBUG_DUMP__
void AOSModule_DadaDataTemplate::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSModule_DadaDataTemplate @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "PATH_DADADATA=" << PATH_DADADATA << std::endl;
  ADebugDumpable::indent(os, indent+1) << "PATH_OUTPUT=" << PATH_OUTPUT << std::endl;

  AOSModuleInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AOSModule_DadaDataTemplate::AOSModule_DadaDataTemplate(AOSServices& services) :
  AOSModuleInterface(services)
{
}

void AOSModule_DadaDataTemplate::addAdminXml(
  AXmlElement& eBase, 
  const AHTTPRequestHeader& request
)
{
  AOSModuleInterface::addAdminXml(eBase, request);
}

void AOSModule_DadaDataTemplate::init()
{
  ADadaDataHolder *pddh = new ADadaDataHolder();
  pddh->readData(m_Services);
  m_Objects.insert(PATH_DADADATA, pddh);
}

bool AOSModule_DadaDataTemplate::execute(AOSContext& context, const AXmlElement& params)
{
  ADadaDataHolder *pddh = dynamic_cast<ADadaDataHolder *>(m_Objects.getObject(PATH_DADADATA));

  if (!pddh)
  {
    context.addError("AOSModule_DadaDataTemplate::execute", AString("AOSModule_DadaDataTemplate: ADadaDataHolder not found at: ")+PATH_DADADATA);
    return false;
  }

  AString templateName;
  if (!context.useRequestParameterPairs().get(ASW("templateName",12), templateName))
  {
    context.addError("AOSModule_DadaDataTemplate::execute", "Please specify 'templateName' parameter.");
    return false;
  }
  
  ADadaDataHolder::TEMPLATES::iterator it = pddh->useTemplates().find(templateName);
  if (it != pddh->useTemplates().end())
  {
    VECTOR_AString& templateLines = (*it).second;
    for (size_t i=0; i<templateLines.size(); ++i)
    {
      _generateLine(pddh, templateLines.at(i), context.useModel());
    }
  }
  else
  {
    context.addError("AOSModule_DadaDataTemplate::execute", AString("Not found templateName=")+templateName);
    return false;
  }
  
  return true;
}

void AOSModule_DadaDataTemplate::deinit()
{
  m_Objects.remove(PATH_DADADATA);
}

void AOSModule_DadaDataTemplate::_generateLine(ADadaDataHolder *pddh, const AString& format, AXmlElement& element)
{
  static const AString delimStart("{");
  static const AString delimEnd("}");
  
  if (format.isEmpty())
    return;
  
  AFile_AString file(format);
  size_t pos = 0;
  size_t readresult;
  char c = '\x0';
  AString target(1024, 1024), strType(64,128);
  AAttributes attributes;
  while (AConstant::npos != file.readUntil(target, delimStart, true, true))
  {
    file.read(c);
    readresult = file.readUntil(strType, delimEnd, true, true);
    if (0 == readresult)
    {
      file.readUntilEOF(target);
      element.addElement(ASW("/dada/line",10), target).addAttributes(attributes);
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
            _appendWordType(pddh, strType, target);
          break;

          case '$':
            //a_ {$variable}
            _appendVariable(pddh, strType, target);
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
  element.addElement(ASW("dada/line",9), target).addAttributes(attributes);
  target.clear();
  attributes.clear();
}

void AOSModule_DadaDataTemplate::_appendVariable(ADadaDataHolder *pddh, const AString& strType, AOutputBuffer& target)
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
  VARIABLEMAP::iterator it = m_globals.find(strTypeName);
  if (it != m_globals.end())
  {
    AString str((*it).second);

    LIST_AString::iterator itN = listControlNames.begin();
    while (itN != listControlNames.end())
    {
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

      ++itN;
    }

    target.append(str);
  }
  else
  {
    //a_Not found in global map
  }
}

void AOSModule_DadaDataTemplate::_appendWordType(ADadaDataHolder *pddh, const AString& strType, AOutputBuffer& target)
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
      
      //a_Verb only control
      if ('$' == (*itN).at(0, '\x0'))
      {
        //a_Add variable to global map
        (*itN).peek(strSaveVariable, 1);
      }
      else if (!strTypeName.compare("verb"))
      {
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

      ++itN;
    }
    
    target.append(str);

    //a_Save it if save variable was detected
    if (!strSaveVariable.isEmpty())
    {
      m_globals[strSaveVariable] = str;
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
