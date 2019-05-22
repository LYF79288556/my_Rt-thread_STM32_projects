/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 �̹߳���
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "stdio.h"
#include "rtthread.h"
#include <string.h>
#include <ctype.h>


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ� */
static rt_thread_t led1_thread = RT_NULL; //led�߳�
static rt_thread_t key_thread = RT_NULL;  //�����߳�
static rt_thread_t usart2_thread = RT_NULL;//����2�߳�
static rt_thread_t usart3_thread = RT_NULL;//����3�߳�

/* �����ź������ƿ� */
rt_sem_t test_sem2 = RT_NULL;
rt_sem_t test_sem3 = RT_NULL;
rt_sem_t CSQ_sem = RT_NULL;
/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */
char* TOPIC = "/device/NB/wz_rt_thread_test"; //��������topic
int error_flag =0; //ʧ��������־λ
int error_time =0; //ʧ�ܵĴ���
char *empty = "";
char ALL_message[700];
char Find_msg[50] = {0};//���ڻ����ȡ���ַ���
/* ��غ궨�� */
extern char Usart2_Rx_Buf[USART2_RBUFF_SIZE];
extern char Usart3_Rx_Buf[USART3_RBUFF_SIZE];

/*
*************************************************************************
*                             ��������
*************************************************************************
*/
/*��������*/
void connected_IP(void);
char *Str_To_ASC(char* a);
void send_MQTT(char* topic,char* message,int len);
int Find_string(char *pcBuf,char*left,char*right, char *pcRes);
/*�߳�����*/
static void led1_thread_entry(void* parameter);
static void key_thread_entry(void* parameter);
static void usart2_thread_entry(void* parameter);
static void usart3_thread_entry(void* parameter);
/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
    /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
  rt_kprintf("����һ��[Ұ��]-STM32F103�Ե�-RTT�̹߳���ʵ�飡\n\n");
  rt_kprintf("����K1�����̣߳�����K2�ָ��߳�\n");
//*******************************************************************************//	
 key_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "key",              /* �߳����� */
                      key_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      2,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (key_thread != RT_NULL)
        rt_thread_startup(key_thread);
    else
        return -1;
//*******************************************************************************//	
		led1_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "led1",              /* �߳����� */
                      led1_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      3,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (led1_thread != RT_NULL)
        rt_thread_startup(led1_thread);
    else
        return -1;
//*******************************************************************************//	
		
		/* ����һ���ź��� */
	test_sem2 = rt_sem_create("test_sem2",/* ��Ϣ�������� */
                     0,     /* �ź�����ʼֵ��Ĭ����һ���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/
	CSQ_sem = rt_sem_create("CSQ_sem",/* ��Ϣ�������� */
                     0,     /* �ź�����ʼֵ��Ĭ����һ���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/	
  if (test_sem2 != RT_NULL)
    rt_kprintf("�ź��������ɹ���\n\n");
    
  usart2_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "usart2",              /* �߳����� */
                      usart2_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      1024,                 /* �߳�ջ��С */
                      4,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (usart2_thread != RT_NULL)
        rt_thread_startup(usart2_thread);
    else
        return -1;
//*******************************************************************************//			
			/* ����һ���ź��� */
	test_sem3 = rt_sem_create("test_sem3",/* ��Ϣ�������� */
                     0,     /* �ź�����ʼֵ��Ĭ����һ���ź��� */
                     RT_IPC_FLAG_FIFO); /* �ź���ģʽ FIFO(0x00)*/
  if (test_sem3 != RT_NULL)
    rt_kprintf("�ź��������ɹ���\n\n");
    
  usart3_thread =                          /* �߳̿��ƿ�ָ�� */
    rt_thread_create( "usart3",              /* �߳����� */
                      usart3_thread_entry,   /* �߳���ں��� */
                      RT_NULL,             /* �߳���ں������� */
                      512,                 /* �߳�ջ��С */
                      5,                   /* �̵߳����ȼ� */
                      20);                 /* �߳�ʱ��Ƭ */
                   
    /* �����̣߳��������� */
   if (usart3_thread != RT_NULL)
        rt_thread_startup(usart3_thread);
    else
        return -1;	
}

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/

