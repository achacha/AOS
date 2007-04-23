#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_Template.hpp"
#include "ATemplate.hpp"
#include "AFile_Physical.hpp"
#include "AException.hpp"
#include "ASync_Mutex.hpp"
#include "ALock.hpp"

#ifdef __DEBUG_DUMP__
void AOSOutput_Template::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSOutput_Template @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Templates={" << std::endl;
  AOSOutput_Template::TEMPLATES::const_iterator cit = mp_Templates->begin();
  while (cit != mp_Templates->end())
  {
    ADebugDumpable::indent(os, indent+2) << (*cit).first << "=";
    (*cit).second->debugDump(os, indent+3);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  AOSOutputGeneratorInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

void AOSOutput_Template::addAdminXml(
  AXmlElement& eBase, 
  const AHTTPRequestHeader& request
)
{
  AOSOutputGeneratorInterface::addAdminXml(eBase, request);

  addPropertyWithAction(
    eBase,
    ASW("TemplateCache",13),
    AString::fromSize_t(mp_Templates->size()),
    ASW("Clear",5),
    ASW("Clear template cache (Dangeous if there are templates being actively used)",74)
  );

  AOSOutput_Template::TEMPLATES::const_iterator cit = mp_Templates->begin();
  while (cit != mp_Templates->end())
  {
    AXmlElement& eObject = eBase.addElement(ASW("object",6)).addAttribute(ASW("name",4), ASW("ATemplate",9));
    addProperty(eObject, ASW("filename",8), (*cit).first);
    addProperty(eObject, ASW("template",8), *(*cit).second, AXmlData::CDataSafe);
    ++cit;
  }

}

void AOSOutput_Template::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AString propertyName, actionName;
  if (!request.getUrl().getParameterPairs().get(PROPERTY, propertyName))
  {
    addError(eBase, "'property' input not found");
    return;
  }
  if (!request.getUrl().getParameterPairs().get(ACTION, actionName))
  {
    addError(eBase, "'action' input not found");
    return;
  }

  if (propertyName.equals("AOSOutputExecutor.Template.TemplateCache"))
  {
    if (actionName.equals("Clear"))
    {
      //a_Swap template container with a new one and then delete the old content
      AOSOutput_Template::TEMPLATES *pOldTemplates = mp_Templates;
      mp_Templates = new AOSOutput_Template::TEMPLATES();
      AOSOutput_Template::TEMPLATES::iterator it = pOldTemplates->begin();
      while (it != pOldTemplates->end())
      {
        delete (*it).second;
        ++it;
      }
      delete pOldTemplates;
      m_Log.add(ASWNL("AOSOutput_Template: Template cache cleared"), ALog::MESSAGE);
    }
  }
  else
    addError(eBase, ARope("Do not know how to modify: ")+propertyName);
}

const AString& AOSOutput_Template::getClass() const
{
  static const AString CLASS("Template");
  return CLASS;
}

AOSOutput_Template::AOSOutput_Template(ALog& alog) :
  AOSOutputGeneratorInterface(alog),
  mp_Templates(new AOSOutput_Template::TEMPLATES()),
  mp_TemplatesGuard(new ASync_Mutex("AOSOutput_Template"))
{
}

AOSOutput_Template::~AOSOutput_Template()
{
  try
  {
    AOSOutput_Template::TEMPLATES::iterator it = mp_Templates->begin();
    while (it != mp_Templates->end())
    {
      delete (*it).second;
      ++it;
    }
    delete mp_Templates;
    delete mp_TemplatesGuard;
  }
  catch(...) {}
}

bool AOSOutput_Template::execute(AOSOutputContext& context)
{
  AFilename filenameBase(context.getConfiguration().getAosBaseDataDirectory());
  AXmlNode::ConstNodeContainer templateNames;
  context.getOutputParams().find(ASW("/params/output/filename", 23), templateNames);
  if(templateNames.size() == 0)
  {
    m_Log.add(ASWNL("AOSOutput_Template: Unable to find '/params/output/filename' parameter"), ALog::FAILURE);
    ATHROW_EX(this, AException::InvalidParameter, "Template requires '/params/output/filename' parameter");
  }

  bool doNotAddToCache = false;
  if (context.getRequestParameterPairs().exists(ASW("noCache", 7)))
    doNotAddToCache = true;

  AString filename(1024, 256);
  AXmlNode::ConstNodeContainer::const_iterator cit = templateNames.begin();
  while (cit != templateNames.end())
  {
    filename.clear();
    filenameBase.emit(filename);
    AString filenamePart;
    (*cit)->emitContent(filenamePart);
    filename.append(filenamePart);
    ATemplate *pTemplate = NULL;
    AOSOutput_Template::TEMPLATES::iterator it = mp_Templates->find(filenamePart);
    if (it == mp_Templates->end() || doNotAddToCache)
    {
      //a_Load and parse
      AFile_Physical tFile(filename);
      tFile.open();
      pTemplate = new ATemplate();
      pTemplate->fromAFile(tFile);
    }
    else
      pTemplate = (*it).second;

    //a_Process template
    AASSERT(this, pTemplate);
    pTemplate->process(context.useOutputBuffer(), context.getOutputRootXmlElement());

    //a_Cache template if needed
    if (!doNotAddToCache)
    {
      if (it == mp_Templates->end())
      {
        ALock lock(mp_TemplatesGuard);
        //a_Make sure another thread did not add this already
        if (mp_Templates->find(filenamePart) == mp_Templates->end())
        {
          (*mp_Templates)[filenamePart] = pTemplate;
          m_Log.add(ASWNL("AOSOutput_Template: Adding parsed template"), filename, ALog::INFO);
        }
        else
          delete pTemplate;
      }
    }
    else
      delete pTemplate;

    ++cit;
  }
  return true;
}
