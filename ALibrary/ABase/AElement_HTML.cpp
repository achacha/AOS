#include "pchABase.hpp"
#include "AElement_HTML.hpp"
#include "AException.hpp"

#define ELEMENT_TERMINATION_STRING "!ps-9"
const AString AElement_HTML::PARSE_END_ELEMENT(" />");
char AElement_HTML::PARSE_START_ELEMENT('<');

#ifdef __DEBUG_DUMP__
void AElement_HTML::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  AElementInterface::debugDump(os, indent+1);

  ADebugDumpable::indent(os, indent+1) << "m__boolExtendedErrorLogging=" << (m__boolExtendedErrorLogging ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_vectorPairs={" << std::endl;
  for (VECTOR_NVPairPtr::const_iterator cit = m_vectorPairs.begin(); cit != m_vectorPairs.end(); ++cit)
  {
    (*cit)->debugDump(os, indent+2);
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_listChildElements={" << std::endl;
  for (LIST_AElementInterface::const_iterator cit = m_listChildElements.begin(); cit != m_listChildElements.end(); ++cit)
  {
    (*cit)->debugDump(os, indent+2);
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}
#endif

AElement_HTML::AElement_HTML(AElementInterface *elementParent) :
  AElementInterface(elementParent, false)  //a_By default HTML elements are assumed to need </TYPE>
{ 
  m__boolExtendedErrorLogging = false;  //a_Extended errors should not be on
}

AElement_HTML::~AElement_HTML()
{
  //a_Destroy the object
  __destroy();
}

void AElement_HTML::clear()
{
  __reset();

	AElementInterface::clear();
}

//a_static method
//a_Determines if the type is a singular type not requiring </TYPE> to end it
bool AElement_HTML::isSingularType(const AString &strType)
{
  //a_Adheres to the HTML 3.2 standard
  //a_TYPE is always lower case!
  if (
       (!strType.compare("hr")) ||
       (!strType.compare("br")) ||
       (!strType.compare("img")) ||
       (!strType.compare("input")) ||
       (!strType.compare("option")) ||
       (!strType.compare("!--")) ||
       (!strType.compare("dt")) ||
       (!strType.compare("dd")) ||
       (!strType.compare("noframe")) ||
       (!strType.compare("meta")) ||
       (!strType.compare("base")) ||
       (!strType.compare("basefont")) ||
       (!strType.compare("link")) ||
       (!strType.compare("isindex")) ||
       (!strType.compare("!doctype"))
     )
    return true;
  
  return false;
}

//a_static method
bool AElement_HTML::isFormItemType(const AString &strType)
{
  //a_Adheres to the HTML 3.2 standard
  //a_TYPE is always upper case!
  //a_Items found inside <FORM>...</FORM>
  if (
       (!strType.compare("INPUT")) ||
       (!strType.compare("SELECT")) ||         //a_OPTION fields belong to a SELECT
       (!strType.compare("TEXTAREA"))
     )
    return true;
  
  return false;
}

//a_static method
//a_Determines if the type is format critical (ie. <PRE>, <TEXTAREA>, etc)
bool AElement_HTML::isFormatCritical(const AString &strType)
{
  //a_Adheres to the HTML 3.2 standard
  //a_TYPE is always upper case!
  if (
       (!strType.compare("PRE")) ||
       (!strType.compare("A")) ||
       (!strType.compare("TEXTAREA")) ||
       (!strType.compare("SCRIPT"))
     )
    return true;

  return false;
}

AString AElement_HTML::beginElement() const
{
  if (m_strType.isEmpty())
  {
    return m__strText;
  }
  else
  {
    AString strReturn;

    //a_HTML block or IW TL variable
    strReturn += "<";
    strReturn += m_strType;
    for (size_t x = 0x0; x < m_vectorPairs.size(); ++x)
    {
      strReturn.append(' ');
      m_vectorPairs[x]->emit(strReturn);
    }
  
    if (!m__strText.isEmpty() && m__strText.compare(ELEMENT_TERMINATION_STRING))
    {
      strReturn.append(m__strText);
    }
    
    if (!m_strType.compare("!--"))
      strReturn.append("--", 2);

    if (isSingular())
      strReturn.append("/>", 2);
    else
      strReturn.append('>');

    return strReturn;
  }
}

AString AElement_HTML::begin(size_t iDepth) const
{
  AString strReturn;
  size_t iX;

  //a_Indent
  if (iDepth > 0)
    for(iX = 0x0; iX < iDepth; ++iX) strReturn += AConstant::ASTRING_TWOSPACES;
  
  //a_Returns self (formatted in text or HTML)
  strReturn += beginElement();

  //a_Depth enable formatting
  if (iDepth >= 0)
    strReturn += AConstant::ASTRING_CRLF;

  //a_Now display the children
  LIST_AElementInterface::const_iterator liElement = m_listChildElements.begin();
  while (liElement != m_listChildElements.end())
  {
    AElement_HTML *peChild = (AElement_HTML *)(*liElement);
    
    //a_Determine new formatting depth or if needed to turn it off
    size_t iNewDepth = iDepth;
    if (AConstant::npos != iDepth)
    {
      //a_Formatting enabled
      if (isFormatCritical(m_strType))
      {
        //a_Do not format children
        iNewDepth = AConstant::npos;
      }
      else
        iNewDepth = (AConstant::npos != iDepth ? iDepth + 1 : AConstant::npos);
    }

    strReturn += peChild->begin(iNewDepth);
    if (!peChild->isSingular())
      strReturn += peChild->end(iNewDepth);
  
    liElement++;
  }

  return strReturn;
}

AString AElement_HTML::end(size_t iDepth) const
{
  AString strReturn;
  
  //a_Indent
  if (iDepth > 0)
    for (size_t iX = 0; iX < iDepth; ++iX) strReturn += AConstant::ASTRING_TWOSPACES;

  if (!isSingular())
  {
    //a_If this is a signular object it doesn't need a close tag
    strReturn += "</";
    strReturn += m_strType + ">";

    //a_Depth enables formatting
    if (AConstant::npos != iDepth)
      strReturn += AConstant::ASTRING_CRLF;
  }
  
  return strReturn;
}

void AElement_HTML::__reset()
{
  //a_Clear the NAME=VALUE pairs
  for (u4 u4X = 0x0; u4X < m_vectorPairs.size(); u4X++)
    pDelete(m_vectorPairs[u4X]);
  m_vectorPairs.clear();

  //a_Clear the type in the AElementInterface class
  m_strType.clear();

  //a_Clear the text
  m__strText.clear();
}

//a_Destroys the object
void AElement_HTML::__destroy()
{
  //a_Clear the object
  __reset();

  //a_Delete children (causes a recursive delete)
  LIST_AElementInterface::iterator liElement = m_listChildElements.begin();
  while (liElement != m_listChildElements.end())
  {
    pDelete(*liElement);
    ++liElement;
  }
  m_listChildElements.clear();
}

//a_The parsing will work with the element
//a_ <TYPE NAME=VALUE NAME="VALUE" NAME=URLEncoded NAME>
//a_Default implementation: (single element) (will be multi-element for FORM, TABLE, and such)
//a_ Must start with '<' and end with '>', everything outside of this will be ignored
//a_ First NAME is the type of the element, first value is ignored (naturally it makes no sense)
//a_ Contents will be parsed in NAME=VALUE pairs, all VALUEs will be URL unencoded and "" will be dropped in storage
//a_Return true if parsing should continue
bool AElement_HTML::parse(const AString &strInput, size_t& position)
{
  //a_End of the string (stopping case)
  if (position >= strInput.getSize())
  {
    AString strError = "Unexpected EOF while parsing";
    if (getParent())
      strError += AString(" under ") + ((AElement_HTML *)getParent())->m_strType;
    strError += ASW(" position=",10)+AString::fromSize_t(position);
    ATHROW_EX(this, AException::InvalidData, strError);
  }

  //a_Reset the object
  __reset();

  //a_Skip over any leading white space
  __parseSkipOverWhiteSpace(strInput, position);
  
  //a_Check if this is an element or a text block
  if (position >= strInput.getSize())
  {
    AString strError = "Unexpected EOF while skipping over whitespace";
    if (getParent())
      strError += AString(" under ") + ((AElement_HTML *)getParent())->m_strType;
    strError += ASW(" position=",10)+AString::fromSize_t(position);
    ATHROW_EX(this, AException::InvalidData, strError);
  }

  if (strInput[position] != '<')
  {
    //a_Text block
    __parseTextBlock(strInput, position);

    if (m__strText.isEmpty()) //a_Nothing in here...
      m__strText = ELEMENT_TERMINATION_STRING;    //a_Remove this useless block
  }
  else
  {
    //a_HTML block to follow
    //a_The next item is the TYPE of element
    position++;    //a_Skip over the '<'
    if (position >= strInput.getSize())
    {
      AString strError = "Unexpected EOF while skipping over <";
      if (getParent())
        strError += AString(" under ") + ((AElement_HTML *)getParent())->m_strType;
      strError += ASW(" position=",10)+AString::fromSize_t(position);
      ATHROW_EX(this, AException::InvalidData, strError);
    }

    __parseSkipOverWhiteSpace(strInput, position);

    if (position >= strInput.getSize())
    {
      AString strError = "Unexpected EOF while skipping over whitespace after <";
      if (getParent())
        strError += AString(" under ") + ((AElement_HTML *)getParent())->m_strType;
      strError += ASW(" position=",10)+AString::fromSize_t(position);
      ATHROW_EX(this, AException::InvalidData, strError);
    }

    bool boolTerminator = false;
    if (strInput[position] == '/')
    {
      //a_Termination TYPE
      boolTerminator = true;
      ++position;

      //a_Skip over space after '/'
      __parseSkipOverWhiteSpace(strInput, position);
    }

    //a_Get the type (until whitespace is encountered or '>')
    if (strInput.find("!--", position) == position)
    {
      //a_Comment has a special case
      m_strType.assign("!--");
      position += 0x3;
    }
    else
    {
      __parseGetType(strInput, position);
    }

    //a_Termination returns the TYPE (stopping case)
    if (boolTerminator)
    {
      //a_Advance to the end of tag;
      __parseAdvanceToEndOfTag(strInput, position);

      m__strText = ELEMENT_TERMINATION_STRING;     //a_Signifies that a child is to be deleted
    
      //a_Check if this type is in the parent tree
      AElement_HTML *pheParent = (AElement_HTML *)getParent();
      while (pheParent)
      {
        //a_Found it
        if (pheParent->m_strType == m_strType)
        {
          __parseAdvanceToNextTag(strInput, position);
        
          return true;
        }

        pheParent = (AElement_HTML *)pheParent->getParent();
      }

      //a_Did not find it, convert to literal and assume this is a tag
      m_strType.assign(AString("/") + m_strType);
      setSingular(true);
  
      if (m__boolExtendedErrorLogging)
      {
        AString strError("Dangling or unevenly closed end tag detected: ");
        strError.append(m_strType);
        strError.append(ASW(" position=",10)+AString::fromSize_t(position));
        ATHROW_EX(this, AException::InvalidData, strError);
      }
    }

    __parseSkipOverWhiteSpace(strInput, position);

    //a_Check if parsing of NAME=VALE should be ignored
    //a_Special case for HTML comments or !doctype
    if (m_strType == "!--")
    {                                             
      m__strText.clear();

      size_t iEndPosition = strInput.find("-->", position);
      if (iEndPosition == AConstant::npos)
      {
        AString strError("Unexpected EOF while parsing <!--");
        strError += ASW(" position=",10) + AString::fromSize_t(position);
        ATHROW_EX(this, AException::InvalidData, strError);
      }
      else
      {
        strInput.peek(m__strText, position, iEndPosition - position);
        position = iEndPosition + 0x2;
      }
    }
    else if (!m_strType.compare("!doctype"))
    {
      while (strInput.getSize() > position)
      {
        if (strInput[position] == '>')
          break;

        m__strText += strInput[position];
        ++position;
      }
    }
    else
    {
      __parseNameValuePairs(strInput, position);
    }

    if (position >= strInput.getSize())
    {
      AString strError = "Unexpected EOF while looking for > or /> in ";
      strError += m_strType;
      if (getParent())
        strError += AString(" under ") + ((AElement_HTML *)getParent())->m_strType;
      strError += ASW(" position=",10)+AString::fromSize_t(position);
      ATHROW_EX(this, AException::InvalidData, strError);
    }

    //a_Check if this is a singular type
    if (strInput[position] == '/')
    {
      setSingular(true);
      ++position;
    }
    else if (isSingularType(m_strType))
      setSingular(true);

    if (m__boolExtendedErrorLogging && strInput[position] != '>' && !isWhiteSpace(strInput[position]))
    {
      ATHROW_EX(this, AException::InvalidData, ASWNL("Possible incorrect end tag location in ") + m_strType + ASW(" position=",10)+AString::fromSize_t(position));
    }

    //a_Advance past the end to the next tag (assumes we are at >)
    __parseAdvanceToNextTag(strInput, position);
 }

  //a_Create the children
  AElement_HTML *pheChild;
  while(strInput.getSize() > position) 
  {
    //a_Create a child (child classes can implement their own create methods
    pheChild = (AElement_HTML *)_create(this);

    AElement_HTML *pheParent = _addChild(pheChild);
    bool ret = pheChild->parse(strInput, position);

    //a_If the child is a terminating element, remove it from the list
    //a_The child is removed from the parent it was added on to
    if ((!ret) && 
        (!pheChild->useText().compare(ELEMENT_TERMINATION_STRING)) )
      pheParent->removeChild(pheChild);

    if (!ret)
    {
      return false;
    }
  }

  if (position >= strInput.getSize())
  {
    //a_We are not singular and our end has not been found
    if ((isSingular() == false) && getParent())
    {
      AElement_HTML *pheParent = ((AElement_HTML *)getParent());
      ATHROW_EX(this, AException::InvalidData, AString("Unexpected EOF, termination for ") + (pheParent ? pheParent->m_strType : AString("{root}")) + ASWNL(" not found")+ASW(" position=",10)+AString::fromSize_t(position));
    }
    //a_If no singular parents, we are done then...
  }

  return true;
}

void AElement_HTML::__parseTextBlock(const AString &strInput, size_t& position)
{
  setSingular(true);    //a_We are the only one
  
  //a_Determine if we need to ignore while space or not
  bool boolIgnoreWhiteSpace = !__isInsideFormatCriticalBlock();
  
  //a_Remove leading white space if not inside a critical block
  while (strInput.getSize() > position)
  {
    //a_Odd while/if setup due to MSVC++ evaluation of parentheic equations
    if (boolIgnoreWhiteSpace && isWhiteSpace(strInput[position]))
      position++;
    else
      break;
  }

  if (position >= strInput.getSize())
  {
    AElement_HTML *pheParent = _findNonSingularParent();
    if (pheParent)
    {
      AString strError = "Unexpected EOF while removing leading spaces from a text block";
      strError += AString(" under ") + pheParent->m_strType;
      strError += ASW(" position=",10)+AString::fromSize_t(position);
      ATHROW_EX(this, AException::InvalidData, strError);
    }
    return;   //a_Ran out of stuff to do, this is the end my friend
  }

  char cX;
  bool boolLastIsWhiteSpace = false;
  position = strInput.peekUntil(m__strText, position, PARSE_START_ELEMENT);
  if (AConstant::npos == position)
  {
    strInput.peek(m__strText, position);
    return;
  }

  //a_Now remove trailing carrige returns and spaces if not inside critical block
  if (boolIgnoreWhiteSpace)
    m__strText.stripTrailing();
}

void AElement_HTML::__parseNameValuePairs(const AString &strInput, size_t& position)
{
  //a_Parse NAME=VALUE pairs
  ANameValuePair *pnvPair;
  while (position < strInput.getSize() && strInput[position] != '>' && strInput[position] != '/')
  {
    //a_Parse all the value types
    pnvPair = new ANameValuePair(ANameValuePair::HTML);
    pnvPair->parse(strInput, position);
    if (pnvPair->getName().isEmpty())
    {
      pDelete(pnvPair);
    }
    else
    {
      m_vectorPairs.push_back(pnvPair);
    }
  }
}

void AElement_HTML::__parseAdvanceToEndOfTag(const AString &strInput, size_t& position)
{
  if ((position = strInput.find('>', position)) == AConstant::npos)
    position = strInput.getSize() + 1;    //a_We are at the end

  if (position >= strInput.getSize())
  {
    ATHROW_EX(this, AException::InvalidData, ASWNL("Unexpected EOF advancing to > in ") + m_strType+ASW(" position=",10)+AString::fromSize_t(position));
  }
}

void AElement_HTML::__parseAdvanceToNextTag(const AString &strInput, size_t& position)
{
  //a_Skip over the '>'
  while(strInput.getSize() > position)
  {
    if (strInput[position] != '>')
      position++;
    else
      break;
  }

  //a_Now advance
  if (strInput.getSize() > position)
  {
    position++;
    
    if (strInput.getSize() > position && !__isInsideFormatCriticalBlock())
      __parseSkipOverWhiteSpace(strInput, position);
  }
  else
  {
    if (position >= strInput.getSize())
    {
      ATHROW_EX(this, AException::InvalidData, ASWNL("Unexpected EOF advancing to next tag after ") + m_strType+ASW(" position=",10)+AString::fromSize_t(position));
    }
  }
}

void AElement_HTML::__parseSkipOverWhiteSpace(const AString &strInput, size_t& position)
{
  if ((position = strInput.findNotOneOf(AConstant::ASTRING_WHITESPACE, position)) == AConstant::npos)
    position = strInput.getSize() + 1;    //a_We are at the end
}

void AElement_HTML::__parseGetType(const AString &strInput, size_t& position)
{
  position = strInput.peekUntilOneOf(m_strType, position, PARSE_END_ELEMENT);
  if (AConstant::npos == position)
  {
    ATHROW_EX(this, AException::InvalidData, ASWNL("Unexpected EOF getting type: ") + m_strType+ASW(" position=",10)+AString::fromSize_t(position));
  }
}

AElementInterface *AElement_HTML::_create(AElementInterface *pParent)
{
  AElement_HTML *peNew = new AElement_HTML(pParent);

  if (m__boolExtendedErrorLogging)
    peNew->setExtendedErrorLogging();

  return peNew;
}

AElement_HTML *AElement_HTML::_addChild(AElementInterface *peChild)
{
  //a_First find someone who is not singular to attach this to
  AElement_HTML *phElement = this, *phLast = this;
  while ((phElement) && phElement->isSingular())
  {
    phLast = phElement;
    phElement = (AElement_HTML *)(phElement->getParent());
  }
  
  if (!phElement)
    phElement = phLast;

  //a_Add element
  AASSERT(this, phElement);
  phElement->m_listChildElements.push_back(peChild);
  ((AElement_HTML *)peChild)->_setParent(phElement);

  //a_Return the recipient of this element
  return phElement;
}

void AElement_HTML::removeChild(AElement_HTML *peChild)
{
  m_listChildElements.remove(peChild);
  pDelete(peChild);
}

AElement_HTML *AElement_HTML::nextChildElement(AElement_HTML *pheStartAfter)
{
  //a_NULL means we are starting here
  if (pheStartAfter == NULL)
  {
    return this;
  }
  
  //a_this means we are starting the iteration from the first child
  if (pheStartAfter == this)
  {
    //a_First child requested
    if (m_listChildElements.size() > 0x0)
      return (AElement_HTML *)(*m_listChildElements.begin());
    else
      return NULL;   //a_This object has no more children
  }
  
  //a_Not a child
  if (!_isChild(pheStartAfter))
    return NULL;                  

  //a_Iterate through the children, find the target and get the next
  AElement_HTML *pheCurrentParent = (AElement_HTML *)(pheStartAfter->getParent());

  //a_Now check if we are at an end of a list and if the parents can be used to advance
  bool boolTraverseChildren = true;
  LIST_AElementInterface::iterator liElement = pheCurrentParent->m_listChildElements.end();
  liElement--;

  if ((*liElement == pheStartAfter) && 
      (pheStartAfter->m_listChildElements.size() == 0x0))
  {
    while ((pheCurrentParent) && (*liElement == pheStartAfter))
    {
      //a_Before moving up a level, if we are the this element or top, we haven't found it
      if ((pheCurrentParent == NULL) || (pheCurrentParent == this) )
        return NULL;

      //a_Last one in the list, move up one level and try again
      pheStartAfter = pheCurrentParent;
      pheCurrentParent = (AElement_HTML *)pheCurrentParent->getParent();

      liElement = pheCurrentParent->m_listChildElements.end();
      liElement--;
    }

    boolTraverseChildren = false;
  }

  //a_Find next element given the current and parent
  pheCurrentParent = (AElement_HTML *)(pheStartAfter->getParent());
  while (pheCurrentParent)
  {
    liElement = pheCurrentParent->m_listChildElements.begin();
    while (liElement != pheCurrentParent->m_listChildElements.end())
    {
      //a_See if this is our target
      if ((*liElement) == pheStartAfter)
      {
        //a_Does this node have any children (prevent endless loops by comparing to original)
        if ((boolTraverseChildren) && 
            (pheStartAfter->m_listChildElements.size() > 0x0))
        {
          //a_Return the first child as the next
          AElement_HTML *pRet = (AElement_HTML *)( *(pheStartAfter->m_listChildElements.begin()) );
          return pRet;
        }
        else
        {
          //a_Go to next and see if this the end
          liElement++;
          return (AElement_HTML *)(*liElement);    //a_Found the next amoung peers
        }
      }

      liElement++;    //a_Next peer
    }

    //a_Move up one level in parent tree
    pheCurrentParent = (AElement_HTML *)pheCurrentParent->getParent();
    if ((pheCurrentParent == NULL) || (pheCurrentParent == this) )
      return NULL;    //a_After all end iterations, we are back here... thus not found
  }

  return NULL;                  //a_Not found!
}

AElement_HTML *AElement_HTML::findType(const AString &strType, AElement_HTML *pheStartAfter)
{
  //a_Find the start location
  AElement_HTML *pheElement = pheStartAfter;

  //a_Keep iterating through all children until the type is found
  while (pheElement = nextChildElement(pheElement))
  {
    if (pheElement->m_strType.compareNoCase(strType) == 0)
      return pheElement;
  }

  return NULL;    //a_Not found
}

AElement_HTML *AElement_HTML::findFormItem(AElement_HTML *pheStartAfter)
{
  //a_Find the start location
  AElement_HTML *pheElement = pheStartAfter;

  //a_Keep iterating through all children until the type is found
  while (pheElement = nextChildElement(pheElement))
  {
    if (isFormItemType(pheElement->m_strType))
      return pheElement;
  }

  return NULL;    //a_Not found
}

AElement_HTML *AElement_HTML::_findNonSingularParent()
{
  AElement_HTML *pheParent = (AElement_HTML *)getParent();
  while (pheParent && pheParent->isSingular())
    pheParent = (AElement_HTML *)pheParent->getParent();

  return pheParent;
}

AElement_HTML *AElement_HTML::findParentType(const AString& strType)
{
  AElement_HTML *pheParent = (AElement_HTML *)getParent();
  while (pheParent && pheParent->m_strType.compare(strType))
    pheParent = (AElement_HTML *)pheParent->getParent();

  return pheParent;
}

bool AElement_HTML::__isInsideFormatCriticalBlock()
{
  AElement_HTML *peParent = this;
  while (peParent)
  {
    if (isFormatCritical(peParent->m_strType) )
    {
      return TRUE;
      break;
    }
    peParent = (AElement_HTML *)peParent->getParent();
  }
  return FALSE;
}

size_t AElement_HTML::_findName(const AString &strName) const
{
  AString strX;
  for (size_t x = 0; x < m_vectorPairs.size(); ++x)
  {
    strX = m_vectorPairs[x]->getName();    
    if (!strName.compareNoCase(strX))
      return x;
  }

  //a_Not found
	return AConstant::npos;
}

void AElement_HTML::changeName(const AString& strOldName, const AString& strNewName)
{
  AASSERT(this, strOldName.getSize());

  size_t index = _findName(strOldName);
  if (index != AConstant::npos)
  {
    m_vectorPairs[index]->setName(strNewName);
  }
}

bool AElement_HTML::getValue(const AString &strName, AString& strValue) const
{
  size_t index = _findName(strName);
  if (index != AConstant::npos)
  {
    strValue = m_vectorPairs[index]->getValue();
    return true;
  }

  return false;
}

void AElement_HTML::setNameValue(const AString &strName, const AString &strValue)
{
  size_t index = _findName(strName);
	if (index != AConstant::npos)
  {
    //a_NAME already exists
    m_vectorPairs[index]->setValue(strValue);
  }
  else
  {
    //a_NAME does not exist, add it
    ANameValuePair *pnvpNew = new ANameValuePair(ANameValuePair::HTML);
    pnvpNew->setNameValue(strName, strValue);
    m_vectorPairs.push_back(pnvpNew);
  }
}

void AElement_HTML::removeName(const AString &strName)
{
  size_t index = _findName(strName);
	if (index != AConstant::npos)
  {
    VECTOR_NVPairPtr::iterator viPair = m_vectorPairs.begin();
    viPair += index;
    pDelete(*viPair);
    m_vectorPairs.erase(viPair);
  }  
}

void AElement_HTML::recurseToString(AString& strReturn, int iIndentLevel) const
{
  //a_Check if this an element or a text block
  bool boolElement = (m__strText.isEmpty() ? true : false);

  if (boolElement)
  {
    //a_This is an HTML type
    for (int iX = iIndentLevel; iX > 0x0; --iX) strReturn += AConstant::ASTRING_TWOSPACES;
    strReturn += "<";
    strReturn += m_strType;
  }
  else
  {
    if (m_strType == "!--")
    {
      strReturn += "<!--";
      strReturn += m__strText;
      strReturn += "-->";
    }
    else
      strReturn += m__strText;
  }

  for (size_t x = 0; x < m_vectorPairs.size(); ++x)
  {
    strReturn.append(' ');
    m_vectorPairs[x]->emit(strReturn);
  }
  
	if (boolElement)
	{
    if (isSingular())
			strReturn.append('/');

		strReturn.append('>');
	}

  if (iIndentLevel >= 0x0) strReturn += AConstant::ASTRING_CRLF;

  LIST_AElementInterface::const_iterator liElement = m_listChildElements.begin();
  while (liElement != m_listChildElements.end())
  {
    ((AElement_HTML *)(*liElement))->recurseToString(strReturn, (iIndentLevel >= 0x0 ? iIndentLevel + 0x1 : iIndentLevel));
    liElement++;
  }

  if (boolElement && !isSingular())
  {
    for (int iX = iIndentLevel; iX > 0x0; --iX) strReturn += AConstant::ASTRING_TWOSPACES;
    strReturn.append("</", 2);
    strReturn.append(m_strType);
    strReturn.append('>');
    if (iIndentLevel >= 0x0)
      strReturn.append(AConstant::ASTRING_CRLF);
  }
}

void AElement_HTML::setExtendedErrorLogging(bool bFlag)
{
  m__boolExtendedErrorLogging = bFlag; 
}
