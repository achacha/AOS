/*
Written by Alex Chachanashvili

$Id: AOSAdmin.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AOSAdminCommand_modify_HPP__
#define INCLUDED__AOSAdminCommand_modify_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminCommand_display.hpp"

class AOSAdminCommand_modify : public AOSAdminCommand_display
{
public:
  //! Command name
  static const AString COMMAND;

public:
  /*!
  ctor with services
  */
  AOSAdminCommand_modify(AOSServices&);

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
//  virtual void _insertStylesheet(AOSAdminCommandContext&);
};

#endif // INCLUDED__AOSAdminCommand_modify_HPP__
