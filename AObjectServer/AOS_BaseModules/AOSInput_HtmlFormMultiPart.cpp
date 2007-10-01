#include "pchAOS_BaseModules.hpp"
#include "AOSInput_HtmlFormMultiPart.hpp"

const AString& AOSInput_HtmlFormMultiPart::getClass() const
{
  static const AString CLASS("multipart/form-data");
  return CLASS;
}

AOSInput_HtmlFormMultiPart::AOSInput_HtmlFormMultiPart(ALog& alog) :
  AOSInputProcessorInterface(alog)
{
}

bool AOSInput_HtmlFormMultiPart::execute(AOSContext& context)
{
  AString strLength;
  if (context.useRequestHeader().find(AHTTPHeader::HT_ENT_Content_Length, strLength))
  {
    //a_Get the boundary string
    AString strBoundary;
    context.useRequestHeader().find(AHTTPHeader::HT_ENT_Content_Type, strBoundary);
    size_t pos = strBoundary.find(ASW("boundary=", 9));
    if (AConstant::npos == pos)
    {
      context.useEventVisitor().set(ASWNL("AOSInput_HtmlFormMultiPart: boundary not found"), true);
      return false;
    }

    strBoundary.remove(pos+7);
    strBoundary.set('-', 0);
    strBoundary.set('-', 1);

    //a_Discard the leading --
    AString str(1024, 1024);
    size_t linelen = context.useSocket().readUntil(str, strBoundary, true, true);
    AASSERT(this, !linelen);
    str.clear();
    linelen = context.useSocket().readLine(str);  //a_Remove CRLF after boundary
    AASSERT(this, !linelen);

    int part = 0;
    ANameValuePair pair(ANameValuePair::FORM_MULTIPART);
    while(context.useSocket().isNotEof())
    {
      //a_Read MIME header info until blank line
      str.clear();
      pair.clear();
      if (AConstant::npos != (linelen = context.useSocket().readLine(str)))
      {
        //a_Empty line encountered
        if (0 == linelen)
        {
          //a_From here until boundary is pure data of this part
          context.useSocket().readUntil(str, strBoundary, true, true);
          if (str.getSize() > 2)
          {
            //a_remove trailing CRLF which are added right before the boundary
            if ('\r' == str.at(str.getSize()-2))
              str.rremove(2);
          }
          AXmlElement& e = context.useOutputRootXmlElement().addElement(ASW("/request/data/part.",19)+AString::fromInt(part)+ASW("/data",5), str, AXmlData::Base64);
          e.addAttribute(ASW("length", 6), AString::fromSize_t(str.getSize()));
          e.addAttribute(ASW("encoding", 8), ASW("base64", 6));

          //a_Remove trailing CRLF or --CRLF (if EOM) after boundary
          str.clear();
          linelen = context.useSocket().readLine(str);  //a_Remove CRLF after boundary
          
          if (str.equals("--",2))
          {
            //a_End of the submit
            break;
          }
          ++part;
          continue;
        }

        //a_Boundary found break out and start reading next part
        if (AConstant::npos != str.find(strBoundary))
        {
          ++part;
          continue;
        }

        pos = 0;
        AString xpath("/request/data/part.",19);
        xpath.append(AString::fromInt(part));
        if (AConstant::npos != (pos = str.findNoCase(ASW("Content-Type:", 13))))
        {
          //a_Content type for this block
          str.removeUntilOneOf();
          context.useOutputRootXmlElement().addElement(xpath+ASW("/content-type",13), str);
        }
        else if (AConstant::npos != (pos = str.findNoCase(ASW("Content-Disposition:", 20))))
        {
          //a_Content disposition for this block
          pos += 21;  //a_Include trainling space
          while (pos < str.getSize())
          {
            //a_Parse pair in the line
            pair.parse(str, pos);

            if (pair.getName().equals(ASW("name",4)))
            {
              //a_Add reference to this as name=$(/request/data/part.N)
              context.useRequestParameterPairs().insert(pair.getValue(), ARope("$(", 2)+xpath+ASW(")", 1));
            }
          }

          str.removeUntilOneOf();
          context.useOutputRootXmlElement().addElement(xpath+ASW("/content-disposition",20), str);
        }
        str.clear();
      }
      else
        break;
    }
  }
  else
  {
    //a_411 Length Required
    context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_411_Length_Required);
    return false;
  }

  return true;
}
