/*
** server.c - a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
                                                                      
#define MYPORT 3490           // the port users will be connecting to
#define BACKLOG 10 	      // how many pending connections queue will hold
#define MAXDATASIZE 1020
#define size 50
#define BUFFERSIZE 1024
#define COPYMORE 0644

int copyFiles(char *src, char *dest);

void print_pwd(int new_fd)
{
     char cwd[100];
     int len;
     memset(cwd, 0, sizeof(cwd));
     len=strlen(getcwd(cwd, sizeof(cwd)));
    
     if (send(new_fd, getcwd(cwd, sizeof(cwd)), len, 0) == -1)
       perror("send");    
}

void cd(int new_fd,char *dirnm)
{
    int len;
    char cwd[100];
    memset(cwd, 0, sizeof(cwd));
    if(!chdir(dirnm)) /* chdir returns 0 on success */
    {  
       len=strlen(getcwd(cwd, sizeof(cwd)));
       if (send(new_fd, getcwd(cwd, sizeof(cwd)), len, 0) == -1)
                perror("send"); 
    }
    else
    {
        if (send(new_fd, "ERROR", 5, 0) == -1)
        perror("send"); 
    }
}

void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}



void main1(char *cmm)                               // for ls command
{
    DIR *pdir;
    FILE *fp;
    int flag = 0;
    const char* chars = "open directory failed !";
    fp=fopen("sendfile.txt","w");

    struct dirent *pent;
    struct stat statbuf;

    pdir=opendir(cmm);
    
    if(pdir){ 
    while ((pent=readdir(pdir))!=NULL && flag==0 )
    {
        stat(pent->d_name,&statbuf);
       
        if(strcmp(".", pent->d_name) == 0 || strcmp("..", pent->d_name) == 0)
        continue;
       
        if(S_ISDIR(statbuf.st_mode))
        {
           fprintf(fp,"%s   ",pent->d_name);
        }
     
        if(!(S_ISDIR(statbuf.st_mode)))
        {
           fprintf(fp,"%s   ",pent->d_name);
        }
    } }
    else
    {
        fprintf(fp,"%s ",chars);
    }
    printf("\n");
    fclose(fp);
    closedir(pdir);
}

void get(int sockfd, int new_fd, char *s22)             // for get command             
{
     int i,j=0,s1=0,s2=0,s3=0,len,numbytes;
     int in_fd, out_fd, n_chars;
     char buf[BUFFERSIZE];
     
                 /* open files */
        if( (in_fd=open(s22, O_RDONLY)) == -1 )
	{
             if (send(new_fd, "ERROR", 5, 0) == -1)
                perror("send");
             printf("Exec and Send...\n");
	}
        else
        {  
            if (send(new_fd, "SUCCES", 6, 0) == -1)
                perror("send");    
             
            if ((numbytes=recv(new_fd, buf,sizeof(buf), 0)) == -1)
            {
                   perror("recv");
                   exit(1);
            }    
            buf[numbytes]='\0';
            printf("File sending...");            
            if(strcmp(buf,"#")==0)
            {  			          /* copy files */
               while( (n_chars = read(in_fd, buf, BUFFERSIZE)) > 0 )
               {  
                  if( send(new_fd, buf, n_chars, 0) != n_chars )
                  {
                     perror("send");
                  }
 
                  if ((numbytes=recv(new_fd, buf,sizeof(buf), 0)) == -1)
                  {
                      perror("recv");
                      exit(1);
                  }     
               }
               if (send(new_fd,"$$$$\0",5, 0) == -1)
               perror("send");
               printf("\n\n%s file succesfuly Send...\n",s22);
            }
            else
            {
               printf("%s file is not sended...\n",s22); 
            }
        }      
}                       

void put(int new_fd, char *s22)
{
    int out_fd,numbytes;   
     char buf[BUFFERSIZE]; 
 
    if( (out_fd=creat(s22, COPYMORE)) == -1 )
    {
       if (send(new_fd,"ERROR",5, 0) == -1)
       perror("send");
    }
    else
    {     
        if (send(new_fd,"SUCCES",6, 0) == -1)
        perror("send");
                    
        while(1)
        {   
           if ((numbytes=recv(new_fd, buf,sizeof(buf), 0)) == -1)
           {
               perror("recv");
               exit(1);
           }    
           if( strcmp(buf,"$$$$") ==0)
           {
               break;
           }
           if( write(out_fd, buf, numbytes) != numbytes )
           {
               printf("ERROR OCCURE !!\n");
               break;
           }    
           if (send(new_fd,"##",2, 0) == -1)
           perror("send");
        }
        printf("\n\n%s File succesfuly recived...\n",s22);   
    }
}     