static void led1_thread_entry(void* parameter)
{	
  
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
       // rt_kprintf("led1_thread running,LED1_ON\r\n");
        
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
        //rt_kprintf("led1_thread running,LED1_OFF\r\n");
    }
}
//*******************************************************************************//	
static void key_thread_entry(void* parameter)
{	
  rt_err_t uwRet = RT_EOK;	
    while (1)
    {
      if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )/* K1 ������ */
      {
        rt_kprintf("����LED1�̣߳�\n");
        uwRet = rt_thread_suspend(led1_thread);/* ����LED1�߳� */
        if(RT_EOK == uwRet)
        {
          rt_kprintf("����LED1�̳߳ɹ���\n");
        }
        else
        {
          rt_kprintf("����LED1�߳�ʧ�ܣ�ʧ�ܴ��룺0x%lx\n",uwRet);
        }
      } 
      if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON )/* K1 ������ */
      {
        rt_kprintf("�ָ�LED1�̣߳�\n");
        uwRet = rt_thread_resume(led1_thread);/* �ָ�LED1�̣߳� */
        if(RT_EOK == uwRet)
        {
          rt_kprintf("�ָ�LED1�̳߳ɹ���\n");
        }
        else
        {
          rt_kprintf("�ָ�LED1�߳�ʧ�ܣ�ʧ�ܴ��룺0x%lx\n",uwRet);
        }
      }
      rt_thread_delay(20);
    }
}

//*******************************************************************************//	
static void usart2_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;	
	
    /* ������һ������ѭ�������ܷ��� */
	Usart2_SendString(USART2,"usart2 runing");
	
		NBiot_Init() ;
    connected_IP();
  while (1)
  {
//	                                                          
//		uwRet = rt_sem_take(test_sem2,	/* ��ȡ�����жϵ��ź��� */
//                        0); 	  /* �ȴ�ʱ�䣺0 */
		Usart3_SendString(USART3,"\r\nCLS(0);\r\n");  //����
		NBiot_SendCmd("AT+CSQ","OK", 1000);
		
    send_MQTT(TOPIC,Str_To_ASC("123156"),strlen(ALL_message)); 
		sprintf(ALL_message,"%s",empty);   
		rt_thread_delay(5000);
//    if(RT_EOK == uwRet)
//    {
//			
//			Usart2_SendString(USART2,"�յ�����");
//			Usart2_SendString(USART2,Usart2_Rx_Buf);
//      memset(Usart2_Rx_Buf,0,USART2_RBUFF_SIZE);/* ���� */
//    }
  }
}
//*******************************************************************************//	
static void usart3_thread_entry(void* parameter)  //��ʱ������ʾ��
{
  rt_err_t uwRet = RT_EOK;	
    /* ������һ������ѭ�������ܷ��� */
	char cRes[20] = {0};//���ڻ����·�����
	Usart3_SendString(USART3,"usart3 runing");
	Usart3_SendString(USART3,"\r\nCLS(0);\r\n");  //����
	rt_thread_delay(2000);
  while (1)
  {
		uwRet = rt_sem_take(CSQ_sem,	/* ��ȡ�����жϵ��ź��� */
                        0); 	  /* �ȴ�ʱ�䣺0 */
    if(RT_EOK == uwRet)
    {
			Usart3_SendString(USART3,"�յ�����");
			//DS12(10,20,'����',4,0);
			Find_string(Usart2_Rx_Buf,"+","OK", cRes);
		Usart3_SendString(USART3,"\r\nDS12(10,20,'");
		Usart3_SendString(USART3,cRes);
		Usart3_SendString(USART3,"',4,0);\r\n");
		memset(cRes,0,sizeof(cRes));/* ���� */
			
      memset(Usart3_Rx_Buf,0,USART3_RBUFF_SIZE);/* ���� */
    }
  }
}
//***********************************�Զ��庯��********************************************//	

