/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchALuaEmbed.hpp"
#include "ALuaEmbed.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AFile_Socket.hpp"
#include "ARope.hpp"
#include "ATextConverter.hpp"
#include "ACookieJar.hpp"

/*!
Request from web via HTTP GET

web.HttpGet(objectname, URL);

Input OBJECTS:
  OPTIONAL: cookiejar        - ACookieJar object to use with AHTTPRequestHeader and AHTTPResponseHeader

Output OBJECTS created:
  OWNED: objectname.response - AHTTPResponseHeader object
  OWNED: objectname.body     - ARope object with response body


LUA e.g.:
  status = web.HttpGet("ping", "http://localhost/ping");
  response = objects.emit("ping.response");
  body = objects.emit("ping.body");

lua namespace: web
lua return: (int) Status code
*/
static int web_HttpGet(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *cc;

  //a_Object name
  cc = luaL_checklstring(L, 1, &len);
  const AString& objectname = AString::wrap(cc, len);

  //a_URL parameter
  cc = luaL_checklstring(L, 2, &len);
  const AString& url = AString::wrap(cc, len);

  lua_pop(L,2);

  //a_Get the cookiejar if any
  ACookieJar *pCookieJar = pLuaContext->useObjects().useAsPtr<ACookieJar>("cookiejar");

  //a_Set up request header
  AHTTPRequestHeader request;
  request.parseUrl(url);
  request.set(AHTTPHeader::HT_GEN_Connection, "close");
  if (pCookieJar)
    pCookieJar->emitCookies(request);

  //a_Log it
  pLuaContext->useEventVisitor().startEvent(request, AEventVisitor::EL_INFO);

  //a_REQUEST
  AFile_Socket httpSocket(request, true);
  httpSocket.open();
  request.toAFile(httpSocket);
  
  //a_RESPONSE and add it to objects
  AHTTPResponseHeader *presponse = new AHTTPResponseHeader();
  presponse->fromAFile(httpSocket);
  if (pCookieJar)
    pCookieJar->parse(request, *presponse);

  //a_Log it
  pLuaContext->useEventVisitor().startEvent(*presponse, AEventVisitor::EL_INFO);

  //a_Add response header to the objects
  pLuaContext->useObjects().insert(objectname+".response", presponse, true);

  //a_Return status code
  lua_pushinteger(L, presponse->getStatusCode());

  //a_Return HTTP response document
  ARope *pbody = new ARope();
  size_t contentLength = presponse->getContentLength();
  if (contentLength != AConstant::npos)
  {
    httpSocket.read(*pbody, contentLength);
  }
  else
  {
    pbody->append("ERROR: Content-Length not provided in response header.");
  }
  pLuaContext->useObjects().insert(objectname+".body", pbody, true);
  
  //a_Log it
  pLuaContext->useEventVisitor().startEvent(*pbody, AEventVisitor::EL_DEBUG);

  return 1;
}

/*!
Request from web via HTTP POST

web.HttpPost(objectname, URL, POSTdata);

Input OBJECTS:
  OPTIONAL: cookiejar        - ACookieJar object to use with AHTTPRequestHeader and AHTTPResponseHeader

Output OBJECTS created:
  OWNED: objectname.response - AHTTPResponseHeader object
  OWNED: objectname.body     - ARope object with response body


LUA e.g.:
status = web.HttpPost("ping", "http://localhost/ping", "form=data&etc=");
response = objects.emit("ping.response");
body = objects.emit("ping.body");

lua namespace: web
lua return: (int) Status code
*/
static int web_HttpPost(lua_State *L)
{
  ATemplateContext *pLuaContext = static_cast<ATemplateContext *>(L->acontext); 
  AASSERT(NULL, pLuaContext);

  size_t len = AConstant::npos;
  const char *cc;

  //a_Object name
  cc = luaL_checklstring(L, 1, &len);
  const AString& objectname = AString::wrap(cc, len);

  //a_URL parameter
  cc = luaL_checklstring(L, 2, &len);
  const AString& url = AString::wrap(cc, len);

  //a_FORM data parameter
  cc = luaL_checklstring(L, 3, &len);
  const AString& data = AString::wrap(cc, len);

  lua_pop(L,3);

  //a_Get the cookiejar if any
  ACookieJar *pCookieJar = pLuaContext->useObjects().useAsPtr<ACookieJar>("cookiejar");

  //a_Set up request header
  AHTTPRequestHeader request;
  request.setMethod(AHTTPRequestHeader::METHOD_POST);
  request.parseUrl(url);
  request.set(AHTTPHeader::HT_GEN_Connection, "close");
  request.set(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(data.getSize()));
  request.set(AHTTPHeader::HT_ENT_Content_Type, AHTTPHeader::CONTENT_TYPE_HTML_FORM);
  if (pCookieJar)
    pCookieJar->emitCookies(request);

  //a_Log it
  pLuaContext->useEventVisitor().startEvent(request, AEventVisitor::EL_INFO);

  //a_REQUEST
  AFile_Socket httpSocket(request, true);
  httpSocket.open();
  request.toAFile(httpSocket);
  httpSocket.write(data);
  
  //a_RESPONSE and add it to objects
  AHTTPResponseHeader *presponse = new AHTTPResponseHeader();
  presponse->fromAFile(httpSocket);
  if (pCookieJar)
    pCookieJar->parse(request, *presponse);

  //a_Log it
  pLuaContext->useEventVisitor().startEvent(*presponse, AEventVisitor::EL_INFO);

  //a_Add response header to the objects
  pLuaContext->useObjects().insert(objectname+".response", presponse, true);

  //a_Return status code
  lua_pushinteger(L, presponse->getStatusCode());

  //a_Return HTTP response document
  ARope *pbody = new ARope();
  size_t contentLength = presponse->getContentLength();
  if (contentLength != AConstant::npos)
  {
    httpSocket.read(*pbody, contentLength);
  }
  else
  {
    pbody->append("ERROR: Content-Length not provided in response header.");
  }
  pLuaContext->useObjects().insert(objectname+".body", pbody, true);

  //a_Log it
  pLuaContext->useEventVisitor().startEvent(*pbody, AEventVisitor::EL_DEBUG);
  
  return 1;
}

static const luaL_Reg web_funcs[] = {
  {"HttpGet", web_HttpGet},
  {"HttpPost", web_HttpPost},
  {NULL, NULL}
};

LUALIB_API int luaopen_web(lua_State *L)
{
  luaL_register(L, "web", web_funcs);
  return 1;
}
