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

bool AOSOutput_generate_image::execute(AOSOutputContext& context)
{
  int x,y;
  AString str;
  if (!context.getOutputParams().emitFromPath(ASW("canvas/x", 8), str))
  {
    m_Services.useLog().append("AOSOutput_generate_image: Unable to find '/output/canvas/x' parameter");
    ATHROW_EX(this, AException::InvalidParameter, ASWNL("AOSOutput_generate_image requires '/output/canvas/x' parameter"));
  }
  x = str.toInt();
  str.clear();
  if (!context.getOutputParams().emitFromPath(ASW("canvas/y", 8), str))
  {
    m_Services.useLog().append("AOSOutput_generate_image: Unable to find '/output/canvas/y' parameter");
    ATHROW_EX(this, AException::InvalidParameter, ASWNL("AOSOutput_generate_image requires '/output/canvas/y' parameter"));
  }
  y = str.toInt();

  AGdCanvas canvas(x,y);
  canvas.drawLine(0, 0, x-1, y-1, gdTrueColor(200,100,250));
  canvas.drawLine(x-1, 0, 0, y-1, gdTrueColor(200,100,250));

  canvas.emitPNG(context.useOutputBuffer());
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("image/png",9));
  return true;
}

