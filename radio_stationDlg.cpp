// radio_stationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "radio_station.h"
#include "radio_stationDlg.h"
#include "encrypt.h"
#include <math.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RADIO_ID_START 34095233
#define RADIO_ID_END 1073741823
#define AREA_TERMINAL_ID_START 4353
#define AREA_TERMINAL_ID_END 262143
#define FREQUENCY_POINT 0X10//FM����Ƶ��

#define FREQUENCY_TERMINAL_START 76.0//�ն�ͨ��Ƶ����Сֵ
#define FREQUENCY_TERMINAL_END 88.0//�ն�ͨ��Ƶ�����ֵ

unsigned char frame_board_check[7+2]={'$','r','d','y','_'};//���Ӽ��֡
unsigned char frame_board_frequency[7+2]={'$','f','r','e','_'};//Ƶ�׼��֡
unsigned char frame_board_control[9+2]={'$','c','o','n','_'};//����֡
//unsigned char frame_board_before_gray[168]={0};//���ױ���ǰ������֡�ı�����
unsigned char frame_board_after_gray[168*2]={0};//���ױ���������֡�ı�����
unsigned char frame_board_data[400+2]={'$','d','a','t','_'};//��������֡
unsigned char frame_board_bits[1100]={0};//��̨�ն�֡����
unsigned char frame_receive[100]={0};//���ջ�����

extern unsigned char cipherkey_base[16];//��̨˽Կ

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadio_stationDlg dialog

CRadio_stationDlg::CRadio_stationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRadio_stationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRadio_stationDlg)
	m_radio_wakeup = 0;//-1
	m_radio_id = 0.0;
	m_frame_counter = 0.0;
	m_wakeup_time = 0;
	m_frequency = 0.0;
	m_frequency_native = 0.0;
	m_unicast_terminal_id = 0.0;
	m_multi_terminal_id_start = 0.0;
	m_multi_terminal_id_end = 0.0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRadio_stationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadio_stationDlg)
	DDX_Control(pDX, IDC_LIST1, m_rssi_list);
	DDX_Control(pDX, IDC_COMBO_ALARM_TYPE, m_alarm_command);
	DDX_Control(pDX, IDC_STATIC_BOARDCONNECT, m_board_connect);
	DDX_Control(pDX, IDC_STATIC_BOARD_LED, m_board_led);
	DDX_Control(pDX, IDC_STATIC_OPENOFF, m_ctrlIconOpenoff);
	DDX_Control(pDX, IDC_COMBO_STOPBITS, m_StopBits);
	DDX_Control(pDX, IDC_COMBO_SPEED, m_Speed);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_Parity);
	DDX_Control(pDX, IDC_COMBO_DATABITS, m_DataBits);
	DDX_Control(pDX, IDC_COMBO_COMSELECT, m_Com);
	DDX_Radio(pDX, IDC_RADIO_BROADCAST, m_radio_wakeup);
	DDX_Control(pDX, IDC_MSCOMM1, m_comm);
	DDX_Text(pDX, IDC_EDIT_ID, m_radio_id);
	DDX_Text(pDX, IDC_EDIT_FRAME_COUNTER, m_frame_counter);
	DDX_Text(pDX, IDC_EDIT_WAKEUP_SECONDS, m_wakeup_time);
	DDX_Text(pDX, IDC_EDIT_FREQUENCE, m_frequency);
	DDX_Text(pDX, IDC_EDIT_BOARD_FREQUENCY, m_frequency_native);
	DDX_Text(pDX, IDC_EDIT_UNICAST, m_unicast_terminal_id);
	DDX_Text(pDX, IDC_EDIT_MULTICAST_START, m_multi_terminal_id_start);
	DDX_Text(pDX, IDC_EDIT_MULTICAST_END, m_multi_terminal_id_end);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRadio_stationDlg, CDialog)
	//{{AFX_MSG_MAP(CRadio_stationDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_BROADCAST, OnRadioBroadcast)
	ON_BN_CLICKED(IDC_RADIO_MULTICAST, OnRadioMulticast)
	ON_BN_CLICKED(IDC_RADIO_UNICASE, OnRadioUnicase)
	ON_BN_CLICKED(IDC_BUTTON_WAKEUP, OnButtonWakeup)
	ON_CBN_SELENDOK(IDC_COMBO_COMSELECT, OnSelendokComboComselect)
	ON_CBN_SELENDOK(IDC_COMBO_DATABITS, OnSelendokComboDatabits)
	ON_CBN_SELENDOK(IDC_COMBO_PARITY, OnSelendokComboParity)
	ON_CBN_SELENDOK(IDC_COMBO_SPEED, OnSelendokComboSpeed)
	ON_CBN_SELENDOK(IDC_COMBO_STOPBITS, OnSelendokComboStopbits)
	ON_BN_CLICKED(IDC_BUTTON_CONNECTBOARD, OnButtonConnectboard)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonIDSave)
	ON_EN_KILLFOCUS(IDC_EDIT_ID, OnKillfocusEditId)
	ON_EN_KILLFOCUS(IDC_EDIT_FREQUENCE, OnKillfocusEditFrequence)
	ON_EN_KILLFOCUS(IDC_EDIT_WAKEUP_SECONDS, OnKillfocusEditWakeupSeconds)
	ON_BN_CLICKED(IDC_BUTTON_BOARD_MODIFY, OnButtonBoardModify)
	ON_BN_CLICKED(IDC_BUTTON_BOARD_CONFIG, OnButtonBoardConfig)
	ON_EN_KILLFOCUS(IDC_EDIT_UNICAST, OnKillfocusEditUnicast)
	ON_EN_KILLFOCUS(IDC_EDIT_MULTICAST_START, OnKillfocusEditMulticastStart)
	ON_EN_KILLFOCUS(IDC_EDIT_MULTICAST_END, OnKillfocusEditMulticastEnd)
	ON_CBN_SELENDOK(IDC_COMBO_ALARM_TYPE, OnSelendokComboAlarmType)
	ON_BN_CLICKED(IDC_BUTTON_ALARM, OnButtonAlarm)
	ON_EN_KILLFOCUS(IDC_EDIT_BOARD_FREQUENCY, OnKillfocusEditBoardFrequency)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadio_stationDlg message handlers

