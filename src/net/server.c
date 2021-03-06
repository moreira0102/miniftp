#include "conhandler.h"
#include "../utils/filefinder.h"
#include <pthread.h>


int server(char *path, int port)
{
  int errnum;

  int server_fd;
  /* Create a new socket of type TCP in an IPV4 domain.
     This socket is represented as a file descriptor  */
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  /* Set the reuse of ADDRESSES and PORTS, which means
  that even if the process is restarted, the kernell will
  give to it the same previous address and port if the
  socket is in the TIME_WAIT state.  */
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)))
  {
    perror("Can't set the options");
    exit(EXIT_FAILURE);
  }

  /* Structure describing an Internet socket address.  */
  struct sockaddr_in address;

  /*Specifing the type of address to IPV4*/
  address.sin_family = AF_INET;

  /* Setting that the socket need to be bind to all interfaces,
   it means that if you have two network interfaces, 
   you will accept connections and packets sent to those interfaces */
  address.sin_addr.s_addr = INADDR_ANY;

  /* The htons() function converts the unsigned short integer
   hostshort from host byte order to network byte order. */
  address.sin_port = htons(port);

  /* Give the socket server_fd the local address "address" (which is addr bytes long). */
  if (bind(server_fd, (struct sockaddr *)&address,
           sizeof(address)) < 0)
  {
    perror("Fail to bind address to socket file descriptor");
    exit(EXIT_FAILURE);
  }

/* Marks the socket referred to by server_fd as a passive
   socket, that is, as a socket that will be used to accept incoming
   connection requests using accept(2).

   The server_fd argument is a file descriptor that refers to a socket
   of type SOCK_STREAM or SOCK_SEQPACKET.

   The backlog argument set to 3 defines the maximum length to which the
   queue of pending connections for sockfd may grow.  If a
   connection request arrives when the queue is full, the client may
   receive an error with an indication of ECONNREFUSED or, if the
   underlying protocol supports retransmission, the request may be
   ignored so that a later reattempt at connection succeeds. */
if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  /* The accept() system call is used with connection-based socket
     types (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first
     connection request on the queue of pending connections for the
     socket(2), bound to a local address with bind(2), and is
     listening socket, sockfd, creates a new connected socket, and
     returns a new file descriptor referring to that socket.  The
     newly created socket is not in the listening state.  The original
     socket sockfd is unaffected by this call.

     The argument sockfd is a socket that has been created with
     listening for connections after a listen(2). See more details
     in accept(2) man page*/
  int connected_socket_fd;
  int addrlen = sizeof(address);
  while ((connected_socket_fd = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) > 0){
    
    pthread_t thread;

    handargs *conargs = (handargs *)malloc(sizeof(handargs));

    conargs->socket_con_accepted = connected_socket_fd;
    conargs->path_ch = path;

    pthread_create(&thread, NULL, conhandler, conargs);

  }
  return 0;
}