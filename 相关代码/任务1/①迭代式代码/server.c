#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h> 
/* 字符串逆序函数 */
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
		  return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	  {
		  *p1 ^= *p2;
		  *p2 ^= *p1;
		  *p1 ^= *p2;
	  }
	  /* UTF-8汉字一个是三个字节，进行相应逆序转化 */
	  for (p2 = str + strlen(str) - 1; p2 >= str; --p2)
	  {
		  if (*p2&0x80)//最高位为1
		  {
			  
			  *p2 ^= *(p2-2);
			  *(p2-2) ^= *p2;
			  *p2 ^= *(p2-2);
			  p2 = p2-2;
		  }
	  }
      return str;
}
int main(int argc, char *argv[])
{
	int server_sock_listen, server_sock_data;
	struct sockaddr_in server_addr;
	char recv_msg[255];
	char stop_word[]="bye";
	int is_esc;
	int i;

	/* 创建socket */
    
    /*************************************************
    函数原型: int socket(int family, int type, int protocol); 
	参数：
	family：协议族。实验用AF_INET
    type：socket类型。常见有SOCK_STREAM、SOCK_DGRAM、SOCK_RAW等
    protocol：具体协议。0表示默认协议
	返回值:
	成功：返回一个新建的socket，用于数据传输
    失败：返回-1，并在全局变量errno中记录错误类型
    *************************************************/
	server_sock_listen = socket(AF_INET, SOCK_STREAM, 0);
	if (errno)
    {
		perror("创建socket失败");
		return 0;
    }

	/* 指定服务器地址 */
    
    /* 用于socket创建通信连接的类型，这里就是ipv4地址类型的通信连接可用 */
	server_addr.sin_family = AF_INET;
    /* 整型变量从主机字节顺序转变成网络字节顺序：Big-Endian */
	server_addr.sin_port = htons(atoi(argv[1]));
    /* INADDR_ANY表示本机所有IP地址 */
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    /* 进行零填充 */
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero)); 
	if (errno)
    {
		perror("指定服务器地址失败");
		return 0;
    }

	/* 绑定socket与地址 */
    
    /*************************************************
    函数原型: int bind(int sockfd, const struct sockaddr *addr, int addrlen); 
	参数：
	sockfd：要绑定的socket的描述符
	addr：要绑定的地址。此处为指向通用地址结构的指针，使用时，要进行强制类型转换
	addrlen：地址结构大小。可使用sizeof自动计算
	返回值：
	成功：返回0
	失败：返回-1，并在全局变量errno中记录错误类型
	*************************************************/
	bind(server_sock_listen, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (errno)
    {
		perror("绑定socket与地址失败");
		return 0;
    }
	
	
	while(1)
	{
		/* 监听socket */
        
        /*************************************************
		函数原型：int listen(int sockfd, int backlog); 
		参数：
		sockfd：要监听的socket的描述符
		backlog：该socket上完成队列的最大长度。完成队列是指已完成三次握手(established)，但尚未被服务器接受（accept）的客户机
		返回值：
		成功：返回0
		失败：返回-1，并在全局变量errno中记录错误类型
		*************************************************/
		listen(server_sock_listen, 0);
		printf("Server is listening....\n");
		if (errno)
		{
			perror("监听socket失败\n");
			return 0;
		}
		
		/* 接收并显示消息 */
        
        /*************************************************
		函数原型：int accept(int sockfd, struct sockaddr *addr, int *addrlen); 
		参数：
		sockfd：用于接受连接的socket的描述符
		addr：客户机地址。此处为指向通用地址结构的指针，使用时，要进行强制类型转换。如果不关心客户机地址，可以设为NULL
		addrlen：客户机地址结构大小。如果不关心，可以和addr一起设为NULL
		返回值：
		成功：返回一个新建的socket，用于数据传输
		失败：返回-1，并在全局变量errno中记录错误类型
		*************************************************/
		server_sock_data = accept(server_sock_listen, NULL, NULL);//服务器接受连接
		printf("Accept......\n");
		if (errno)
		{
			perror("服务器接受连接失败\n");
			return 0;
		}
		
		while(1)
		{
			is_esc = 0;
			memset(recv_msg, 0, sizeof(recv_msg)); //接收数组置零
            
            /*************************************************
            函数原型：int recv(int sockfd, void *buf, int len, int flags); 
			参数：
			sockfd：用于接收数据的socket的描述符
			buf：指向数据的指针
			len：接收数据大小
			flags：额外选项。本次实验设为0
			返回值：
			成功：返回接收的数据大小
			失败：如果对方已关闭连接，返回0；其他错误返回-1，并在全局变量errno中记录错误类型
			*************************************************/
            
            /* 若连接断开则退出此客户机的连接，监听下一个客户机 */
			if (recv(server_sock_data, recv_msg, sizeof(recv_msg), 0) == 0)
			{
				printf("Connection interrupted\n\n");
				break;
			}
            
			printf("Recv: %s\n", recv_msg);
			if (errno)
			{
				perror("显示消息失败");
				return 0;
			}
			
			for(i = 0; i < strlen(recv_msg); i++)
			{
				if(recv_msg[i] == 27)
				{
					is_esc = 1;
					break;
				}
			}
			/* 接收到bye或者esc就断开连接 */
			if (strcmp(recv_msg,stop_word) == 0 || is_esc )
			{
				printf("\n");
				break;
			}
			
            /* 字符串逆序 */
			strrev(recv_msg);
			
			/* 发送消息 */
			printf("Send: %s\n", recv_msg);
            
            /*************************************************
			函数原型：int send(int sockfd, const void *buf, int len, int flags); 
			接收4个参数：
			sockfd：用于发送数据的socket的描述符
			buf：指向数据的指针
			len：发送数据大小
			flags：额外选项。本次实验设为0
			返回值：
			成功：返回发送的数据大小
			失败：返回-1，并在全局变量errno中记录错误类型
			*************************************************/
			send(server_sock_data, recv_msg, strlen(recv_msg), 0);
			if (errno)
			{
				perror("发送消息失败");
				return 0;
			}
		}
		
		/* 关闭数据socket */
        
        /*************************************************
		函数原型：int close(int sockfd); 
		参数：
		sockfd：要关闭的socket的描述符
		返回值：
		成功：返回0
		失败：返回-1，并在全局变量errno中记录错误类型
		*************************************************/
		close(server_sock_data);
		if (errno)
		{
			perror("关闭数据socket失败");
			return 0;
		}
	}
	
	/* 关闭监听socket */
    
	close(server_sock_listen);
	if (errno)
    {
		perror("关闭监听socket失败");
		return 0;
    }

	return 0;
}