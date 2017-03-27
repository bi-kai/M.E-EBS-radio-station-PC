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
	HICON m_hIconRed;    //���ڴ�ʱ�ĺ��ͼ����
	HICON m_hIconOff;    //���ڹر�ʱ��ָʾͼ����
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CRadio_stationDlg)
	enum { IDD = IDD_RADIO_STATION_DIALOG };
	CStatic	m_frame_send_state;
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
	int		m_frame_counter;
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
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	bool flag_fre_is_scaning;//����ɨ��Ƶ��
	int timer_board_disconnect_times;//��ʱ��3ͳ�Ƴ������Ӵ������ﵽ3�����ж��Ӱ�δ����
//	LRESULT OnShowTask( WPARAM wParam,LPARAM lParam );
	void ToTray();
	void four_bits_ASCII(unsigned char* a,unsigned char* b,int len,int index);
	void int_bits(int a, unsigned char* b, int len);
	unsigned char XOR(unsigned char *BUFF, int len);
	unsigned char frame_type[2];//֡����:01��ʾ��֡�ǻ���֡��10�������֡��11������֤����֡��00������֤����֡
	unsigned char wakeup_type[2];//����֡����:����01���㲥10���鲥11
	unsigned char communication_fre_point[8];//ͨ��Ƶ�㣺����־λ��ֵ��ʾ��87MHz��100KHzΪ��λ��ƫ����
	unsigned char source_address[36];//Դ��ַ����̨��ID����̨�ĵ绰��ת���Ķ����ƴ������ڴ��ڿ����û�ҲҪ����Ӧ����Ϣ�����Ըñ�־λ��Ϊ�ж˵�һ���ο�
	unsigned char target_address_unicast[24];//����Ŀ���ַ
	unsigned char target_address_multicast_start[24];//�ಥĿ����ʼ��ַ
	unsigned char target_address_multicast_end[24];//�ಥĿ����ֹ��ַ
	unsigned char frame_counter[36];//֡������
	unsigned char MAC_AES[36];//AES��׺

//	int	m_frame_counter;//֡������
	
//	unsigned char

private:
	int m_DCom;
	int m_DStopbits;
	char m_DParity;
	int m_DDatabits;
	LONG m_DBaud;
	int alarm_index;
	
	BOOL SerialPortOpenCloseFlag;//���ڴ򿪹رձ�־λ
	bool flag_modified;//�޸Ļ���֡�ֶα�־λ
	bool flag_com_init_ack;//��λ�������ѯ��λ������λ���Բ�ѯ��Ϣ��Ӧ���־λ��1:���ӳɹ�
	int frame_index;//���ջ���֡������
	int frame_send_index;//���ͻ���������֡������������
	int frame_board_send_index;//�Ӱ�ͨ������֡������
	int index_frequency_point;//��λ������Ƶ�������
//	int index_before_gray;//���ױ���ǰ��������
	int index_after_gray;//���ױ������������
	unsigned char index_wakeup_times;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	unsigned char index_scan_times;//Ƶ��ɨ��֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	unsigned char index_control_times;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	unsigned char index_data_times;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	bool flag_board_modified;//�޸���λ�����ñ�־λ
//	bool flag_scan_button;//ɨ�谴ť�������л�
	int index_resent_data_frame;//�ش�֡���

	CStatusBarCtrl *m_StatBar;//״̬��
	bool flag_voice_broad;//��������Ͽ�ʼ�㲥ֹͣ�㲥�����л���־λ
	int voice_broad;//�����㲥��־λ3����ʼ�㲥��4��ֹͣ�㲥��

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIO_STATIONDLG_H__A14BA863_D51F_4F40_943C_E46A6ED6E397__INCLUDED_)
