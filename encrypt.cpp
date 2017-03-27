#include "stdafx.h"
#include "encrypt.h"
#define N 50
#define M 12

unsigned char expandedkey[176],temp[4][4],key[4][4];		//将状态矩阵与轮密钥设置为全局变量

unsigned char cipherkey_base[16]={43,126,21,22,40,174,210,166,171,247,21,136,9,207,79,60};		//初始密钥

const  unsigned char sbox[16][16] = {
	{0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76},
	{0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0},
	{0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15},
	{0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75},
	{0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84},
	{0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf},
	{0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8},
	{0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2},
	{0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73},
	{0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb},
	{0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79},
	{0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08},
	{0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a},
	{0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e},
	{0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf},
	{0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16}
};
 unsigned char Polymatrix[4][4]={{2,3,1,1},{1,2,3,1},{1,1,2,3},{3,1,1,2}};//多项式矩阵


void Encrypt(unsigned char state[4][4],unsigned char cipherkey[16])
{
	int i,j,k;
	unsigned char expandedkey1[16],temp[4][4];	
	KeyExpansion(cipherkey,expandedkey);	//通过函数调用，密钥扩展获取扩展密钥
	/************取出expandedkey[176]的前16个元素****************/
	for(i=0; i<16; i++)
		expandedkey1[i]=expandedkey[i];
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			temp[i][j]=expandedkey1[i+(j<<2)];
	RoundKeyAddition(state,temp);		//函数调用，第一轮的初始轮密钥加
	/***********进行轮变换**************/
	for(i=1; i<10; i++)
	{
		ByteSub(state);
		ShiftRow(state);
		MixColumn(state);
		for(j=0; j<16; j++)
			expandedkey1[j]=expandedkey[(i<<4)+j];
		for(j=0; j<4; j++)
			for(k=0; k<4; k++)
				temp[j][k]=expandedkey1[j+(k<<2)];
		RoundKeyAddition(state,temp);
	}
	/**********最后一轮的轮运算***********/
	ByteSub(state);
	ShiftRow(state);
	for(i=0; i<16; i++)
		expandedkey1[i]=expandedkey[160+i];
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			temp[i][j]=expandedkey1[i+(j<<2)];
	RoundKeyAddition(state,temp);
/*	printf("The matrix of state after Encryption is: \n");
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			if(state[i][j]<16)
				printf("0%x ",state[i][j]);
			else
				printf("%x ",state[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/
}
void KeyExpansion(unsigned char cipherkey[16],unsigned char enpandedkey[176])
{
	int i,j,k,l;
	static unsigned char temp[4];
	unsigned char Rcon[4]={1,0,0,0};
	for(i=0; i<16; i++)
		expandedkey[i]=cipherkey[i];
	for(i=16; i<176; i+=16)
	{
		for(l=0; l<4; l++)
			temp[l]=expandedkey[i-4+l];		//四个字节一组进行移位字节替换运算
		RotSub(temp);//函数调用，进行循环左移然后字节替换
		for(l=0; l<4; l++)
			temp[l]^=expandedkey[i-16+l];//与前一块的首字的四个字节进行异或运算
		/**********将与前一块首字异或的结果与每一轮轮常数进行异或*************/
		Rcon[0]<<=((i>>4)-1);		//对每一次循环的Rcon[0]进行重新计算
		if((i>>4)>8)
			Rcon[0]=27<<((i>>4)-9);//对于i/16大于8之后的数用x^4+x^3+x+1来代替x^8
		for(l=1; l<4; l++)
			Rcon[l]=0;
		for(l=0; l<4; l++)
			temp[l]^=Rcon[l];
		Rcon[0]=1;
		for(j=0; j<4; j++)
		{
			expandedkey[i+j] = temp[j];		//获取4个字为一块的首字
			temp[j] = 0;					//对暂存字节的临时数组重新置零
		}
		for(k=i+4; k<(i+16); k++)
			expandedkey[k]=expandedkey[k-4]^expandedkey[k-16];
	}
}
void RotSub(unsigned char temp[4])
{
	int i;
	int row,column;
	//左循环一个字节
	unsigned char b=temp[0];
	for(i=0; i<3; i++)
		temp[i]=temp[(i+1)&0x03];//一个字中的四个字节进行循环左移
	temp[3]=b;
	//字节替换
	for(i=0; i<4; i++)
	{
		row = temp[i] >> 4;
		column = temp[i] & 0x0f;
		temp[i] = sbox[row][column];
	}
}
void ByteSub(unsigned char state[4][4])
{
	int i,j;
	int row,column;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
		{
			row = state[i][j] >> 4;
			column = state[i][j] & 0x0f;
			state[i][j] = sbox[row][column];
		}
}
void ShiftRow(unsigned char state[4][4])
{
	int i,j;
	char temp1[4][4];
	for(i=1; i<4; i++)
	{
		for(j=0; j<4; j++)
			temp1[i][j]=state[i][j];			//将状态矩阵整体暂存
		for(j=0; j<=3; j++)
			state[i][j]=state[i][(j+i)&0x03];
		for(j=1; j<=i; j++)
			state[i][4-j]=temp1[i][(4+i-j)&0x03];//通过暂存矩阵求得被覆盖掉的元素
	}
}
void MixColumn(unsigned char state[4][4])
{
	int i,j,k;
	static unsigned char state1[4][4];
	unsigned char temp=0;
	/*******两矩阵Xtime乘然后异或模块*******/
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
		{
			for(k=0; k<4; k++)
			{
				temp^=Xtime(state[k][j],Polymatrix[i][k]);
			}
			state1[i][j]=temp;
			temp=0;
		}
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			state[i][j]=state1[i][j];
}
unsigned char Xtime(unsigned char a,unsigned char b)
{
	unsigned char temp=0;
	if(b==1)
		a*=b;
	else if(b==2)
		if(a>=128)
		{
			a<<=1;
			a^=27;
		}
		else
			a<<=1;
	else if(b==3)
	{
		temp=a;
		a=Xtime(a,(b-1));
		a^=temp;
	}
	return a;
}
void RoundKeyAddition(unsigned char state[4][4],unsigned char temp[4][4])
{
	int i,j;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			state[i][j]^=temp[i][j];			//状态矩阵元素与轮密钥矩阵对应元素异或
}

