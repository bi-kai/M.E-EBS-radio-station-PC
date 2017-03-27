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
#define FREQUENCY_POINT 0X10//FM可用频点

#define FREQUENCY_TERMINAL_START 76.0//终端通信频点最小值
#define FREQUENCY_TERMINAL_END 88.0//终端通信频点最大值

unsigned char frame_board_check[7+2]={'$','r','d','y','_'};//连接检测帧
unsigned char frame_board_frequency[7+2]={'$','f','r','e','_'};//频谱检测帧
unsigned char frame_board_control[9+2]={'$','c','o','n','_'};//控制帧
//unsigned char frame_board_before_gray[168]={0};//格雷编码前的数据帧的比特流
unsigned char frame_board_after_gray[168*2]={0};//格雷编码后的数据帧的比特流
unsigned char frame_board_data[400+2]={'$','d','a','t','_'};//发送数据帧
unsigned char frame_board_bits[1100]={0};//电台终端帧冲区
unsigned char frame_receive[100]={0};//接收缓冲区

extern unsigned char cipherkey_base[16];//电台私钥

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
	
/*************************串口*****************************/
	m_DCom=1;
	m_DStopbits=1;
	m_DParity='N';
	m_DDatabits=8;
	m_DBaud=115200;
	alarm_index=1;
	SerialPortOpenCloseFlag=FALSE;//默认关闭串口
	flag_modified=0;//修改电台ID标志位
	flag_com_init_ack=0;//未收到下位机的应答
	frame_index=0;//接收缓冲帧计数器初始化
	frame_send_index=0;//发送缓冲计数器初始化
	index_wakeup_times=0;//连接帧发送次数计数器，上下位机通信，保证每帧数据都不同
	index_scan_times=0;//频谱扫描帧发送次数计数器，上下位机通信，保证每帧数据都不同
	index_control_times=0;//控制帧发送次数计数器，上下位机通信，保证每帧数据都不同
	index_data_times=0;//数据帧发送次数计数器，上下位机通信，保证每帧数据都不同
	flag_board_modified=0;//子板修改标志位
	frame_board_send_index=5;//子板通信数据帧计数器，前五位被占用
//	index_before_gray=0;
	index_after_gray=0;
	index_frequency_point=0;//频点计数器归零

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

	m_comm.SetCommPort(1); //选择com1
	m_comm.SetInputMode(1); //输入方式为二进制方式
	m_comm.SetInBufferSize(1024); //设置输入缓冲区大小
	m_comm.SetOutBufferSize(10240); //设置输出缓冲区大小
	m_comm.SetSettings("115200,n,8,1"); //波特率115200，无校验，8个数据位，1个停止位	 
	m_comm.SetRThreshold(1); //参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
	m_comm.SetInputLen(0); //设置当前接收区数据长度为0
	//	 m_comm.GetInput();    //先预读缓冲区以清除残留数据
	if(!m_comm.GetPortOpen())
	{		 
		//		m_comm.SetPortOpen(TRUE);//打开串口(此处不必打开，后边用“打开串口”按钮实现)
	}
	else
		 MessageBox("can not open serial port");
