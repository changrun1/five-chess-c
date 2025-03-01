

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

struct remote_stete
{
    int is_server;
    SOCKET server_socket;
    SOCKET client_socket;
} remote_state;

#define SUCCESS 1
#define FAIL 0




struct game_setting
{

    int time_limit;
    int forbidden_rule;
    int player;
    int is_success;
};

int create_room();

struct game_setting join_room(char *ip);



int send_game_setting(struct game_setting setting);



int server_wait(struct game_setting setting);



#define COORDINATE 1
#define OUT_OF_TIME 2

struct remote_input
{
    int x;
    int y;
    int type;
};



/*struct remote_input get_remote_input();*/

void send_remote_input(struct remote_input input);

void send_out_of_time();

struct remote_input handle_communication();


void close_remote();