BOOL CRadio_stationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	
	// TODO: Add extra initialization here
	
/*************************����*****************************/
	m_DCom=1;
	m_DStopbits=1;
	m_DParity='N';
	m_DDatabits=8;
	m_DBaud=115200;
	alarm_index=1;
	SerialPortOpenCloseFlag=FALSE;//Ĭ�Ϲرմ���
	flag_modified=0;//�޸ĵ�̨ID��־λ
	flag_com_init_ack=0;//δ�յ���λ����Ӧ��
	frame_index=0;//���ջ���֡��������ʼ��
	frame_send_index=0;//���ͻ����������ʼ��
	index_wakeup_times=0;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	index_scan_times=0;//Ƶ��ɨ��֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	index_control_times=0;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	index_data_times=0;//����֡���ʹ���������������λ��ͨ�ţ���֤ÿ֡���ݶ���ͬ
	flag_board_modified=0;//�Ӱ��޸ı�־λ
	frame_board_send_index=5;//�Ӱ�ͨ������֡��������ǰ��λ��ռ��
//	index_before_gray=0;
	index_after_gray=0;
	index_frequency_point=0;//Ƶ�����������

	m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);
// 	GetDlgItem(IDC_COMBO_COMSELECT)->SetWindowText(_T("COM1"));
// 	GetDlgItem(IDC_COMBO_SPEED)->SetWindowText(_T("115200"));
// 	GetDlgItem(IDC_COMBO_PARITY)->SetWindowText(_T("NONE"));
// 	GetDlgItem(IDC_COMBO_DATABITS)->SetWindowText(_T("8"));
// 	GetDlgItem(IDC_COMBO_STOPBITS)->SetWindowText(_T("1"));
	m_Com.SetCurSel(0);
	m_DataBits.SetCurSel(0);
	m_Speed.SetCurSel(5);
	m_StopBits.SetCurSel(0);
	m_Parity.SetCurSel(0);

	GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WAKEUP_SECONDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FREQUENCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_ALARM)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BOARD_CONFIG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_UNICAST)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_END)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(FALSE);

	m_comm.SetCommPort(1); //ѡ��com1
	m_comm.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_comm.SetInBufferSize(1024); //�������뻺������С
	m_comm.SetOutBufferSize(10240); //���������������С
	m_comm.SetSettings("115200,n,8,1"); //������115200����У�飬8������λ��1��ֹͣλ	 
	m_comm.SetRThreshold(1); //����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_comm.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
	//	 m_comm.GetInput();    //��Ԥ���������������������
	if(!m_comm.GetPortOpen())
	{		 
		//		m_comm.SetPortOpen(TRUE);//�򿪴���(�˴����ش򿪣�����á��򿪴��ڡ���ťʵ��)
	}
	else
		 MessageBox("can not open serial port");
