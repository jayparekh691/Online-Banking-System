// MT2022052 Jay Parekh
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
struct password_change
{
	char oldpassword[1024];
	char newpassword[1024];
};

struct acc_no
	{
		long long int accnum;
	};
	struct customer{
		char username[1024];
		char password[1024];
		long long int accnum;
		double balance;
		char type;
	}user;

// Add user
void addnew(int sd){
	struct customer add1,add2;
	struct acc_no a;
	int fd=open("customer",O_CREAT|O_RDWR,0744);
	printf("\nEnter Username (Unique) : ");
	scanf("%s",add1.username);
	printf("\nEnter password:");
	scanf("%s",add1.password);
	printf("\nEnter Customer type (a:admin n:normal j:join) : ");
	scanf(" %c",&add1.type);
	add1.balance=0.0;
	if(add1.type=='j'){
		printf("\nEnter username:");
		scanf("%s",add2.username);
		printf("\nEnter password:");
		scanf("%s",add2.password);
		printf("\nEnter Customer type (a:admin n:normal j:join):");
		scanf(" %c",&add2.type);
		add2.balance=0.0;
		write(sd,&add2,sizeof(add2));	
	}
	write(sd,&add1,sizeof(add1));
	struct customer ans;
	read(sd,&ans,sizeof(ans));
	if(strcmp(ans.username,"ok")==0)
	{
 		printf("\n\nYour Information is modified successfully.\n\n");
	}
	else if(strcmp(ans.username,"exist")==0)
	{
		printf("\n\nUsername exist. please use diffrenet username\n\n");
	}
	else if(strcmp(ans.username,"default")==0)
	{
		printf("\n\nUsername not found :(\n\n");
	}
}

// Customer modification function for customer data 
struct customer modification(struct customer data,int sd)
{
	struct modify{
		char oldusername[1024];
		char newusername[1024];
		char password[1024];
		char type;
		int update;
	};
	struct modify new;
	int c=1;
	while(c==1)
	{
	printf("\nChoose What Wants To Update\n");
	printf("1.Update UserName\n");
	printf("2.Password\n");
	printf("3.Usertype\n");
	int choice;
	scanf("%d",&choice);
	
	
	printf("\nPlease Enter following details\n");
	printf("\nEnter original username(UNIQUE):");
	scanf("%s",new.oldusername);
	switch(choice)
	{
		case 1:
			printf("\nEnter new username(UNIQUE):");
			scanf("%s",new.newusername);
			new.update=1;
			c=0;
			break;	
		case 2:
			printf("\nEnter new password:");
			scanf("%s",new.password);
			new.update=2;
			c=0;
			break;	
		case 3:
			printf("\nEnter usertype(n:normal a:admin): ");
			scanf(" %c",&new.type);
			new.update=3;
			c=0;
			break;	
		default:
			printf("\nInvalid choice");
			break;
	
	}
	}
	write(sd,&new,sizeof(new));
	struct modify ans;
	read(sd,&ans,sizeof(ans));
	if(strcmp(ans.oldusername,"ok")==0)
	{
 		printf("\n\nYour Information is modified successfully.\n\n");
	}
	else if(strcmp(ans.oldusername,"exist")==0)
	{
		printf("\n\nUsername exist. please use diffrenet username\n\n");
	}
	else if(strcmp(ans.oldusername,"Not found")==0)
	{
		printf("\n\nUsername not found :(\n\n");
	}
}

