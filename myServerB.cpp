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
#define IP_addr	"127.0.0.1"

double arr[555555][5];
double res[5];
double Link_ID;

int get_data(double clientrequest)
{
	FILE*pFILE;
	pFILE=fopen("./database_b.csv","r");// Open the file and read 
	if(pFILE==NULL){
		perror("ERROR");
		return 0;
	}
	int lineN,i;
	int m=0;
	while(!feof(pFILE)){
		int read=fscanf(pFILE,"%lf,%lf,%lf,%lf,%lf",&arr[lineN][0],&arr[lineN][1],&arr[lineN][2],&arr[lineN][3],&arr[lineN][4]);
		if (read>0)
			++lineN;// Get the total number of the line
	}
	for (i=0;i<lineN;++i)
	{
		if (arr[i][0]==clientrequest) //compare the request and line
		{
			res[0]=arr[i][0];// get the information from the database 
			res[1]=arr[i][1];
			res[2]=arr[i][2];
			res[3]=arr[i][3];
			res[4]=arr[i][4];
			m=1;
		}
	}
	return m;
}
int main()
{
	//serverA UDP address
	struct sockaddr_in serverB_addr,client_addr;
	memset (&serverB_addr, 0, sizeof(serverB_addr));  //fill with zeros
	memset (&client_addr,0,sizeof(client_addr));

	serverB_addr.sin_family = AF_INET;  //IPv4
	serverB_addr.sin_addr.s_addr = inet_addr(IP_addr);  //IP address,decimal
	serverB_addr.sin_port = htons(serverB_PORT);  //port number, USC ID last three number:453

	//Create UDP socket at B
	int sock_serverB = socket(AF_INET, SOCK_DGRAM, 0);//
	if (sock_serverB == -1)
	{
		printf("error in creating socket: %d\n", sock_serverB);
		exit(1);
	}
	//Bind
	int status = bind(sock_serverB, (struct sockaddr*)&serverB_addr, sizeof(serverB_addr));
	if (status != 0)
	{
		printf("error in binding socket: %d\n", status);
		exit(1);
	}
	printf("The Server B is up and running using UDP on port <22453>.\n");

	//Get Link_ID
	while(1)
	{
		socklen_t client_addrlen;

		double client_message=0;

		client_addrlen=sizeof(client_addr);
		//printf("test\n");
		client_message=recvfrom(sock_serverB,&Link_ID,sizeof(Link_ID),0,(struct sockaddr*)&client_addr,&client_addrlen);

		printf("The Server B received input <%.0lf>.\n",Link_ID);

		//get data from database_a
		double FAIL=-1;
		double SUCC=1;
		double result[5];
		if (get_data(Link_ID)){
			int j=0;
			sendto(sock_serverB,&SUCC,sizeof(SUCC),0,(struct sockaddr*)&client_addr,client_addrlen);
			for (0;j<=4;j++){
				result[j]=res[j];
				sendto(sock_serverB,&result[j],sizeof(result[j]),0,(const struct sockaddr*)&client_addr,client_addrlen);// send the information back to client
				
			}
			printf("The Server B has found <1> match.\n");
		}
		else{
			sendto(sock_serverB,&FAIL,sizeof(FAIL),0,(const struct sockaddr*)&client_addr,client_addrlen);
			printf("The Server B has found <0> match.\n");
		}
		printf("The Server B finished sending the output to AWS.\n");

	}
}
