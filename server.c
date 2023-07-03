#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

int gfd;

//struct flock is a predefined structure used to describe file locks for file operations
struct flock lock,lock2;
struct password_change
{
	char oldpassword[1024];
	char newpassword[1024];
};
struct passbook
{
	char data[5000];
	

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
	}logintime;
struct modify{
		char oldusername[1024];
		char newusername[1024];
		char password[1024];
		char type;
		int update;
	};
void lockjoint(struct customer joint)
{
	int size;
	lseek(gfd,0,SEEK_SET);
	struct customer r;
	char res='N';
	while((size=read(gfd,&r,sizeof(r)))!=0)
	{
		if(joint.accnum==r.accnum&&strcmp(joint.username,r.username)!=0)
		{
			write(0,"joint2\n",6);
			lseek(gfd,-1*sizeof(r),SEEK_CUR);
					
					
					lock2.l_type=F_WRLCK;
					lock2.l_whence=SEEK_CUR;
					lock2.l_start=0;
					lock2.l_len=sizeof(r);
					lock2.l_pid=getpid();
					fcntl(gfd,F_SETLK,&lock2);
					break;
		}
	}	

}
char login(struct customer request,int login)
{
	// Open file in read-write mode
	gfd=open("customer",O_RDWR,0744);
	if(login==1)
	{
		int size;
		// Moving to file begining
		lseek(gfd,0,SEEK_SET);
		struct customer r;
		char ans='I';
		int i;
		// Find user and return its type
		while((size=read(gfd,&r,sizeof(r)))!=0)
		{
			if(strcmp(request.username,r.username)==0)
			{
				if(strcmp(request.password,r.password)==0)
				{
					// copying r in customer data structure
					logintime=r;
					// Moving to begining of record r
					lseek(gfd,-1*sizeof(r),SEEK_CUR);

					lock.l_type=F_WRLCK; // Write lock
					lock.l_whence=SEEK_CUR; // Position
					lock.l_start=0; // Lock start with ref to l_wence
					lock.l_len=sizeof(r); // Length of lock region 
					lock.l_pid=getpid();  // pid of process holding lock
					write(0,"before\n",8);
					// Apply lock
					i=fcntl(gfd,F_SETLK,&lock); 
					if(i==-1)
					{
						write(0,"locked\n",8);
						ans='L';
						perror("error:");
						break;
					}
					// If joint account then lock other account as well
					if(r.type=='j')
					{
						write(0,"rtype\n",6);
						lockjoint(r);	
					}
					write(0,"Inside\n",8);
					ans=r.type;
					break;
				}
			}
		}
		return ans;
	}
	else
	{
		fcntl(gfd,F_UNLCK,&lock);
		close(gfd);
		return 's';
	}
}
struct customer delete(struct customer del)
{
	int fd=open("customer",O_RDWR,0744);
	int fd2=open("copy",O_CREAT|O_RDWR,0744);
	int size;
	lseek(fd,0,SEEK_SET);
	struct customer r;
	char res='N';
	int delt=0;
	while((size=read(fd,&r,sizeof(r)))!=0)
	{
		if(strcmp(del.username,r.username)==0)
		{
			delt=1;
			continue;
		}
		write(fd2,&r,sizeof(r));
	}
	close(fd);
	close(fd2);
	remove("customer");
	rename("copy","customer");
	if(delt==0)
	{
		strcpy(del.username,"Not found");
	}
	else
	{
		strcpy(del.username,"found");
	}
	return del;
}
struct customer search(struct customer ser)
{
	int fd=open("customer",O_RDWR,0744);
	int size;
	lseek(fd,0,SEEK_SET);
	struct customer r;
	char res='N';
	while((size=read(fd,&r,sizeof(r)))!=0)
	{
		if(strcmp(ser.username,r.username)==0)
		{
			return r;	
		}
	}
	strcpy(ser.username,"Not found");
	return ser;
}
char add(struct customer add_req)
{
	int fd=open("customer",O_RDWR,0744);
	int size;
	lseek(fd,0,SEEK_SET);
	struct customer r;
	char res='N';
	while((size=read(fd,&r,sizeof(r)))!=0)
	{
		if(strcmp(add_req.username,r.username)==0)
		{
			res='E';	
			break;
		}
	}
	if(res=='E')
	{
		return res;
	}
	
	struct acc_no a;
	int fd2=open("acc_no",O_CREAT|O_RDWR,0744);
	read(fd2,&a,sizeof(a));
	add_req.accnum=++a.accnum;
	lseek(fd2,0,SEEK_SET);
	write(fd2,&a,sizeof(a));
	
	lseek(fd,0,SEEK_END);
	write(fd,&add_req,sizeof(add_req));
	close(fd);
	close(fd2);
	login(add_req,1);
	return 'S';
}	

