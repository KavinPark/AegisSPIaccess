
// AegisSPIDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "AegisSPI.h"
#include "AegisSPIDlg.h"
#include "afxdialogex.h"

#include "libMPSSE_spi.h"
#include "ftd2xx.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAegisSPIDlg ��ȭ ����




CAegisSPIDlg::CAegisSPIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAegisSPIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sAddr = _T("0x00000000");
	m_sData = _T("0x00000000");
}

void CAegisSPIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ADDR, m_sAddr);
	DDX_Text(pDX, IDC_EDIT_DATA, m_sData);
}

BEGIN_MESSAGE_MAP(CAegisSPIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_DATA, &CAegisSPIDlg::OnEnChangeEditData)
	ON_EN_CHANGE(IDC_EDIT_ADDR, &CAegisSPIDlg::OnEnChangeEditAddr)
	ON_BN_CLICKED(IDC_BUTTON_WORD_WRITE, &CAegisSPIDlg::OnBnClickedButtonWordWrite)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CAegisSPIDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_WORD_READ, &CAegisSPIDlg::OnBnClickedButtonWordRead)
	ON_BN_CLICKED(IDC_BUTTON_LOOP, &CAegisSPIDlg::OnBnClickedButtonLoop)
	ON_BN_CLICKED(IDC_BUTTON_LOOPBURST, &CAegisSPIDlg::OnBnClickedButtonLoopburst)
END_MESSAGE_MAP()


// CAegisSPIDlg �޽��� ó����

BOOL CAegisSPIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CAegisSPIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CAegisSPIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CAegisSPIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAegisSPIDlg::OnEnChangeEditData()
{
	UpdateData(TRUE);
}


void CAegisSPIDlg::OnEnChangeEditAddr()
{
	UpdateData(TRUE);
}


void CAegisSPIDlg::OnBnClickedButtonConnect()
{
	mobjCommSPI.Open();
}

void CAegisSPIDlg::OnBnClickedButtonWordWrite()
{
	uint8 tx_buffer[8];

	// EDIT value 
	Utils::Convert_Str2Hex(m_sAddr, &tx_buffer[0]);
	Utils::Convert_Str2Hex(m_sData, &tx_buffer[4]);
	tx_buffer[0] |= 0x80;	// Write Cmd

	// SPI Send
	mobjCommSPI.WriteData(tx_buffer, 8);
}



void CAegisSPIDlg::OnBnClickedButtonWordRead()
{
	uint8 tx_buffer[8] = {0, };
	uint8 rx_buffer[8] = {0, };

	// EDIT value : tx_buffer[4..7] = 0
	Utils::Convert_Str2Hex(m_sAddr, &tx_buffer[0]);
	tx_buffer[0] &= ~(0x80);	// Read Cmd

	// SPI Write/Read
	mobjCommSPI.WriteReadData(tx_buffer, rx_buffer, 8);
		// rx_buffer[4..7] valid
	uint32 nData = rx_buffer[4]<<24 | rx_buffer[5]<<16 | rx_buffer[6]<<8 | rx_buffer[7]<<0;

	// Display EDIT window
	CString sData = Utils::Convert_Hex2Str(nData);
	((CEdit *)GetDlgItem(IDC_EDIT_DATA))->SetWindowText(sData);
}



void CAegisSPIDlg::OnBnClickedButtonLoop()
{
	uint8 tx_buffer[8] = {0, };
	uint8 rx_buffer[8] = {0, };

	// EDIT value 
	Utils::Convert_Str2Hex(m_sAddr, &tx_buffer[0]);
	Utils::Convert_Str2Hex(m_sAddr, &tx_buffer[4]);

	// SPI Write/Read
	mobjCommSPI.WriteReadData(tx_buffer, rx_buffer, 8);
		// rx_buffer[4..7] valid
	uint32 nData = rx_buffer[4]<<24 | rx_buffer[5]<<16 | rx_buffer[6]<<8 | rx_buffer[7]<<0;

	// Display EDIT window
	CString sData = Utils::Convert_Hex2Str(nData);
	((CEdit *)GetDlgItem(IDC_EDIT_DATA))->SetWindowText(sData);	
}


void CAegisSPIDlg::OnBnClickedButtonLoopburst()
{
	uint8 tx_buffer[200] = {0, };
	uint8 rx_buffer[200] = {0, };

	// EDIT value 
	Utils::Convert_Str2Hex(m_sAddr, &tx_buffer[0]);

	for (uint32 i=0; i<((200-4)/4); i++)
	{
		tx_buffer[4+i*4] = tx_buffer[0];
		tx_buffer[5+i*4] = tx_buffer[1];
		tx_buffer[6+i*4] = tx_buffer[2];
		tx_buffer[7+i*4] = tx_buffer[3];
	}

	// SPI Write/Read
	mobjCommSPI.WriteReadData(tx_buffer, rx_buffer, 200);
		// rx_buffer[4..7] valid
	uint32 nData = rx_buffer[200-4]<<24 | rx_buffer[200-3]<<16 | rx_buffer[200-2]<<8 | rx_buffer[200-1]<<0;

	// Display EDIT window
	CString sData = Utils::Convert_Hex2Str(nData);
	((CEdit *)GetDlgItem(IDC_EDIT_DATA))->SetWindowText(sData);	
}
