#ifndef APUE_SRC_CHAPTER5_CLIENT_SERVER_EXAMPLE_H
#define APUE_SRC_CHAPTER5_CLIENT_SERVER_EXAMPLE_H

void client_server_example_main(int argc, char **argv);

/**
 * Echo server example 1.
 */
void client_server_example_main_1(int argc, char **argv);
void client_server_example_main_2(int argc, char **argv);

void client_echo(int fd);
void server_echo(int fd);

#endif // APUE_SRC_CHAPTER5_CLIENT_SERVER_EXAMPLE_H
