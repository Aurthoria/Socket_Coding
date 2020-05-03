from server import Server
import socket

def get_host_ip():
    '''用于查询本机ip地址，返回值为ip'''
    
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
        
    return ip

if __name__ == '__main__':
    ip = get_host_ip()
    print('服务器的ip：%s'%ip)
    port=int(input('请输入要绑定的端口号：'))
    s = Server(serv_address = (ip,port))
    with open("user.txt",'r+') as f:
        f.truncate()#清空user.txt
    s.start()
