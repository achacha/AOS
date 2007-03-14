#include "AString.hpp"
#include "AXmlElement.hpp"
#include "AFile_AString.hpp"
#include "AXmlDocument.hpp"
#include "ATextGenerator.hpp"
#include "ARandomNumberGenerator.hpp"

const char xml_data_small[] = "\
<?xml version = \"1.0\"?>\
<xml id=\"EQInterfaceDefinitionLanguage\">\
	<Schema xmlns = \"EverQuestData\" xmlns:dt = \"EverQuestDataTypes\"/>\
	<TextColor>\
		<R>240</R>\
		<G>240</G>\
		<B>240</B>\
	</TextColor>\
</xml>\
";

const char xml_data_big[] ="\
<?xml version = \"1.0\"?>\
<XML ID = \"EQInterfaceDefinitionLanguage\">\
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

void testParse()
{
	AXmlElement exml;
	AString str;
	u4 pos = 0;


/*  exml.parse(xml_data_small, pos);
	exml.recurseToString(str);
	std::cout << str.c_str() << std::endl << std::endl;

	pos = 0;
	exml.clear();
	exml.parse(xml_data_big, pos);
	exml.recurseToString(str, 0);
	std::cout << str.c_str() << std::endl << std::endl;
*/
}

void testPathAdd()
{
  AXmlElement elem("root");
  elem.addElement("/path0/path1/obj0", "value0");
  elem.addElement("/path0/path2/obj1", "value1");
  elem.addElement("/path0/path1/obj2", "value2");

  AString str(8188, 1024);
  elem.emitXml(str, 0);
  std::cout << str << std::endl;
  elem.debugDump();

  AXmlNode *p;

  if (p = elem.find("/root/path0/path1/"))
  {
    p->useAttributes().insert("attrib0", "0");
    p->useAttributes().insert("attrib1", "0");
    p->useAttributes().insert("attrib2", "0");
    p->addContent(new AXmlElement("foo"));
    p->addContent(new AXmlData("more data"));
    p->addComment("This is the comment");

    str.clear();
    elem.emitXml(str, 0);
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
  restored.emitXml(str, 0);
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
  elem.emitXml(str0);
  std::cout << str0 << std::endl;
}

void testRandomXml()
{
  AXmlDocument doc("root");
  AString xpath(256, 128);
  for (int i=0; i<80; ++i)
  {
    xpath.assign('/');
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
  doc.emitXml(output, 0);
  std::cout << output << std::endl;
}

void testMultiAdd()
{
  AXmlElement e("root");
  e.addElement("/base/path0/obj0", "v");
  e.addElement("/base/path0/obj1", "v");
  e.addElement("/base/path0/obj2", "v");
  e.addElement("/base/path0/obj3", "v");
  e.addElement("/base/path0/obj3", "v");

  AString str0(8188, 1024);
  e.emitXml(str0,0);
  std::cout << str0 << std::endl;
}

void testXmlEmit()
{
  AXmlElement e("root");
  e.addElement("/base/path0/obj0", "v0").addData("foo0");
  e.addElement("/base/path0/obj1", "v1");
  e.addElement("/base/path0/obj2", "v2").addData("foo1");
  e.addElement("/base/path0/obj3", "v3");
  e.addElement("/base/path0/obj3", "v4");
  e.addElement("/base/path1/obj0", "v10");
  e.addElement("/base/path1/obj1", "v11").addData("foo2", AXmlData::CDataDirect);
  e.addElement("/base/path2/obj0", "v20");
  e.addElement("/base/path2/obj1", "v21").addData("foo3", AXmlData::CDataSafe);

  AString str;
  e.emitXml(str,0);
  std::cout << str << std::endl;

  AXmlElement target("newroot");
  AXmlNode *pNode = e.find("/root/base/path2");
  if (pNode)
  {
    pNode->emit(target);
    str.clear();
    target.emitXml(str,0);
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
  e.emitXml(str,0);
  std::cout << str << std::endl;
  
  AXmlElement newroot("nr");
  newroot.addContent(e.clone());
  
  str.clear();
  newroot.emitXml(str,0);
  std::cout << str << std::endl;
}

int main()
{
  //testParse();
  //testPathAdd();
  //testOutput();
  //testRandomXml();
  //testMultiAdd();
  //testXmlEmit();
  testClone();

	return 0;
}