void addafterop(struct customer add_req)
{
	int fd=open("customer",O_RDWR,0744);	
	lseek(fd,0,SEEK_END);
	write(fd,&add_req,sizeof(add_req));
	close(fd);
}


struct modify modification(struct modify m)
{
	int fd=open("customer",O_RDWR,0744);
	int size,size1;
	lseek(fd,0,SEEK_SET);
	struct customer r;
	int found=0;
	struct customer updated;
	struct customer delet;
	while((size=read(fd,&r,sizeof(r)))!=0)
	{
		if(strcmp(m.oldusername,r.username)==0)
		{
			found=1;
			updated=r;
			delet=r;
			switch(m.update)
			{
				case 1:
					
					lseek(fd,0,SEEK_SET);
					struct customer r1;
					while((size1=read(fd,&r1,sizeof(r1)))!=0)
					{
						if(strcmp(m.newusername,r1.username)==0)
						{
							strcpy(m.oldusername,"exist");
							return m;
						}
					}
					strcpy(updated.username,m.newusername);
					break;
				case 2:
					strcpy(updated.password,m.password);
					break;
				case 3:
					updated.type=m.type;
					break;
					
			}
			delete(delet);
			addafterop(updated);
			strcpy(m.oldusername,"ok");
			return m;
			
		}
	}
	if(found==0)
	{
		strcpy(m.oldusername,"Not found");
	}
	return m;
}

void writejoint2(struct customer joint)
{
	int fd=open("customer",O_RDWR,0744);
	int size;
	lseek(fd,0,SEEK_SET);
	struct customer r;
	char res='N';
	while((size=read(fd,&r,sizeof(r)))!=0)
	{
		if(joint.accnum==r.accnum&&strcmp(joint.username,r.username)!=0)
		{
			r.balance=joint.balance;
			delete(r);
			addafterop(r);
			break;
		}
	}
}

void updatepassbook(struct customer data)
{
	char buf[100],path[100];
	sprintf(path,"passbook/%lld",data.accnum);
	time_t t;
	time(&t);
	int len=sprintf(buf,"%s Balance=%f\n",ctime(&t),data.balance);
	int fd=open(path,O_CREAT|O_RDWR,0744);
	lseek(fd,0,SEEK_END);
	write(fd,buf,len);
	close(fd);
}

