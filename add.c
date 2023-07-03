#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
void main()
{
	struct acc_no
	{
		long long int accnum;
	}a;
	struct customer{
		char username[1024];
		char password[1024];
		long long int accnum;
		double balance;
		char type;
	}add1,add2;
	int fd=open("customer",O_CREAT|O_RDWR,0744);
	printf("\nEnter Username (Unique) : ");
	scanf("%s",add1.username);
	printf("\nEnter password:");
	scanf("%s",add1.password);
	printf("\nEnter Customer type (a:admin n:normal j:join) : ");
	scanf(" %c",&add1.type);
	add1.balance=0.0;
	int fd2=open("acc_no",O_CREAT|O_RDWR,0744);
	read(fd2,&a,sizeof(a));
	int acnum=++a.accnum;
	add1.accnum=acnum;
	lseek(fd2,0,SEEK_SET);
	write(fd2,&a,sizeof(a));
	if(add1.type=='j'){
		printf("\nEnter username:");
		scanf("%s",add2.username);
		printf("\nEnter password:");
		scanf("%s",add2.password);
		printf("\nEnter Customer type (a:admin n:normal j:join):");
		scanf(" %c",&add2.type);
		add2.balance=0.0;
		add2.accnum=acnum;		
		lseek(fd,0,SEEK_END);
		write(fd,&add2,sizeof(add2));
		printf("Successfully added into database.\n");
	}
	lseek(fd,0,SEEK_END);
	write(fd,&add1,sizeof(add1));
	printf("Successfully added into database.\n");		
	return;	
}
