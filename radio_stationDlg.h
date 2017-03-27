// radio_stationDlg.h : header file
//
//{{AFX_INCLUDES()
#include "mscomm.h"
//}}AFX_INCLUDES

#if !defined(AFX_RADIO_STATIONDLG_H__A14BA863_D51F_4F40_943C_E46A6ED6E397__INCLUDED_)
#define AFX_RADIO_STATIONDLG_H__A14BA863_D51F_4F40_943C_E46A6ED6E397__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRadio_stationDlg dialog

class CRadio_stationDlg : public CDialog
{
// Construction
public:
	CRadio_stationDlg(CWnd* pParent = NULL);	// standard constructor
	HICON m_hIconRed;    //串口打开时的红灯图标句柄
	HICON m_hIconOff;    //串口关闭时的指示图标句柄
// Dialog Data
	//{{AFX_DATA(CRadio_stationDlg)
	enum { IDD = IDD_RADIO_STATION_DIALOG };
	CListCtrl	m_rssi_list;
	CComboBox	m_alarm_command;
	CStatic	m_board_connect;
	CStatic	m_board_led;
	CStatic	m_ctrlIconOpenoff;
	CComboBox	m_StopBits;
	CComboBox	m_Speed;
	CComboBox	m_Parity;
	CComboBox	m_DataBits;
	CComboBox	m_Com;
	int		m_radio_wakeup;
	CMSComm	m_comm;
	double	m_radio_id;
	double	m_frame_counter;
	int		m_wakeup_time;
	double	m_frequency;
	double	m_frequency_native;
	double	m_unicast_terminal_id;
	double	m_multi_terminal_id_start;
	double	m_multi_terminal_id_end;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadio_stationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRadio_stationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRadioBroadcast();
	afx_msg void OnRadioMulticast();
	afx_msg void OnRadioUnicase();
	afx_msg void OnButtonWakeup();
	afx_msg void OnSelendokComboComselect();
	afx_msg void OnSelendokComboDatabits();
	afx_msg void OnSelendokComboParity();
	afx_msg void OnSelendokComboSpeed();
	afx_msg void OnSelendokComboStopbits();
	afx_msg void OnButtonConnectboard();
	afx_msg void OnComm1();
	afx_msg void OnButtonModify();
	afx_msg void OnButtonIDSave();
	afx_msg void OnKillfocusEditId();
	afx_msg void OnKillfocusEditFrequence();
	afx_msg void OnKillfocusEditWakeupSeconds();
	afx_msg void OnButtonBoardModify();
	afx_msg void OnButtonBoardConfig();
	afx_msg void OnKillfocusEditUnicast();
	afx_msg void OnKillfocusEditMulticastStart();
	afx_msg void OnKillfocusEditMulticastEnd();
	afx_msg void OnSelendokComboAlarmType();
	afx_msg void OnButtonAlarm();
	afx_msg void OnKillfocusEditBoardFrequency();
	afx_msg void OnButtonScan();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void four_bits_ASCII(unsigned char* a,unsigned char* b,int len,int index);
	void int_bits(int a, unsigned char* b, int len);
	unsigned char XOR(unsigned char *BUFF, int len);
	unsigned char frame_type[2];//帧类型:01表示本帧是唤醒帧、10代表控制帧、11代表认证数据帧、00代表认证续传帧
	unsigned char wakeup_type[2];//唤醒帧类型:单播01、广播10、组播11
	unsigned char communication_fre_point[8];//通信频点：本标志位的值表示对87MHz以100KHz为单位的偏移量
	unsigned char source_address[36];//源地址：电台的ID，电台的电话卡转换的二进制串，由于存在跨区用户也要收听应急信息，所以该标志位作为中端的一个参考
	unsigned char target_address_unicast[24];//单播目标地址
	unsigned char target_address_multicast_start[24];//多播目标起始地址
	unsigned char target_address_multicast_end[24];//多播目标终止地址
	unsigned char frame_counter[36];//帧计数器
	unsigned char MAC_AES[36];//AES后缀
	
//	unsigned char

private:
	int m_DCom;
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;
	int alarm_index;
	
	BOOL SerialPortOpenCloseFlag;//串口打开关闭标志位
	bool flag_modified;//修改唤醒帧字段标志位
	bool flag_com_init_ack;//上位机软件查询下位机，下位机对查询信息的应答标志位
	int frame_index;//接收缓冲帧的索引
	int frame_send_index;//发送缓冲区无线帧比特流计数器
	int frame_board_send_index;//子板通信数据帧计数器
	int index_frequency_point;//下位机反馈频点计数器
//	int index_before_gray;//格雷编码前数组索引
	int index_after_gray;//格雷编码后数组索引
	unsigned char index_wakeup_times;//连接帧发送次数计数器，上下位机通信，保证每帧数据都不同
	unsigned char index_scan_times;//频谱扫描帧发送次数计数器，上下位机通信，保证每帧数据都不同
	unsigned char index_control_times;//控制帧发送次数计数器，上下位机通信，保证每帧数据都不同
	unsigned char index_data_times;//数据帧发送次数计数器，上下位机通信，保证每帧数据都不同
	bool flag_board_modified;//修改下位机配置标志位

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIO_STATIONDLG_H__A14BA863_D51F_4F40_943C_E46A6ED6E397__INCLUDED_)
