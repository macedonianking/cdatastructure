#ifndef APUE_CHAPTER7_SOCK_OPT_H
#define APUE_CHAPTER7_SOCK_OPT_H

/**
getsockopt
setsockopt
SOL_SOCKET
IPPROTO_IP
IPPROTO_TCP
IPPROTO_SCTP
IPPROTO_IPV6
SO_DEBUG, 必须有权限才能设置
SO_DONTROUTE, 忽略默认路由
*/

/**
 * sock_opt的主入口
 */
void chapter7_sock_opt_main(int argc, char **argv);

/**
 * 
 */
void chapter7_sock_opt_main_1(int argc, char **argv);

/**
 * Print default socket options.
 */
void chapter7_sock_opt_main_2(int argc, char **argv);

#endif // APUE_CHAPTER7_SOCK_OPT_H
