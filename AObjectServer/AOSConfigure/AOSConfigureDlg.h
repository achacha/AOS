
// AOSConfigureDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CConfigureDlg dialog
class CConfigureDlg : public CDialog
{
// Construction
public:
	CConfigureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AOSCONFIGURE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

  void _initializeImageList();
  BOOL _getBaseFolder(CString& cstrBasePath);
  void _reset();
  void _init();
  void _addFolderNode(HTREEITEM htiParent, const AFilename&);

  // Icon offsets
  CImageList m_ImageList;
  std::vector<int> m_Icons;

public:
  afx_msg void OnFileOpen();
  
  CTreeCtrl m_WebSiteTree;
  afx_msg void OnBnClickedOk();
};
