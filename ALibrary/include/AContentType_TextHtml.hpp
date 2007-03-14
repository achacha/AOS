#ifndef INCLUDED__AContentType_TextHtml_HPP__
#define INCLUDED__AContentType_TextHtml_HPP__

#include "apiABase.hpp"
#include "AContentTypeInterface.hpp"
#include "AElement_HTML.hpp"
#include "AElementObserver_FORM.hpp"
#include "AElementObserver_HREF.hpp"
#include "AElementObserver_SRC.hpp"

class AFile;

class ABASE_API AContentType_TextHtml : public AContentTypeInterface
{
public:
  typedef std::vector<AElementObserver_FORM *> VECTOR_AFormObservers;
  typedef std::vector<AElementObserver_HREF *> VECTOR_AHrefObservers;
  typedef std::vector<AElementObserver_SRC *>  VECTOR_ASrcObservers;

public:
  //a_ctor/dtor
  AContentType_TextHtml();
  ~AContentType_TextHtml();
  
  //a_Access methods
  AElement_HTML *getHead() const;
  AElement_HTML *findType(const AString &strType, AElement_HTML *pheStartFrom = NULL) const;

  //a_Activation method
  virtual void clear();
  virtual void parse();
  
  //a_Basic linting
  void lint();      //a_After parsing it validates with some basic rules

  //a_AFile use
  virtual void fromAFile(AFile& fileIn);
  virtual void toAFile(AFile& fileIn) const;

  //a_indent >=0 will format
  virtual void emit(AOutputBuffer&, size_t indent = AConstant::npos) const;  

  //a_Call ALL parsing routines
  //a_Must proivide a base AUrl object used in request for correct
  //a_ offset and path from base
  void parseAll(const AUrl& url);          

  //a_Access to FORMs via AFormObserver types
  //a_Must provide AUrl object of the request (ideally the same one used to construct AHTTPHeader)
  void parseForms(const AUrl&);    
  size_t getFormCount() const { return m__vectorForms.size(); }
  AElementObserver_FORM &getFormObserver(size_t i) const {return *m__vectorForms[i]; }

  //a_Access to HRef via AHrefObserver
  //a_Must provide AUrl object of the request (ideally the same one used to construct AHTTPHeader)
  void parseHrefs(const AUrl&);
  size_t getHrefCount() const { return m__vectorHrefs.size(); }
  AElementObserver_HREF &getHrefObserver(size_t i) const { return *m__vectorHrefs[i]; }

  //a_Access to FRAMEs via AFrameObserver
  //a_Must provide AUrl object of the request (ideally the same one used to construct AHTTPHeader)
  void parseSrcs(const AUrl&);
  size_t getSrcCount() const { return m__vectorSrcs.size(); }
  AElementObserver_SRC &getSrcObserver(size_t i) const { return *m__vectorSrcs[i]; }

  //a_Targets
  size_t getTargetCount() const;
  //a_(0)------(iFormThreshold)------(iHrefThreshold)-----(iMaxThreshold)
  //a_    FORM                  HREF                  SRC
  const AUrl getRandomTargetUrl(size_t iFormThreshold = 40, size_t iHrefThreshold = 80, size_t iMaxThreshold = 100);

  //a_Formatting of the output
  void setFormatted(bool boolFlag = true) { m__boolFormatted = boolFlag; }
  //a_Extended error reporting
  void setExtendedErrorLogging(bool boolFlag = true) { m__boolExtendedErrorLogging = boolFlag; }
  
private:
  static const AString CONTENT_TYPE;  // text/html

  //a_HTML page hierarchy
  AElement_HTML *m__pheHead;     //a_Head of the AHtmlElement tree
  void __resetHierarchy();       //a_Reset the AHtmlElement hierarchy

  //Formatting
  bool m__boolFormatted;
  bool m__boolExtendedErrorLogging;

  //a_Observers
  VECTOR_AFormObservers m__vectorForms;
  VECTOR_AHrefObservers m__vectorHrefs;
  VECTOR_ASrcObservers  m__vectorSrcs;

  //a_Parsing flag
  bool m__boolNeedParsing;
};

#endif

