
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
static rt_thread_t usart2_thread = RT_NULL;//����2�߳�
static rt_thread_t usart3_thread = RT_NULL;//����3�߳�

/* �����ź������ƿ� */
rt_sem_t test_sem2 = RT_NULL;
rt_sem_t test_sem3 = RT_NULL;
rt_sem_t CSQ_sem = RT_NULL;
/************************* ȫ�ֱ������� ****************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 connected_IP(IP,Port,ID,User,Pwd);
 FE 04 00 00 00 01 25 C5
 */
char T_message[10],EC1_message[10];
unsigned char     TH[8] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9}; //��ʪ�ȴ�������ѯ��
unsigned char     EC_1[8] = {0xFE, 0x04, 0x00, 0x00, 0x00, 0x01, 0x25, 0xC5}; //��ʪ�ȴ�������ѯ��
char* ip = "47.105.157.158";
char* port = "1883";
char* id = "wenzheng";
char* user = "root";
char* passward = "citc2018";
char* TOPIC = "/device/NB/wz_rt_thread_test"; //��������topic
int error_flag =0; //ʧ��������־λ
int error_time =0; //ʧ�ܵĴ���
char *empty = "";
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
char *Str_To_ASC(char* a);
void send_MQTT(char* topic, char* message);
void connected_IP(char* IP,char*Port,char* ID,char* User,char* Pwd);
int Find_string(char *pcBuf,char*left,char*right, char *pcRes);
/*�߳�����*/
static void led1_thread_entry(void* parameter);
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
  rt_kprintf("������ RT-thread OS ��\n\n");

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
    rt_kprintf("�ź���2�����ɹ���\n\n");
    
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
    rt_kprintf("�ź���3�����ɹ���\n\n");
    
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
        //rt_kprintf("led1_thread running,LED1_ON\r\n");
        
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
        //rt_kprintf("led1_thread running,LED1_OFF\r\n");
    }
}

//*******************************************************************************//	
static void usart2_thread_entry(void* parameter)
{
  rt_err_t uwRet = RT_EOK;	
	
    /* ������һ������ѭ�������ܷ��� */
		NBiot_Init() ;
    connected_IP(ip,port,id,user,passward);
  while (1)
  {
	                                                          
		uwRet = rt_sem_take(test_sem2,	/* ��ȡ�����жϵ��ź��� */
                        0); 	  /* �ȴ�ʱ�䣺0 */
		NBiot_SendCmd("AT+CSQ","OK", 1000);
    send_MQTT(TOPIC,T_message);  
		memset(T_message,0,sizeof(T_message));/* ���� */
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
	rt_thread_delay(2000);
  while (1)
  {
		uwRet = rt_sem_take(CSQ_sem,	/* ��ȡ�����жϵ��ź��� */
                        0); 	  /* �ȴ�ʱ�䣺0 */
		sprintf(T_message, "%d", RS485_SendCmd(TH, sizeof(TH),9,3,100));
		rt_thread_delay(200);
		sprintf(EC1_message, "%d", RS485_SendCmd(EC_1, sizeof(EC_1),7,3,100));
//	rt_kprintf("�յ����ݣ�\n");
//	rt_kprintf("T_M:%s\r\n",T_message);
//	rt_kprintf("EC1:%s\r\n",EC1_message);
		rt_thread_delay(1000);
    if(RT_EOK == uwRet)
    {
//		rt_kprintf("�յ����ݣ�\n");
//		rt_kprintf(T_message);
		//Usart3_SendString(USART3,"�յ�����");
		//Usart3_SendString(USART3,Usart3_Rx_Buf);
    //memset(Usart3_Rx_Buf,0,USART3_RBUFF_SIZE);/* ���� */
    }
  }
}
//***********************************�Զ��庯��********************************************//	



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
  //AT+MQTTPUB="device/nb/citc",1,1,0,0,"##0112QN=20190306152401;ST=32;CN=51;PW=CITC2018+;MN=112019015222;Flag=5 ;CP=&&LA=30;TE=25;HU=75;&&1C80"
***********************************************************/
void send_MQTT(char* topic, char* message)
{
  char* head = "AT+MQTTPUB=\"";
  char* modle = "\",1,1,0,0";
  char* fu_L = ",\"";
  char* fu_R = "\"";
  char output [100];
  sprintf(output, "%s%s%s%s%s%s", head, topic, modle, fu_L, message, fu_R); //ƴ���ַ���
  if (!NBiot_SendCmd(output, "+MQTTPUBACK", 4000))
  {
    error_time += 1; 
    rt_kprintf("error_time:%d\r\n", error_time);
  }
  else error_time = 0 ;
  if (error_time >= 5) // �������error 5��  ����
  {
    error_flag = 1;  //������	
    connected_IP(ip,port,id,user,passward); 
		error_time = 0;
  }
  memset(output,0,sizeof(output));/*�������� */
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
		rt_kprintf("Mail name not found!\n");
	}
	else
	{
		pcBegin += strlen(left);
		memcpy(pcRes, pcBegin, pcEnd-pcBegin);
	}
	return 0;
}