struct customer bank(struct customer data,int op,struct password_change pass)
{	double nbalance;
	switch(op)
	{
		case 1:
			nbalance=logintime.balance+data.balance;
			logintime.balance=nbalance;
			data.balance=nbalance;
			delete(logintime);
			addafterop(logintime);
			if(logintime.type=='j')
			{
				writejoint2(logintime);
			}
			updatepassbook(logintime);
			login(logintime,1);
			strcpy(data.username,"success");
			break;
		case 2:
			if(logintime.balance>=data.balance)
			{
				nbalance = logintime.balance - data.balance;
				logintime.balance = nbalance;
				data.balance = nbalance;
				delete(logintime);
				addafterop(logintime);
				if(logintime.type=='j')
				{
					writejoint2(logintime);
				}
				updatepassbook(logintime);
				login(logintime,1);
				strcpy(data.username,"success");
			}
			else{
				strcpy(data.username,"notenough");
			}
			break;
		case 3:
			data.balance=logintime.balance;
			strcpy(data.username,"success");
			break;
		case 4:
			if(strcmp(logintime.password,pass.oldpassword)==0)
			{
				strcpy(logintime.password,pass.newpassword);
				delete(logintime);
				addafterop(logintime);
				login(logintime,1);
				strcpy(data.username,"success");
			}
			else
			{
				strcpy(data.username,"notmatch");
			}
			break;
			
		
	}
	return data;

}
char jointadd(struct customer new,struct customer newj)
{
	int fd=open("customer",O_RDWR,0744);
	int size;
	int i=2;
	struct customer test=new;
	while(i-->0)
	{
		lseek(fd,0,SEEK_SET);
		struct customer r;
		char res='N';
		while((size=read(fd,&r,sizeof(r)))!=0)
		{
			if(strcmp(test.username,r.username)==0)
			{
				res='E';
				return res;	
			}
		}
		test=newj;
	}
	
	struct acc_no a;
	int fd2=open("acc_no",O_CREAT|O_RDWR,0744);
	read(fd2,&a,sizeof(a));
	new.accnum=++a.accnum;
	newj.accnum=new.accnum;
	lseek(fd2,0,SEEK_SET);
	write(fd2,&a,sizeof(a));
	
	lseek(fd,0,SEEK_END);
	write(fd,&new,sizeof(new));
	write(fd,&newj,sizeof(newj));
	close(fd);
	close(fd2);
	return 'S';
}

