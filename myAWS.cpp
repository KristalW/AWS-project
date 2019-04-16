#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define serverA_PORT 21453
#define serverB_PORT 22453
#define serverC_PORT 23453
#define AWS_UDP_PORT  24453
#define AWS_TCPclient_PORT 25453
#define AWS_TCPmonitor_PORT 26453
#define IP_addr	"127.0.0.1"

double Link_ID;
double size;
double power;

double ID_a;
double bandwithe_a;
double length_a;
double velcovity_a;
double noisepower_a;

double ID_b;
double bandwithe_b;
double length_b;
double velcovity_b;
double noisepower_b;

double Tt;
double Tp;
double endtoenddelay;

double FLAG;
double SUCC;
double FAIL;

int  countA;//for the loop

int main(){
	//create the socket between client and AWS upon TCP Port
	int socket_client=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in client_addr;
	client_addr.sin_family = AF_INET;  //IPv4
	client_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address
	client_addr.sin_port = htons(AWS_TCPclient_PORT);  //port number, USC ID last three number:453
	bind(socket_client,(struct sockaddr*)&client_addr,sizeof(client_addr));
	listen(socket_client,10);//listening for incoming connection 

	//create the socket between monitor and AWS upon TCP Port
	int socket_monitor=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in monitor_addr;
	monitor_addr.sin_family = AF_INET;  //IPv4
	monitor_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address,decimal
	monitor_addr.sin_port = htons(AWS_TCPmonitor_PORT);  //port number, USC ID last three number:453
	bind(socket_monitor,(struct sockaddr*)&monitor_addr,sizeof(monitor_addr));
	listen(socket_monitor,10);
	printf("The AWS is up and running.\n");//AWS event#1
	

	int m=0;
	socklen_t client_addrlen, monitor_addrlen;
	client_addrlen=sizeof(client_addr);
	monitor_addrlen=sizeof(monitor_addr);
		
	int socket_clientchild=socket(AF_INET,SOCK_STREAM,0);
	int socket_monitorchild=socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_storage client_Naddr,monitor_Naddr;

	socklen_t client_Naddrlen,monitor_Naddrlen;	
	client_Naddrlen=sizeof(client_Naddr);
	monitor_Naddrlen=sizeof(monitor_Naddr);
	socket_monitorchild=accept(socket_monitor,(struct sockaddr*)&monitor_addr,&monitor_Naddrlen);

	while(1)
	{
		//client and monitor TCP and create the child socket

		
		socket_clientchild=accept(socket_client,(struct sockaddr*)&client_addr,&client_Naddrlen);
		
		//AWS received the information from the client
		recv(socket_clientchild,&Link_ID,sizeof(Link_ID),0);
		recv(socket_clientchild,&size,sizeof(size),0);
		recv(socket_clientchild,&power,sizeof(power),0);
		printf("The AWS received link ID=<%.0lf>,size=<%.2lf>,and power=<%.2lf> from the client using TCP over port <25453>\n", Link_ID,size,power);//AWS event#2

		//AWS send the three parameter to the monitor
		//send(socket_monitorchild,&countA,sizeof(countA),0);// for the loop

		send(socket_monitorchild,&Link_ID,sizeof(Link_ID),0);
		send(socket_monitorchild,&size,sizeof(size),0);
		send(socket_monitorchild,&power,sizeof(power),0);
		printf("The AWS send link ID=<%.0lf>,size=<%.2lf>,and power=<%.2lf> to the monitor using TCP over port <26453>\n", Link_ID,size,power);//AWS event#3


		//begin to search A database 
		struct sockaddr_in a_addr;
		memset (&a_addr, 0, sizeof(a_addr));  //fill with zeros

		a_addr.sin_family = AF_INET;  //IPv4
		a_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address,decimal
		a_addr.sin_port = htons(serverA_PORT);  //port number, USC ID last three number:453
		//send the linkid to server A
		int sock_a = socket(AF_INET, SOCK_DGRAM, 0);
		socklen_t a_addrlen;
		a_addrlen=sizeof(a_addr);
		sendto(sock_a,&Link_ID,sizeof(Link_ID),0,(const struct sockaddr*)&a_addr,sizeof(a_addr));//upon UDP
		printf("The AWS sent link ID=<%.0lf> to Backend-Server <A> using UDP  over port <21453>\n", Link_ID);//AWS event#4-A
		//received
		
		recvfrom(sock_a,&FLAG,sizeof(FLAG),0,(struct sockaddr*)&a_addr,&a_addrlen);
		//printf("%.2lf\n", FLAG);
		if (FLAG!=-1){
			recvfrom(sock_a,&Link_ID,sizeof(Link_ID),0,(struct sockaddr*)&a_addr,&a_addrlen);
			recvfrom(sock_a,&bandwithe_a,sizeof(bandwithe_a),0,(struct sockaddr*)&a_addr,&a_addrlen);
			recvfrom(sock_a,&length_a,sizeof(length_a),0,(struct sockaddr*)&a_addr,&a_addrlen);
			recvfrom(sock_a,&velcovity_a,sizeof(velcovity_a),0,(struct sockaddr*)&a_addr,&a_addrlen);
			recvfrom(sock_a,&noisepower_a,sizeof(noisepower_a),0,(struct sockaddr*)&a_addr,&a_addrlen);
			printf("The AWS received <1> matches from Backend-Server <A> using UDP over port <21453>\n");//AWS event#5-A

			//send the parameter to C
			int sock_c=socket(AF_INET,SOCK_DGRAM,0);
			struct sockaddr_in c_addr;
			c_addr.sin_family = AF_INET;  //IPv4
			c_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address
			c_addr.sin_port = htons(serverC_PORT);  //port number, USC ID last three number:453	
			socklen_t c_addrlen;
			c_addrlen=sizeof(c_addr);

			//send the seven parameter to Server C

			sendto(sock_c,&Link_ID,sizeof(Link_ID),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
			sendto(sock_c,&length_a,sizeof(length_a),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
			sendto(sock_c,&velcovity_a,sizeof(velcovity_a),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
			sendto(sock_c,&power,sizeof(power),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
			sendto(sock_c,&noisepower_a,sizeof(noisepower_a),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
			sendto(sock_c,&bandwithe_a,sizeof(bandwithe_a),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
			sendto(sock_c,&size,sizeof(size),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));	
			//printf("Link_ID,length_a,velcovity_a,power,noisepower_a,bandwithe_a,size%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf,%.2lf\n", Link_ID,length_a,velcovity_a,power,noisepower_a,bandwithe_a,size);
			printf("The AWS sent link ID=<%.0lf>,size=<%.2lf>,power=<%.2lf>, and link information to Backend-Server C using UDP over port <23453>\n", Link_ID,size,power);
			//AWS event#6 

			//AWS received the ouput from Server C
			recvfrom(sock_c,&Tp,sizeof(Tp),0,(struct sockaddr*)&c_addr,&c_addrlen);
			recvfrom(sock_c,&Tt,sizeof(Tt),0,(struct sockaddr*)&c_addr,&c_addrlen);
			recvfrom(sock_c,&endtoenddelay,sizeof(endtoenddelay),0,(struct sockaddr*)&c_addr,&c_addrlen);
			printf("The AWS received outputs from Backend-Server C using UDP over port <23453>\n");//AWS event#7

			//send the delay to the client
			m=1;
			send(socket_clientchild,&m,sizeof(m),0);
			send(socket_clientchild,&endtoenddelay,sizeof(endtoenddelay),0);
			printf("The AWS sent delay=<%.2lf>ms to the client using TCP over port <25453>\n", endtoenddelay);//AWS event#8
			//printf("%d,%.2lf\n", m,endtoenddelay);

			//send the results to the monitor
			m=1;
			send(socket_monitorchild,&m,sizeof(m),0); //Flag

			send(socket_monitorchild,&Tt,sizeof(Tt),0);
			send(socket_monitorchild,&Tp,sizeof(Tp),0);
			send(socket_monitorchild,&endtoenddelay,sizeof(endtoenddelay),0);
			printf("The AWS sent detailed results to the monitor using TCP over port <26453>\n");//AWS event#9
			//printf("%d,%0.2lf,%.2lf,%0.2lf\n", m,Tt,Tp,endtoenddelay);
			countA=countA+1;

		}
		else{
			//begin to search B database 
			printf("The AWS received <0> matches from Backend-Server <A> using UDP over port <21453>\n");//AWS event#5 A has no match

			struct sockaddr_in b_addr;
			memset (&b_addr, 0, sizeof(b_addr));  //fill with zeros
			b_addr.sin_family = AF_INET;  //IPv4
			b_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address,decimal
			b_addr.sin_port = htons(serverB_PORT);  //port number, USC ID last three number:453

			//send the linkid to server B
			int sock_b = socket(AF_INET, SOCK_DGRAM, 0);
			socklen_t b_addrlen;
			b_addrlen=sizeof(b_addr);
			sendto(sock_b,&Link_ID,sizeof(Link_ID),0,(const struct sockaddr*)&b_addr,sizeof(b_addr));//upon UDP
			printf("The AWS sent link ID=<%.0lf> to Backend-Server <B> using UDP  over port <22453>\n", Link_ID);//AWS event#4-B
			//received 
			
			recvfrom(sock_b,&FLAG,sizeof(FLAG),0,(struct sockaddr*)&b_addr,&b_addrlen);
			//printf("%.0lf\n", FLAG);
			if (FLAG!=-1){
				recvfrom(sock_b,&Link_ID,sizeof(Link_ID),0,(struct sockaddr*)&b_addr,&b_addrlen);
				recvfrom(sock_b,&bandwithe_b,sizeof(bandwithe_a),0,(struct sockaddr*)&b_addr,&b_addrlen);
				recvfrom(sock_b,&length_b,sizeof(length_b),0,(struct sockaddr*)&b_addr,&b_addrlen);
				recvfrom(sock_b,&velcovity_b,sizeof(velcovity_b),0,(struct sockaddr*)&b_addr,&b_addrlen);
				recvfrom(sock_b,&noisepower_b,sizeof(noisepower_b),0,(struct sockaddr*)&b_addr,&b_addrlen);
				printf("The AWS received <1> matches from Backend-Server <B> using UDP over port <22453>\n");//AWS event#5-B

				//send the parameter to C
				int sock_c=socket(AF_INET,SOCK_DGRAM,0);
				struct sockaddr_in c_addr;
				c_addr.sin_family = AF_INET;  //IPv4
				c_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address
				c_addr.sin_port = htons(serverC_PORT);  //port number, USC ID last three number:453	
				socklen_t c_addrlen;
				c_addrlen=sizeof(c_addr);

				//send the seven parameter to Server C
				sendto(sock_c,&Link_ID,sizeof(Link_ID),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
				sendto(sock_c,&length_b,sizeof(length_b),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
				sendto(sock_c,&velcovity_b,sizeof(velcovity_b),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
				sendto(sock_c,&power,sizeof(power),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
				sendto(sock_c,&noisepower_b,sizeof(noisepower_b),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
				sendto(sock_c,&bandwithe_b,sizeof(bandwithe_b),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));
				sendto(sock_c,&size,sizeof(size),0,(const struct sockaddr*)&c_addr,sizeof(c_addr));	
				printf("The AWS sent link ID=<%.0lf>,size=<%.2lf>,power=<%.2lf>, and link information to Backend-Server C using UDP over port <23453>\n", Link_ID,size,power);
				//AWS event#6 

				//AWS received the ouput from Server C
				recvfrom(sock_c,&Tp,sizeof(Tp),0,(struct sockaddr*)&c_addr,&c_addrlen);
				recvfrom(sock_c,&Tt,sizeof(Tt),0,(struct sockaddr*)&c_addr,&c_addrlen);
				recvfrom(sock_c,&endtoenddelay,sizeof(endtoenddelay),0,(struct sockaddr*)&c_addr,&c_addrlen);
				printf("The AWS received outputs from Backend-Server C using UDP over port <23453>\n");//AWS event#7	

				//send the delay to the client
				m=1;
				send(socket_clientchild,&m,sizeof(m),0);
				send(socket_clientchild,&endtoenddelay,sizeof(endtoenddelay),0);
				printf("The AWS sent delay=<%.2lf>ms to the client using TCP over port <25453>\n", endtoenddelay);//AWS event#8

				//send the results to the monitor
				m=1;
				send(socket_monitorchild,&m,sizeof(m),0);//Flag

				send(socket_monitorchild,&Tt,sizeof(Tt),0);
				send(socket_monitorchild,&Tp,sizeof(Tp),0);
				send(socket_monitorchild,&endtoenddelay,sizeof(endtoenddelay),0);
				printf("The AWS sent detailed results to the monitor using TCP over port <26453>\n");//AWS event#9
				//printf("%d,%0.2lf,%.2lf,%0.2lf\n", m,Tt,Tp,endtoenddelay);
				countA=countA+1;

			}
			else{
				m=0;
				send(socket_clientchild,&m,sizeof(m),0);
				send(socket_monitorchild,&m,sizeof(m),0);
				printf("The AWS received <0> matches from Backend-Server <B> using UDP over port <22453>\n");//AWS event#5 B has no match
				printf("The AWS sent No Match to the monitor and the client using TCP over ports <21453> and <22453>, respectively\n");//AWS event#10 NO MATCH

				//countA=countA+1;// judge the time
				//NEED A JUMP TO THE BEGIN//

			}
		}
		//close(socket_clientchild);
		//close(socket_monitorchild);
	}


}
