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
#include <math.h>

#define serverA_PORT 21453
#define serverB_PORT 22453
#define serverC_PORT 23453
#define AWS_UDP_PORT  24453
#define IP_addr	"127.0.0.1"

//define the parameter used in calculation
double Link_ID;
double Tp;
double Length;
double Velocity;
double Tt;
double S;
double power;
double N;
double noise;
double SNR;
double C;
double size;
double B;
double endtoenddelay;

int parametercalculate()
{
	Tp=Length/Velocity/10; // propagation delay
	S=pow(10,(power/10)-3);
	N=pow(10,(noise/10)-3);
	SNR=S/N;
	C=1000000*B*(log(1+SNR)/log(2));
	Tt=1000*(size/C);// transmission delay
	endtoenddelay=Tt+Tp;//end to dnd delay
	printf("The Server C finished the calculation for link <%.0lf>.\n",Link_ID);
	//printf("S%lf,N%lf,SNR%lf,C%lf,Tt%lf,endtoenddelay%lf\n", S,N,SNR,C,Tt,endtoenddelay);
}

int main(void){
	//serverA UDP address
	struct sockaddr_in serverC_addr,client_addr;
	memset (&serverC_addr, 0, sizeof(serverC_addr));  //fill with zeros
	memset (&client_addr,0,sizeof(client_addr));

	serverC_addr.sin_family = AF_INET;  //IPv4
	serverC_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address,decimal
	serverC_addr.sin_port = htons(serverC_PORT);  //port number, USC ID last three number:453

	//Create UDP socket at C
	int sock_serverC = socket(AF_INET, SOCK_DGRAM, 0);//
	if (sock_serverC == -1)
	{
		printf("error in creating socket: %d\n", sock_serverC);
		exit(1);
	}
	//Bind
	int status = bind(sock_serverC, (struct sockaddr*)&serverC_addr, sizeof(serverC_addr));
	if (status != 0)
	{
		printf("error in binding socket: %d\n", status);
		exit(1);
	}
	printf("The server C is up and running using UDP on port <23453>.\n");

	while(1){
		socklen_t client_addrlen;
		client_addrlen=sizeof(client_addr);
		recvfrom(sock_serverC,&Link_ID,sizeof(Link_ID),0,(struct sockaddr*)&client_addr,&client_addrlen);
		recvfrom(sock_serverC,&Length,sizeof(Length),0,(struct sockaddr*)&client_addr,&client_addrlen);
		recvfrom(sock_serverC,&Velocity,sizeof(Velocity),0,(struct sockaddr*)&client_addr,&client_addrlen);
		recvfrom(sock_serverC,&power,sizeof(power),0,(struct sockaddr*)&client_addr,&client_addrlen);
		recvfrom(sock_serverC,&noise,sizeof(noise),0,(struct sockaddr*)&client_addr,&client_addrlen);
		recvfrom(sock_serverC,&B,sizeof(B),0,(struct sockaddr*)&client_addr,&client_addrlen);
		recvfrom(sock_serverC,&size,sizeof(size),0,(struct sockaddr*)&client_addr,&client_addrlen);
		printf("The Server C received link information of link <%.0lf>, file size <%.2lf>, and signal power <%.2lf>.\n", Link_ID,size,power);
		//get the parameter already
		parametercalculate();
		sendto(sock_serverC,&Tp,sizeof(Tp),0,(const struct sockaddr*)&client_addr,client_addrlen);
		sendto(sock_serverC,&Tt,sizeof(Tt),0,(const struct sockaddr*)&client_addr,client_addrlen);
		sendto(sock_serverC,&endtoenddelay,sizeof(endtoenddelay),0,(const struct sockaddr*)&client_addr,client_addrlen);
		//get the calculation results already
		
		printf("The Server C finished sending the output to AWS.\n");
		//printf("Length%.2lf,Velocity%.2lf,noise%.2lf,B%.2lf,Link_ID%.0lf,size%.2lf,power%.2lf\n", Length,Velocity,noise,B,Link_ID,size,power);
	}

}