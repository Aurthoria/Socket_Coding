import socket
import sys
import threading

SPLIT = "|" #用于隔开用户名和消息以及判断是发送过来的信息是客户登录时的用户名还是聊天信息
BUFSIZE = 1024 #要接收的最大数据量

class Client(socket.socket):
    '''这是一个继承socket.socket的类，用于实现客户端程序'''
    
    def __init__(self, name, address):
        '''用父类socket.socket的初始化方法来初始化继承的属性'''
        #socket.SOCK_DGRAM用于不可靠传输UDP
        super(Client, self).__init__(socket.AF_INET,socket.SOCK_DGRAM)
        self.__name = name#客户名
        self.__address = address#客户机地址

    def start(self):
        '''该函数用于启动该客户端'''
        
        #首先进行服务器连接检测
        try:
            self.sendto(self.__name.encode(encoding='UTF-8',errors='ignore'), self.__address)
        except :
            print("----------服务器连接失败----------")

        #客户名合法性检测    
        name_verify = self.recv(BUFSIZE).decode(encoding='UTF-8',errors='ignore')
        
        while name_verify == "error":
            name=input("该昵称已被用，请重新输入：")
            while '\\' in name or '|' in name:
                name = input("昵称存在非法字符，请重新输入：")
            self.sendto(name.encode(encoding='UTF-8',errors='ignore'), self.__address)
            self.__name = name
            name_verify = self.recv(BUFSIZE).decode(encoding='UTF-8',errors='ignore')
        
        t=threading.Thread(target=self.recv_msg)#创建另一个线程使得发送消息和接收消息不冲突
        t.start()
        
        while True:
            word=input().strip()#发送消息
            if not word:
                continue  # 避免回车导致服务器的死循环
            self.send_msg(word)
            if word == "exit":
                self.close()
                break

    def send_msg(self,word):
        '''该函数用于向客户端发送消息，消息包括用户名和实际内容，用'|'隔开'''
        
        data = (self.__name + SPLIT + word).encode(encoding='UTF-8',errors='ignore')
        self.sendto(data, self.__address)

    def recv_msg(self):
        '''该函数用于接收来自服务端的消息'''
        
        while True:
            try:
                msg = self.recv(BUFSIZE)
                self.show_msg(msg)

            except ConnectionResetError as e:
                print("----------服务器连接失败----------")

            except OSError as f:
                print("----------连接关闭----------")
                break

    def show_msg(self,data):
        '''该函数用于向该客户展示信息内容，其中不显示客户自己发的信息'''
        name, word = data.decode(encoding='UTF-8',errors='ignore').split(SPLIT)
        if name == 'Broadcast'or name == 'Unicast':
            print(word,'\n\n')
        elif name!=self.__name:
            print("%s : %s"%(name, word))
