#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h> 
/* 不接收换行符的fgets()函数 */
void my_fgets(char data[], int count)
{
    fgets(data, count, stdin);
    /* 找出data中的"\n" */
    char *find = strchr(data, '\n');
    /* 截取换行符前的字符串 */
    if(find)
        *find = '\0';
}
int main(int argc, char *argv[])
{
	int client_sock;
    errno = 0; 
	struct sockaddr_in server_addr;
	char send_msg[255];
	char recv_msg[255];
	char stop_word[]="bye";
	int is_esc;
	int i;
	
	/* 创建socket */
    /*************************************************
	函数原型:int socket(int family, int type, int protocol); 
	参数：
    family：协议族。实验用AF_INET
    type：socket类型。常见有SOCK_STREAM、SOCK_DGRAM、SOCK_RAW等
    protocol：具体协议。0表示默认协议
	返回值:
	成功：返回一个新建的socket，用于数据传输
    失败：返回-1，并在全局变量errno中记录错误类型
    *************************************************/
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (errno)
    {
		perror("创建socket失败");
		return 0;
    }
	
	/* 指定服务器地址 */
    
    /* 用于socket创建通信连接的类型，这里就是ipv4地址类型的通信连接可用 */
	server_addr.sin_family = AF_INET;
    /* 整型变量从主机字节顺序转变成网络字节顺序：Big-Endian */
	server_addr.sin_port = htons(atoi(argv[2]));
    /* 将点分十进制的IP地址转化为无符号长整数型数的网络字节序 */
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); 
    /* 进行零填充 */
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero)); 
    if (errno)
    {
		perror("指定服务器地址失败");
		return 0;
    }
	
	/* 连接服务器 */
    
    /*************************************************
	函数原型: int connect(int sockfd, const struct sockaddr *addr, int addrlen); 
	参数：
	sockfd：用于发起连接的socket的描述符
    addr：服务器地址。此处为指向通用地址结构的指针，使用时，要进行强制类型转换
    addrlen：服务器地址结构大小。可使用sizeof自动计算
	返回值：
    成功：返回0
    失败：返回-1，并在全局变量errno中记录错误类型
    *************************************************/
	connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (errno)
    {
		perror("连接服务器失败");
		return 0;
    }
	
	while(1)
	{
		/* 发送消息 */
        
        is_esc=0;
		printf("Myself: ");
		my_fgets(send_msg, 256);
        /*************************************************
		函数原型: int send(int sockfd, const void *buf, int len, int flags); 
		参数：
		sockfd：用于发送数据的socket的描述符
		buf：指向数据的指针
		len：发送数据大小
		flags：额外选项。本次实验设为0
		返回值：
		成功：返回发送的数据大小
		失败：返回-1，并在全局变量errno中记录错误类型
		*************************************************/
		send(client_sock, send_msg, strlen(send_msg), 0);
		if (errno)
		{
			perror("发送消息失败");
			return 0;
		}
		
		for(i = 0; i < strlen(send_msg); i++)
		{
			if(send_msg[i] == 27)
			{
				is_esc = 1;
				break;
			}
		}
        /* 如果发送消息是bye或者含有ESC就退出 */ 
		if (strcmp(send_msg,stop_word) == 0 || is_esc )
		{
			printf("\n");
			break;
		}
		
		/* 接收并显示消息 */
        
        /* 接收数组置零 */
		memset(recv_msg, 0, sizeof(recv_msg));
        
        /*************************************************
		函数原型: int recv(int sockfd, void *buf, int len, int flags); 
		参数：
		sockfd：用于接收数据的socket的描述符
		buf：指向数据的指针
		len：接收数据大小
		flags：额外选项。本次实验设为0
		返回值：
		成功：返回接收的数据大小
		失败：如果对方已关闭连接，返回0；其他错误返回-1，并在全局变量errno中记录错误类型
		*************************************************/
		if (recv(client_sock, recv_msg, sizeof(recv_msg), 0) == 0)
		{
			printf("Connection interrupted\n");
			break;
		}
		printf("Server: %s\n", recv_msg);
		if (errno)
		{
			perror("发送消息失败");
			return 0;
		}
	}
	
	
	
	/* 关闭socket */
    
    /*************************************************
	函数原型: int close(int sockfd); 
	参数：
	sockfd：要关闭的socket的描述符
	返回值：
	成功：返回0
	失败：返回-1，并在全局变量errno中记录错误类型
	*************************************************/
	close(client_sock);
	if (errno)
	{
		perror("关闭socket失败");
		return 0;
	}

	return 0;
}