/**************************INI����*******************************/
	CFileFind finder;   //�����Ƿ����ini�ļ����������ڣ�������һ���µ�Ĭ�����õ�ini�ļ��������ͱ�֤�����Ǹ��ĺ������ÿ�ζ�����
	BOOL ifFind = finder.FindFile(_T(".\\config_radiostation.ini"));
	if(!ifFind)
	{
		::WritePrivateProfileString("ConfigInfo","ID","34095233",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","frame_counter","100",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","wakeup_time","1",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","frequency","79.0",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","frequency_native","79.0",".\\config_radiostation.ini");//��λ��RDA5820����Ƶ��
	}
		CString strBufferReadConfig,strtmpReadConfig;
		
		GetPrivateProfileString("ConfigInfo","ID","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=strBufferReadConfig;
		m_radio_id= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
		
		GetPrivateProfileString("ConfigInfo","frame_counter","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frame_counter= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
	
		GetPrivateProfileString("ConfigInfo","wakeup_time","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_wakeup_time= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

		GetPrivateProfileString("ConfigInfo","frequency","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frequency= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

		GetPrivateProfileString("ConfigInfo","frequency_native","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frequency_native= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

	UpdateData(FALSE);
//	((CButton *)GetDlgItem(IDC_RADIO_BROADCAST))->SetCheck(TRUE);//ѡ��
/**************************RSSI�б�����**********************************/
	m_rssi_list.ModifyStyle( 0, LVS_REPORT );// ����ģʽ
	m_rssi_list.SetExtendedStyle(m_rssi_list.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);// �����+��ѡ��
	
	m_rssi_list.InsertColumn(0,"Ƶ��(MHz)", LVCFMT_LEFT, 40);
	m_rssi_list.InsertColumn(1,"RSSI", LVCFMT_LEFT, 40);
	
	CRect rect;
	m_rssi_list.GetClientRect(rect); //��õ�ǰ�ͻ�����Ϣ
	m_rssi_list.SetColumnWidth(0, rect.Width()*6/11); //�����еĿ�ȡ�
	m_rssi_list.SetColumnWidth(1, rect.Width()*3/11);
	int nRow = m_rssi_list.InsertItem(0,"79.2");//������
	m_rssi_list.SetItemText(nRow,1,"44");//��������
	
	LVFINDINFO info;
	int nIndex;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = "79.4";
	nIndex = m_rssi_list.FindItem(&info);  // nIndexΪ����(��0��ʼ)

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRadio_stationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRadio_stationDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRadio_stationDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CRadio_stationDlg::OnRadioBroadcast() 
{
	// TODO: Add your control notification handler code here
	m_radio_wakeup=0;//��һ����ѡ��ť��ѡ������
	GetDlgItem(IDC_EDIT_UNICAST)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_END)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnRadioMulticast() 
{
	// TODO: Add your control notification handler code here
	m_radio_wakeup=2;//�ڶ�����ѡ��ť��ѡ������
	GetDlgItem(IDC_EDIT_UNICAST)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MULTICAST_END)->EnableWindow(TRUE);
	OnKillfocusEditMulticastStart();
	OnKillfocusEditMulticastEnd();
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnRadioUnicase() 
{
	// TODO: Add your control notification handler code here
	m_radio_wakeup=1;//��������ѡ��ť��ѡ������
	GetDlgItem(IDC_EDIT_UNICAST)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MULTICAST_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_END)->EnableWindow(FALSE);
	OnKillfocusEditUnicast();
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnButtonWakeup() 
{
	// TODO: Add your control notification handler code here//
	int k=0;
	int i=0;
	unsigned char char_buf[4][4]={0};//AES����
	unsigned char bits_buf[128]={0};//AES����
	unsigned char before_gray[12]={0};//���ױ���ǰ��12λ�Ĵ�
	unsigned char after_gray[24]={0};//���ױ�����24λ�Ĵ�

	frame_board_send_index=5;//�Ӱ�ͨ������֡��������ǰ��λ��ռ��
	
	frame_type[0]=0;//֡���ͣ�����֡01
	frame_type[1]=1;

	i=(int)((m_frequency-FREQUENCY_TERMINAL_START)*10);//Ƶ���ȡ�������ƻ�
	int_bits(i,communication_fre_point,8);
	
	i=(int)m_radio_id;//��̨ID
	source_address[35]=0;
	source_address[34]=0;
	source_address[33]=0;
	source_address[32]=0;
	source_address[31]=0;
	source_address[30]=0;
	int_bits(i,source_address,30);
	
	i=(int)m_frame_counter;
	frame_counter[35]=0;
	frame_counter[34]=0;
	frame_counter[33]=0;
	frame_counter[32]=0;
	int_bits(i,frame_counter,32);
/*****************��ʼ��֡********************************/
	frame_board_bits[0]=frame_type[0];
	frame_board_bits[1]=frame_type[1];
	frame_send_index=4;//ǰ���Ѿ�����4���ֽڣ��Ӵ˿�ʼ++
	for (k=0;k<8;k++)//ͨ��Ƶ��
	{
		frame_board_bits[frame_send_index]=communication_fre_point[k];
		frame_send_index++;
	}
	for (k=0;k<36;k++)//Դ��ַ
	{
		frame_board_bits[frame_send_index]=source_address[k];
		frame_send_index++;
	}

/*****************���ֻ���֡�Ĳ�ͬ��start*******************************/
	if (m_radio_wakeup==0)//�㲥����֡
	{		
		wakeup_type[0]=1;//����01���㲥10���鲥11
		wakeup_type[1]=0;
	} 
	else if(m_radio_wakeup==1)//��������֡
	{
		wakeup_type[0]=0;//����01���㲥10���鲥11
		wakeup_type[1]=1;

		i=(int)(m_unicast_terminal_id-m_radio_id*pow(2,18));//�ն�ID
		int_bits(i,target_address_unicast,24);
		for (k=0;k<24;k++)//תΪ��������֡
		{
			frame_board_bits[frame_send_index]=target_address_unicast[k];
			frame_send_index++;
		}
	}
	else if (m_radio_wakeup==2)//�鲥����֡
	{		
		wakeup_type[0]=1;//����01���㲥10���鲥11
		wakeup_type[1]=1;

		i=(int)(m_multi_terminal_id_start-m_radio_id*pow(2,18));//�ն���ʼID
		int_bits(i,target_address_multicast_start,24);
		for (k=0;k<24;k++)//תΪ��������֡
		{
			frame_board_bits[frame_send_index]=target_address_multicast_start[k];
			frame_send_index++;
		}

		i=(int)(m_multi_terminal_id_end-m_radio_id*pow(2,18));//�ն���ֹID
		int_bits(i,target_address_multicast_end,24);
		for (k=0;k<24;k++)//תΪ��������֡
		{
			frame_board_bits[frame_send_index]=target_address_multicast_end[k];
			frame_send_index++;
		}
	}
/*****************���ֻ���֡�Ĳ�ͬ��end*********************************/

	frame_board_bits[2]=wakeup_type[0];
	frame_board_bits[3]=wakeup_type[1];
	for (k=0;k<36;k++)//֡������
	{
		frame_board_bits[frame_send_index]=frame_counter[k];
		frame_send_index++;
	}
	
/*****************AES����********************************/
	for(i=0;i<128;i++){
		if(i<frame_send_index){//�Ѹ��������������к�36λǰ�����ݷŵ�aes_bits[]��
			bits_buf[i]=frame_board_bits[i];
		}else{
			bits_buf[i]=0;//����128λ���㣬����128����˴���ִ�У���ȡǰ128λ
		}
	}
	bit_char(bits_buf,char_buf);
	Encrypt(char_buf,cipherkey_base);
	char_bit(char_buf,bits_buf);
	for (k=0;k<36;k++)//AES��
	{
		frame_board_bits[frame_send_index]=bits_buf[k];
		frame_send_index++;
	}
/*****************gray����************************************/
	index_after_gray=0;
	for (k=0;k<(frame_send_index/12);k++)//���ױ������
	{
		for (i=0;i<12;i++)
		{
			before_gray[i]=frame_board_bits[k*12+i];
		}
		gray_encode(before_gray,after_gray);
		for (i=0;i<24;i++)
		{
			frame_board_after_gray[index_after_gray]=after_gray[i];
			index_after_gray++;
		}
		
	}

//	CString str1;
//	str1.Format("%d",k);
//	AfxMessageBox(str1,MB_OK,0);
/******************���ڷ�������*******************************/
	if (index_data_times<200)
	{
		index_data_times++;
	} 
	else
	{
		index_data_times=0;
	}
	frame_board_data[frame_board_send_index]=index_data_times;
	frame_board_send_index++;
	frame_board_data[frame_board_send_index]=(index_after_gray/4)/256;
	frame_board_send_index++;
	frame_board_data[frame_board_send_index]=(index_after_gray/4)%256+1;//���һλ�����У���
	frame_board_send_index++;
	four_bits_ASCII(frame_board_after_gray,frame_board_data,index_after_gray,frame_board_send_index);
	frame_board_send_index+=index_after_gray/4;
	frame_board_data[frame_board_send_index]=XOR(frame_board_data,frame_board_send_index);
	frame_board_send_index++;
	frame_board_data[frame_board_send_index]='\r';
	frame_board_send_index++;
	frame_board_data[frame_board_send_index]='\n';
	frame_board_send_index++;

	if (frame_board_data[frame_board_send_index]=='$')
	{
		frame_board_data[frame_board_send_index]++;//����������$����ֵ��һ
	}
	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(frame_board_send_index);
	
	for (i=0;i<frame_board_send_index;i++)
	{
		Array.SetAt(i,frame_board_data[i]);
	}
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//��������
	}	

}
//frame_board_bits
void CRadio_stationDlg::OnSelendokComboComselect() 
{
	// TODO: Add your control notification handler code here
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
	app->m_nCom=m_Com.GetCurSel()+1;
	m_DCom=app->m_nCom;
	UpdateData();
}

void CRadio_stationDlg::OnSelendokComboDatabits() 
{
	// TODO: Add your control notification handler code here
	int i=m_DataBits.GetCurSel();
	switch(i)
	{
	case 0:
		i=8;
		break;
	case 1:
		i=7;
		break;
	case 2:
		i=6;
		break;
	}
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
	app->m_nDatabits=i;
	m_DDatabits=app->m_nDatabits;
	UpdateData();
}

void CRadio_stationDlg::OnSelendokComboParity() 
{
	// TODO: Add your control notification handler code here
	char temp;
	int i=m_Parity.GetCurSel();
	switch(i)
	{
	case 0:
		temp='N';
		break;
	case 1:
		temp='O';
		break;
	case 2:
		temp='E';
		break;
	}
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
	app->m_cParity=temp;
	m_DParity=app->m_cParity;
	UpdateData();
}

void CRadio_stationDlg::OnSelendokComboSpeed() 
{
	// TODO: Add your control notification handler code here
	int i=m_Speed.GetCurSel();
	switch(i)
	{
	case 0:
		i=300;
		break;
	case 1:
		i=600;
		break;
	case 2:
		i=1200;
		break;
	case 3:
		i=2400;
		break;
	case 4:
		i=4800;
		break;
	case 5:
		i=9600;
		break;
	case 6:
		i=19200;
		break;
	case 7:
		i=38400;
		break;
	case 8:
		i=43000;
		break;
	case 9:
		i=56000;
		break;
	case 10:
		i=57600;
		break;
	case 11:
		i=115200;
		break;
	default:
		break;
		
	}
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
	app->m_nBaud=i;
	m_DBaud=app->m_nBaud;
	UpdateData();	
}

void CRadio_stationDlg::OnSelendokComboStopbits() 
{
	// TODO: Add your control notification handler code here
	int i=m_StopBits.GetCurSel();
	switch(i)
	{
	case 0:
		i=1;
		break;
	case 1:
		i=2;
		break;
	}
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //����ָ��Ӧ�ó������ָ��
	app->m_nStopbits=i;
	m_DStopbits=app->m_nStopbits;
	UpdateData();
}

void CRadio_stationDlg::OnButtonConnectboard() 
{
	// TODO: Add your control notification handler code here
	char buff[2];
	CString string1="",string2="";
	buff[1]='\0';
	buff[0]=m_DParity;
	string1.Format(_T("%d"),m_DBaud);
	string1+=",";
	string2=buff;
	string1+=string2;
	string1+=",";
	string2.Format(_T("%d"),m_DDatabits); 
	string1+=string2;
	string1+=",";
	string2.Format(_T("%d"),m_DStopbits);
	string1+=string2;
/*
	CString   tmp;
	tmp.Format( "%d ",string1);
	MessageBox( "config:"+string1);
*/
	if(SerialPortOpenCloseFlag==FALSE)
	{
		SerialPortOpenCloseFlag=TRUE;

		//�����Ǵ��ڵĳ�ʼ������
		if(m_comm.GetPortOpen())//�򿪶˿�ǰ�ļ�⣬�ȹأ��ٿ�
			MessageBox("can not open serial port");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm.SetCommPort(m_DCom); //ѡ��˿ڣ�Ĭ����com1
		m_comm.SetSettings((LPSTR)(LPCTSTR)string1); //������9600����У�飬8������λ��1��ֹͣλ
		if(!m_comm.GetPortOpen())
		{			
			m_comm.SetPortOpen(TRUE);//�򿪴���
			GetDlgItem(IDC_BUTTON_CONNECTBOARD)->SetWindowText("�رմ���");

			m_ctrlIconOpenoff.SetIcon(m_hIconRed);
			UpdateData();

			if (index_wakeup_times<200)
			{
				index_wakeup_times++;
			} 
			else
			{
				index_wakeup_times=0;
			}
			frame_board_check[5]=index_wakeup_times;
			frame_board_check[6]=XOR(frame_board_check,6);
			frame_board_check[7]='\r';
			frame_board_check[8]='\n';
			CByteArray Array;
			Array.RemoveAll();
			Array.SetSize(7+2);
									
			for (int i=0;i<(7+2);i++)
			{
				Array.SetAt(i,frame_board_check[i]);
			}

			if(m_comm.GetPortOpen())
			{
				m_comm.SetOutput(COleVariant(Array));//��������
			}
		}
		else
			MessageBox("�޷��򿪴��ڣ������ԣ�");	 
	}
	else
	{
		SerialPortOpenCloseFlag=FALSE;
		GetDlgItem(IDC_BUTTON_CONNECTBOARD)->SetWindowText("�򿪴���");
		m_ctrlIconOpenoff.SetIcon(m_hIconOff);
		m_board_led.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("�忨δ����!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ALARM)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(FALSE);
		flag_com_init_ack=0;//�Ӱ�δ����
		m_comm.SetPortOpen(FALSE);//�رմ���

	}
}
// 		GetDlgItem(IDC_BUTTON_SYSTEMCHECK)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_BUTTON_ICCHECK)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_BUTTON3_POWERCHECK)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
// 		GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);

BEGIN_EVENTSINK_MAP(CRadio_stationDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CRadio_stationDlg)
	ON_EVENT(CRadio_stationDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm1, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CRadio_stationDlg::OnComm1() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //����BYTE����
	CString strDisp="",strTmp="";
	int frequency_point=0;//Ƶ��ɨ����ܵ�Ƶ����
	double frequency_buf=0;//Ƶ�����
	
	if((m_comm.GetCommEvent()==2)) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{
		variant_inp=m_comm.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		}
		
		//			AfxMessageBox("OK",MB_OK,0);
		//			frame=frame_len[frame_index];
		//			frame_lock=0;
		//			frame_len[frame_index]=0;
		
		frame_index=0;
		for(k=0;k<len;k++)//������ת��ΪCString����
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
				if (rxdata[0]!='$')
				{
					return;//֡���ݴ�����
				}
			frame_receive[frame_index]=bt;
			frame_index++;
// 			strTmp.Format("%c",bt);    //���ַ�������ʱ����strtemp���
// 			strDisp+=strTmp;  //������ձ༭���Ӧ�ַ���
			
		}
//		AfxMessageBox(strDisp,MB_OK,0);
	if ((flag_com_init_ack==0)&&(frame_receive[1]=='r')&&(frame_receive[2]=='d')&&(frame_receive[3]=='y')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_wakeup_times)&&(frame_receive[7]==XOR(frame_receive,7)))//�״���������
	{
		flag_com_init_ack=1;
		m_board_led.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("�忨������!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(TRUE);
	}else if ((flag_com_init_ack==1)&&(frame_receive[1]=='f')&&(frame_receive[2]=='r')&&(frame_receive[3]=='e')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_scan_times)&&(frame_receive[9]==XOR(frame_receive,9)))//Ƶ��ɨ��
	{
		frequency_buf=76.0+(double)frame_receive[7]/10;
		strTmp.Format("%.1f",frequency_buf);
		m_rssi_list.InsertItem(frame_receive[7],strTmp);//������
		strTmp.Format("%d",frame_receive[8]);
		m_rssi_list.SetItemText(frame_receive[7],1,strTmp);//��������
		m_rssi_list.SendMessage(WM_VSCROLL,SB_BOTTOM,NULL); //�����ݹ���

	}else if ((flag_com_init_ack==1)&&(frame_receive[1]=='c')&&(frame_receive[2]=='o')&&(frame_receive[3]=='n')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[7]==XOR(frame_receive,7)))//����֡
	{
		AfxMessageBox("��λ��Ƶ�����óɹ���",MB_OK,0);


	}else if ((flag_com_init_ack==1)&&(frame_receive[1]=='d')&&(frame_receive[2]=='a')&&(frame_receive[3]=='t')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_data_times)&&(frame_receive[7]==XOR(frame_receive,7)))//����֡������Ϣ
	{
		m_board_led.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("�忨������!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(TRUE);
	} 
	else
	{
		AfxMessageBox("��λ��֡�д���",MB_OK,0);
	}
// 		if (framelen==(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))
// 		{
// 			//֡�������
// 			//  			CString buf;
// 			//  			buf.Format("%d",frame_index);
// 			//  			AfxMessageBox(buf,MB_OK,0);
// 			
// 			frame_len[frame_index]=framelen;
// 			framelen=0;
// 			frame_flag[frame_index]=1;
// 			//			unsigned char* frame_buf=frame_receive[frame_index];
// 			for (short i=0;i<received_frame_size;i++)
// 			{
// 				if (frame_flag[i]==1)
// 				{
// 					decodeheads (frame_receive[i]);
// 					frame_flag[i]=0;//���Ϊδʹ��
// 				}
// 			}
// 			//			frame_index++;//�յ�������һ֡
// 			if(frame_index==received_frame_size) frame_index=0;
// 			
// 		}
// 		m_showmsg+=strDisp;
		UpdateData(FALSE);
	}	
}

void CRadio_stationDlg::OnButtonModify() 
{
	// TODO: Add your control notification handler code here
	if (flag_modified==0)
	{
		flag_modified=1;//��ԭΪ�����޸�ǰ�İ���״̬
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_WAKEUP_SECONDS)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FREQUENCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(_T("ȡ��"));
	} 
	else
	{
		CString strBufferReadConfig,strtmpReadConfig;
		
		GetPrivateProfileString("ConfigInfo","ID","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=strBufferReadConfig;
		m_radio_id= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
		
		GetPrivateProfileString("ConfigInfo","wakeup_time","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_wakeup_time= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);
		
		GetPrivateProfileString("ConfigInfo","frequency","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frequency= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);

		flag_modified=0;
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_WAKEUP_SECONDS)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FREQUENCE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(_T("�޸�"));
		UpdateData(FALSE);
		
	}
	
}

void CRadio_stationDlg::OnButtonIDSave() 
{
	// TODO: Add your control notification handler code here
	flag_modified=0;//��ԭΪ�����޸�ǰ�İ���״̬
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WAKEUP_SECONDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FREQUENCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(_T("�޸�"));	
	UpdateData();
	CString strTemp;
	strTemp.Format(_T("%f"),m_radio_id);
	::WritePrivateProfileString("ConfigInfo","ID",strTemp,".\\config_radiostation.ini");
	strTemp.Format(_T("%d"),m_wakeup_time);
	::WritePrivateProfileString("ConfigInfo","wakeup_time",strTemp,".\\config_radiostation.ini");
	strTemp.Format(_T("%.1f"),m_frequency);
	::WritePrivateProfileString("ConfigInfo","frequency",strTemp,".\\config_radiostation.ini");

	OnKillfocusEditMulticastStart();
	OnKillfocusEditMulticastEnd();
	OnKillfocusEditUnicast();
}

unsigned char CRadio_stationDlg::XOR(unsigned char *BUFF, int len)
{
	unsigned char result=0;
	int i;
	for(result=BUFF[0],i=1;i<len;i++)
	{
		result ^= BUFF[i];
	}
	return result;
}

// void CRadio_stationDlg::OnChangeEditId() 
// {
// 	// TODO: If this is a RICHEDIT control, the control will not
// 	// send this notification unless you override the CDialog::OnInitDialog()
// 	// function and call CRichEditCtrl().SetEventMask()
// 	// with the ENM_CHANGE flag ORed into the mask.
// 	
// 	// TODO: Add your control notification handler code here
// 	UpdateData(TRUE);    
// 	if ((m_radio_id>200000000) || (m_radio_id<100000000))    
// 	{        
// 		m_radio_id = 15929724771;        
// 		UpdateData(FALSE);    
// 	}
// }
// 
// void CRadio_stationDlg::OnChangeEditFrequence() 
// {
// 	// TODO: If this is a RICHEDIT control, the control will not
// 	// send this notification unless you override the CDialog::OnInitDialog()
// 	// function and call CRichEditCtrl().SetEventMask()
// 	// with the ENM_CHANGE flag ORed into the mask.
// 	
// 	// TODO: Add your control notification handler code here
// 	UpdateData(TRUE);    
// 	if ((m_frequency>108.0) || (m_frequency<70.0))    
// 	{        
// 		m_frequency = 93.6;           
// 	}else{
// 		m_frequency=(int)(m_frequency*10);
// 		m_frequency/=10;
// 	}
// 	UpdateData(FALSE); 
// }
// 
// void CRadio_stationDlg::OnChangeEditWakeupSeconds() 
// {
// 	// TODO: If this is a RICHEDIT control, the control will not
// 	// send this notification unless you override the CDialog::OnInitDialog()
// 	// function and call CRichEditCtrl().SetEventMask()
// 	// with the ENM_CHANGE flag ORed into the mask.
// 	
// 	// TODO: Add your control notification handler code here
// 	UpdateData(TRUE);    
// 	if ((m_wakeup_time>10) || (m_wakeup_time<0))    
// 	{        
// 		m_wakeup_time = 1;        
// 		UpdateData(FALSE);    
// 	}
// }

void CRadio_stationDlg::OnKillfocusEditId() 
{
	// TODO: Add your control notification handler code here
	CString strBufferReadConfig,strtmpReadConfig;
	UpdateData(TRUE);    
	if ((m_radio_id>RADIO_ID_END) || (m_radio_id<RADIO_ID_START))    
	{   
		GetPrivateProfileString("ConfigInfo","ID","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=strBufferReadConfig;
		m_radio_id= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);       
		UpdateData(FALSE);    
	}
}

void CRadio_stationDlg::OnKillfocusEditFrequence() 
{
	// TODO: Add your control notification handler code here
	CString strBufferReadConfig,strtmpReadConfig;
	UpdateData(TRUE);    
	if ((m_frequency>FREQUENCY_TERMINAL_END) || (m_frequency<FREQUENCY_TERMINAL_START))    
	{        
		GetPrivateProfileString("ConfigInfo","frequency","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frequency= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);          
	}else{
		m_frequency=(int)(m_frequency*10);
		m_frequency=(double)m_frequency; 
		m_frequency/=10;
	}
	UpdateData(FALSE); 
}

void CRadio_stationDlg::OnKillfocusEditWakeupSeconds() 
{
	// TODO: Add your control notification handler code here
	CString strBufferReadConfig,strtmpReadConfig;
	UpdateData(TRUE);    
	if ((m_wakeup_time>10) || (m_wakeup_time<0))    
	{        
		GetPrivateProfileString("ConfigInfo","wakeup_time","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_wakeup_time= (int)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);        
		UpdateData(FALSE);    
	}
}

void CRadio_stationDlg::OnButtonBoardModify() 
{
	// TODO: Add your control notification handler code here
	if (flag_board_modified==0)
	{
		flag_board_modified=1;//��ԭΪ�����޸�ǰ�İ���״̬
		GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BOARD_MODIFY)->SetWindowText(_T("ȡ��"));
		GetDlgItem(IDC_BUTTON_BOARD_CONFIG)->EnableWindow(TRUE);
	} 
	else
	{
		flag_board_modified=0;
		GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BOARD_MODIFY)->SetWindowText(_T("�޸�"));
		GetDlgItem(IDC_BUTTON_BOARD_CONFIG)->EnableWindow(FALSE);
		UpdateData(FALSE);
		
	}
}



void CRadio_stationDlg::OnButtonBoardConfig() 
{
	// TODO: Add your control notification handler code here
	flag_board_modified=0;//��ԭΪ�����޸�ǰ�İ���״̬
	GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BOARD_MODIFY)->SetWindowText(_T("�޸�"));
	GetDlgItem(IDC_BUTTON_BOARD_CONFIG)->EnableWindow(FALSE);	
	UpdateData();
	CString strTemp;
	strTemp.Format(_T("%.1f"),m_frequency_native);
	::WritePrivateProfileString("ConfigInfo","frequency_native",strTemp,".\\config_radiostation.ini");

	if (index_control_times<200)
	{
		index_control_times++;
	} 
	else
	{
		index_control_times=0;
	}
	frame_board_control[5]=index_control_times;
	frame_board_control[6]=2;
	frame_board_control[7]=(unsigned char)((m_frequency_native-FREQUENCY_TERMINAL_START)*10);
	frame_board_control[8]=XOR(frame_board_control,8);
	frame_board_control[9]='\r';
	frame_board_control[10]='\n';
	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(9+2);
	
	for (int i=0;i<(9+2);i++)
	{
		Array.SetAt(i,frame_board_control[i]);
	}
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//��������
			}
}

