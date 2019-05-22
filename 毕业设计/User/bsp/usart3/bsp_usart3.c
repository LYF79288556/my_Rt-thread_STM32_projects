/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �ض���c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "bsp_usart3.h"
#include "rtthread.h"
#include "string.h"
#include "CRC.h"

/* �ⲿ�����ź������ƿ� */
extern rt_sem_t test_sem3;

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration2(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART3_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO ����,������������
  * @param  ��
  * @retval ��
  */
void USART3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	

	// �򿪴���GPIO��ʱ��
	DEBUG_USART3_GPIO_APBxClkCmd(DEBUG_USART3_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	DEBUG_USART3_APBxClkCmd(DEBUG_USART3_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART3_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART3_RX_GPIO_PORT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //
	GPIO_SetBits(GPIOB,GPIO_Pin_1);                  //485EN
	
	
	
	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART3_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USART3, &USART_InitStructure);
	// �����ж����ȼ�����
	NVIC_Configuration2();
	// ���� ���ڿ���IDEL �ж�
	USART_ITConfig(DEBUG_USART3, USART_IT_IDLE, ENABLE);  
  // ��������DMA����
	USART_DMACmd(DEBUG_USART3, USART_DMAReq_Rx, ENABLE); 
	// ʹ�ܴ���
	USART_Cmd(DEBUG_USART3, ENABLE);	    
}

char Usart3_Rx_Buf[USART3_RBUFF_SIZE];

void USART3_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		// ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART3_DR_ADDRESS;
		// �ڴ��ַ(Ҫ����ı�����ָ��)
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Usart3_Rx_Buf;
		// ���򣺴��ڴ浽����	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// �����С	
		DMA_InitStructure.DMA_BufferSize = USART3_RBUFF_SIZE;
		// �����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = 
	  DMA_PeripheralDataSize_Byte;
		// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
		// DMAģʽ��һ�λ���ѭ��ģʽ
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ; //����ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 //ѭ��ģʽ
		// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 
		// ��ֹ�ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		// ����DMAͨ��		   
		DMA_Init(USART3_RX_DMA_CHANNEL, &DMA_InitStructure);		
    // ���DMA���б�־
		DMA_ClearFlag(DMA1_FLAG_GL3);
    ///DMA_ClearFlag(DMA1_FLAG_GL5);
    DMA_ITConfig(USART3_RX_DMA_CHANNEL, DMA_IT_TE, ENABLE);
		// ʹ��DMA
		DMA_Cmd (USART3_RX_DMA_CHANNEL,ENABLE);
}
//���ڸ㶨����2��DMA����  ����Ϊʲô����1�Ͳ���Ҫ�ж�������  �ܷѽ⣡������
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  // �����ж�

    {
        Uart3_DMA_Rx_Data();

        USART_ReceiveData( USART3 ); // Clear IDLE interrupt flag bit
    }
}

void Uart3_DMA_Rx_Data(void)
{
   // �ر�DMA ����ֹ����
   DMA_Cmd(USART3_RX_DMA_CHANNEL, DISABLE);      
   // ��DMA��־λ
	 DMA_ClearFlag( DMA1_FLAG_GL3 ); 
   ///DMA_ClearFlag( DMA1_FLAG_GL5 );          
   //  ���¸�ֵ����ֵ��������ڵ��������ܽ��յ�������֡��Ŀ
   USART3_RX_DMA_CHANNEL->CNDTR = USART3_RBUFF_SIZE;    
   DMA_Cmd(USART3_RX_DMA_CHANNEL, ENABLE);       
   //������ֵ�ź��� �����ͽ��յ������ݱ�־����ǰ̨�����ѯ
   rt_sem_release(test_sem3);  	
  /* 
    DMA �������ȴ����ݡ�ע�⣬����жϷ�������֡�����ʺܿ죬MCU����������˴ν��յ������ݣ�
    �ж��ַ������ݵĻ������ﲻ�ܿ������������ݻᱻ���ǡ���2�ַ�ʽ�����

    1. �����¿�������DMAͨ��֮ǰ����LumMod_Rx_Buf��������������ݸ��Ƶ�����һ�������У�
    Ȼ���ٿ���DMA��Ȼ�����ϴ����Ƴ��������ݡ�

    2. ����˫���壬��LumMod_Uart_DMA_Rx_Data�����У���������DMA_MemoryBaseAddr �Ļ�������ַ��
    ��ô�´ν��յ������ݾͻᱣ�浽�µĻ������У������ڱ����ǡ�
  */
}


