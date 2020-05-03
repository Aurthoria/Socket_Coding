# -*- coding: utf-8 -*-
import socket
import sys,time,os

SPLIT = "|" #用于隔开用户名和消息以及判断是发送过来的信息是客户登录时的用户名还是聊天信息
BUFSIZE = 1024 #要接收的最大数据量

class Server(socket.socket):
    '''这是一个继承socket.socket的类，用于实现服务端程序'''
    
    def __init__(self,serv_address):
        '''用父类socket.socket的初始化方法来初始化继承的属性'''
        #socket.SOCK_DGRAM用于不可靠传输UDP
        super(Server, self).__init__(socket.AF_INET,socket.SOCK_DGRAM)
        self.__serv_address = serv_address#服务器地址
        self.__clnt_address = dict()#客户机地址

    def get_clnt_addr(self):
        '''该函数用于访问私有变量self.__clnt_address'''
        return self.__clnt_address

    def send_msg(self, msg, address, Type):
        '''该函数用于发送消息
        msg是要发送的字符串
        address是目的地址，形式为（ipaddr，port）的元组
        Type是发送对象的类型，包括广播（代表系统发送的消息）、单播、多播
        '''
        if Type == 'Broadcast':
            msg = 'Broadcast' + SPLIT + msg
            print(self.__clnt_address,time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time())))#服务端打印发送时间
            for name, address in self.__clnt_address.items():
                self.sendto(msg.encode(encoding='UTF-8',errors='ignore'),address)
                
        elif Type == 'Unicast':
            #只发送给address
            msg = 'Unicast' + SPLIT + msg
            self.sendto(msg.encode(encoding='UTF-8',errors='ignore'), address)
            
        elif Type=='Multicast':
            #发送给所有人，但不包括服务端
            for name, address in self.__clnt_address.items():
                if address != self.__serv_address:
                    self.sendto(msg.encode(encoding='UTF-8',errors='ignore'), address)

    def start(self):
        '''该函数用于启动该服务端'''
        
        #首先客户机对地址进行绑定
        self.bind(self.__serv_address)
        while True:
            try:
                data, addr = self.recvfrom(BUFSIZE)#接收信息
                data = data.decode(encoding='UTF-8',errors='ignore')

                if SPLIT not in data: #客户第一次登录，发送过来的只有不包含'|'的用户名
                    
                    with open("user.txt", 'r+') as f:
                        #往user.txt添加客户名
                        user_list=f.read().split('\n')
                        while data in user_list:#如果用户名已存在就发送error代表客户名重复
                            self.sendto(b'error',addr)
                            data, addr = self.recvfrom(BUFSIZE)#重新接收客户名
                            data = data.decode(encoding='UTF-8',errors='ignore')
                        f.write(data+"\n")
                    self.__clnt_address[data] = addr
                    self.send_msg("欢迎%s进入聊天室...." % data,'','Broadcast')
                    self.send_msg("------------------%s------------------"%data,addr,'Unicast')#只发送给该用户

                elif data.split(SPLIT)[1] == "exit":#传来消息为"exit"则代表有客户退出聊天室

                    self.__clnt_address.pop(data.split(SPLIT)[0])
                    self.send_msg("%s离开了聊天室...."%data.split(SPLIT)[0],'','Broadcast')#通知其他客户该客户离开聊天室
                    #在user.txt去除该客户名
                    with open("user.txt",'r') as f:
                        l=''.join(f.read().split(data.split(SPLIT)[0]+'\n'))
                    with open("user.txt",'w') as f:
                        f.write(l)

                else:#其他消息就进行群发
                    
                    self.send_msg(data, '','Multicast')

            except ConnectionError as e:
                print(e)
                pass

        self.close()  
