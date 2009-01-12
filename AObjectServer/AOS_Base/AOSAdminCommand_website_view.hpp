/*
Written by Alex Chachanashvili

$Id: AOSAdmin.hpp 218 2008-05-29 23:23:59Z achacha $
*/
#ifndef INCLUDED__AOSAdminCommand_website_view_HPP__
#define INCLUDED__AOSAdminCommand_website_view_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminCommandInterface.hpp"

class AOSAdminCommand_website_view : public AOSAdminCommandInterface
{
public:
  //! Command name
  static const AString COMMAND;

public:
  /*!
  ctor with services
  */
  AOSAdminCommand_website_view(AOSServices&);

  /*!
  Process a command
  */
  virtual void process(AOSAdminCommandContext&);

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

private:
  LIST_AString m_SupportedMime;
};

#endif // INCLUDED__AOSAdminCommand_website_view_HPP__
