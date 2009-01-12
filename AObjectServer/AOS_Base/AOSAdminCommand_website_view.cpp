/*
Written by Alex Chachanashvili

$Id: AOSAdmin.cpp 218 2008-05-29 23:23:59Z achacha $
*/
#include "pchAOS_Base.hpp"
#include "AOSAdminCommand_website_view.hpp"
#include "AXmlElement.hpp"
#include "AFile_Physical.hpp"

const AString AOSAdminCommand_website_view::COMMAND("website_view",12);

const AString& AOSAdminCommand_website_view::getName() const
{
  return COMMAND;
}

AOSAdminCommand_website_view::AOSAdminCommand_website_view(AOSServices& services) :
  AOSAdminCommandInterface(services)
{
  m_SupportedMime.push_back("text/plain");
  m_SupportedMime.push_back("application/x-javascript");
  m_SupportedMime.push_back("image/");
  m_SupportedMime.push_back("audio/");
  m_SupportedMime.push_back("video/");
}

void AOSAdminCommand_website_view::process(AOSAdminCommandContext& context)
{
  AString locale;
  context.useRequestHeader().useUrl().useParameterPairs().get("locale", locale);
  if (!locale.isEmpty())
    context.useRequestHeader().set(AHTTPHeader::HT_REQ_Accept_Language, locale);

  AOutputBuffer& out = context.useOutput();
  context.useResponseHeader().set(AHTTPHeader::HT_ENT_Content_Type, "text/plain; charset=utf-8");

  AString str;
  AFilename path;
  if (context.useRequestHeader().useUrl().useParameterPairs().get("static", str))
  {
    m_Services.useConfiguration().getAosStaticDirectory(context.useRequestHeader(), path);
    path.join(str, false);
  }
  else if (context.useRequestHeader().useUrl().useParameterPairs().get("dynamic", str))
  {
    path.set(m_Services.useConfiguration().getAosBaseDynamicDirectory());
    path.join(str, false);
  }
  else
  {
    context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_500_Internal_Server_Error);
    out.append("Missing required parameter(s).");
    return;
  }

  if (!AFileSystem::exists(path))
  {
    AFilename abs;
    AFileSystem::expand(path, abs);
    context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
    out.append(AString("File not found: ")+abs);
    return;
  }


  //
  // Set content type
  //
  AString ext;
  path.emitExtension(ext);
  m_Services.useConfiguration().setMimeTypeFromExt(ext, context.useResponseHeader());

  str.clear();
  bool isBinary = false;
  bool isSupported = false;
  if (context.useResponseHeader().get(AHTTPHeader::HT_ENT_Content_Type, str))
  {
    for (LIST_AString::const_iterator cit = m_SupportedMime.begin(); cit != m_SupportedMime.end(); ++cit)
    {
      if (str.startsWith(*cit))
      {
        isSupported = true;
        break;
      }
    }
  }
  if (!isSupported)
  {
    if (str.startsWith("application/"))
    {
      // Binary mode
      isBinary = true;
    }

    // If not supported of mime type not set, make it plain text
    m_Services.useConfiguration().setMimeTypeFromExt("txt", context.useResponseHeader());
  }

  if (path.useFilename().endsWith(".aos.xml"))
  {
    // Special case for controllers
    m_Services.useConfiguration().setMimeTypeFromExt("xml", context.useResponseHeader());
  }

  
  //
  // Exists, read and display
  //
  AFile_Physical f(path);
  f.open();
  
  if (isBinary)
  {
    AString str;
    f.emit(str);
    ATextConverter::convertStringToHexDump(str, out);
  }
  else
    out.append(f);
  f.close();
}

void AOSAdminCommand_website_view::_process(AOSAdminCommandContext& context)
{
}
