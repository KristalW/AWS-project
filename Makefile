###Makefile###

all:
	g++ -o aws myAWS.cpp
	g++ -o serverA myServerA.cpp
	g++ -o serverB myServerB.cpp
	g++ -o serverC myServerC.cpp
	g++ -o monitor myMonitor.cpp
	g++ -o client myClient.cpp

serverA:
	./serverA

serverB:
	./serverB

serverC:
	./serverC

aws:
	./aws

monitor:
	./monitor

.PHONY: serverA serverB serverC aws monitor
