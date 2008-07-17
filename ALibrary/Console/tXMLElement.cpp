#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "ATextGenerator.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AFilename.hpp"

const char xml_data_small[] = "\
<root>\
  <!-- url>http://</url -->\
  <!--0123456789-->\
	<Empty/>\
</root>\
";

const char xml_doc_small[] = "\
<?xml version = \"1.0\"?>\
<xml id=\"EQInterfaceDefinitionLanguage\">\
  <Schema xmlns = \"EverQuestData\" xmlns:dt = \"EverQuestDataTypes\"/>\
  <!--comment-->\
  <TextColor>\
		<R>240</R>\
		<G>240</G>\
		<B>240</B>\
	</TextColor>\
</xml>\
";

const char xml_config[]= "\
<root>\
  <http>\
   <port>80</port>\
   <host>one</host>\
 </http>\
  <http>\
   <port>81</port>\
   <host>two</host>\
 </http>\
  <http>\
   <port>82</port>\
   <host>three</host>\
 </http>\
</root>\
";

const char xml_data_big[] ="\
<?xml version = \"1.0\"?>\
<!-- root level comment -->\
<?xml stylesheet=\"acb\"?>\
<XML ID = \"EQInterfaceDefinitionLanguage\">\
  <!-- first level comment -->\
	<Schema xmlns = \"EverQuestData\" xmlns:dt = \"EverQuestDataTypes\"/>\
	<Gauge item = \"Casting_Gauge\">\
		<ScreenID>Gauge</ScreenID>\
		<!--<Font>3</Font> -->\
		<RelativePosition>true</RelativePosition>\
		<Location>\
			<X>0</X>\
			<Y>2</Y>\
		</Location>\
		<Size>\
			<CX>116</CX>\
			<CY>8</CY>\
		</Size>\
		<GaugeOffsetY>0</GaugeOffsetY>\
		<Style_VScroll>false</Style_VScroll>\
		<Style_HScroll>false</Style_HScroll>\
		<Style_Transparent>false</Style_Transparent>\
		<!--<TooltipReference/>-->\
		<FillTint>\
				<R>240</R>\
				<G>0</G>\
				<B>240</B>\
		</FillTint>\
		<LinesFillTint>\
				<R>0</R>\
				<G>0</G>\
				<B>0</B>\
		</LinesFillTint>\
		<DrawLinesFill>false</DrawLinesFill>\
		<EQType>7</EQType>\
		<GaugeDrawTemplate>\
			<Background>A_GaugeBackground</Background>\
			<Fill>A_GaugeFill</Fill>\
			<Lines>A_GaugeLines</Lines>\
			<LinesFill>A_GaugeLinesFill</LinesFill>\
			<EndCapLeft>A_GaugeEndCapLeft</EndCapLeft>\
			<EndCapRight>A_GaugeEndCapRight</EndCapRight>\
		</GaugeDrawTemplate>\
	</Gauge>\
	<Screen item = \"CastingWindow\">\
		<!--<ScreenID/>-->\
		<!--<Font/>-->\
		<RelativePosition>false</RelativePosition>\
		<Location>\
			<X>290</X>\
			<Y>50</Y>\
		</Location>\
		<Size>\
			<CX>128</CX>\
			<CY>38</CY>\
		</Size>\
		<Text>Casting Time</Text>\
		<Style_VScroll>false</Style_VScroll>\
		<Style_HScroll>false</Style_HScroll>\
		<Style_Transparent>false</Style_Transparent>\
		<TooltipReference>The Breath Meter</TooltipReference>\
		<DrawTemplate>WDT_Rounded</DrawTemplate>\
		<Style_Titlebar>true</Style_Titlebar>\
		<Style_Closebox>false</Style_Closebox>\
		<Style_Minimizebox>false</Style_Minimizebox>\
		<Style_Border>true</Style_Border>\
		<Style_Sizable>false</Style_Sizable>\
		<Pieces>Casting_Gauge</Pieces>\
	</Screen>\
	</XML>\
	";

const char xml_small_single_quote[] ="<?xml version=\"1.0\" encoding=\"utf-8\" ?><module class='Test' name='start'/>";

const char xml_single_quote[] ="\
<?xml version=\"1.0\" encoding=\"utf-8\" ?>\
<controller name='TestFail'>\
	<input/>\
	<module class='Test' name='start'>\
		<name>foo</name>\
		<value>bar</value>\
	</module>\
	<module class='TestFail' name='work'>\
		<name>red</name>\
		<value>pink</value>\
	</module>\
	<module class='Test' name='end'>\
		<name>blue</name>\
		<value>teal</value>\
	</module>\
</controller>\
";

