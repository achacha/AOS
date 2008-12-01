/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSInput_HtmlFormMultiPart.hpp"

const AString AOSInput_HtmlFormMultiPart::CLASS("multipart/form-data");

const AString& AOSInput_HtmlFormMultiPart::getClass() const
{
  return CLASS;
}

AOSInput_HtmlFormMultiPart::AOSInput_HtmlFormMultiPart(AOSServices& services) :
  AOSInputProcessorInterface(services)
{
}

AOSContext::ReturnCode AOSInput_HtmlFormMultiPart::execute(AOSContext& context)
{
  if (AHTTPRequestHeader::METHOD_ID_POST == context.useRequestHeader().getMethodId())
  {
    //a_Force a close since this is a POST
    context.useResponseHeader().set(AHTTPResponseHeader::HT_GEN_Connection, ASW("close",5));
    
    AString strLength;
    if (context.useRequestHeader().get(AHTTPHeader::HT_ENT_Content_Length, strLength))
    {
      //a_Increase read buffer
      //context.useSocket().setReadBlockSize(32767);
      
      //a_Get the boundary string
      AString strBoundary;
      context.useRequestHeader().get(AHTTPHeader::HT_ENT_Content_Type, strBoundary);
      size_t pos = strBoundary.find(ASW("boundary=", 9));
      if (AConstant::npos == pos)
      {
        context.addError(getClass(), ASWNL("AOSInput_HtmlFormMultiPart: boundary not found"));
        return AOSContext::RETURN_ERROR;
      }

      strBoundary.remove(pos+7);
      strBoundary.set('-', 0);
      strBoundary.set('-', 1);

      //a_Discard the leading --
      AString str(1024, 1024);
      AString strName;
      
      size_t bytesRead = AConstant::unavail;
      while (AConstant::unavail == bytesRead)
      {
        bytesRead = context.useSocket().readUntil(str, strBoundary, true, true);
        switch(bytesRead)
        {
          case AConstant::npos:
            context.addError(getClass(), ASWNL("AOSInput_HtmlFormMultiPart: unable to read to boundary"));
            return AOSContext::RETURN_ERROR;

          case AConstant::unavail:
            AThread::sleep(1);
        }
      }
      str.clear();

      bytesRead = AConstant::unavail;
      while(AConstant::unavail == bytesRead)
      {
        bytesRead = context.useSocket().readLine(str);  //a_Remove CRLF after boundary
        switch(bytesRead)
        {
          case AConstant::npos:
            context.addError(getClass(), ASWNL("AOSInput_HtmlFormMultiPart: unable to read line after boundary"));
            return AOSContext::RETURN_ERROR;

          case AConstant::unavail:
            AThread::sleep(1);
        }
      }
      AASSERT_EX(this, !bytesRead, AString::fromSize_t(bytesRead));

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
        bytesRead = AConstant::unavail;
        while(AConstant::unavail == bytesRead)
        {
          bytesRead = context.useSocket().readLine(str);
          switch(bytesRead)
          {
            case AConstant::npos:
              context.addError(getClass(), ASWNL("AOSInput_HtmlFormMultiPart: unable to read MIME line "));
              return AOSContext::RETURN_ERROR;

            case AConstant::unavail:
              AThread::sleep(1);
          }
        }

        if (AConstant::npos != bytesRead)
        {
          //a_Empty line encountered
          if (0 == bytesRead)
          {
            AASSERT(&context, pePart);
            AASSERT(&context, !strName.isEmpty());

            //a_From here until boundary is pure data of this part
            AAutoPtr<AString> pData(new AString(), true);

            bytesRead = AConstant::unavail;
            while (AConstant::unavail == bytesRead)
            {
              bytesRead = context.useSocket().readUntil(*pData, strBoundary, true, true);
              switch(bytesRead)
              {
                case AConstant::npos:
                  context.addError(getClass(), ASWNL("AOSInput_HtmlFormMultiPart: unable to read to boundary"));
                  return AOSContext::RETURN_ERROR;

                case AConstant::unavail:
                  AThread::sleep(1);
              }
            }

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
                context.useEventVisitor().addEvent(ARope("Adding multipart file parameter: ",33)+strName, AEventVisitor::EL_DEBUG);
                pePart->addElement(ASW("context-object-name",19), strObjectName);
                context.useRequestParameterPairs().insert(strName, strObjectName);
                context.useRequestParameterPairs().insert(strName+".filename", strFilename);
                context.useContextObjects().insert(strObjectName, pData.use(), true, true);
                pData.setOwnership(false);
              }
              else
                AASSERT(&context, pData->getSize() <= 2);  // Usually empty line when no filename is provided
            }
            else
            {
              //a_Add to request query
              context.useEventVisitor().addEvent(ARope("Adding multipart form parameter: ",33)+strName, AEventVisitor::EL_DEBUG);
              context.useRequestParameterPairs().insert(strName, *pData);
            }

            //a_Remove trailing CRLF or --CRLF (if EOM) after boundary
            str.clear();
            bytesRead = AConstant::unavail;
            while(AConstant::unavail == bytesRead)
            {
              bytesRead = context.useSocket().readLine(str);  //a_Remove CRLF after boundary
              switch(bytesRead)
              {
                case AConstant::npos:
                  context.addError(getClass(), ASWNL("AOSInput_HtmlFormMultiPart: unable to read line after boundary"));
                  return AOSContext::RETURN_ERROR;

                case AConstant::unavail:
                  AThread::sleep(1);
              }
            }
            
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
      //a_411 Length Required for POST
      context.useEventVisitor().startEvent(getClass()+ASW(": Content-Length missing, http error 411",40), AEventVisitor::EL_WARN);
      context.useResponseHeader().set(AHTTPResponseHeader::HT_GEN_Connection, ASW("close",5));
      context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_411_Length_Required);
      return AOSContext::RETURN_ERROR;
    }
  }
  else
  {
    context.useEventVisitor().startEvent(ARope("Non-POST request, skipping ",27)+getClass(), AEventVisitor::EL_INFO);
  }

  return AOSContext::RETURN_OK;
}
