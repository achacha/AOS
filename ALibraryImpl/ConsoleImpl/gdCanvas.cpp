#include "AFile_Physical.hpp"
#include "AGdCanvas.hpp"

int main()
{
  AGdCanvas canvas(100,100);

  char *error = gdImageStringFTCircle(
    canvas.useGdImagePtr(),
    50,
    50,
    50,
    50,
    0.8,
    "C:\\WINNT\\Fonts\\COMIC.TTF",
    24,
    "top text",
    "bottom text",
    gdTrueColorAlpha (192, 250, 255, 32)
  );

  canvas.drawLine(0,0,99,99,gdTrueColor(200,100,250));

  {
    AFile_Physical file("test_output.jpg", "wb");
    file.open();
    canvas.writeJPEG(file);
    file.close();
  }

  {
    AFile_Physical file("test_output.gif", "wb");
    file.open();
    canvas.writeGIF(file);
    file.close();
  }

  {
    AFile_Physical file("test_output.png", "wb");
    file.open();
    canvas.writePNG(file);
    file.close();
  }

  {
    ARope rope;
    canvas.emitPNG(rope);
    //rope.debugDump();

    AFile_Physical file("test_output_rope.png", "wb");
    file.open();
    rope.emit(file);
  }

  if (error) 
  {
    std::cout << "gdImageStringFTEx error: " << error << std::endl;
  }
  
  return 0;
}
