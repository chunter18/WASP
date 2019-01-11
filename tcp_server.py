"""
Base TCP server - adding command set arch
written by Cole Hunter

PROG INFO

standard mount point == /mnt/usb

Command Set: - currently supported
HIBERNATE
HIBERNATEXXX //XXX will be a time interval to hibernate for



"""
import socket
import time
import sys
import os
from threading import Thread
import logging


module_ips = []

def server(host, port, store_dir, file):
    print("Setting up Server...")
    print('')
    filepath = '/mnt/usb/' + store_dir + '/' +  file + '.txt'
    os.makedirs(os.path.dirname(filepath), exist_ok=True)
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    #bind sockets. if not successful, exit program.
    try:
        s.bind((host, port))
        s.listen(1)
    except socket.error as msg:
        print("ERROR: {}".format(msg))
        s.close()
        s = None
    if s is None:
        sys.exit(1)

    #mainloop.
    while True:
        print("Listening on: {}:{}".format(host, port))
        data_len = 0
        
        #listen and accept connections
        try:
            conn, addr = s.accept()
        #alllow fo user quit
        except KeyboardInterrupt:
            print("Closing Connection")
            s.close()
            s = None
            sys.exit(1)

        print('Incoming connection accepted: {}'.format(addr))
        #thread spawn here

        try:
            while 1:
                data = conn.recv(4096)
                if not data:
                    break
                #debug print here
                print(time.strftime("%b %d %H:%M:%S ", time.localtime()), addr[0], ":", repr(data))
                print("")

                #save data to /mnt/usb
                with open(filepath, "w") as f:
                    f.write(str(data))
                    f.write('\n')
                         
                #send response. current response is an echo.
                conn.send(data)
                
        except KeyboardInterrupt:
            print("Closing Connection")
            s.close()
            s = None
            sys.exit(1)

        conn.close()

def client(host, port):
    pass

if __name__ == '__main__':

    #server(host='', port=50007, store_dir='TEST', file='test')

    logging.basicConfig(level=logging.DEBUG)

    serverthread = Thread(target=server, args=('',50007,'TEST','test'))
    clientthread = Thread(target=client, args=('',50008))

    serverthread.start()
    clientthread.start()

    print('control section')