/*****************  ����һ���ֽ� **********************/
void Usart3_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*******************************************************************************  
* �����ֽ� 
*******************************************************************************/    
void uart3_send_char(u8 temp)      
{        
    USART_SendData(USART3,(u8)temp);        
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET); //USART_FLAG_TXE �������ݼĴ����ձ�־λ        
}    

/*******************************************************************************  
* �����ַ���  
*******************************************************************************/    
void uart3_send_buff(u8 *buf,u8 len)     
{    
    u8 t;    
    for(t=0;t<len;t++){    
    USART_SendData(USART3,buf[t]);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET); //USART_FLAG_TXE �������ݼĴ����ձ�־λ
		}			
}
/****************** ����8λ������ ************************/
void Usart3_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart3_SendByte(pUSARTx,array[i]);	
  
  }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  �����ַ��� **********************/
void Usart3_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart3_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart3_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


/***********************************************************
�������ƣ�int RS485_SendCmd(u8 *cmd,u8 len,int x,int wait)
�������ܣ�RS485��ѯ�뷵��ָ��
��ڲ�����cmd����ѯָ��
         len�����ݳ���
           x��������ʼλ
        wait����ѯ��ʱ
���ڲ��������ݵ�
�� ע��
rt_kprintf("[NBiot_SendCmd] %s\r\n", cmd);
  rt_thread_delay(100);   ��ʱ100��tick 	
***********************************************************/
 
int RS485_SendCmd(u8 *cmd,u8 len,int max_len,int x,int wait)
{   
  int Val = 0; 
	unsigned short CRC_Tmp;
	unsigned short crc;
	memset(Usart3_Rx_Buf,0,USART3_RBUFF_SIZE);//����ջ�����
  //rt_kprintf("[RS485_SendCmd] %d\r\n",len);
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	rt_thread_delay(2);
  uart3_send_buff(cmd, len);
	rt_thread_delay(2);//������ʱһ�£�ԭ��ȥ�鿴sp3485���ֲ��
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);//
  rt_thread_delay(wait);
 if (strcmp(Usart3_Rx_Buf, "")!= 0) //����ֵ��Ϊ��if (strcmp(Usart2_Rx_Buf, "") == 0) //����ֵΪ��
  {			
//      for(i=0;i<len+1;i++)  //��ӡ�������յİ���9������
//      {
//      rt_kprintf("%X@",Usart3_Rx_Buf[i]);	
//      }		
//			rt_kprintf("��������2; %d\r\n",strlen(Usart3_Rx_Buf));
		  GPIO_SetBits(GPIOB,GPIO_Pin_1);
		  rt_thread_delay(2);//������ʱһ�£�ԭ��ȥ�鿴sp3485���ֲ��
			crc = ((unsigned short)Usart3_Rx_Buf[max_len-2]<<8) +(unsigned short)Usart3_Rx_Buf[max_len-1]; //�յ����ݵ�crcУ��ֵ
			CRC_Tmp = CRC_16_HEX((unsigned char *)Usart3_Rx_Buf,(unsigned char)(max_len-2)); //�����ȥ�����λ������CRCУ�飬���crcУ��ֵ
//			rt_kprintf("%X\r\n",crc);
//			rt_kprintf("%X\r\n",CRC_Tmp);
			if (CRC_Tmp == crc){   //�Ƚ�CRCУ��ֵ�Ƿ���ȣ�����������һ������
      Val = (Usart3_Rx_Buf[x]*256) + (Usart3_Rx_Buf[x+1]*1);
			return Val;
			}
			return Val;
    }  
}





///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
//int fputc(int ch, FILE *f)
//{
//		/* ����һ���ֽ����ݵ����� */
//		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
//		
//		/* �ȴ�������� */
//		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
//	
//		return (ch);
//}

/////�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
//int fgetc(FILE *f)
//{
//		/* �ȴ������������� */
//		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

//		return (int)USART_ReceiveData(DEBUG_USARTx);
//}

