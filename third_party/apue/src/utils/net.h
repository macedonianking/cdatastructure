#ifndef APUE_NET_H
#define APUE_NET_H

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>

#include <netdb.h>

// inet_ntop
// inet_pton

// INET_ADDRSTRLEN
// INET6_ADDRSTRLEN
// struct in_addr
// struct sockaddr
// struct sockaddr_in
// inet_ntop
// inet_pton

int get_interface_addr(struct in_addr *addr);
int get_loopback_addr(struct in_addr *addr);
int resolve_host(const char *node, const char *service, struct sockaddr_in *addr);
int look_up_protocol(const char *name, int *protocol);

int fill_socket_addr(struct sockaddr_storage *addr, const char *src);

char *inet_ip(struct in_addr *addr);

#define SERVER_PORT     8081
#define CLIENT_PORT     8082

#define SERVER_OPTION   "--server"
#define CLIENT_OPTION   "--client"
#define LOCAL_IP_ADDR   "127.0.0.1"
#define LISTEN_BACKLOG_COUNT   30

#define SA struct sockaddr

#define TCP_NO  6

static inline int choose_process_main_routing(int argc, char **argv,
    void (*server_routing)(int, char **), void (*client_routing)(int, char **)) {
    if (argc < 2) {
       return -1;
    }

    if (!strcmp(SERVER_OPTION, argv[1])) {
        server_routing(argc, argv);
    } else if (!strcmp(CLIENT_OPTION, argv[1])) {
        client_routing(argc, argv);
    } else {
        return -1;
    }

    return 0;
}

/**
 * get socket destription.
 */
char *sock_ntop(struct sockaddr *addr, socklen_t len, char *buf, int size);

/**
 * @param rd
 */
int say_daytime(int fd);

int create_connection(const struct sockaddr_in *addr);
int create_local_connection(in_port_t port);

/**
 * Get local socket address from LOCAL_IP_ADDR.
 * @param addr: the sockaddr_in address that pointed to the address.
 * @param host_port: the port number in host binary order.
 */
int get_local_socket_addr(struct sockaddr_in *addr, in_port_t host_port);

/**
 * Create a server tcp listen socket from the specified address.
 *
 * @param addr: The specified address that socket will bind.
 * @param backlog: the listen backlog count.
 */
int create_server_socket(struct sockaddr_in *addr, int backlog);

/**
 * Query host service information.
 * 
 * @param node: Host name
 * @param service: Service name.
 * @param family: AF_INET or AF_INET6
 * @param socktype: 0, SOCK_STREAM or SOCK_DGRAM.
 */
struct addrinfo *host_serv(const char *node, const char *service, int family, int socktype);

/**
 * Create tcp socket and connect.
 */
int tcp_connect(const char *node, const char *service);

/**
 * 创建一个监听用的socket
 */
int tcp_listen(const char *service, socklen_t *addrlen);

int tcp_bind_wildcard(int domain, uint16_t port);

/**
 * 创建一个本地的监听描述符
 */
int tcp_listen_wildcard(int domain, uint16_t port);

#endif