/**************************INI配置*******************************/
	CFileFind finder;   //查找是否存在ini文件，若不存在，则生成一个新的默认设置的ini文件，这样就保证了我们更改后的设置每次都可用
	BOOL ifFind = finder.FindFile(_T(".\\config_radiostation.ini"));
	if(!ifFind)
	{
		::WritePrivateProfileString("ConfigInfo","ID","34095233",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","frame_counter","100",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","wakeup_time","1",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","frequency","79.0",".\\config_radiostation.ini");
		::WritePrivateProfileString("ConfigInfo","frequency_native","79.0",".\\config_radiostation.ini");//下位机RDA5820工作频点
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
//	((CButton *)GetDlgItem(IDC_RADIO_BROADCAST))->SetCheck(TRUE);//选上
/**************************RSSI列表配置**********************************/
	m_rssi_list.ModifyStyle( 0, LVS_REPORT );// 报表模式
	m_rssi_list.SetExtendedStyle(m_rssi_list.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);// 间隔线+行选中
	
	m_rssi_list.InsertColumn(0,"频点(MHz)", LVCFMT_LEFT, 40);
	m_rssi_list.InsertColumn(1,"RSSI", LVCFMT_LEFT, 40);
	
	CRect rect;
	m_rssi_list.GetClientRect(rect); //获得当前客户区信息
	m_rssi_list.SetColumnWidth(0, rect.Width()*6/11); //设置列的宽度。
	m_rssi_list.SetColumnWidth(1, rect.Width()*3/11);
	int nRow = m_rssi_list.InsertItem(0,"79.2");//插入行
	m_rssi_list.SetItemText(nRow,1,"44");//设置数据
	
	LVFINDINFO info;
	int nIndex;
	info.flags = LVFI_PARTIAL|LVFI_STRING;
	info.psz = "79.4";
	nIndex = m_rssi_list.FindItem(&info);  // nIndex为行数(从0开始)

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
	m_radio_wakeup=0;//第一个单选按钮被选择上了
	GetDlgItem(IDC_EDIT_UNICAST)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MULTICAST_END)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CRadio_stationDlg::OnRadioMulticast() 
{
	// TODO: Add your control notification handler code here
	m_radio_wakeup=2;//第二个单选按钮被选择上了
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
	m_radio_wakeup=1;//第三个单选按钮被选择上了
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
	unsigned char char_buf[4][4]={0};//AES加密
	unsigned char bits_buf[128]={0};//AES加密
	unsigned char before_gray[12]={0};//格雷编码前的12位的串
	unsigned char after_gray[24]={0};//格雷编码后的24位的串

	frame_board_send_index=5;//子板通信数据帧计数器，前五位被占用
	
	frame_type[0]=0;//帧类型：唤醒帧01
	frame_type[1]=1;

	i=(int)((m_frequency-FREQUENCY_TERMINAL_START)*10);//频点获取，二进制化
	int_bits(i,communication_fre_point,8);
	
	i=(int)m_radio_id;//电台ID
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
/*****************开始组帧********************************/
	frame_board_bits[0]=frame_type[0];
	frame_board_bits[1]=frame_type[1];
	frame_send_index=4;//前面已经有了4个字节，从此开始++
	for (k=0;k<8;k++)//通信频点
	{
		frame_board_bits[frame_send_index]=communication_fre_point[k];
		frame_send_index++;
	}
	for (k=0;k<36;k++)//源地址
	{
		frame_board_bits[frame_send_index]=source_address[k];
		frame_send_index++;
	}

/*****************三种唤醒帧的不同，start*******************************/
	if (m_radio_wakeup==0)//广播唤醒帧
	{		
		wakeup_type[0]=1;//单播01、广播10、组播11
		wakeup_type[1]=0;
	} 
	else if(m_radio_wakeup==1)//单播唤醒帧
	{
		wakeup_type[0]=0;//单播01、广播10、组播11
		wakeup_type[1]=1;

		i=(int)(m_unicast_terminal_id-m_radio_id*pow(2,18));//终端ID
		int_bits(i,target_address_unicast,24);
		for (k=0;k<24;k++)//转为比特流组帧
		{
			frame_board_bits[frame_send_index]=target_address_unicast[k];
			frame_send_index++;
		}
	}
	else if (m_radio_wakeup==2)//组播唤醒帧
	{		
		wakeup_type[0]=1;//单播01、广播10、组播11
		wakeup_type[1]=1;

		i=(int)(m_multi_terminal_id_start-m_radio_id*pow(2,18));//终端起始ID
		int_bits(i,target_address_multicast_start,24);
		for (k=0;k<24;k++)//转为比特流组帧
		{
			frame_board_bits[frame_send_index]=target_address_multicast_start[k];
			frame_send_index++;
		}

		i=(int)(m_multi_terminal_id_end-m_radio_id*pow(2,18));//终端终止ID
		int_bits(i,target_address_multicast_end,24);
		for (k=0;k<24;k++)//转为比特流组帧
		{
			frame_board_bits[frame_send_index]=target_address_multicast_end[k];
			frame_send_index++;
		}
	}
/*****************三种唤醒帧的不同，end*********************************/

	frame_board_bits[2]=wakeup_type[0];
	frame_board_bits[3]=wakeup_type[1];
	for (k=0;k<36;k++)//帧计数器
	{
		frame_board_bits[frame_send_index]=frame_counter[k];
		frame_send_index++;
	}
	
/*****************AES加密********************************/
	for(i=0;i<128;i++){
		if(i<frame_send_index){//把格雷译码后的数据中后36位前的内容放到aes_bits[]中
			bits_buf[i]=frame_board_bits[i];
		}else{
			bits_buf[i]=0;//不够128位则补零，超过128，则此处不执行，仅取前128位
		}
	}
	bit_char(bits_buf,char_buf);
	Encrypt(char_buf,cipherkey_base);
	char_bit(char_buf,bits_buf);
	for (k=0;k<36;k++)//AES串
	{
		frame_board_bits[frame_send_index]=bits_buf[k];
		frame_send_index++;
	}
/*****************gray编码************************************/
	index_after_gray=0;
	for (k=0;k<(frame_send_index/12);k++)//格雷编码次数
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
/******************串口发送数据*******************************/
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
	frame_board_data[frame_board_send_index]=(index_after_gray/4)%256+1;//最后一位是异或校验和
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
		frame_board_data[frame_board_send_index]++;//如果异或结果是$，则值加一
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
		m_comm.SetOutput(COleVariant(Array));//发送数据
	}	

}
//frame_board_bits
void CRadio_stationDlg::OnSelendokComboComselect() 
{
	// TODO: Add your control notification handler code here
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //生成指向应用程序类的指针
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
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //生成指向应用程序类的指针
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
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //生成指向应用程序类的指针
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
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //生成指向应用程序类的指针
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
	CRadio_stationApp *app = (CRadio_stationApp *)AfxGetApp(); //生成指向应用程序类的指针
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

		//以下是串口的初始化配置
		if(m_comm.GetPortOpen())//打开端口前的检测，先关，再开
			MessageBox("can not open serial port");
//			m_comm.SetPortOpen(FALSE);	//	
		m_comm.SetCommPort(m_DCom); //选择端口，默认是com1
		m_comm.SetSettings((LPSTR)(LPCTSTR)string1); //波特率9600，无校验，8个数据位，1个停止位
		if(!m_comm.GetPortOpen())
		{			
			m_comm.SetPortOpen(TRUE);//打开串口
			GetDlgItem(IDC_BUTTON_CONNECTBOARD)->SetWindowText("关闭串口");

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
				m_comm.SetOutput(COleVariant(Array));//发送数据
			}
		}
		else
			MessageBox("无法打开串口，请重试！");	 
	}
	else
	{
		SerialPortOpenCloseFlag=FALSE;
		GetDlgItem(IDC_BUTTON_CONNECTBOARD)->SetWindowText("打开串口");
		m_ctrlIconOpenoff.SetIcon(m_hIconOff);
		m_board_led.SetIcon(m_hIconOff);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("板卡未连接!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_ALARM)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(FALSE);
		flag_com_init_ack=0;//子板未连接
		m_comm.SetPortOpen(FALSE);//关闭串口

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
	BYTE rxdata[2048]; //设置BYTE数组
	CString strDisp="",strTmp="";
	int frequency_point=0;//频率扫描的总的频点数
	double frequency_buf=0;//频点计算
	
	if((m_comm.GetCommEvent()==2)) //事件值为2表示接收缓冲区内有字符
	{
		variant_inp=m_comm.GetInput(); //读缓冲区
		safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		for(k=0;k<len;k++)
		{
			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		}
		
		//			AfxMessageBox("OK",MB_OK,0);
		//			frame=frame_len[frame_index];
		//			frame_lock=0;
		//			frame_len[frame_index]=0;
		
		frame_index=0;
		for(k=0;k<len;k++)//将数组转化为CString类型
		{
			BYTE bt=*(char*)(rxdata+k);    //字符型
				if (rxdata[0]!='$')
				{
					return;//帧数据串错误
				}
			frame_receive[frame_index]=bt;
			frame_index++;
// 			strTmp.Format("%c",bt);    //将字符送入临时变量strtemp存放
// 			strDisp+=strTmp;  //加入接收编辑框对应字符串
			
		}
//		AfxMessageBox(strDisp,MB_OK,0);
	if ((flag_com_init_ack==0)&&(frame_receive[1]=='r')&&(frame_receive[2]=='d')&&(frame_receive[3]=='y')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_wakeup_times)&&(frame_receive[7]==XOR(frame_receive,7)))//首次连接握手
	{
		flag_com_init_ack=1;
		m_board_led.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("板卡已连接!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(TRUE);
	}else if ((flag_com_init_ack==1)&&(frame_receive[1]=='f')&&(frame_receive[2]=='r')&&(frame_receive[3]=='e')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_scan_times)&&(frame_receive[9]==XOR(frame_receive,9)))//频谱扫描
	{
		frequency_buf=76.0+(double)frame_receive[7]/10;
		strTmp.Format("%.1f",frequency_buf);
		m_rssi_list.InsertItem(frame_receive[7],strTmp);//插入行
		strTmp.Format("%d",frame_receive[8]);
		m_rssi_list.SetItemText(frame_receive[7],1,strTmp);//设置数据
		m_rssi_list.SendMessage(WM_VSCROLL,SB_BOTTOM,NULL); //随数据滚动

	}else if ((flag_com_init_ack==1)&&(frame_receive[1]=='c')&&(frame_receive[2]=='o')&&(frame_receive[3]=='n')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_control_times)&&(frame_receive[7]==XOR(frame_receive,7)))//控制帧
	{
		AfxMessageBox("下位机频点配置成功！",MB_OK,0);


	}else if ((flag_com_init_ack==1)&&(frame_receive[1]=='d')&&(frame_receive[2]=='a')&&(frame_receive[3]=='t')&&(frame_receive[4]=='_')
		&&(frame_receive[5]=='_')&&(frame_receive[6]==index_data_times)&&(frame_receive[7]==XOR(frame_receive,7)))//数据帧反馈信息
	{
		m_board_led.SetIcon(m_hIconRed);
		GetDlgItem(IDC_STATIC_BOARDCONNECT)->SetWindowText("板卡已连接!");
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_WAKEUP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_VOICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SCAN)->EnableWindow(TRUE);
	} 
	else
	{
		AfxMessageBox("下位机帧有错误！",MB_OK,0);
	}
