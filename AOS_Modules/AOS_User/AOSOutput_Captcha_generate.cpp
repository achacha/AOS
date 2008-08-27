/*
Written by Alex Chachanashvili

$Id: AOSOutput_generate_image.cpp 95 2008-06-09 21:27:46Z achacha $
*/
#include "pchAOS_User.hpp"
#include "AOSOutput_captcha_generate.hpp"
#include "AGdCanvas.hpp"
#include "ATextGenerator.hpp"
#include "ADigest_SHA1.hpp"

const AString& AOSOutput_Captcha_generate::getClass() const
{
  static const AString CLASS("Captcha.generate");
  return CLASS;
}

AOSOutput_Captcha_generate::AOSOutput_Captcha_generate(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_Captcha_generate::execute(AOSContext& context)
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
  AFilename filename(m_Services.useConfiguration().getAosBaseDataDirectory(), fontPath, false);
  fontPath.clear();
  filename.emit(fontPath);

  //a_Text for captcha
  AString captcha;
  ATextGenerator::generateRandomWord(captcha, 5);
  
  //a_Get SHA-1 digest of the captcha
  ADigest_SHA1 digest;
  digest.update(captcha);
  digest.finalize();
  context.useSessionData().useData().overwriteElement(AOS_User_Constants::SESSION_CAPTCHA).setData(digest);

  //a_Some position and size randomness
  int fontsize=ARandomNumberGenerator::get().nextRange(36, 12);
  int px=ARandomNumberGenerator::get().nextRange(40, 5);
  int py=ARandomNumberGenerator::get().nextRange(y-1-fontsize, fontsize+1);
  double angle=double(ARandomNumberGenerator::get().nextRange(20, -20))/100;

  int rcolor = ARandomNumberGenerator::get().nextRange(255, 160);
  int gcolor = ARandomNumberGenerator::get().nextRange(255, 160);
  int bcolor = ARandomNumberGenerator::get().nextRange(255, 160);

  //a_Build the image
  int brect[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
  char *err;
  AGdCanvas canvas(x,y);
  if (err = gdImageStringTTF(
    canvas.useGdImagePtr(), 
    brect, 
    gdTrueColor(rcolor, gcolor, bcolor), 
    fontPath.startUsingCharPtr(), 
    fontsize, 
    angle, 
    px, 
    py, 
    captcha.startUsingCharPtr()
  )) 
  { 
    context.addError(getClass(), AString(err)); return AOSContext::RETURN_ERROR;
  }

  canvas.emitPNG(context.useOutputBuffer());
  context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("image/png",9));
  return AOSContext::RETURN_OK;
}
