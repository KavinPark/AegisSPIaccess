
// AegisSPIDlg.h : ��� ����
//

#pragma once

#include "CcommSPI.h"

// CAegisSPIDlg ��ȭ ����
class CAegisSPIDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CAegisSPIDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_AEGISSPI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

	// Communication Port Object
	CcommSPI	mobjCommSPI;

	// EDIT box..
	CString m_sAddr;
	CString m_sData;



// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditData();
	afx_msg void OnEnChangeEditAddr();
	afx_msg void OnBnClickedButtonWordWrite();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonWordRead();
	afx_msg void OnBnClickedButtonLooptest();
};