int main()
{
	// struct sockaddr_in is a C data structure used to represent an IPv4 socket address.
	struct sockaddr_in server; 
	// IPV4 domain communication , TCP type socket , 0 
	int sd = socket (AF_INET, SOCK_STREAM, 0); 
	if (sd == -1) {
        	perror("socket");
        	return 1;
    	}
	// Represents IPV4 address family
	server.sin_family = AF_INET; 
	// Binding the socket to INADDR_ANY means that the server will listen on all available network interfaces of the machine.
	server.sin_addr.s_addr = INADDR_ANY;  
	// This line sets the port number for the server socket.  The htons function is used to convert the port number from host byte order to network byte order, which is required when working with 	networking functions.
	server.sin_port = htons(7940); 
	/* connect system call itself. It is used to establish a connection to a server using the socket descriptor sd. The connect function takes three arguments:
		sd: The socket descriptor representing the client socket that wants to connect to the server.
		The connect function tries to establish a connection to the server specified by the server address. If the connection is successful, the client socket (sd) will be connected to the server, 			allowing data exchange between them. If the connection fails, the value of connect_result will be set to -1, and you can check the specific error using errno to handle any potential 			connection issues.
*/
	int connect_result=connect (sd, (void *)(&server), sizeof(server));
	char ch='Y';
	while(ch=='Y'){
		char name[1024],pass[1024];
		printf("\nEnter useranme:");
		scanf("%s",user.username);
		printf("Enter Password:");
		scanf("%s",user.password);
		write(sd,&user,sizeof(user));
		int size;
		char buf[100];
		read(sd,buf,100);
		if(strcmp(buf,"admin")==0)
		{
			printf("\n\n\t\tAdmin LOGIN");
			bool inside=1;
			while(inside==1)
			{
				printf("\nChoose Option:\n");
				printf("1.Add New Account\n2.Modify Account\n3.Search an Account\n4.Delete Account\n5.Quit\n");
				int choice;
				scanf("%d",&choice);
				switch(choice)
				{
					case 1:
						write(sd,"add",4);
						addnew(sd);
						break;
					case 2:
						write(sd,"mod",4);
						struct customer mod;
						mod=modification(mod,sd);
						break;
					case 3:
						write(sd,"srh",4);
						struct customer ser;
						printf("Please Enter following details\n");
						printf("\nEnter username : ");
						scanf("%s",ser.username);
						write(sd,&ser,sizeof(ser));
						int ssize;
						while((ssize=read(sd,&ser,sizeof(ser)))==0);
						printf("%s",ser.username);
						if(strcmp(ser.username,"Not found")==0)
						{
							printf("\n\nUsername does not Exist :(\n\n");
						}
						else
						{
							printf("\nUser information:\n");
							printf("\tUsername:%s\n",ser.username);
							printf("\tAccount no:%lld\n",ser.accnum);
							printf("\tBalance:%f\n",ser.balance);
							printf("\tUser type(j:joint,n:normal,a:admin) %c",ser.type);
						}
						break;
					case 4:
						write(sd,"del",4);
						struct customer del;
						printf("\nPlease Enter following details\n");
						printf("\nEnter username to be deleted:");
						scanf("%s",del.username);
						write(sd,&del,sizeof(del));
						int dsize;
						while((dsize=read(sd,&del,sizeof(del)))==0);
						if(strcmp(del.username,"Not found")==0)
						{
							printf("\n\nUsername does not Exist :(\n\n");
						}
						else
						{
							printf("\n\nAccount deleted successfully :)\n\n");
						}
						
						break;
					case 5:	
							write(sd,"out",4);
							inside=0;
							break;
					default:
						printf("\n\nInvalid choice\n\n");
						
				}
			}
		}
		else if(strcmp(buf,"normal")==0||strcmp(buf,"joint")==0)
		{
			if(strcmp(buf,"joint")==0)
			{
				printf("\n\n\t\tJoin USER");
				
			int inside=1;
			while(inside==1)
			{
				printf("\nEnter Choice\n");
				printf("1.Deposit\n");
				printf("2.Withdraw\n");
				printf("3.Balance Enquiry\n");
				printf("4.Password Change\n");
				printf("5.View Passbook\n");
				printf("6.Exit\n");
				int choice;
				scanf("%d",&choice);
				switch(choice)
				{
					case 1:
						write(sd,"dep",4);
						struct customer depo,ans;
						printf("Enter Deposit Amount:\n");
						scanf("%lf",&depo.balance);
						printf("%lf",depo.balance);
						write(sd,&depo,sizeof(depo));
						read(sd,&ans,sizeof(ans));
						if(strcmp(ans.username,"unsuccess")==0)
						{
							printf("\n\nTransaction Unsuccessful :(\n\n");
						}
						else if(strcmp(ans.username,"success")==0)
						{
							printf("\n\nTransaction successfull :)\nYour current balance is %f\n\n",ans.balance);
						}
						break;
						
					case 2:
						write(sd,"wit",4);
						struct customer wit,ans1;
						//struct ansponce ans;
						printf("Enter Withdraw Amount:\n");
						scanf("%lf",&wit.balance);
						write(sd,&wit,sizeof(wit));
						read(sd,&ans1,sizeof(ans1));
						if(strcmp(ans1.username,"notenough")==0)
						{
							printf("\n\nNot enough money in your account  :(\n\n");
						}
						else if(strcmp(ans1.username,"success")==0)
						{
							printf("\n\nTransaction successfull :)\nYour current balance is %f\n\n",ans1.balance);
						}
						break;
					case 3:
						write(sd,"bal",4);
						struct customer ans2;
						read(sd,&ans2,sizeof(ans2));
						if(strcmp(ans2.username,"unsuccess")==0)
						{
							printf("\n\nRequest Unsuccessful :(\n\n");
						}
						else if(strcmp(ans2.username,"success")==0)
						{
							printf("\n\nYour current balance is %f\n\n",ans2.balance);
						}
						break;
					case 4:
						write(sd,"pas",4);
						struct password_change ps;
						struct customer ans3;
						printf("\nPlease enter old password:");
						scanf("%s",ps.oldpassword);
						printf("\nPlease enter new password:");
						scanf("%s",ps.newpassword);
						write(sd,&ps,sizeof(ps));
						read(sd,&ans3,sizeof(ans3));
						if(strcmp(ans3.username,"notmatch")==0)
						{
							printf("\n\nYour old password does not match :(\n\n");
						}
						else if(strcmp(ans3.username,"success")==0)
						{
							printf("\n\nYour password is changed successfully :)\n\n");
						}
						break;
					case 5:
						write(sd,"vie",4);
						char buf[10000];
						int len=read(sd,buf,10000);
						if(strcmp(buf,"empty")==0)
						{
							printf("\nYour passbook is empty :(\n\n");
						}
						else
						{
							printf("\n\t\tPassbook");
							printf("\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
							printf("\n%s",buf);
							printf("\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n");
						}
						break;
					case 6:
						write(sd,"out",4);
						inside=0;
						break;
					default:
						printf("\n\nInvalid choice\n\n");
						break;
				}			
			}
			}
			else
			{
				printf("\n\n\t\tNormal USER");	
			int inside=1;
			while(inside==1)
			{
				printf("\nEnter Choice\n");
				printf("1.Deposit\n");
				printf("2.Withdraw\n");
				printf("3.Balance Enquiry\n");
				printf("4.Password Change\n");
				printf("5.View Passbook\n");
				printf("6.Exit\n");
				int choice;
				scanf("%d",&choice);
				switch(choice)
				{
					case 1:
						write(sd,"dep",4);
						struct customer depo,ans;
						printf("Enter Deposit Amount:\n");
						scanf("%lf",&depo.balance);
						printf("%lf",depo.balance);
						write(sd,&depo,sizeof(depo));
						read(sd,&ans,sizeof(ans));
						if(strcmp(ans.username,"unsuccess")==0)
						{
							printf("\n\nTransaction Unsuccessful :(\n\n");
						}
						else if(strcmp(ans.username,"success")==0)
						{
							printf("\n\nTransaction successfull :)\nYour current balance is %f\n\n",ans.balance);
						}
						break;
						
					case 2:
						write(sd,"wit",4);
						struct customer wit,ans1;
						printf("Enter Withdraw Amount:\n");
						scanf("%lf",&wit.balance);
						write(sd,&wit,sizeof(wit));
						read(sd,&ans1,sizeof(ans1));
						if(strcmp(ans1.username,"notenough")==0)
						{
							printf("\n\nNot enough money in your account  :(\n\n");
						}
						else if(strcmp(ans1.username,"success")==0)
						{
							printf("\n\nTransaction successfull :)\nYour current balance is %f\n\n",ans1.balance);
						}
						break;
					case 3:
						write(sd,"bal",4);
						struct customer ans2;
						read(sd,&ans2,sizeof(ans2));
						if(strcmp(ans2.username,"unsuccess")==0)
						{
							printf("\n\nRequest Unsuccessful :(\n\n");
						}
						else if(strcmp(ans2.username,"success")==0)
						{
							printf("\n\nYour current balance is %f\n\n",ans2.balance);
						}
						break;
					case 4:
						write(sd,"pas",4);
						struct password_change ps;
						struct customer ans3;
						printf("\nPlease enter old password:");
						scanf("%s",ps.oldpassword);
						printf("\nPlease enter new password:");
						scanf("%s",ps.newpassword);
						write(sd,&ps,sizeof(ps));
						read(sd,&ans3,sizeof(ans3));
						if(strcmp(ans3.username,"notmatch")==0)
						{
							printf("\n\nYour old password does not match :(\n\n");
						}
						else if(strcmp(ans3.username,"success")==0)
						{
							printf("\n\nYour password is changed successfully :)\n\n");
						}
						break;
					case 5:
						write(sd,"vie",4);
						char buf[10000];
						int len=read(sd,buf,10000);
						if(strcmp(buf,"empty")==0)
						{
							printf("\nYour passbook is empty :(\n\n");
						}
						else
						{
							printf("\n\t\tPassbook");
							printf("\n%s",buf);	
						}
						break;
					case 6:
						write(sd,"out",4);
						inside=0;
						break;
					default:
						printf("\n\nInvalid choice\n\n");
						break;
				}
			
			    }
			}
			
		}
		else if(strcmp(buf,"Invalid")==0)
		{
			printf("\n\nUsername or Password is Invalid. Please try again :)\n\n");
			continue;
		}
		else if(strcmp(buf,"logged")==0)
		{
			printf("\n\nUser is already logged in. Please try later :)\n\n");
			continue;
		}
		else
		{
			printf("\ndefault\n");
		}
	

	printf("\n\nYou are logged out\n\n");
	printf("Do you want to continue(Y/N)?");
	scanf(" %c",&ch);
	if(ch=='y')
		ch='Y';
	}
}

