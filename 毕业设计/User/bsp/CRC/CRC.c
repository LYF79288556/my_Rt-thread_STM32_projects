#include "CRC.h"
//CRC16У���㷨 �������ַ�������
unsigned short crc16(unsigned char *ptr,unsigned char len)
{
	unsigned long wcrc=0XFFFF;//Ԥ��16λcrc�Ĵ�������ֵȫ��Ϊ1
	int i=0,j=0;//�������
	for(i=0;i<len;i++)//ѭ������ÿ������
  {
		wcrc^=*ptr++;//����λ������crc�Ĵ������.ָ���ַ���ӣ�ָ���¸�����
		for(j=0;j<8;j++)//ѭ���������ݵ�
		{
			if(wcrc&0X0001)//�ж����Ƴ����ǲ���1�������1�������ʽ�������
			{
				wcrc=wcrc>>1^0XA001;//�Ƚ���������һλ��������Ķ���ʽ�������
			}
			else//�������1����ֱ���Ƴ�
			{
				wcrc>>=1;//ֱ���Ƴ�
			}
		}
	}
	return (wcrc);//Ĭ�Ͼ��� ��λ��ǰ  ��λ�ں�   
}
//CRC16У���㷨 ������16���ƴ���
unsigned short CRC_16_HEX(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned long CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;
    for (i=0;i<CRC_CNT; i++){
        //printf("%x@",Buf[i]);      
        CRC_Temp ^= Buf[i];        
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
		
    return(CRC_Temp>>8|CRC_Temp<<8);  //
}
//����CRCУ�鷽ʽ����
unsigned int CRC16_Checkout ( unsigned char *puchMsg, unsigned int usDataLen ) 
	{ 
	unsigned int i,j,crc_reg,check; 
  crc_reg = 0xFFFF; 
	for(i=0;i<usDataLen;i++)  
	{ 
		crc_reg = (crc_reg>>8) ^ puchMsg[i];   
		for(j=0;j<8;j++) 
		{ 
			check = crc_reg & 0x0001;  
			crc_reg >>= 1;       
			if(check==0x0001)  
      {    
	    crc_reg ^= 0xA001;  
	     } 
	}
	} 
	return crc_reg; 
}


