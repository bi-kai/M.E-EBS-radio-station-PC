// radio_stationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "radio_station.h"
#include "radio_stationDlg.h"
#include <math.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SENDBUFFERSIZE 1100
#define RECEIVEBUFFERSIZE 100
#define RADIO_ID_START 34095233
#define RADIO_ID_END 1073741823
#define AREA_TERMINAL_ID_START 4353
#define AREA_TERMINAL_ID_END 262143
unsigned char frame_board_check[7]={'$','r','d','y','_'};//���Ӽ��֡
unsigned char frame_board_frequency[7]={'$','f','r','e','_'};//Ƶ�׼��֡
unsigned char frame_board_control[7]={'$','c','o','n','_'};//����֡
unsigned char frame_board_data[SENDBUFFERSIZE]={'$','d','a','t','_'};//����֡
unsigned char frame_receive[RECEIVEBUFFERSIZE]={0};//���ջ�����
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
	SerialPortOpenCloseFlag=FALSE;//Ĭ�Ϲرմ���
	flag_modified=0;//�޸ĵ�̨ID��־λ
	flag_com_init_ack=0;//δ�յ���λ����Ӧ��
	frame_index=0;//���ջ���֡��ʼ��
	index_wakeup_times=0;
	flag_board_modified=0;//�Ӱ��޸ı�־λ

	m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);
	GetDlgItem(IDC_COMBO_COMSELECT)->SetWindowText(_T("COM1"));
	GetDlgItem(IDC_COMBO_SPEED)->SetWindowText(_T("115200"));
	GetDlgItem(IDC_COMBO_PARITY)->SetWindowText(_T("NONE"));
	GetDlgItem(IDC_COMBO_DATABITS)->SetWindowText(_T("8"));
	GetDlgItem(IDC_COMBO_STOPBITS)->SetWindowText(_T("1"));

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
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnRadioUnicase() 
{
	// TODO: Add your control notification handler code here
	m_radio_wakeup=1;//��������ѡ��ť��ѡ������
	GetDlgItem(IDC_EDIT_UNICAST)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MULTICAST_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_END)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnButtonWakeup() 
{
	// TODO: Add your control notification handler code here
	if (m_radio_wakeup==0)
	{
		frame_type[0]=0;//����֡01
		frame_type[1]=1;
		
		wakeup_type[0]=1;//����01���㲥10���鲥11
		wakeup_type[1]=0;

	} 
	else if(m_radio_wakeup==1)
	{
		frame_type[0]=0;//����֡01
		frame_type[1]=1;
		
		wakeup_type[0]=0;//����01���㲥10���鲥11
		wakeup_type[1]=1;

	}
	else if (m_radio_wakeup==2)
	{
		frame_type[0]=0;//����֡01
		frame_type[1]=1;
		
		wakeup_type[0]=1;//����01���㲥10���鲥11
		wakeup_type[1]=1;

	}
}

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
			GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
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
			CByteArray Array;
			Array.RemoveAll();
			Array.SetSize(7);
									
			for (int i=0;i<7;i++)
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
	if ((flag_com_init_ack==0)&&(frame_receive[1]=='o')&&(frame_receive[2]='k'))//�״���������
	{
		flag_com_init_ack=1;
		m_board_led.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("�忨������!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ALARM)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(TRUE);
		
	} 
	else
	{
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
	if ((m_frequency>88.0) || (m_frequency<76.0))    
	{        
		GetPrivateProfileString("ConfigInfo","frequency","0",strBufferReadConfig.GetBuffer(MAX_PATH),MAX_PATH,".\\config_radiostation.ini");
		strBufferReadConfig.ReleaseBuffer();
		strtmpReadConfig+=","+strBufferReadConfig;
		m_frequency= (double)atof((char *)(LPTSTR)(LPCTSTR)strBufferReadConfig);          
	}else{
		m_frequency=(int)(m_frequency*10);
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
	strTemp.Format(_T("%f"),m_frequency_native);
	::WritePrivateProfileString("ConfigInfo","frequency_native",strTemp,".\\config_radiostation.ini");
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
