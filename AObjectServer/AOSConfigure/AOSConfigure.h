/*
$Id: AObjectServer.cpp 322 2008-12-08 23:46:57Z achacha $
*/
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CConfigureApp:
// See AOSConfigure.cpp for the implementation of this class
//

class CConfigureApp : public CWinAppEx
{
public:
	CConfigureApp();
  virtual ~CConfigureApp();

  void initServices(const AString& rootpath);
  void deinitServices();
  AOSServices& useServices();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

private:
  AOSServices *mp_Services;
};

extern CConfigureApp theApp;