void testParse()
{
  AFile_AString strfile;

  //AXmlElement exml;
  //strfile.useAString().assign(xml_data_small);
  //exml.fromAFile(strfile);
  //std::cout << exml << "\r\n" << std::endl;

	AXmlDocument xdoc("root");
//  strfile.useAString().assign(xml_data_big);
  strfile.useAString().assign(xml_small_single_quote);
  strfile.reset();
  xdoc.fromAFile(strfile);

  xdoc.debugDump();

  std::cout << xdoc << std::endl;
}

void testPathAdd()
{
  AXmlElement elem("root");
  elem.addElement("path0/path1/obj0").addData("value0");
  elem.addElement("path0/path2/obj1").addData("value1");
  elem.addElement("path0/path1/obj2").addData("value2");

  AString str(8188, 1024);
  elem.emit(str, 0);
  std::cout << str << std::endl;
  elem.debugDump();

  AXmlElement *p;

  if (p = elem.findElement("/root/path0/path1/"))
  {
    p->useAttributes().insert("attrib0", "0");
    p->useAttributes().insert("attrib1", "0");
    p->useAttributes().insert("attrib2", "0");
    p->addContent(new AXmlElement("foo"));
    p->addContent(new AXmlData(ASWNL("more data")));
    p->addComment("This is the comment");

    str.clear();
    elem.emit(str, 0);
    std::cout << str << std::endl;
  }
  else
    std::cerr << "/root/path0/path1/ not found" << std::endl;

  AFile_AString strfile;
  elem.toAFile(strfile);

  std::cout << strfile.useAString() << std::endl;

  AXmlElement restored;
  restored.fromAFile(strfile);
  str.clear();
  restored.emit(str, 0);
  std::cout << str << std::endl;
}

void testOutput()
{
  AXmlElement elem("root");
  elem.addAttribute("a1", "foo");
  elem.addAttribute("a2", "foo");
  elem.addAttribute("a3", "foo");
  elem.addElement("path0").addElement("path1").addElement("path2").addComment("My comment");
  AString str0(8188, 1024), str1(8188, 1024);
  elem.emit(str0);
  std::cout << str0 << std::endl;
}

void testRandomXml()
{
  AXmlDocument doc("root");
  AString xpath(256, 128);
  for (int i=0; i<80; ++i)
  {
    int level = ARandomNumberGenerator::get().nextRange(6, 2);
    for (int j=1; j<level; ++j)
    {
      ATextGenerator::generateRandomWord(xpath, ARandomNumberGenerator::get().nextRange(2,1));
      xpath.append('/');
    }
    ATextGenerator::generateRandomWord(xpath, ARandomNumberGenerator::get().nextRange(2,1));
    
    AString value;
    ATextGenerator::generateRandomWord(value, ARandomNumberGenerator::get().nextRange(12,1));
    doc.useRoot().addElement(xpath, value);
  }

  doc.addComment("This is header comment");

  AString output(10240, 2048);
  doc.emit(output, 0);
  std::cout << output << std::endl;
}

void testMultiAdd()
{
  AXmlElement e("root");
  e.addElement("base/path0/obj0").addData("v");
  e.addElement("base/path0/obj1").addData("v");
  e.addElement("base/path0/obj2").addData("v");
  e.addElement("base/path0/obj3").addData("v");
  e.addElement("base/path0/obj3").addData("v");

  AString str0(8188, 1024);
  e.emit(str0,0);
  std::cout << str0 << std::endl;
}

void testEmitClone()
{
  AXmlElement e("root");
  e.addElement(ASWNL("base/path2/obj0"), ASWNL("a0"));
  e.addElement(ASWNL("base/path2/obj1"), ASWNL("b1"));

  AString str;
  AXmlElement target("newroot");
  AXmlElement *pNode = e.findElement("/root/base/path2");
  if (pNode)
  {
    pNode->emitXml(target);
    str.clear();
    target.emit(str,0);
  }

  e.debugDump();
  target.debugDump();
  std::cout << str << std::endl;
}

void testXmlEmit()
{
  AXmlElement e("root");
  e.addElement(ASWNL("base/path0/obj0"), ASWNL("v0")).addData(ASWNL("foo0"));
  e.addElement(ASWNL("base/path0/obj1"), ASWNL("v1"));
  e.addElement(ASWNL("base/path0/obj2"), ASWNL("v2")).addData(ASWNL("foo1"));
  e.addElement(ASWNL("base/path0/obj3"), ASWNL("v3"));
  e.addElement(ASWNL("base/path0/obj3"), ASWNL("v4"));
  e.addElement(ASWNL("base/path1/obj0"), ASWNL("v10"));
  e.addElement(ASWNL("base/path1/obj1"), ASWNL("v11")).addData(ASWNL("foo2"), AXmlElement::ENC_CDATADIRECT);
  e.addElement(ASWNL("base/path2/obj0"), ASWNL("v20"));
  e.addElement(ASWNL("base/path2/obj1"), ASWNL("v21")).addData(ASWNL("foo3"), AXmlElement::ENC_CDATASAFE);

  AString str;
  e.emit(str,0);
  std::cout << str << std::endl;

  AXmlElement target("newroot");
  AXmlElement *pNode = e.findElement("/root/base/path2");
  if (pNode)
  {
    pNode->emitXml(target);
    str.clear();
    target.emit(str,0);
    std::cout << str << std::endl;
  }

  target.debugDump();
}

