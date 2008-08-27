/*
Written by Alex Chachanashvili

$Id: ADatabase_NOP.hpp 205 2008-05-29 13:58:16Z achacha $
*/
#ifndef INCLUDED__ADatabase_NOP_HPP__
#define INCLUDED__ADatabase_NOP_HPP__

#include "apiABase.hpp"
#include "ADatabase.hpp"

class AResultSet;

/*!
NOP database (does nothing)
*/
class ABASE_API ADatabase_NOP : public ADatabase
{
public:
  ADatabase_NOP();
  ADatabase_NOP(const AUrl& urlConnection);
  virtual ~ADatabase_NOP();

  /*!
  Init
  @return true
  */
  virtual bool init(AString& error) ;
  
  //! Deinit, does nothing
  virtual void deinit();
  
  /*!
  Reconnects, but really does nothing
  @return true
  */
  virtual bool reconnect(AString& error);
  
  /*!
  Execute a query, but really do nothing
  @return 0
  */
  virtual size_t executeSQL(const AString& query, AResultSet&, AString& error);
  
  /*!
  @return new instance of this class
  */
  virtual ADatabase_NOP *clone(AString& error) const;
};

#endif //INCLUDED__ADatabase_NOP_HPP__