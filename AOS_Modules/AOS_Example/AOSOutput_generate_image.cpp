/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Example.hpp"
#include "AOSOutput_generate_image.hpp"
#include "AGdCanvas.hpp"

const AString& AOSOutput_generate_image::getClass() const
{
  static const AString CLASS("Example.generateImage");
  return CLASS;
}

AOSOutput_generate_image::AOSOutput_generate_image(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

void AOSOutput_generate_image::init()
{
  if (!m_Services.useConfiguration().useConfigRoot().emitContentFromPath(ASWNL("AOS_Example/freetype/fontpath"), m_FontPath))
    m_FontPath.assign("C:/WINDOWS/Fonts/ARIAL.TTF");
}

AOSContext::ReturnCode AOSOutput_generate_image::execute(AOSContext& context)
{
  int x,y;
  AString str;
  if (!context.getOutputParams().emitString(ASW("canvas/x", 8), str))
  {
    context.addError(getClass(), ASWNL("Unable to find '/output/canvas/x' parameter"));
    return AOSContext::RETURN_ERROR;
  }
  x = str.toInt();
  str.clear();
  if (!context.getOutputParams().emitString(ASW("canvas/y", 8), str))
  {
    context.addError(getClass(), ASWNL("Unable to find '/output/canvas/y' parameter"));
    return AOSContext::RETURN_ERROR;
  }
  y = str.toInt();

  //a_Build the image
  AGdCanvas canvas(x,y);
  canvas.drawLine(0, 0, x-1, y-1, gdTrueColor(200,100,250));
  canvas.drawLine(x-1, 0, 0, y-1, gdTrueColor(200,100,250));

  //a_Note that the silly C API actually wants (char *) even though (const char *) is more appropriate
  int brect[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
  char *err;
  if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(250,200,250), m_FontPath.startUsingCharPtr(), 12, 0.0, 20, 20, "This")) { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }
  if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(250,250,200), m_FontPath.startUsingCharPtr(), 12, -0.1, 20, 40, "is a")) { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }
  if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(200,250,250), m_FontPath.startUsingCharPtr(), 12, -0.2, 20, 60, "test!")) { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }

  canvas.emitPNG(context.useOutputBuffer());
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("image/png",9));
  return AOSContext::RETURN_OK;
}

