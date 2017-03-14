// radio_station.h : main header file for the RADIO_STATION application
//

#if !defined(AFX_RADIO_STATION_H__6B1717B3_C598_4D8C_B849_4D6354159F36__INCLUDED_)
#define AFX_RADIO_STATION_H__6B1717B3_C598_4D8C_B849_4D6354159F36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#define SKINSPACE _T("/SPATH:") ////  ע�⣺������������#include�����棡����
/////////////////////////////////////////////////////////////////////////////
// CRadio_stationApp:
// See radio_station.cpp for the implementation of this class
//

class CRadio_stationApp : public CWinApp
{
public:
	CRadio_stationApp();
	int m_nBaud;       //������
	int m_nCom;         //���ں�
	char m_cParity;    //У��
	int m_nDatabits;    //����λ
	int m_nStopbits;    //ֹͣλ

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadio_stationApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRadio_stationApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIO_STATION_H__6B1717B3_C598_4D8C_B849_4D6354159F36__INCLUDED_)
