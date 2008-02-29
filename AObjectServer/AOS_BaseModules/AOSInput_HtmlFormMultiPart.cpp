#include "pchAOS_BaseModules.hpp"
#include "AOSInput_HtmlFormMultiPart.hpp"

const AString& AOSInput_HtmlFormMultiPart::getClass() const
{
  static const AString CLASS("multipart/form-data");
  return CLASS;
}

AOSInput_HtmlFormMultiPart::AOSInput_HtmlFormMultiPart(AOSServices& services) :
  AOSInputProcessorInterface(services)
{
}

AOSContext::ReturnCode AOSInput_HtmlFormMultiPart::execute(AOSContext& context)
{
  AString strLength;
  if (context.useRequestHeader().getPairValue(AHTTPHeader::HT_ENT_Content_Length, strLength))
  {
    //a_Get the boundary string
    AString strBoundary;
    context.useRequestHeader().getPairValue(AHTTPHeader::HT_ENT_Content_Type, strBoundary);
    size_t pos = strBoundary.find(ASW("boundary=", 9));
    if (AConstant::npos == pos)
    {
      context.addError(getClass(), ASW("AOSInput_HtmlFormMultiPart: boundary not found",46));
      return AOSContext::RETURN_ERROR;
    }

    strBoundary.remove(pos+7);
    strBoundary.set('-', 0);
    strBoundary.set('-', 1);

    //a_Discard the leading --
    AString str(1024, 1024);
    AString strName;
    size_t linelen = context.useSocket().readUntil(str, strBoundary, true, true);
    AASSERT(this, !linelen);
    str.clear();
    linelen = context.useSocket().readLine(str);  //a_Remove CRLF after boundary
    AASSERT(this, !linelen);

    int part = 0;
    ANameValuePair pair(ANameValuePair::FORM_MULTIPART);
    AXmlElement& eInput = context.useModel().overwriteElement(ASW("input",5));
    AXmlElement *pePart = NULL;
    bool isFilename = false;
    AString strFilename;
    while(context.useSocket().isNotEof())
    {
      //a_Build object name
      AString strObjectName("multipart.",10);
      strObjectName.append(AString::fromInt(part));

      //a_Read MIME header info until blank line
      str.clear();
      if (AConstant::npos != (linelen = context.useSocket().readLine(str)))
      {
        //a_Empty line encountered
        if (0 == linelen)
        {
          AASSERT(&context, pePart);
          AASSERT(&context, !strName.isEmpty());

          //a_From here until boundary is pure data of this part
          AAutoPtr<AString> pData(new AString());
          context.useSocket().readUntil(*pData, strBoundary, true, true);
          if (pData->getSize() > 2)
          {
            //a_remove trailing CRLF which are added right before the boundary
            if ('\r' == pData->at(pData->getSize()-2))
              pData->rremove(2);
          }
                    
          //a_Create element with name and properties that can be used as lookup if needed
          pePart->addElement(ASW("length", 6)).addData(AString::fromSize_t(pData->getSize()));

          //a_Add the data for this multipart to the context objects
          if (isFilename)
          {
            //a_If file type submitted is blank (not selected by user) it will not have a filename
            if (!strFilename.isEmpty())
            {
              pePart->addElement(ASW("context-object-name",19), strObjectName);
              context.useRequestParameterPairs().insert(strName, strObjectName);
              context.useContextObjects().insert(strObjectName, pData.use(), true, true);
              pData.setOwnership(false);
            }
            else
              AASSERT(&context, pData->getSize() <= 2);  // Usually empty line when no filename is provided
          }
          else
          {
            //a_Add to request query
            context.useRequestParameterPairs().insert(strName, *pData);
          }

          //a_Remove trailing CRLF or --CRLF (if EOM) after boundary
          str.clear();
          linelen = context.useSocket().readLine(str);  //a_Remove CRLF after boundary
          
          if (str.equals("--",2))
          {
            //a_End of the submit
            break;
          }
          ++part;
          strName.clear();
          pePart = NULL;
          isFilename = false;
          strFilename.clear();
          continue;
        }

        //a_Boundary found break out and start reading next part
        if (AConstant::npos != str.find(strBoundary))
        {
          ++part;
          strName.clear();
          pePart = NULL;
          isFilename = false;
          strFilename.clear();
          continue;
        }

        pos = 0;
        if (AConstant::npos != (pos = str.findNoCase(ASW("Content-Type:", 13))))
        {
          AASSERT(&context, pePart);

          //a_Content type for this block
          str.removeUntilOneOf();
          pePart->addElement(ASW("content-type",12)).addData(str, AXmlElement::ENC_CDATADIRECT);
        }
        else if (AConstant::npos != (pos = str.findNoCase(ASW("Content-Disposition:", 20))))
        {
          pePart = &eInput.addElement(ASW("part",4));
          AXmlElement& eDisposition = pePart->addElement(ASW("content-disposition",19));

          //a_Content disposition for this block
          pos += 21;  //a_Include trailing space
          while (pos < str.getSize())
          {
            //a_Parse pair in the line
            pair.parse(str, pos);

            if (pair.getName().equalsNoCase(ASW("name",4)))
            {
              //a_Gets current part's name
              strName.assign(pair.getValue());
            }
            else if (pair.getName().equalsNoCase(ASW("filename",8)))
            {
              isFilename = true;  //a_Content gets saved outside of parameter pairs if so specified
              strFilename.assign(pair.getValue());
            }

            eDisposition.addElement(pair.getName()).addData(pair.getValue());
          }

          str.removeUntilOneOf();
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
    context.useEventVisitor().startEvent(ASW("Content-Length missing, http error 411",38));
    context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_411_Length_Required);
    return AOSContext::RETURN_ERROR;
  }

  return AOSContext::RETURN_OK;
}
