#include "AFile_Physical.hpp"
#include "gdIOCtx_AFile.hpp"

int main()
{
  gdImagePtr im;
  im = gdImageCreateTrueColor (200, 100);
  char *error = gdImageStringFTCircle (im,
			 100, // x
			 100, // y
			 100, //radius
			 60,  //radius/2
			 0.9,
       "c:\\WINDOWS\\fonts\\ARIAL.TTF",
			 26,
			 "HFAPOISD",
			 "",
			 gdTrueColorAlpha (142, 190, 255, 32));
  if (error) 
  {
    std::cout << "gdImageStringFTEx error: " << error << std::endl;
  }
  else
  {
    AFile_Physical myfile("output.png", "wb");
    gdIOCtx_AFile ioCtx(myfile);
    myfile.open();

    gdImagePngCtx(im, ioCtx.useContextPtr());
  }

  return 0;
}