void CRadio_stationDlg::OnKillfocusEditUnicast() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);    
	if ((m_unicast_terminal_id>(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_END)) || (m_unicast_terminal_id<(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START)))    
	{   
		m_unicast_terminal_id=m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START;       
		    
	}
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnKillfocusEditMulticastStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if ((m_multi_terminal_id_start>(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_END)) || (m_multi_terminal_id_start<(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START)))    
	{   
		m_multi_terminal_id_start=m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START;		    
	}
	if ((m_multi_terminal_id_start>m_multi_terminal_id_end)&&((m_multi_terminal_id_end<=(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_END)) && (m_multi_terminal_id_end>=(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START))))
	{
		m_multi_terminal_id_start=m_multi_terminal_id_end;
	}
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnKillfocusEditMulticastEnd() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if ((m_multi_terminal_id_end>(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_END)) || (m_multi_terminal_id_end<(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START)))    
	{   
		m_multi_terminal_id_end=m_radio_id*pow(2,18)+AREA_TERMINAL_ID_END;		    
	}
	if ((m_multi_terminal_id_start>m_multi_terminal_id_end)&&((m_multi_terminal_id_start<=(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_END)) && (m_multi_terminal_id_start>=(m_radio_id*pow(2,18)+AREA_TERMINAL_ID_START))))
	{
		m_multi_terminal_id_end=m_multi_terminal_id_start;
	}
	UpdateData(FALSE);	
}

