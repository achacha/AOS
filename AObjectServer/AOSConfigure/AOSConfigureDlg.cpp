/*
$Id: AObjectServer.cpp 322 2008-12-08 23:46:57Z achacha $
*/

#include "stdafx.h"
#include "AboutDlg.h"
#include "AOSConfigure.h"
#include "AOSConfigureDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ICON_FolderOpen    0
#define ICON_FolderClosed  1
#define ICON_File          2
#define ICON_Unknown       3
#define ICON_StaticDynamic 4
#define ICON_LAST          5

class CAboutDlg;

CConfigureDlg::CConfigureDlg(CWnd* pParent /*=NULL*/) :
  CDialog(CConfigureDlg::IDD, pParent)
{
  m_Icons.resize(ICON_LAST, 0);
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConfigureDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TREE_WEBSITE, m_WebSiteTree);
}

BEGIN_MESSAGE_MAP(CConfigureDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
  ON_COMMAND(ID_FILE_OPEN_ROOT, &CConfigureDlg::OnFileOpen)
  ON_BN_CLICKED(IDOK, &CConfigureDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CConfigureDlg message handlers

BOOL CConfigureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

  // Attach to image list
  _initializeImageList();
  m_WebSiteTree.SetImageList(&m_ImageList, TVSIL_NORMAL);

  //a_Populate is directory provided
  if (theApp.m_lpCmdLine && *(theApp.m_lpCmdLine) != '\x0')
  {
    CString cstrBasePath = theApp.m_lpCmdLine;
    theApp.initServices(AString(cstrBasePath.GetBuffer()));
    _init();
  }
	
  return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CConfigureDlg::_getBaseFolder(CString& cstrBasePath)
{
  BROWSEINFO bi = { 0 };
  bi.hwndOwner = GetSafeHwnd();
  bi.lpszTitle = _T("Select your aos_root directory");

  LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );
  if ( pidl != 0 )
  {
    // get the name of the folder
    VERIFY(SHGetPathFromIDList ( pidl, cstrBasePath.GetBufferSetLength(MAX_PATH)));

    // free memory used
    IMalloc * imalloc = 0;
    if ( SUCCEEDED( SHGetMalloc ( &imalloc )) )
    {
      imalloc->Free ( pidl );
      imalloc->Release ( );
    }
  }
  else
    return FALSE;

  return TRUE;
}

void CConfigureDlg::_reset()
{
  m_WebSiteTree.DeleteAllItems();
}

void CConfigureDlg::_init()
{
  HTREEITEM hti = m_WebSiteTree.InsertItem(_T("/"));
  m_WebSiteTree.SetItemImage(hti, m_Icons[ICON_FolderClosed], m_Icons[ICON_FolderOpen]);

  // Static
  const AFilename& staticDir = theApp.useServices().useConfiguration().getAosBaseStaticDirectory();
  _addFolderNode(hti, staticDir);

  m_WebSiteTree.Expand(hti, TVE_EXPAND);
}

void CConfigureDlg::_addFolderNode(HTREEITEM htiParent, const AFilename& base)
{
  HTREEITEM htiFolderLast = NULL;
  HTREEITEM htiFileLast = NULL;

  AFileSystem::FileInfos files;
  AFileSystem::dir(base, files, false, false);
  for (AFileSystem::FileInfos::const_iterator cit = files.begin(); cit != files.end(); ++cit)
  {
    if ((*cit).isDirectory())
    {
      if (NULL == htiFolderLast)
        htiFolderLast = m_WebSiteTree.InsertItem((*cit).filename.getPathNames().back().c_str(), htiParent, TVI_FIRST);
      else
        htiFolderLast = m_WebSiteTree.InsertItem((*cit).filename.getPathNames().back().c_str(), htiParent, htiFolderLast);

      m_WebSiteTree.SetItemImage(htiFolderLast, m_Icons[ICON_FolderClosed], m_Icons[ICON_FolderOpen]);

      // Recurse
      _addFolderNode(htiFolderLast, (*cit).filename);
    }
    else if ((*cit).isFile())
    {
      if (NULL == htiFileLast)
        htiFileLast = m_WebSiteTree.InsertItem((*cit).filename.getFilename().c_str(), htiParent, TVI_LAST);
      else
        htiFileLast = m_WebSiteTree.InsertItem((*cit).filename.getFilename().c_str(), htiParent, htiFileLast);

      m_WebSiteTree.SetItemImage(htiFileLast, m_Icons[ICON_File], m_Icons[ICON_File]);
    }
    else
    {
      m_WebSiteTree.SetItemImage(m_WebSiteTree.InsertItem((*cit).filename.toAString().c_str(), htiParent), m_Icons[ICON_Unknown], m_Icons[ICON_Unknown]);
    }
  }
}

void CConfigureDlg::_initializeImageList()
{
	HIMAGELIST	hImgList ;
	SHFILEINFO	sFI ;

  // Get closed folder icon
	hImgList = (HIMAGELIST)::SHGetFileInfo(_T("C:\\RECYCLER\\"), 
                FILE_ATTRIBUTE_DIRECTORY, 
						    &sFI,
                sizeof(SHFILEINFO), 
						    SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
  m_Icons[ICON_FolderClosed] = sFI.iIcon;
  m_ImageList.Attach(hImgList);

	hImgList = (HIMAGELIST)::SHGetFileInfo(_T("C:\\RECYCLER\\"), 
                FILE_ATTRIBUTE_DIRECTORY, 
						    &sFI,
                sizeof(SHFILEINFO), 
						    SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX|SHGFI_OPENICON|SHGFI_SMALLICON);
  m_Icons[ICON_FolderOpen] = sFI.iIcon;
  
  hImgList = (HIMAGELIST)::SHGetFileInfo(_T("C:\\TEMP\\temp.txt"), 
                FILE_ATTRIBUTE_NORMAL, 
						    &sFI,
                sizeof(SHFILEINFO), 
						    SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
  m_Icons[ICON_File] = sFI.iIcon;

  hImgList = (HIMAGELIST)::SHGetFileInfo(_T("C:\\TEMP\\temp.txt"), 
                FILE_ATTRIBUTE_OFFLINE, 
						    &sFI,
                sizeof(SHFILEINFO), 
						    SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
  m_Icons[ICON_Unknown] = sFI.iIcon;

  HICON hicon = theApp.LoadIconA(IDR_ICON_SD);
  m_Icons[ICON_StaticDynamic] = m_ImageList.Add(hicon);
}

void CConfigureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CConfigureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CConfigureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CConfigureDlg::OnFileOpen()
{
  CString cstrBasePath;
  if (_getBaseFolder(cstrBasePath))
  {
    theApp.initServices(AString(cstrBasePath.GetBuffer()));
    _init();
  }
  else
  {
    _reset();
  }
  
}

void CConfigureDlg::OnBnClickedOk()
{
  theApp.deinitServices();

  OnOK();
}