/***********************************************************
�������ƣ�connected_IP
�������ܣ�NB���ӷ�����   47.105.157.158  60.205.203.64
��ڲ�������
���ڲ�������
�� ע����Ϊ��һ���ϵ����� ������ĸ�λ�����������
***********************************************************/
void connected_IP(void)
{  
		rt_thread_delay(3000);
		 if (error_flag ==0)  //��������
		 {
			 
		   while(!NBiot_SendCmd("AT+CEREG?","+CEREG: 0,1", 2000)); //ѭ�����ӣ�ֱ�����ӵ�������
			 
			 while(!NBiot_SendCmd("AT+EMQNEW=\"60.205.203.64\",\"1883\",12000,1000","OK", 2000)); //ѭ�����ӣ�ֱ�����ӵ�������
			rt_thread_delay(1000);
		 if(NBiot_SendCmd("AT+EMQCON=0,3,\"rt_thread_test\",80000,1,0,\"root\",\"citc2018\"","OK", 2000)!=0){
				 rt_kprintf("%s","connected_success��");
		    }
		 }
		 else{  //��λ����
		 if(NBiot_SendCmd("AT+ZRST","OK", 1000)){ 
		 while(!NBiot_SendCmd("AT+EMQNEW=\"60.205.203.64\",\"1883\",12000,1000","OK", 2000));
		rt_thread_delay(1000);
		 if(NBiot_SendCmd("AT+EMQCON=0,3,\"rt_thread_test\",80000,1,0,\"root\",\"citc2018\"","OK", 2000)!=0)
		 {
		  rt_thread_delay(1000);
				 rt_kprintf("%s\r\n","connected_success��");
						}
				}
			 error_flag = 0;
			}
	}

/***********************************************************
�������ƣ�send_MQTT
�������ܣ�NB������Ϣ��MQTT������
��ڲ�����
char* topic:   ������Ϣ������
char* message: ���͵���Ϣ����
int len:       ���͵���Ϣ����
���ڲ�������
�� ע����������ʱ�������� ��λNBģ��������
����len�������봫�� ��Ϊָ�������޷��������ݳ���
***********************************************************/
void send_MQTT(char* topic,char* message,int len)
{
	char* head = "AT+EMQPUB=0,\"";
	char* modle = "\",1,0,0," ;
	char* fu_L = ",\"" ;
	char* fu_R = "\"" ;
	char char_len[10];
	char output [100];
	sprintf(output,"%s",""); 
	sprintf(char_len,"%s",""); 
	sprintf(char_len,"%d",len/2);    //ת��Ϊ�ַ���
	sprintf(output, "%s%s%s%s%s%s%s", head,topic,modle,char_len,fu_L,message,fu_R); //ƴ���ַ���
	if (!NBiot_SendCmd(output,"OK",3000))
		{
		 error_time += 1;
	}	
	else error_time = 0 ;
	if (error_time >6) // �������error 6��  ����
	{
	 error_flag = 1;  //������
	 connected_IP();
	 error_time =0;
	}		
}	
	
