#Demo of a small server
#Examples from https://docs.python.org/2/library/socket.html

# Echo server program
import socket

HOST = ''                 # Symbolic name meaning all available interfaces
PORT = 50007              # Arbitrary non-privileged port
while 1:
    print 'Opening new connection'
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    print 'Connected by', addr
    while 1:
        data = conn.recv(1024)
        print 'Received data: ', data
        print 'data length: ',len(data)
        if not data: break
        conn.sendall(data)
    conn.close()