int main()
{
	
	// struct sockaddr_in is a C data structure used to represent an IPv4 socket address for server and client
	struct sockaddr_in serv, cli;
	mkdir("passbook",0777);
	// struct sockaddr_in is a C data structure used to represent an IPv4 socket address.
	int sd,nsd,size_client; 
	// creating a new socket IPV4 domain communication , TCP type socket , 0 
	sd= socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
        	perror("socket");
        	return 1;
    	}
	// Represents IPV4 address family
	serv.sin_family = AF_INET;
	// Binding the socket to INADDR_ANY means that the server will listen on all available network interfaces of the machine.
	serv.sin_addr.s_addr = INADDR_ANY;
	// Sets the port number for the server socket.  The htons function is used to convert the port number from host byte order to network byte order, which is required when working with networking functions.
	serv.sin_port = htons(7940);
	// associate a socket with a specific local address and port
	int bind_result=bind(sd, (void *)(&serv), sizeof(serv));
	if (bind_result == -1) {
        perror("bind");
        close(sd);
        return 1;
    }
	// To accept incoming connections from client sockets and 5 pending connections that can be queued up for this listening socket.
	int listen_result=listen (sd, 5);
	 if (listen_result == -1) {
        perror("listen");
        close(sd);
        return 1;
    }
	printf("Server Initializing...\nSocket returns :: %d\nBind result:: %d\nListening status %d\n",sd,bind_result,listen_result);
	while(1) {
	int sz=sizeof(cli);
	// accepting an incoming connection on a listening socket
	nsd =accept (sd, (void *)&cli,&sz);
	printf("%d",nsd);
	if (!fork( )) {
		   while(1)
		   {
				// in child process closes the original listening socket and starts processing the client's communication through its own copy of the socket.
			   close(sd);
			   write(0,"start\n",7);
			   struct customer request;
			   char c;
			   int size=read(nsd,&request,sizeof(request));
			   c=login(request,1);
			   login(request,1);	
			   if(c=='a')
			   {
			   	
			   		write(0,"adminstart\n",12);
			   		write(nsd,"admin",6);
			   		struct customer new;
			   		int in=0,newsize;
			   		while(in==0)
			   		{
				   		char cmd[10];
				   		read(nsd,&cmd,10);
						if(strcmp(cmd,"add")==0)
				   		{
					   		int j=1;
					   		char c;
					   		struct customer newj;
					   		while(1)
					   		{
					   		while((newsize=read(nsd,&new,sizeof(new)))==0);
					   		if(new.type=='j')
					   		{
					   			read(nsd,&newj,sizeof(newj));
					   			c=jointadd(new,newj);
					   		}
					   		else
					   		{
					   			c=add(new);
					   		}
					   	
						   		switch(c)
						   		{
						   			case 'E':
						   				write(nsd,"exist",6);
						   				break;
						   			case 'S':
						   				write(nsd,"ok",3);
										break;
						   				break;
						   			default:
						   				write(nsd,"default",7);
						   				break;
						   				break;
						   		}
						   	}
					   	}
					   	else if(strcmp(cmd,"mod")==0)
					   	{
					   		struct modify m;
					   		read(nsd,&m,sizeof(m));
					   		m=modification(m);
					   		write(nsd,&m,sizeof(m));
					   	
					   	}
					   	else if(strcmp(cmd,"srh")==0)
					   	{
					   		struct customer ser;
					   		read(nsd,&ser,sizeof(ser));
					   		ser=search(ser);
					   		write(nsd,&ser,sizeof(ser));
					   	
					   	}
					   	else if(strcmp(cmd,"del")==0)
					   	{
					   		struct customer del;
					   		read(nsd,&del,sizeof(del));
					   		del=delete(del);
					   		write(nsd,&del,sizeof(del));
					   	}
					   	else if(strcmp(cmd,"out")==0)
					   	{
					   		in=1;
					   	}
					   	
			   		}
			   	}
			   	else if(c=='n'||c=='j')
			   	{
			   		write(0,"normalstart\n",14);
			   		if(c=='j')
			   		{
			   			write(nsd,"joint",6);
			   		}
			   		else
			   		{
			   			write(nsd,"normal",6);
			   		}
			   		char cmd[10];
			   		int in_n=1;
			   		
			   		while(in_n==1)
			   		{
			   			read(nsd,&cmd,10);
			   			struct password_change dummy;
			   			if(strcmp(cmd,"dep")==0)
			   			{
			   				struct customer dep;
			   				read(nsd,&dep,sizeof(dep));
			   				dep=bank(dep,1,dummy);
			   				write(nsd,&dep,sizeof(dep));
			   			}
			   			else if(strcmp(cmd,"wit")==0)
			   			{
			   				struct customer wit;
			   				read(nsd,&wit,sizeof(wit));
			   				wit=bank(wit,2,dummy);
			   				write(nsd,&wit,sizeof(wit));
			   			}
			   			else if(strcmp(cmd,"bal")==0)
			   			{
			   				struct customer bal;
			   				bal=bank(bal,3,dummy);
			   				write(nsd,&bal,sizeof(bal));
			   			}
			   			else if(strcmp(cmd,"pas")==0)
			   			{
			   				struct password_change pass;
			   				struct customer res;
			   				read(nsd,&pass,sizeof(pass));
			   				res=bank(res,4,pass);
			   				write(nsd,&res,sizeof(res));
			   				
			   			}
			   			else if(strcmp(cmd,"vie")==0)
			   			{
			   				
							char path[100],buf[10000];
							sprintf(path,"passbook/%lld",logintime.accnum);
							int fd=open(path,O_CREAT|O_RDONLY,0744);
							int len=read(fd,buf,10000);
							if(len==-1||len==0)
							{
								write(nsd,"empty",6);	
							}
							else
							{
								write(nsd,buf,len);
							}
							close(fd);
							
			   				
			   			}
			   			else if(strcmp(cmd,"out")==0)
			   			{
			   				in_n=0;
			   			}
			   			
			   		}
			   		write(0,"normalend\n",10);			   	}
			   	else if(c=='I')
			   	{
			   		write(nsd,"Invalid",7);
			   	}	
			   	else if(c=='L')
			   	{
			   	
			   		write(nsd,"logged",7);
			   	}
			   	else
			   	{
			   		write(nsd,"default\n",6);
			   	}
			   	struct customer dumy;
			   	login(dumy,0);
			   }		   
	}
	else{
	 	close(nsd);
	    }
	   
	}
}
