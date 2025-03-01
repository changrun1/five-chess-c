#include "../include/remote.h"


void set_nonblocking(SOCKET sock) {
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
}

int create_room() {
    WSADATA wsa;
    SOCKET server_socket;
    struct sockaddr_in server_addr;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return FAIL;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return FAIL;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) return FAIL;
    
    if (listen(server_socket, 3) < 0) return FAIL;
    
    set_nonblocking(server_socket);

    remote_state.server_socket = server_socket;
    remote_state.is_server = 1;

    return SUCCESS;
}

struct game_setting join_room(char *ip) {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return (struct game_setting){0, 0, 0, FAIL};

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return (struct game_setting){0, 0, 0, FAIL};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) return (struct game_setting){0, 0, 0, FAIL};

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 && WSAGetLastError() != WSAEWOULDBLOCK) {
        return (struct game_setting){0, 0, 0, FAIL};
    }

    set_nonblocking(client_socket);
    remote_state.client_socket = client_socket;
    remote_state.is_server = 0;

    char setting_buffer[BUFFER_SIZE] = {0};
    struct game_setting setting = {0, 0, 0, FAIL};
    int bytes_received = 0;
    while (bytes_received <= 0) {
        bytes_received = recv(client_socket, setting_buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            sscanf(setting_buffer, "%d %d %d %d", &setting.time_limit, &setting.forbidden_rule, &setting.player, &setting.is_success);
            setting.is_success = SUCCESS;
        }
        Sleep(100);  // 避免忙等
    }


    return setting;
}

int server_wait(struct game_setting setting) {
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    SOCKET client_socket = accept(remote_state.server_socket, (struct sockaddr *)&client_addr, &addrlen);
    
    if (client_socket == INVALID_SOCKET && WSAGetLastError() == WSAEWOULDBLOCK) return FAIL;
    
    set_nonblocking(client_socket);
    remote_state.client_socket = client_socket;

    send_game_setting(setting);
    return SUCCESS;
}

int send_game_setting(struct game_setting setting) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d %d %d %d", setting.time_limit, setting.forbidden_rule, setting.player, SUCCESS);
    send(remote_state.client_socket, buffer, strlen(buffer), 0);
    return SUCCESS;
}

void send_remote_input(struct remote_input input) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d %d %d", input.x, input.y, COORDINATE);
    send(remote_state.client_socket, buffer, strlen(buffer), 0);
}

void send_out_of_time() {
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d %d %d", 0, 0, OUT_OF_TIME);
    send(remote_state.client_socket, buffer, strlen(buffer), 0);
}

struct remote_input get_remote_input() {
    char buffer[BUFFER_SIZE] = {0};
    struct remote_input input = {0, 0, 0};
    
    int valread = recv(remote_state.client_socket, buffer, BUFFER_SIZE, 0);
    if (valread > 0) {
        sscanf(buffer, "%d %d %d", &input.x, &input.y, &input.type);
    }
    return input;
}

struct remote_input handle_communication() {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(remote_state.client_socket, &read_fds);

    struct timeval timeout = {0, 100000};
    if (select(0, &read_fds, NULL, NULL, &timeout) > 0) {
        struct remote_input input = get_remote_input();
        return input;
    }
    return (struct remote_input){0, 0, FAIL};
}


void close_remote() {
    
    if (remote_state.client_socket != INVALID_SOCKET) {
        closesocket(remote_state.client_socket);
        remote_state.client_socket = INVALID_SOCKET;
    }
    if (remote_state.server_socket != INVALID_SOCKET && remote_state.is_server) {
        closesocket(remote_state.server_socket);
        remote_state.server_socket = INVALID_SOCKET;
    }
    WSACleanup();


}