void CRadio_stationDlg::int_bits(int a, unsigned char* b, int len)//a:��������ֵ��b:���len���ȵ����飻len:����
{
	int counter=0;
	for (int i=(len-1);i>=0;i--)
	{
		b[counter]=(a&(0x01<<i))?1:0; 
		counter++;
	}
}

void CRadio_stationDlg::four_bits_ASCII(unsigned char *a, unsigned char *b, int len,int index)//a:������������飻b:���ASCII���飻len:���ȣ�index:��b�ĵ�indexλ��ʼ�洢
{
	int i=0;
	if ((len%4)!=0)
	{
		AfxMessageBox("����λ������",MB_OK,0);
	} 
	else
	{
		for (i=0;i<len/4;i++)
		{
			b[index+i]=a[i*4]*8+a[i*4+1]*4+a[i*4+2]*2+a[i*4+3]*1+48;//ASCII 0���Ӧʮ������48
		}
	}
}

void CRadio_stationDlg::OnSelendokComboAlarmType() 
{
	// TODO: Add your control notification handler code here
	alarm_index=m_alarm_command.GetCurSel()+1;
// 	CString str1;
// 	str1.Format("%d",alarm_index);
// 	AfxMessageBox(str1,MB_OK,0);
	GetDlgItem(IDC_BUTTON_ALARM)->EnableWindow(TRUE);
	UpdateData();
}