void testClone()
{
  AString str;
  AFile_AString strfile("<phrase><![CDATA[ My name is Enigo Montoya, you have killed my father, prepare to die! ]]></phrase>");

  AXmlElement e;
  e.fromAFile(strfile);
  e.emit(str,0);
  std::cout << str << std::endl;
  
  AXmlElement newroot("nr");
  newroot.addContent(e.clone());
  
  str.clear();
  newroot.emit(str,0);
  std::cout << str << std::endl;
}

void testAppend()
{
  AXmlElement e("root");
  e.addElement("dir00/dir01").addData("1");
  e.addElement("dir00/dir02").addData("2");
  e.addElement("dir00/dir03").addData("3");
  e.addElement("dir00/dir01").addData("4");

  AString str;
  e.emit(str,0);
  std::cout << str << std::endl;
}

void testFind1()
{
  AFile_AString f;
  f.useAString().assign(xml_doc_small);

  AXmlDocument doc(f);

  AXmlElement *pNode = doc.useRoot().findElement("/xml/TextColor");
  if (pNode)
  {
    AString str;
    pNode->emit(str,0);
    std::cout << str << std::endl;

    AXmlElement *pR = pNode->findElement("R");
    if (pR)
    {
      str.clear();
      pR->emit(str,0);
      std::cout << str << std::endl;
    }
    else
      std::cout << "Not found." << std::endl;
  }
  else
    std::cout << "Not found." << std::endl;
}

void testFind2()
{
  AFile_AString f;
  f.useAString().assign(xml_config);

  AXmlDocument doc(f);

  AXmlElement::CONST_CONTAINER nodes;
  doc.useRoot().find("/root/http", nodes);
  if (nodes.size() > 0)
  {
    AString str;
    AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin();
    while (cit != nodes.end())
    {
      AString str("port=");
      (*cit)->emitContentFromPath("port", str);
      str.append("  host=");
      (*cit)->emitContentFromPath("host", str);
      std::cout << str << std::endl;
      ++cit;
    }
  }
  else
    std::cout << "Not found." << std::endl;
}

void testFind3()
{
  AFile_AString f;
  f.useAString().assign(xml_config);

  AXmlDocument doc(f);

  AXmlElement::CONST_CONTAINER nodes;
  doc.useRoot().find("/root/http", nodes);
  if (nodes.size() > 0)
  {
    AString str;
    AXmlElement::CONST_CONTAINER::const_iterator cit = nodes.begin();
    while (cit != nodes.end())
    {
      std::cout << (*cit)->getName() << std::endl;
      ++cit;
    }
  }
  else
    std::cout << "Not found." << std::endl;
}

void testInsertWithPath()
{
  AXmlElement element("root");
  element.addElement("level-0").addElement("level-1").addAttribute("foo","bar");

  {
    AString str;
    AXmlElement base("base");
    base.addContent(element.clone());
    base.emit(str, 0);
    std::cout << str << std::endl;
  }

  {
    AString str;
    AXmlElement base("base");
    base.addContent(element.clone(), "/");
    base.emit(str, 0);
    std::cout << str << std::endl;
  }

  {
    AString str;
    AXmlElement base("base");
    base.addContent(element.clone(), "some/new/path");
    base.emit(str, 0);
    std::cout << str << std::endl;
  }

  {
    AString str;
    AXmlElement base("base");
    base.addContent(element.clone(), "/some/new/path/");
    base.emit(str, 0);
    std::cout << str << std::endl;
  }
}

void testFilenameEmit()
{
  AFilename f(ASWNL("c:/aaa/bbb/ccc/d.txt"), false);
  AXmlElement e("fn");
  f.emitXml(e);
  AString str;
  e.emit(str);
  std::cout << str << std::endl;
  return;
}

int main()
{
  testParse();
//  testPathAdd();
//  testOutput();
//  testRandomXml();
//  testMultiAdd();
//  testEmitClone();
//  testXmlEmit();
//  testClone();
//  testAppend();
//  testFind1();
//  testFind2();
//  testFind3();
//  testInsertWithPath();
//  testFilenameEmit();

	return 0;
}