// 		if (framelen==(frame_receive[frame_index][5]*256+frame_receive[frame_index][6]))
// 		{
// 			//帧接收完成
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
// 					frame_flag[i]=0;//标记为未使用
// 				}
// 			}
// 			//			frame_index++;//收到完整的一帧
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
		flag_modified=1;//还原为按下修改前的按键状态
		GetDlgItem(IDC_EDIT_ID)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_WAKEUP_SECONDS)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FREQUENCE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(_T("取消"));
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
		GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(_T("修改"));
		UpdateData(FALSE);
		
	}
	
}

void CRadio_stationDlg::OnButtonIDSave() 
{
	// TODO: Add your control notification handler code here
	flag_modified=0;//还原为按下修改前的按键状态
	GetDlgItem(IDC_EDIT_ID)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_WAKEUP_SECONDS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FREQUENCE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MODIFY)->SetWindowText(_T("修改"));	
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
		flag_board_modified=1;//还原为按下修改前的按键状态
		GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BOARD_MODIFY)->SetWindowText(_T("取消"));
		GetDlgItem(IDC_BUTTON_BOARD_CONFIG)->EnableWindow(TRUE);
	} 
	else
	{
		flag_board_modified=0;
		GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BOARD_MODIFY)->SetWindowText(_T("修改"));
		GetDlgItem(IDC_BUTTON_BOARD_CONFIG)->EnableWindow(FALSE);
		UpdateData(FALSE);
		
	}
}



