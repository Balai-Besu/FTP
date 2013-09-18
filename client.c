/*
** client.c - a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 3490							 // the port client will be connecting to
#define MAXDATASIZE 1024 						// max number of bytes we can get at once
#define BUFFERSIZE 1024
#define size 50
#define COPYMORE 0644

void print_cwd(void)
{
     char cwd[100];
     memset(cwd, 0, sizeof(cwd));
     printf("Client Current Directory: %s\n", getcwd(cwd, sizeof(cwd)));    
}

void cd(char *dirnm)
{
    if(!chdir(dirnm)) /* chdir returns 0 on success */
    {
        print_cwd();
    }
    else
    {
        perror("ERROR");
    }
}

void put(int sockfd, int in_fd, char *s22)
{ 
    char buf[BUFFERSIZE];
    int n_chars,numbytes;

    if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
    {
        perror("recv");
        exit(1);
    }    
    buf[numbytes]='\0';            
    
    if(strcmp(buf,"SUCCES")==0)
    {                                                              /* copy files */
         printf("File sending...");
         while( (n_chars = read(in_fd, buf, BUFFERSIZE)) > 0 )
         {  
            if( send(sockfd, buf, n_chars, 0) != n_chars )
            {
                perror("send");
            }
 
            if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
            {
                perror("recv");
                exit(1);
            }     
         }
         if (send(sockfd,"$$$$\0",5, 0) == -1)
         perror("send");
         printf("\n\n%s file succesfuly Send...\n",s22);
    }
    else
    {
        printf("%s : File creation error !!\n",buf);   
    }            
}        


