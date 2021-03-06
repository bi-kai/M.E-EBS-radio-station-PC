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
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CRadio_stationDlg)
	enum { IDD = IDD_RADIO_STATION_DIALOG };
	CStatic	m_board_led_YW;
	CStatic	m_ctrlIconOpenoff_YW;
	CComboBox	m_Com_YW;
	CSliderCtrl	m_POWER_SELECT;
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
	int		m_wakeup_time;
	double	m_frequency;
	double	m_frequency_native;
	double	m_unicast_terminal_id;
	double	m_multi_terminal_id_start;
	double	m_multi_terminal_id_end;
	CString	m_power_num;
	CMSComm	m_comm_YW;
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
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnButtonVoice();
	afx_msg void OnButtonIdentify();
	afx_msg void OnButtonBoardReset();
	afx_msg void OnButtonTTS();
	afx_msg void OnButtonNMIC();
	afx_msg void OnButtonAdvanced();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnCancelMode();
	afx_msg void OnReleasedcaptureSliderPower(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonConnect_YW();
	afx_msg void OnComm_YW();
	afx_msg void OnEditchangeComboComselectYw();
	afx_msg void OnSelendokComboComselectYw();
	afx_msg void OnChangeEditUnicast();
	afx_msg void OnChangeEditMulticastStart();
	afx_msg void OnChangeEditMulticastEnd();
	afx_msg void OnChangeEditId();
	afx_msg void OnChangeEditWakeupSeconds();
	afx_msg void OnChangeEditFrequence();
	afx_msg void OnChangeEditBoardFrequency();
	afx_msg void OnRadio20w();
	afx_msg void OnRadio30w();
	afx_msg void OnRadio40w();
	afx_msg void OnRadio50w();
	afx_msg void OnRadio60w();
	afx_msg void OnRadio10w();
	afx_msg void OnButtonTeleMsg();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void power_amplifier_control(int index);
	void W_select();
	void STM32_is_busy(int state);
	bool flag_fre_is_scaning;//正在扫描频谱
	int timer_board_disconnect_times;//定时器3统计尝试连接次数，达到3次则判断子板未连接
	int timer_board_disconnect_times_YW;//定时器6统计尝试连接次数，达到3次则判断运维板未连接
//	LRESULT OnShowTask( WPARAM wParam,LPARAM lParam );
	void ToTray();
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

	int	m_frame_counter;//帧计数器
	int terminal_control_index;//发送给终端的控制索引

	int power_select;//发射整瓦数调整
	
//	unsigned char

private:
	int m_DCom;//广播板串口号
	int m_DCom_YW;//运维板串口号
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;
	int alarm_index;
	
	BOOL SerialPortOpenCloseFlag;//串口打开关闭标志位
	BOOL SerialPortOpenCloseFlag_YW;//运维串口打开关闭标志位
	bool flag_modified;//修改唤醒帧字段标志位
	bool flag_com_init_ack;//上位机软件查询下位机，下位机对查询信息的应答标志位。1:连接成功；0：连接失败；
	bool flag_com_init_ack_YW;//上位机软件查询运维板，运维板对查询信息的应答标志位。1:连接成功；0：连接失败；
	int frame_index;//接收缓冲帧的索引
	int frame_index_YW;//运维串口接收缓冲帧的索引
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
//	bool flag_scan_button;//扫描按钮上文字切换
	int index_resent_data_frame;//重传帧编号，0：空闲(查询帧：查询帧不使用出错重传机制，因为是不停的查询的)；1~3：唤醒帧编号；4：报警帧编号；5：认证帧编号；6：运维板复位帧编号；7：频谱扫描，继电器控制帧；

	CStatusBarCtrl *m_StatBar;//状态栏
	bool flag_voice_broad;//软件界面上开始广播停止广播文字切换标志位
	int voice_broad;//语音广播标志位3：开始广播；4：停止广播；

	bool flag_button_advanced;//“高级”按钮。1：按下；0：弹起；

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIO_STATIONDLG_H__A14BA863_D51F_4F40_943C_E46A6ED6E397__INCLUDED_)