void CRadio_stationDlg::OnButtonBoardConfig() 
{
	// TODO: Add your control notification handler code here
	flag_board_modified=0;//还原为按下修改前的按键状态
	GetDlgItem(IDC_EDIT_BOARD_FREQUENCY)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SYS_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_BOARD_MODIFY)->SetWindowText(_T("修改"));
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
		m_comm.SetOutput(COleVariant(Array));//发送数据
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

void CRadio_stationDlg::int_bits(int a, unsigned char* b, int len)//a:输入整型值；b:输出len长度的数组；len:长度
{
	int counter=0;
	for (int i=(len-1);i>=0;i--)
	{
		b[counter]=(a&(0x01<<i))?1:0; 
		counter++;
	}
}

void CRadio_stationDlg::four_bits_ASCII(unsigned char *a, unsigned char *b, int len,int index)//a:输入比特流数组；b:输出ASCII数组；len:长度；index:从b的第index位开始存储
{
	int i=0;
	if ((len%4)!=0)
	{
		AfxMessageBox("数据位数出错！",MB_OK,0);
	} 
	else
	{
		for (i=0;i<len/4;i++)
		{
			b[index+i]=a[i*4]*8+a[i*4+1]*4+a[i*4+2]*2+a[i*4+3]*1+48;//ASCII 0码对应十进制是48
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
	unsigned char char_buf[4][4]={0};//AES加密
	unsigned char bits_buf[128]={0};//AES加密
	unsigned char before_gray[12]={0};//格雷编码前的12位的串
	unsigned char after_gray[24]={0};//格雷编码后的24位的串
	unsigned char control_region[10]={0};//控制域

	frame_board_send_index=5;//子板通信数据帧计数器，前五位被占用
	
	frame_type[0]=1;//帧类型：控制帧10
	frame_type[1]=0;

	int_bits(alarm_index,control_region,10);
	
	i=(int)m_frame_counter;
	frame_counter[35]=0;
	frame_counter[34]=0;
	frame_counter[33]=0;
	frame_counter[32]=0;
	int_bits(i,frame_counter,32);
/*****************开始组帧********************************/
	frame_board_bits[0]=frame_type[0];
	frame_board_bits[1]=frame_type[1];
	frame_send_index=2;//前面已经有了2个字节，从此开始++
	for (k=0;k<10;k++)//通信频点
	{
		frame_board_bits[frame_send_index]=control_region[k];
		frame_send_index++;
	}

	for (k=0;k<36;k++)//帧计数器
	{
		frame_board_bits[frame_send_index]=frame_counter[k];
		frame_send_index++;
	}
	
/*****************AES加密********************************/
	for(i=0;i<128;i++){
		if(i<frame_send_index){//把格雷译码后的数据中后36位前的内容放到aes_bits[]中
			bits_buf[i]=frame_board_bits[i];
		}else{
			bits_buf[i]=0;//不够128位则补零，超过128，则此处不执行，仅取前128位
		}
	}
	bit_char(bits_buf,char_buf);
	Encrypt(char_buf,cipherkey_base);
	char_bit(char_buf,bits_buf);
	for (k=0;k<36;k++)//AES串
	{
		frame_board_bits[frame_send_index]=bits_buf[k];
		frame_send_index++;
	}
/*****************gray编码************************************/
	index_after_gray=0;
	for (k=0;k<(frame_send_index/12);k++)//格雷编码次数
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
/******************串口发送数据*******************************/
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
	frame_board_data[frame_board_send_index]=(index_after_gray/4)%256+1;//最后一位是异或校验和
	frame_board_send_index++;
	four_bits_ASCII(frame_board_after_gray,frame_board_data,index_after_gray,frame_board_send_index);
	frame_board_send_index+=index_after_gray/4;
	frame_board_data[frame_board_send_index]=XOR(frame_board_data,frame_board_send_index);
	frame_board_send_index++;
	if (frame_board_data[frame_board_send_index]=='$')
	{
		frame_board_data[frame_board_send_index]++;//如果异或结果是$，则值加一
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
		m_comm.SetOutput(COleVariant(Array));//发送数据
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
// 		m_rssi_list.DeleteItem(i);//清空
// 
// 	}


	index_frequency_point=0;//频点计数器归零
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
		m_comm.SetOutput(COleVariant(Array));//发送数据
	}
}
