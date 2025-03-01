//#include "include/remote.h"
#include "src/remote.c"
#include <stdio.h>

void test_server() {
    struct game_setting setting = {60, 1, 2, SUCCESS};
    if (create_room() == SUCCESS) {
        printf("Server created, waiting for client...\n");
        while (server_wait(setting) == FAIL);
        printf("Client connected!\n");
        while (1) {
            handle_communication();
            struct remote_input input = {rand() % 10, rand() % 10};
            send_remote_input(input);
            Sleep(1000);
        }
    } else {
        printf("Failed to create server.\n");
    }
}

void test_client(char *server_ip) {
    struct game_setting setting = join_room(server_ip);
    printf("setting: %d %d %d %d\n", setting.time_limit, setting.forbidden_rule, setting.player, setting.is_success);
    if (setting.is_success == SUCCESS) {
        printf("Connected to server!\n");
        printf("Time limit: %d\n", setting.time_limit);
        while (1) {
            printf("Waiting for server...\n");
            handle_communication();
            struct remote_input input = {rand() % 10, rand() % 10};
            send_remote_input(input);
            Sleep(100);
        }
    } else {
        printf("Failed to connect to server.\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <server|client> [server_ip]\n", argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "server") == 0) {
        test_server();
    } else if (strcmp(argv[1], "client") == 0 && argc == 3) {
        test_client(argv[2]);
    } else {
        printf("Invalid arguments.\n");
    }
    return 0;
}
