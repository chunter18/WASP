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
import select
import signal
import struct


net_addrs = []
datalist = []
s = ''
def write_data(filepath, l):
    with open(filepath, "a") as f: 
        for elt in l:
            #print(sys.getsizeof(l))
            #var = struct.unpack('ii', elt) #sprintf adds null?
            #var = struct.unpack('I 27s Q 236H',var) #the struct unpack - check its right
            f.write(str(elt))
            f.write('\n')
    return

def server(host, port, store_dir):
    print("Setting up Server...")
    print('')
    timestr = time.strftime("%b-%d-%H:%M", time.localtime())
    file = 'test-'+timestr
    filepath = '/mnt/usb/' + store_dir + '/' +  file + '.txt'
    os.makedirs(os.path.dirname(filepath), exist_ok=True)

    global s
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) #udp
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) #reuse so we dont get addr in use error

    #bind sockets. if not successful, exit program.
    try:
        s.bind((host, port))
        #s.listen(1)
    except socket.error as msg:
        print("ERROR: {}".format(msg))
        s.close()
        s = None
    if s is None:
        sys.exit(1)

    no_echo = True

    count = 0
    print("Listening on: {}:{}".format(host, port))
    #mainloop.
    while True:
        a,b,c = select.select([s], [], [], 0.01)
        if s in a:
            data, address = s.recvfrom(65536)   # This is the max. UDP frame size
            if not data:
                break
            datalist.append(data)
            net_addrs.append(address)
            count += 1
            print(count, len(data))
            if count == 1:
                break
            #print('wrote to file')
            if not no_echo:
                sent = 0;
                while sent < len( data ):
                    sent += s.sendto("echo: "+data[sent:], address)
            #print time.strftime("%b %d %H:%M:%S ", time.localtime()), address[0], ":", repr(data.split("\x00")[0])
    s.close() #from break after if not data
    write_data(filepath, datalist)
            

def client(host, port):
    pass

if __name__ == '__main__':

    server(host='', port=50007, store_dir='TEST')

    logging.basicConfig(level=logging.DEBUG)
    

    serverthread = Thread(target=server, args=('',50007,'TEST'))
    clientthread = Thread(target=client, args=('',50008))

    #serverthread.start()
    #clientthread.start()

    #print('control section')
