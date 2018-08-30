
// AegisSPIDlg.h : 헤더 파일
//

#pragma once

#include "CcommSPI.h"

// CAegisSPIDlg 대화 상자
class CAegisSPIDlg : public CDialogEx
{
// 생성입니다.
public:
	CAegisSPIDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AEGISSPI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	// Communication Port Object
	CcommSPI	mobjCommSPI;

	// EDIT box..
	CString m_sAddr;
	CString m_sData;



// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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
