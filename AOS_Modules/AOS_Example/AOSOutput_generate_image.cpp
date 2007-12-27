#include "pchAOS_Example.hpp"
#include "AOSOutput_generate_image.hpp"
#include "AGdCanvas.hpp"

const AString& AOSOutput_generate_image::getClass() const
{
  static const AString CLASS("GenerateImage");
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

  AGdCanvas canvas(x,y);
  canvas.drawLine(0, 0, x-1, y-1, gdTrueColor(200,100,250));
  canvas.drawLine(x-1, 0, 0, y-1, gdTrueColor(200,100,250));

  canvas.emitPNG(context.useOutputBuffer());
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("image/png",9));
  return AOSContext::RETURN_OK;
}

