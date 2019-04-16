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
int m;
double endtoenddelay;
char* Link_IDc;
char* sizec;
char* powerc;

int main(int argc, char* argv[])
{

	//AWS TCP address
	struct sockaddr_in AWS_addr;
	memset (&AWS_addr, 0, sizeof(AWS_addr));  //fill with zeros
	AWS_addr.sin_family = AF_INET;  //IPv4
	AWS_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address
	AWS_addr.sin_port = htons(AWS_TCPclient_PORT);  //port number, USC ID last three number:453
	//Create TCP socket at client
	int sock_client = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_client == -1)
	{
		printf("error in creating socket: %d\n", sock_client);
		exit(1);
	}
	//Establish connection
	int status = connect(sock_client, (struct sockaddr*)&AWS_addr, sizeof(AWS_addr));

	if (status == -1)
	{
		printf("error in connection: %d\n", status);
		exit(1);
	}
	printf("The client is up and running\n");

	//get the parameter 
	Link_ID=strtod(argv[1],NULL);
	size=strtod(argv[2],NULL);
	power=strtod(argv[3],NULL);

	printf("The​ ​client​ ​sent​ ​ID=<%.0lf>​, size=<%.2lf>, ​and​ power=<%.2lf> ​to​ ​AWS\n", Link_ID,size,power);
	//send the parameter to the server
	send(sock_client,&Link_ID,sizeof(Link_ID),0);
	send(sock_client,&size,sizeof(size),0);
	send(sock_client,&power,sizeof(power),0);

	recv(sock_client,&m,sizeof(m),0);
	if (m!=0)
	{
		recv(sock_client,&endtoenddelay,sizeof(endtoenddelay),0);
		printf("The delay for link <%.0lf> is <%.2lf>ms\n", Link_ID,endtoenddelay);
	}
	else
	{
		printf("Found no matches for link <%.0lf>\n", Link_ID);
	}

}