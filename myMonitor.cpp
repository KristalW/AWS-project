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
double Tt;
double Tp;
double endtoenddelay;
int m;
int countC=0;
int countA;

int main(void){
	//match server address and port
	struct sockaddr_in Monitor_addr;
	memset (&Monitor_addr, 0, sizeof(Monitor_addr));  //fill with zeros
	Monitor_addr.sin_family = AF_INET;  //IPv4
	Monitor_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address,decimal
	Monitor_addr.sin_port = htons(AWS_TCPmonitor_PORT);  //port number, USC ID last three number:453

	//Create TCP socket at Monitor
	int sock_Monitor = socket(AF_INET, SOCK_STREAM, 0);//
	if (sock_Monitor == -1)
	{
		printf("error in creating socket: %d\n", sock_Monitor);
		exit(1);
	}
	//Connect
	int status = connect(sock_Monitor, (struct sockaddr*)&Monitor_addr, sizeof(Monitor_addr));
	if (status != 0)
	{
		printf("error in connecting socket: %d\n", status);
		exit(1);
	}
	printf("The Monitor is up and running.\n");
	//Monitor boot up 

	while(1)
	{
		//recv(sock_Monitor,&countA,sizeof(countA),0);
		//if (sock_Monitor)//get the information for AWS
		//if (countA==countC)
		//{

			recv(sock_Monitor,&Link_ID,sizeof(Link_ID),0);
			recv(sock_Monitor,&size,sizeof(size),0);
			recv(sock_Monitor,&power,sizeof(power),0);
			recv(sock_Monitor,&m,sizeof(m),0);

			printf("The monitor received link ID=<%.0lf>, size=<%.2lf>, and power=<%.2lf> from the AWS\n", Link_ID,size,power);
			//printf("%d\n", m);
			
			if (m==1)
			{
				recv(sock_Monitor,&Tt,sizeof(Tt),0);
				recv(sock_Monitor,&Tp,sizeof(Tp),0);
				recv(sock_Monitor,&endtoenddelay,sizeof(endtoenddelay),0);
				printf("The result for link <%.0lf>:\n Tt=<%.2lf>ms,\n Tp=<%.2lf>ms,\n Delay=<%.2lf>ms\n", Link_ID,Tt,Tp,endtoenddelay);
				//printf("%.2lf,%.2lf,%.2lf,%.2lf\n", Link_ID,Tt,Tp,endtoenddelay);
			}
			else
			{
				printf("Found no matches for link <%.0lf>\n", Link_ID);
			}
			//break;
			//countC=countC+1;
		//}
	}
	close(sock_Monitor);
}