/***********************************************************
  �������ƣ�connected_IP
  �������ܣ�NB���ӷ�����   47.105.157.158  60.205.203.64
  ��ڲ�������
  ���ڲ�������
  �� ע����Ϊ��һ���ϵ����� ������ĸ�λ�����������
       �����������200�γ���ʧ�ܣ���λNB������
char* TOPIC = "/device/NB/zx11111111111111";                              //��������topic
char* SUB_TOPIC = "AT+MQTTSUB=\"/device/NB/zx11111111111111_back\",1";    //���Ļش�����
char* Connect = "AT+MQTTCFG=\"47.105.157.158\",1883,\"zx11111111111111\",60,\"root\",\"citc2018\",1"; //����������ָ��
***********************************************************/
void connected_IP(char* IP,char*Port,char* ID,char* User,char* Pwd)
{
  int try_time = 0;
	char Connect [100];
	char SUB_TOPIC [100];
  rt_thread_delay(4000);
	sprintf(Connect,"%s%s%s%s%s%s%s%s%s%s%s","AT+MQTTCFG=\"",IP,"\",",Port,",\"",ID,"\",60,\"",User,"\",\"",Pwd,"\",1");
	sprintf(SUB_TOPIC,"%s%s%s","AT+MQTTSUB=\"",TOPIC,"_back\",1");
	//printf("Connect:%s",Connect);
	//printf("SUB_TOPIC:%s",SUB_TOPIC);
  if (error_flag == 0) //��������
  {
    Get_IP();  //ѭ�����ӣ�ֱ�����ӵ�������
    rt_thread_delay(1000);
		//IWDG_Feed(); //ιһ�ι�
    if (NBiot_SendCmd(Connect,"OK", 2000)) {
      rt_thread_delay(1000);
      if (NBiot_SendCmd("AT+MQTTOPEN=1,1,0,0,0,\"\",\"\"", "OK", 5000)) { //�˴�û��������������ȷ���յ�����ֵ ��ʱ�ӳ���
        rt_kprintf("%s", "connected_success��");
        while (!NBiot_SendCmd(SUB_TOPIC,"OK",2000)) {
          try_time += 1;
          rt_kprintf("Try_Time:%d\r\n", try_time);
          if (try_time == 6) {
            try_time = 0;
            return;
          }
        }
        rt_kprintf("%s", "SUB_TOPIC_success��");
      } else rt_kprintf("%s", "connected_error��");
    }
  }
  else { //��λ����
    if (NBiot_SendCmd("AT+CMRB", "ATREADY", 1000)) {    //�������⣺�������У������ΪOK�Ļ��п��ܻ�������⣡������20190522
      Get_IP();  //ѭ�����ӣ�ֱ�����ӵ�������
       rt_thread_delay(1000);
      if (NBiot_SendCmd(Connect,"OK", 2000)) {
         rt_thread_delay(1000);
        if (NBiot_SendCmd("AT+MQTTOPEN=1,1,0,0,0,\"\",\"\"", "OK", 5000) != 0) //�˴�û��������������ȷ���յ�����ֵ ��ʱ�ӳ���
          rt_kprintf("%s", "connected_success��");
        while (!NBiot_SendCmd(SUB_TOPIC, "OK", 2000)) {
          try_time += 1;
          rt_kprintf("Try_Time:%d\r\n", try_time);
          if (try_time == 6) {
            try_time = 0;
            return;
          }
        }
        rt_kprintf("%s", "SUB_TOPIC_success��");
      }
      error_flag = 0;
    }
  }
}


/********************************END OF FILE****************************/
