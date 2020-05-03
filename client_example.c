#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
int s2n(char *str)
{
    char *p = str;
    int sum = 0;
    while(*p++)
    {
        sum *=10;
        sum += *p-48;
        printf("%d",*p);
    }
    return sum;
}
int main(int argc, char *argv[])
{
	int client_sock;
	struct sockaddr_in server_addr;
	char send_msg[] = "Hello Network!\n";
	char recv_msg[255];

	/* 创建socket */
	client_sock = socket(AF_INET, SOCK_STREAM, 0);

	/* 指定服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(s2n(argv[1]));
	server_addr.sin_addr.s_addr = inet_addr(argv[0]); //127.0.0.1指向本机
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero)); //零填充

	/* 连接服务器 */
	connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

	/* 发送消息 */
	printf("Send: %s", send_msg);
	send(client_sock, send_msg, strlen(send_msg), 0);

	/* 接收并显示消息 */
	memset(recv_msg, 0, sizeof(recv_msg)); //接收数组置零
	recv(client_sock, recv_msg, sizeof(recv_msg), 0);
	printf("Recv: %s", recv_msg);

	/* 关闭socket */
	close(client_sock);

	return 0;
}
