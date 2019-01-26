all: waspudp wasptcp

waspudp: waspudp.c
	gcc -o waspudp waspudp.c

wasptcp: wasptcp.c
	gcc -o wasptcp wasptcp.c
