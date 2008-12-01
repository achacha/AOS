/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Example.hpp"
#include "AOSOutput_generate_image.hpp"
#include "AGdCanvas.hpp"

const AString AOSOutput_generate_image::CLASS("Example.generateImage");

const AString& AOSOutput_generate_image::getClass() const
{
  return CLASS;
}

AOSOutput_generate_image::AOSOutput_generate_image(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
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

  AString fontPath;
  if (!context.getOutputParams().emitString(ASW("fontpath", 8), fontPath))
  {
    context.addError(getClass(), ASWNL("Unable to find '/output/fontpath' parameter"));
    return AOSContext::RETURN_ERROR;
  }
  
  //a_Get the text to paste onto image
  AString strText;
  context.useRequestParameterPairs().get(ASW("text",4), strText);
  
  
  AFilename filename(m_Services.useConfiguration().getAosBaseDataDirectory(), fontPath, false);
  fontPath.clear();
  filename.emit(fontPath);

  //a_Build the image
  AGdCanvas canvas(x,y);
  canvas.drawLine(0, 0, x-1, y-1, gdTrueColor(200,100,250));
  canvas.drawLine(x-1, 0, 0, y-1, gdTrueColor(200,100,250));

  //a_Note that the silly C API actually wants (char *) even though (const char *) is more appropriate
  int brect[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
  char *err;
  if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(220,100,220), fontPath.startUsingCharPtr(), 12, 0.0, 20, 20, "This")) { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }
  if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(220,220,100), fontPath.startUsingCharPtr(), 12, -0.1, 20, 40, "is a")) { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }
  if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(100,220,220), fontPath.startUsingCharPtr(), 12, -0.2, 20, 60, "test!")) { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }
  
  if (strText.getSize())
    if (err = gdImageStringTTF(canvas.useGdImagePtr(), brect, gdTrueColor(250,250,250), fontPath.startUsingCharPtr(), 20, 0.0, 70, 50, strText.startUsingCharPtr())) 
      { context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR; }

  canvas.emitPNG(context.useOutputBuffer());
  context.useResponseHeader().set(AHTTPHeader::HT_ENT_Content_Type, ASW("image/png",9));
  return AOSContext::RETURN_OK;
}