void CRadio_stationDlg::OnButtonAlarm() 
{
	// TODO: Add your control notification handler code here
	int k=0;
	int i=0;
	unsigned char char_buf[4][4]={0};//AES����
	unsigned char bits_buf[128]={0};//AES����
	unsigned char before_gray[12]={0};//���ױ���ǰ��12λ�Ĵ�
	unsigned char after_gray[24]={0};//���ױ�����24λ�Ĵ�
	unsigned char control_region[10]={0};//������

	frame_board_send_index=5;//�Ӱ�ͨ������֡��������ǰ��λ��ռ��
	
	frame_type[0]=1;//֡���ͣ�����֡10
	frame_type[1]=0;

	int_bits(alarm_index,control_region,10);
	
	i=(int)m_frame_counter;
	frame_counter[35]=0;
	frame_counter[34]=0;
	frame_counter[33]=0;
	frame_counter[32]=0;
	int_bits(i,frame_counter,32);
/*****************��ʼ��֡********************************/
	frame_board_bits[0]=frame_type[0];
	frame_board_bits[1]=frame_type[1];
	frame_send_index=2;//ǰ���Ѿ�����2���ֽڣ��Ӵ˿�ʼ++
	for (k=0;k<10;k++)//ͨ��Ƶ��
	{
		frame_board_bits[frame_send_index]=control_region[k];
		frame_send_index++;
	}

	for (k=0;k<36;k++)//֡������
	{
		frame_board_bits[frame_send_index]=frame_counter[k];
		frame_send_index++;
	}
	
/*****************AES����********************************/
	for(i=0;i<128;i++){
		if(i<frame_send_index){//�Ѹ��������������к�36λǰ�����ݷŵ�aes_bits[]��
			bits_buf[i]=frame_board_bits[i];
		}else{
			bits_buf[i]=0;//����128λ���㣬����128����˴���ִ�У���ȡǰ128λ
		}
	}
	bit_char(bits_buf,char_buf);
	Encrypt(char_buf,cipherkey_base);
	char_bit(char_buf,bits_buf);
	for (k=0;k<36;k++)//AES��
	{
		frame_board_bits[frame_send_index]=bits_buf[k];
		frame_send_index++;
	}
/*****************gray����************************************/
	index_after_gray=0;
	for (k=0;k<(frame_send_index/12);k++)//���ױ������
	{
		for (i=0;i<12;i++)
		{
			before_gray[i]=frame_board_bits[k*12+i];
		}
		gray_encode(before_gray,after_gray);
		for (i=0;i<24;i++)
		{
			frame_board_after_gray[index_after_gray]=after_gray[i];
			index_after_gray++;
		}
		
	}

//	CString str1;
//	str1.Format("%d",index_after_gray);
//	AfxMessageBox(str1,MB_OK,0);
/******************���ڷ�������*******************************/
	if (index_data_times<200)
	{
		index_data_times++;
	} 
	else
	{
		index_data_times=0;
	}
	frame_board_data[frame_board_send_index]=index_data_times;
	frame_board_send_index++;
	frame_board_data[frame_board_send_index]=(index_after_gray/4)/256;
	frame_board_send_index++;
	frame_board_data[frame_board_send_index]=(index_after_gray/4)%256+1;//���һλ�����У���
	frame_board_send_index++;
	four_bits_ASCII(frame_board_after_gray,frame_board_data,index_after_gray,frame_board_send_index);
	frame_board_send_index+=index_after_gray/4;
	frame_board_data[frame_board_send_index]=XOR(frame_board_data,frame_board_send_index);
	frame_board_send_index++;
	if (frame_board_data[frame_board_send_index]=='$')
	{
		frame_board_data[frame_board_send_index]++;//����������$����ֵ��һ
	}
	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(frame_board_send_index);
	
	for (i=0;i<frame_board_send_index;i++)
	{
		Array.SetAt(i,frame_board_data[i]);
	}
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//��������
	}	
}

