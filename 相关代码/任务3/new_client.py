from client import Client
import os
if __name__ == '__main__':
    ip=input('请输入服务器的ip：')
    port=int(input('请输入服务器绑定的端口号：'))
    print('-----欢迎来到聊天室，退出聊天室请输入 “exit”-----' )
    name=input("输入你的昵称: ")
    c = Client(name = name, address = (ip,port))
    c.start()