/***********************************************************
�������ƣ�Str_To_ASC
�������ܣ������ַ���   ����������תΪASCII��ʽ��NB
��ڲ�����char* a
���ڲ�������
�� ע��
	##0112QN=20190306152401;ST=32;CN=51;PW=CITC2018+;MN=112019015222;Flag=5 ;CP=&&LA=30;TE=25;HU=75;&&1C80\r\n
***********************************************************/
char *Str_To_ASC(char* a)  
{
	int i;
	int w;
	int x;
	char letter_out;
	char val[10];
	char letter[10];
  char mess[110];	
	static char out_msg[110];
	sprintf(out_msg,"%s",empty);
    for(i=0;a[i]!='\0';i++)
		{
			//�����ַ�����
			if(a[i]=='&')
			  strcat(mess,"26"); 
			else if(a[i]=='+') 
				strcat(mess,"2b"); 
			else if(a[i]=='=')
				strcat(mess,"3d"); 
			else if(a[i]==';')
				strcat(mess,"3b");
			else if(a[i]=='#')	
				strcat(mess,"23");
			else if(a[i]=='-')
				strcat(mess,"2d");  //ƴ���ַ�   �ұߵ�ƴ�����
			else if (islower(a[i])) // islower(a[i]) Сд��ĸ���� 
			{					
				if(a[i] >='a' && a[i]<='i'){
					x = a[i] - 'a'+61;
					sprintf(letter,"%d",x);
					strcat(mess,letter);   //ƴ���ַ�	
				}
				if(a[i] >='j' && a[i]<='o'){
					switch (a[i] - 'a'+61){
						case 70 : 
					    strcat(mess,"6a"); break;  //ƴ���ַ�	
					  case 71 : 
					    strcat(mess,"6b"); break;  //ƴ���ַ�
						case 72 : 
					    strcat(mess,"6c"); break;  //ƴ���ַ�
						case 73 : 
					    strcat(mess,"6d"); break;  //ƴ���ַ�
						case 74 : 
					    strcat(mess,"6e"); break;  //ƴ���ַ�
						case 75 : 
					    strcat(mess,"6f"); break;  //ƴ���ַ�
					}
				}
				if(a[i] >='p' && a[i]<='y'){
					x = a[i] - 'a'+61-6;
					sprintf(letter,"%d",x);
					strcat(mess,letter);   //ƴ���ַ�	
				}
				if(a[i] == 'z') strcat(mess,"7a");
				
			}
			else if (isupper(a[i])) //  isupper(c) ��д��ĸ����
			{		
				if(a[i] >='A' && a[i]<='I'){
					x = a[i] - 'A'+41;
					sprintf(letter,"%d",x);
					strcat(mess,letter);   //ƴ���ַ�	
				}
				if(a[i] >='J' && a[i]<='O'){
					letter_out = a[i] -42 + 'A';
					sprintf(letter,"%d%s",4,&letter_out);
					strcat(mess,letter);   //ƴ���ַ�	
				}
				if(a[i] >='P' && a[i]<='Y'){
					x = a[i] - 'A'+41-6;
					sprintf(letter,"%d",x);
					strcat(mess,letter);   //ƴ���ַ�	
				}
				if(a[i] == 'Z') strcat(mess,"5a");
			}	
			else if (a[i] >='0' && a[i]<='9')  //���ִ���
					{
						w = a[i] - '0'+30;  //�����ַ�ת  int
						sprintf(val,"%d",w);
						strcat(mess,val);   //ƴ���ַ�
					}
		}
		strcpy(out_msg,mess);
		sprintf(letter,"%s",empty);
		sprintf(mess,"%s",empty);
		return out_msg;
}

/***********************************************************
  �������ƣ�Find_string(char *pcBuf,char*left,char*right, char *pcRes)
  �������ܣ�Ѱ���ض��ַ���
  ��ڲ�����
           char *pcBuf Ϊ������ַ���
           char*left   Ϊ�����ַ�����߱�ʶ��  ���磺"["
           char*right  Ϊ�����ַ����ұ߱�ʶ��  ���磺"]"
					 char *pcRes Ϊ���ת����ַ�������
  ����ֵ������У���Ƿ�ɹ�������ν�ġ�
  ��ע��
 �������·����������+MQTTPUBLISH: 0,0,0,0,/device/NB/zx99999999999999_back,6,[reastrobot]
***********************************************************/
int Find_string(char *pcBuf,char*left,char*right, char *pcRes)
{
	char *pcBegin = NULL;
	char *pcEnd = NULL;
	
	pcBegin = strstr(pcBuf, left);
	pcEnd = strstr(pcBuf, right);
 
	if(pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
	{
		printf("Mail name not found!\n");
	}
	else
	{
		pcBegin += strlen(left);
		memcpy(pcRes, pcBegin, pcEnd-pcBegin);
	}
 
	return 0;
}
/********************************END OF FILE****************************/
