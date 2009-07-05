/*
Written by Alex Chachanashvili

$Id: ATemplateNodeHandler_DADA.hpp 219 2008-07-15 18:47:37Z achacha $
*/
#ifndef INCLUDED__ATemplateNodeHandler_DADA_HPP__
#define INCLUDED__ATemplateNodeHandler_DADA_HPP__

#include "apiAOS_DadaData.hpp"
#include "ATemplateNode.hpp"
#include "ADadaDataHolder.hpp"
#include "ATemplateNodeHandler.hpp"
#include "ABasePtrContainer.hpp"

class AOutputBuffer;
class AXmlElement;
class AFile;

/*!
Handles tag: DADA
*/
class AOS_DADADATA_API ATemplateNodeHandler_DADA : public ATemplateNodeHandler
{
public:
  /*!
  Tag name of this handler
  */
  static const AString TAGNAME;

public:
  /*!
  ctor
  */
  ATemplateNodeHandler_DADA(AOSServices& services);

  /*!
  dtor
  */
  virtual ~ATemplateNodeHandler_DADA();

  /*!
  Initialize internal resources
  */
  virtual void init();

  /*!
  Release resources
  */
  virtual void deinit();

  /*!
  Name of the tag that this node can handle
  */
  virtual const AString& getTagName() const;

  /*!
  Creator method used for parsing this tag
  */
  virtual ATemplateNode *create(AFile&);

  /*!
  Clone
  */
  virtual ATemplateNodeHandler *clone();

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

public:
  /*!
  Node class that handles CODE tag
  */
  class AOS_DADADATA_API Node : public ATemplateNode
  {
  public:
    /*!
    Ctor
    */
    Node(ATemplateNodeHandler *pHandler);
    
    /*!
    Copy ctor
    */
    Node(const ATemplateNodeHandler_DADA::Node&);

    /*!
    dtor
    */
    virtual ~Node();

    /*!
    Process the template node that contains the data for this tag to handle
    */
    virtual void process(ATemplateContext& context, AOutputBuffer& output);

  private:
    class VARIABLEMAP : public ABase
    {
    public:
      MAP_AString_AString vmap;
    };

    void _generateLine(ADadaDataHolder *pddh, MAP_AString_AString& globals, const AString& format, ATemplateContext& context, AOutputBuffer& output);
    void _appendWordType(ADadaDataHolder *pddh, MAP_AString_AString& globals, const AString& strType, AOutputBuffer& output);
    void _appendVariable(ADadaDataHolder *pddh, MAP_AString_AString& globals, const AString& strType, AOutputBuffer& output);
  };

private:
  AOSServices& m_Services;

  ABasePtrContainer m_Objects;

  static const AString S_DELIM_START;
  static const AString S_DELIM_END;
 
  typedef std::map<AString, VECTOR_AString> TEMPLATES;
  TEMPLATES m_Templates;
};

#endif //INCLUDED__ATemplateNodeHandler_DADA_HPP__