int main(int argc, char *argv[])
{  
   int i,j=0,s1=0,s2=0,s3=0,len,out_fd,N,flag=1;
   char s[size],s11[size],s22[size],s33[size],ch[5];  
   
   int sockfd, numbytes,in_fd;
   char buf[MAXDATASIZE];
  
   struct hostent *he;
   struct sockaddr_in their_addr; 					// connector’s address information
  
   if (argc != 2) 
   {
      fprintf(stderr,"usage: client hostname\n");
      exit(1);
   }
  
   if ((he=gethostbyname(argv[1])) == NULL) 
   {
       perror("gethostbyname");
       exit(1);
   }
  							 		// get the host info
  
   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
     perror("socket");
     exit(1);
   }
   their_addr.sin_family = AF_INET;
   								       // host byte order
   their_addr.sin_port = htons(PORT); 					// short, network byte order
   their_addr.sin_addr = *((struct in_addr *)he->h_addr);
   memset(&(their_addr.sin_zero),'\0', 8); 				// zero the rest of the struct
   
   if (connect(sockfd, (struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1) 
   {
      perror("connect");
      exit(1);
   }


   if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
   {
     perror("recv");
     exit(1);
   }
   buf[numbytes] = '\0';
   printf("%s",buf);
   while(1)
   {
       printf("Myserver:-$ ");
       gets(s);
       len=strlen(s);
       for(i=0;i<len;i++)
       {
           if(s[i] == ' ')
           {
              j++;
           }
           else
           {
             if(j==0)
             {
                s11[s1]=s[i];
                s1++;
             }
             if(j==1)
             {
                s22[s2]=s[i];
                s2++;
             }
             if(j==2)
             {
                s33[s3]=s[i];
                s3++;
             }
           }
       }
       s11[s1++]='\0';
       s22[s2++]='\0';
       s33[s3++]='\0';
      
            
       if(  strcmp(s11,"ls")==0 || strcmp(s11,"bye")==0 || strcmp(s11,"lls")==0 || strcmp(s11,"clear")==0 || strcmp(s11,"get")==0 ||                  strcmp(s11,"lcd")==0 || strcmp(s11,"lpwd")==0  || strcmp(s11,"cd")==0  || strcmp(s11,"pwd")==0 || strcmp(s11,"put")==0 || strcmp(s11,"mget")==0 || strcmp(s11,"mput")==0)
       
       { 
          
          if(strcmp(s11,"lls")==0)
           {
             system("ls");
           }   
 
          if(strcmp(s11,"clear")==0)
          {
             system("clear");
          }
          
          if(strcmp(s11,"lcd")==0)
          {
             if(j==1)
             {
                cd(s22);
             }
             else
             {
                 printf("Command line error !!\n");
             }
          }
          
          if(strcmp(s11,"cd")==0)
          {
             if(j==1)
             {
                 if (send(sockfd,s,strlen(s), 0) == -1)
                  perror("send");
                
                 if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
                 {
     	            perror("recv");
                    exit(1);
                 }
                 buf[numbytes]='\0';
                 if(strcmp(buf,"ERROR")!=0)
                 {
                    printf("Server Current Directory = %s\n",buf);
                 }
                 else
                 {
                       printf("ERROR: No such directory !!\n");
                 }
             }
             else
             {
                 printf("Command line error !!\n");
             }
          }
          
          if(strcmp(s11,"lpwd")==0)
          {
             if(j==0)
              print_cwd();
             else
              printf("Command line error !!\n");
          }
           
          if(strcmp(s11,"pwd")==0)
          {
             if(j==0)
             {
                if (send(sockfd,s,strlen(s), 0) == -1)
                  perror("send");
                if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
                 {
     	            perror("recv");
                    exit(1);
                 }
                 buf[numbytes]='\0';
                 printf("Server Current directory = %s\n",buf);
             }   
             else
              printf("Command line error !!\n");
          }
        
          if(strcmp(s11,"put")==0)
          {
             if(j==1)
             {
                 if( (in_fd=open(s22, O_RDONLY)) == -1 )
                 { 
                    printf("%s file is not present...!!\n",s22);
                 }
                 else
                 {
                    if (send(sockfd,s,strlen(s), 0) == -1)
                    perror("send");
                    put(sockfd,in_fd,s22);
                 }
             } 
             else
             {
                printf("Invalid argument...!!\n");
             }              
          }

          if(strcmp(s11,"mput")==0)
          {
             if(j==2)
             {
               for(N=1;N<=j;N++)
               {
                  if(N==1)
                  {
                     if( (in_fd=open(s22, O_RDONLY)) == -1 )
                     { 
                       printf("%s file is not present...!!\n",s22);
                     }
                     else
                     {
                       if (send(sockfd,s,strlen(s), 0) == -1)
                       perror("send");
                       put(sockfd,in_fd,s22);
                     }
                  }
                  if(N==2)
                  {
                     if( (in_fd=open(s33, O_RDONLY)) == -1 )
                     { 
                       printf("%s file is not present...!!\n",s33);
                     } 
                     else
                     {
                       if (send(sockfd,s,strlen(s), 0) == -1)
                       perror("send");
                       put(sockfd,in_fd,s33);
                     }
                  }
                }
             } 
             else
             {
                printf("Invalid argument...!!\n");
             }              
          }

          if(strcmp(s11,"get")==0)
          {
             if(j==2)
             {
                 if (send(sockfd,s,strlen(s), 0) == -1)
                 perror("send");
                 
                 if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
                 {
     	            perror("recv");
                    exit(1);
                 }
                 buf[numbytes]='\0';
                 
                 if(strcmp(buf,"SUCCES")==0)
                 {     
                    if( (out_fd=creat(s33, COPYMORE)) == -1 )
                    {
                        printf("ERROR OCCURE !!\n");
                        if (send(sockfd,"####",4, 0) == -1)
                        perror("send");
                    }
                    else
                    {
                       printf("please wait some time while file is reciving...");
                      if (send(sockfd,"#",1, 0) == -1)
                       perror("send");
                    
                      while(1)
                      {   
                         if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
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
                         
                         if (send(sockfd,"##",2, 0) == -1)
                         perror("send");
                      }
                      printf("\n\n%s file succesfuly recived...\n",s33);
                   }
                 }
                 else
                 {
                    printf("ERROR: %s file not present\n",s22);
                 }
              }
              else
              {
                  printf("Invalid arguments !!\n");     
              }
          }

           if(strcmp(s11,"mget")==0)
           {
              if(j==2 || j==1)
              {
                  if (send(sockfd,s,strlen(s), 0) == -1)
                  perror("send");
             
                  for(N=1;N<=j;N++)
                  {
                      if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
                      {
     	                 perror("recv");
                         exit(1);
                      }
                      buf[numbytes]='\0';
                 
                      if(strcmp(buf,"SUCCES")==0)
                      {   
                          if(N==1)  
                          printf("%s file is ready to recive. If u want to\nsave then enter 'y' or else 'n': ",s22);
                          if(N==2)
                          printf("%s file is ready to recive. If u want to\n save then enter 'y' or else 'n': ",s33);
                          gets(ch);
                          if(strcmp(ch,"y")==0)
                          {
                             if(N==1)
                             {
                                  if( (out_fd=creat(s22, COPYMORE)) == -1 )
                                  {
                          	      printf("ERROR OCCURE !!\n");
                                      if (send(sockfd,"####",4, 0) == -1)
                                      perror("send");
                                      flag=0;
                                  }
                             }
                             if(N==2)
                             {
                                 if( (out_fd=creat(s33, COPYMORE)) == -1 )
                                 {
                        	      printf("ERROR OCCURE !!\n");
                   		      if (send(sockfd,"####",4, 0) == -1)
                 	              perror("send");
                     		      flag=0;
                                  }
                             }
                             if(flag==1)
                             {
                                printf("please wait some time while file is reciving...");
                                if (send(sockfd,"#",1, 0) == -1)
                                perror("send");
                    
                                while(1)
                                {   
                                    if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
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
                         
                                    if (send(sockfd,"##",2, 0) == -1)
                                    perror("send");
                                }
                                if(N==1)
                                printf("\n\n%s file succesfuly recived...\n",s22);
                                if(N==2)
                                printf("\n\n%s file succesfuly recived...\n",s33);
                             } 
                             flag=1;
                          }
                          else
                          {
                             if (send(sockfd,"####",4, 0) == -1)
                             perror("send");
                             if(N==1)
                             printf("%s file not recived...\n",s22);
                             if(N==2)
                             printf("%s file not recived...\n",s33);
                          }
                      }
                      else
                      {
                          printf("ERROR: %s file not present\n",s22);
                      }
                  }
              }
              else
              {
                  printf("Invalid arguments !!\n");     
              }
          }          
 
          if(strcmp(s11,"ls")==0)
          {   
                         
              if (send(sockfd,s,strlen(s), 0) == -1)
              perror("send");
              
     
              while(1)
              {    
                  
                  if ((numbytes=recv(sockfd, buf,sizeof(buf), 0)) == -1)
                  {
     	            perror("recv");
                    exit(1);
                  }
                
                  buf[numbytes] = '\0';
                  
                  if( strcmp(buf,"$") !=0)
                  printf("%s   ",buf);
                  else
                  break;     
                 
                  if (send(sockfd,"#",1, 0) == -1)
                  perror("send");
              }
              printf("\n");
         }
        
         if(strcmp(s11,"bye")==0)
         {  
               if (send(sockfd,"bye",3, 0) == -1)
               perror("send");
               break;
         }
      }
      
      else
      {
          printf("%s:command not found\n",s);
      }
      j=0;
      s1=0;
      s2=0;
      s3=0;
   }  
   printf("\n\n");
   close(sockfd);
   return 0;
} 

