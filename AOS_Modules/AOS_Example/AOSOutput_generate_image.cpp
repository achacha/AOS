#include "pchAOS_Example.hpp"
#include "AOSOutput_generate_image.hpp"
#include "AGdCanvas.hpp"

const AString& AOSOutput_generate_image::getClass() const
{
  static const AString CLASS("GenerateImage");
  return CLASS;
}

AOSOutput_generate_image::AOSOutput_generate_image(ALog& alog) :
  AOSOutputGeneratorInterface(alog)
{
}

bool AOSOutput_generate_image::execute(AOSOutputContext& context)
{
  int x,y;
  AString str;
  if (!context.getOutputParams().emitFromPath(ASW("/params/output/canvas/x", 23), str))
  {
    m_Log.append("AOSOutput_generate_image: Unable to find '/params/output/canvas/x' parameter");
    ATHROW_EX(this, AException::InvalidParameter, "AOSOutput_generate_image requires '/params/output/canvas/x' parameter");
  }
  x = str.toInt();
  str.clear();
  if (!context.getOutputParams().emitFromPath(ASW("/params/output/canvas/y", 23), str))
  {
    m_Log.append("AOSOutput_generate_image: Unable to find '/params/output/canvas/y' parameter");
    ATHROW_EX(this, AException::InvalidParameter, "AOSOutput_generate_image requires '/params/output/canvas/y' parameter");
  }
  y = str.toInt();

  AGdCanvas canvas(x,y);
  canvas.drawLine(0, 0, x-1, y-1, gdTrueColor(200,100,250));
  canvas.drawLine(x-1, 0, 0, y-1, gdTrueColor(200,100,250));

  canvas.emitPNG(context.useOutputBuffer());
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("image/png",9));
  return true;
}

