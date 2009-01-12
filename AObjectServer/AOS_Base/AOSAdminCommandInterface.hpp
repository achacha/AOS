/*
Written by Alex Chachanashvili

$Id: AOSAdmin.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AOSAdminCommandInterface_HPP__
#define INCLUDED__AOSAdminCommandInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminCommandContext.hpp"
#include "AOSServices.hpp"

class AOSAdminCommandInterface : public ABase
{
public:
  /*!
  ctor with services
  */
  AOSAdminCommandInterface(AOSServices&);
  virtual ~AOSAdminCommandInterface();

  /*!
  Process a command
  */
  virtual void process(AOSAdminCommandContext&);

  /*!
  Get the name of the command the class can handle

  @return name of the command
  */
  virtual const AString& getName() const = 0;

protected:
  /*!
  Command specific logic
  */
  virtual void _process(AOSAdminCommandContext&) = 0;

  /*!
  Insert stylesheet into the model
  */
  virtual void _insertStylesheet(AOSAdminCommandContext&);

  //! AOS services object
  AOSServices& m_Services;
};

#endif // INCLUDED__AOSAdminCommandInterface_HPP__
