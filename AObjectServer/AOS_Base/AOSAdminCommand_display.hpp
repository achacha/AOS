/*
Written by Alex Chachanashvili

$Id: AOSAdmin.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AOSAdminCommand_display_HPP__
#define INCLUDED__AOSAdminCommand_display_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminCommandInterface.hpp"

class AOSAdminCommand_display : public AOSAdminCommandInterface
{
public:
  /*!
  ctor with services
  */
  AOSAdminCommand_display(AOSServices&);

  /*!
  Get the name of the command the class can handle

  @return name of the command
  */
  virtual const AString& getName() const;

protected:
  /*!
  Process a command
  */
  virtual void _process(AOSAdminCommandContext&);

  /*!
  Insert stylesheet into the model
  */
  virtual void _insertStylesheet(AOSAdminCommandContext&);
};

#endif // INCLUDED__AOSAdminCommand_display_HPP__