int main(void)
{
      int i,j=0,s1=0,s2=0,s3=0,len,call=1,N=1;
      char s[size],s11[size],s22[size],s33[size];
      
      char buf[MAXDATASIZE],ch[100];
      int sockfd, new_fd, numbytes; 				// listen on sock_fd, new connection on new_fd
      FILE *fp;

      struct sockaddr_in my_addr;                                       // my address information
      struct sockaddr_in their_addr;                                    // connector’s address information
      
      int sin_size;
      struct sigaction sa;
      int yes=1;
      
      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
      {
        perror("socket");
        exit(1);
      }
      
      if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
      {
        perror("setsockopt");
        exit(1);
      }
      
      my_addr.sin_family = AF_INET;
      my_addr.sin_port = htons(MYPORT);
      my_addr.sin_addr.s_addr = INADDR_ANY;
      memset(&(my_addr.sin_zero), '\0', 8);
                                                     /* host byte order short, network byte order
					                automatically fill with my IP zero the rest of the struct  */
     
     if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1) 
     {
          perror("bind");
          exit(1);
     }
    
     if (listen(sockfd, BACKLOG) == -1)
     {
        perror("listen");
        exit(1);
     }
    
     sa.sa_handler = sigchld_handler;					 // reap all dead processes
     sigemptyset(&sa.sa_mask);
     sa.sa_flags = SA_RESTART;
    
     if (sigaction(SIGCHLD, &sa, NULL) == -1)
     {
          perror("sigaction");
          exit(1);
     }
    
     while(1) 
     {                   			                // main accept() loop
         sin_size = sizeof(struct sockaddr_in);
         
         if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1)
         {
             perror("accept");
             continue;
         }        
         printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
      
         if(!fork()){
         if (send(new_fd, "\n\tHi...\n\tWelcome to Myserver...\n\n", 39, 0) == -1)
         perror("send");
         while(1)
         {     
            if ((numbytes=recv(new_fd, buf,sizeof(buf), 0)) == -1)
            {
     	        perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            
            
            len=strlen(buf);
            for(i=0;i<len;i++)
            {
                if(buf[i] == ' ')
                {
                   j++;
                }
                else
                {
                   if(j==0)
                   {
                      s11[s1]=buf[i];
                      s1++;
                   }
                   if(j==1)
                   {
                     s22[s2]=buf[i];
                     s2++;
                   }
                   if(j==2)
                   {
                     s33[s3]=buf[i];
                     s3++;
                   }
                }
            }
            s11[s1++]='\0';
    	    s22[s2++]='\0';
            s33[s3++]='\0';
            
            if(strcmp(s11,"cd")==0)
            {
              if(j==1)
              {
                 cd(new_fd,s22);
              }
            }
            
            if(strcmp(s11,"pwd")==0)
            {
               if(j==0)
               {
                  print_pwd(new_fd);
               }
            }
            
            if(strcmp(s11,"get")==0)
            {
                 printf("Executing get command...\n");
                 get(sockfd,new_fd,s22);
            }
           
            if(strcmp(s11,"mget")==0)
            {
                 printf("Executing mget command...\n");
                 if(j==1)
                 {
                    get(sockfd,new_fd,s22);
                 } 
                 if(j==2)
                 {
                     if(call==1)
                     {
                        get(sockfd,new_fd,s22);
                        call++;                
                     }
                     if(call==2)
                     {
                        get(sockfd,new_fd,s33);
                     }
                 }
                 call=1;
            }

            if(strcmp(s11,"put")==0)
            {
                if(j==1)
                {
                   put(new_fd, s22);
                }
            }

            if(strcmp(s11,"mput")==0)
            {
                if(j==2)
                {
                   if(N==1)
                   {
                     put(new_fd, s22);
                   }
                   if(N==2)
                   {
                     put(new_fd, s33);
                     N++;
                   }
                }
                if(N==3)
                 N=1;
                else
                 N=2;
            }

 
            if(strcmp(s11,"ls")==0)
            {
               printf("Executing ls command...\n");
               if(j==0){
               main1(".");
               }
               if(j==1){
               main1(s22);
               }
               if(j<2){  
               
               fp=fopen("sendfile.txt","r");
            
               while( !feof(fp) )
               {     
                 fscanf(fp,"%s",ch);
                 if (send(new_fd,ch, sizeof(ch), 0) == -1)
                 perror("send");
               
                 if ((numbytes=recv(new_fd, buf,sizeof(buf), 0)) == -1)
                 {
     	           perror("recv");
                   exit(1);
                 }                     
               }
            
               if (send(new_fd,"$",1, 0) == -1)
               perror("send");
               printf("Exec and Send...\n");
              }
            }
            

            else            
            {   
                 if( strcmp(buf,"bye") == 0 ) 
                 {   
                   printf("Connetion closed by 1 client !\n",new_fd);
                   break;
                 }
            }
          j=0;
          s1=0;
          s2=0;
          s3=0;
          }                 // end of fork while loop
          exit(0);
         }                       // end of fork
         close(new_fd);
     }                                   // for main while loop
     return 0;
}

