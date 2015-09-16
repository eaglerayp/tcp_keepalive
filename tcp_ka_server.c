
            /* --- begin of keepalive test program --- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[] )
{
   int listen_sock,clientsock,n;
   clientsock=abs(clientsock);  // random make it positive
   int optval;
   struct sockaddr_in serv_addr, client_addr;
   socklen_t optlen = sizeof(optval);
   pid_t     childpid;
   char buffer[15]="test keepalive";
   char mesg[1000];

   /* Create the socket */
   if((listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("socket()");
      exit(EXIT_FAILURE);
   }

   /* Check the status for the keepalive option */
   if(getsockopt(listen_sock, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(listen_sock);
      exit(EXIT_FAILURE);
   }
   printf("SO_KEEPALIVE default is %s\n", (optval ? "ON" : "OFF"));

   /* Set the option active */
   optval = 1;
   optlen = sizeof(optval);
   
   if(setsockopt(listen_sock, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
      perror("setsockopt()");
      close(listen_sock);
      exit(EXIT_FAILURE);
   }
   printf("SO_KEEPALIVE set on socket\n");

   /* Check the status again */
   if(getsockopt(listen_sock, IPPROTO_TCP, TCP_KEEPIDLE, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(listen_sock);
      exit(EXIT_FAILURE);
   }
   printf("TCP_KEEPIDLE is %d\n", optval );
      /* Check the status again */
   if(getsockopt(listen_sock, IPPROTO_TCP, TCP_KEEPCNT, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(listen_sock);
      exit(EXIT_FAILURE);
   }
   printf("TCP_KEEPCNT is %d\n", optval);
      /* Check the status again */
   if(getsockopt(listen_sock, IPPROTO_TCP, TCP_KEEPINTVL, &optval, &optlen) < 0) {
      perror("getsockopt()");
      close(listen_sock);
      exit(EXIT_FAILURE);
   }
   printf("TCP_KEEPINTVL is %d\n", optval );
  /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   int portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

      /* Now bind the host address using bind() call.*/
   if (bind(listen_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
      perror("ERROR on binding");
      exit(1);
      }
      
   /* Now start listening for the clients, here process will
   * go in sleep mode and will wait for the incoming connection
   */
   listen(listen_sock,5);
   printf("listening on port:%d\n\n",portno);
   while(1){
      int newsockfd = accept(listen_sock, (struct sockaddr *)&client_addr, &clientsock);
      if (newsockfd < 0){
         printf("newsockfd:%d client_port:%d\n",newsockfd,ntohs(client_addr.sin_port));
         perror("ERROR on accept");
         exit(1);
      }
      /*getsockopt(newsockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) ;
      printf("client SO_KEEPALIVE:%d\n",optval);*/
      struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_addr;
      int ipAddr = pV4Addr->sin_addr.s_addr;
      char ipstr[INET_ADDRSTRLEN];
      inet_ntop( AF_INET, &ipAddr, ipstr, INET_ADDRSTRLEN );
      printf("client ip:%s  port: %d\n",ipstr,ntohs(client_addr.sin_port));
      if((childpid=fork())==0){
         int pid=getpid();
         printf("In the child process id: %d\n",pid); 
        close(listen_sock);
         while(1){
            n = recvfrom(newsockfd,mesg,1000,0,(struct sockaddr *)&client_addr,&clientsock);
            sendto(newsockfd,mesg,n,0,(struct sockaddr *)&client_addr,sizeof(client_addr));
            if(n<1){
               if(shutdown(newsockfd,2)<0){
                  perror("error on close");
                  exit(1);
               }
               printf("D/C, close connection successfully\n");
               break;
            }
            printf("-------------------------------------------------------%d\n",n);
            mesg[n] = 0;
            printf("Received the following:\n");
            printf("%s",mesg);
            printf("-------------------------------------------------------\n");
         }
         printf("end child process %d\n",pid);
         _Exit(0);
      }
      close(newsockfd);
   }
   close(listen_sock);

   exit(EXIT_SUCCESS);
}