void CRadio_stationDlg::OnKillfocusEditBoardFrequency() 
{
	// TODO: Add your control notification handler code here
	CString strBufferReadConfig,strtmpReadConfig;
	UpdateData(TRUE);    
	if ((m_frequency_native>FREQUENCY_TERMINAL_END) || (m_frequency_native<FREQUENCY_TERMINAL_START))    
	{        
		GetPrivateProfileString("ConfigInfo","frequency_native","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frequency_native= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);          
	}else{
		m_frequency_native=(int)(m_frequency_native*10);
		m_frequency_native=(double)m_frequency_native;
		m_frequency_native/=10;
	}
	UpdateData(FALSE); 
}

void CRadio_stationDlg::OnButtonScan() 
{
	// TODO: Add your control notification handler code here
	int nRows=0,nIndex=0,i=0;
	m_rssi_list.DeleteAllItems();

// 	int nColumnCount = m_rssi_list.GetHeaderCtrl()->GetItemCount();		
// 	for (i=0;i <nColumnCount;i++)
// 	{
// 		m_rssi_list.DeleteColumn(0);
// 	}

	
	
// 	nRows = m_rssi_list.GetItemCount();
// 	for(i=nRows-1;i>=0;i--)
// 	{
// // 		if(m_rssi_list.GetItemText(i, 5).Find("1234") >= 0)
// // 		{
// // 			nIndex = i;
// // 		}
// 		m_rssi_list.DeleteItem(i);//���
// 
// 	}


	index_frequency_point=0;//Ƶ�����������
	if (index_scan_times<200)
	{
		index_scan_times++;
	} 
	else
	{
		index_scan_times=0;
	}
	frame_board_frequency[5]=index_scan_times;
	frame_board_frequency[6]=XOR(frame_board_frequency,6);
	frame_board_frequency[7]='\r';
	frame_board_frequency[8]='\n';
	CByteArray Array;
	Array.RemoveAll();
	Array.SetSize(7+2);
	
	for (i=0;i<(7+2);i++)
	{
		Array.SetAt(i,frame_board_frequency[i]);
	}
	
	if(m_comm.GetPortOpen())
	{
		m_comm.SetOutput(COleVariant(Array));//��������
	}
}