void char_bit(unsigned char char_array[4][4],unsigned char bit_array[128]){//类小端模式
	
	char i=0,j=0,k=0;
	int counter=0;
	
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			for(k=7;k>=0;k--){
				bit_array[counter]=(char_array[i][j]& (0x01<<k))?1:0; 
				counter++;
			}
		}
	}
	
}


void bit_char(unsigned char bit_array[128],unsigned char char_array[4][4]){
	
	char i=0,j=0;
	int counter=0;
	
	for (i=0;i<4;i++)
	{
		for (j=0;j<4;j++)
		{
			char_array[i][j]=0;//清零
			for (counter=(i*4*8+j*8);counter<(i*4*8+j*8+8);counter++)
			{
				char_array[i][j]|=bit_array[counter]<<(7-counter%8);
			}
		}
	}
	
}

void gray_encode(
		unsigned char bitin[],      /*待编码的数据*/
		unsigned char code[]		/*编码后的数据有这样的关系 code[ 24]={bitin[11]--bitin[0],Reg[10],Reg[0],parity} 构成*/
			)      
{	
	unsigned char Reg[11]={0};             /*11个寄存器初始化为0*/
	unsigned char i=0,j=0,sum=0;                        /*循环控制变量*/
	unsigned char charer_value[12];			/*中间变量节点*/
	
	/*=============根据生成系统码的乘除法电路得到以下关系====================================================*/
	for(i=0;i<M;i++)
	{	
		/*计算中间变量节点的数据*/
		charer_value[11]=(bitin[M-i-1]+Reg[10])%2;  /*输入的数据是低位先输入，即bitin[0]表示m0*/
		charer_value[10]=charer_value[11];          /*charer_value[11]是由当前输入的数据位和寄存器Reg[10]异或得到*/
		charer_value[6]=charer_value[10];
		charer_value[5]=charer_value[6];
		charer_value[4]=charer_value[5];
		charer_value[2]=charer_value[4];
		charer_value[0]=charer_value[2];
		/*计算寄存器节点的数据*/
		Reg[10]=(Reg[9]+charer_value[10])%2;           /*Reg[4]是由Reg[3]的数据和charer_value[4]异或得到*/
		Reg[9]=Reg[8];                                        /*Reg[3]是由Reg[2]的数据更新得到*/
		Reg[8]=Reg[7];
		Reg[7]=Reg[6];
		Reg[6]=(Reg[5]+charer_value[6])%2;          /*Reg[2]是由Reg[1]的数据和charer_value[2]异或后更新得到*/
		Reg[5]=(Reg[4]+charer_value[5])%2;
		Reg[4]=(Reg[3]+charer_value[4])%2;
		Reg[3]=Reg[2];											/*Reg[1]是由Reg[0]的数据更新得到*/
		Reg[2]=(Reg[1]+charer_value[2])%2;
		Reg[1]=Reg[0];	
		Reg[0]=charer_value[0];								/*Reg[0]是由charer_value[0]的数据更新得到*/
		/*计算当前输入数据的编码结果*/
		code[i]=bitin[M-i-1];                     /*每个码字的信息位直接由输入的信息构成*/
		
	}
	/*=====================后M（R-1）次循环得到码字的校验位========================*/
	for(j=0;j<11;j++)
	{	
		code[i]=Reg[10-j];			/*校验位是由最后的Reg[10]--Reg[0]构成*/
		i++;
	}
	for(i=0;i<23;i++)
		sum=sum+code[i];
	code[23]=sum%2;
	
	return;
}