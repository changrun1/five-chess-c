#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include "print_utils.h" //印出棋盤和訊息
#include "menu.h" //顯示選單
#include "game_logic.h" //遊戲邏輯
#include "input_handling.h" //取得輸入
#include "remote.h" //遠程連線

// 遊戲狀態
#define PLAYER1_ROUND 0
#define PLAYER2_ROUND 1
#define MAIN_MENU 2
#define GAME_OVER 3
#define HELP 4
#define SETTING 5
#define SETTING_TIME_LIMIT 51
#define SETTING_INPUT_MODE 52
#define SETTING_FORBIDDEN_RULE 53
#define SETTING_AI_LEVEL 54
#define SETTING_FIRST_PLAYER 55
#define ONLINE_SETTING 6
#define SERVER_WAITING 61
#define ONLINE_SETTING_CLIENT 62

#define REVIEW 7

#define REGRET 4



#define LEGAL 0
#define OUT_OF_BOARD 1
#define OCCUPIED 2
#define FORBIDDEN 3

#define LOCAL 0
#define ONLINE 1
#define AI 2

// 輸入模式
#define NUMBER_KEY 0 
#define WASD 1

int input_mode = NUMBER_KEY;
int time_limit = 15;
int (*board)[15];
int history[225][2];
int history_count = 0;
int player = 1;
int game_state = MAIN_MENU;
int game_mode = LOCAL;

// 輸入狀態
InputState input_state = {-1, -1, 0, 0};

// 回合開始時間
unsigned long long start_time = 0;


int regret()
{
    if (history_count > 0)
    {
        history_count--;
        int x = history[history_count][0];
        int y = history[history_count][1];
        board[x][y] = 0;
        return 1;
    }
    return 0;
}

void init_game()
{
    board = (int(*)[15])malloc(sizeof(int) * 15 * 15);
    memset(board, 0, sizeof(int) * 15 * 15);
    init_input_state(&input_state);
    history_count = 0;
    init_print_utils(board);
    init_game_logic(board);
    clear_input_buffer();
}

int check_time_out(unsigned long long start_time)
{
    if (time_limit == -1)
    {
        return 0;
    }
    if ((int)(clock() - start_time) > time_limit * CLOCKS_PER_SEC)
    {
        return 1;
    }
    return 0;
}

void place_piece(int x, int y, int player)
{
    board[x][y] = player;
    history[history_count][0] = x;
    history[history_count][1] = y;
    history_count++;
}

int review(int reset)// 離開返回1
{ 
    static int index = 0;
    int input = get_keyboard_input();
    if (input == 'd')
    { // 上下左右會轉換成 wasd
        if (index < history_count)
        {
            index++;
        }
    }
    else if (input == 'a')
    { // 上下左右會轉換成 wasd
        if (index > 0)
        {
            index--;
        }
    }
    else if (input == 'q')
    {
        index = 0;
        return 1;
    }
    else if (reset)
    {
        index = 0;
    }
    else
    {
        return 0;
    }

    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            board[i][j] = 0;
        }
    }

    int x = history[index][0];
    int y = history[index][1];
    if(index == history_count){
        x = -1;
        y = -1;
        index--;
    }

    for (int i = 0; i <= index; i++)
    {
        int x = history[i][0];
        int y = history[i][1];
        board[x][y] = (i % 2 == 0) ? 1 : 2;
    }
    

    update_info("\n按 右 顯示下一步，按 左 顯示上一步，按 q 返回主菜單\n");
    append_info("目前顯示第");
    char index_str[10];
    sprintf(index_str, "%d", index + 1);
    append_info(index_str);
    append_info("步\n");
    print_board(x, y);

    return 0;
}

void turn_to_player(int player, int *game_state) 
{
    if (player == 1)
    {
        start_time = clock();
        *game_state = PLAYER1_ROUND;
    }
    else
    {
        start_time = clock();
        *game_state = PLAYER2_ROUND;
    }
    init_input_state(&input_state);
    print_board(-1, -1);
}

void game_over(int *game_state)
{

    print_board(-1, -1);
    printf("\n請按任意鍵繼續\n");

    clear_input_buffer();
    getch();

    *game_state = GAME_OVER;

    print_game_over_option();
}


void handle_player_turn(int current_player, InputState *input_state, int *game_state)
{
    char *player_symbol = (current_player == 1) ? "○" : "●";
    char *next_player_symbol = (current_player == 1) ? "●" : "○";

    if (check_time_out(start_time))
    {
        init_input_state(input_state);
        update_info("\n玩家");
        append_info(player_symbol);
        append_info("超時，");
        append_info(next_player_symbol);
        append_info("獲勝");
        game_over(game_state);
        if(game_mode == ONLINE){
            send_out_of_time();
            close_remote();
        }
        return;
    }
    if(time_limit != -1)
        update_time_info(start_time, time_limit);


    if (get_coordinate(input_state, game_mode, current_player, player, input_mode))
    {
        int x = input_state->x - 1;
        int y = input_state->y - 1;
        int state_result = input_state->result;
        init_input_state(input_state);
        if (state_result == REGRET && game_mode != ONLINE)
        {
            if (regret())
            {
                update_info("\n目前輪到");
                append_info(next_player_symbol);
                append_info("\n請輸入座標：");
                turn_to_player(3 - current_player, game_state);
            }

            return;
        }

        int move_result = is_move_illegal(x, y, current_player);
        switch (move_result)
        {
        case LEGAL:

            if(game_mode == ONLINE && current_player == player){
                struct remote_input input;
                input.x = x;
                input.y = y;
                input.type = COORDINATE;
                send_remote_input(input);
            }
            place_piece(x, y, current_player);
            update_info("\n目前輪到");
            append_info(next_player_symbol);
            append_info("\n請輸入座標：");

            turn_to_player(3 - current_player, game_state);
            break;
        case OUT_OF_BOARD:
            update_info("\n座標超出範圍");
            append_info("\n目前輪到");
            append_info(player_symbol);
            append_info("\n請輸入座標：");
            printf("\a");
            break;
        case OCCUPIED:
            update_info("\n此位置已經有棋子了");
            append_info("\n目前輪到");
            append_info(player_symbol);
            append_info("\n請輸入座標：");
            printf("\a");

            break;
        case FORBIDDEN:
            update_info("\n禁手規則違反");
            append_info("\n目前輪到");
            append_info(player_symbol);
            append_info("\n請輸入座標：");
            printf("\a");
            break;
        default:
            update_info("\n未知錯誤");
            append_info("\n目前輪到");
            append_info(player_symbol);
            append_info("\n請輸入座標：");
            printf("\a");
        }

        print_board(-1, -1);

        if (check_win(x, y))
        {
            if(game_mode == ONLINE){
                close_remote();
            }
            update_info("\n玩家");
            append_info(player_symbol);
            append_info("獲勝");
            game_over(game_state);
        }
    }
}

void change_game_state(int *game_state, int new_state)
{
    switch (new_state)
    {
    case MAIN_MENU:
        init_game();
        print_menu();
        break;
    case REVIEW:
        review(1);
        break;
    case SETTING:
        print_setting_info();
        break;
    case SETTING_FORBIDDEN_RULE:
        print_forbidden_rule_setting_info();
        break;

    case SETTING_FIRST_PLAYER:
        print_first_player_setting_info();
        break;
    case SETTING_AI_LEVEL:
        print_ai_level_setting_info();
        break;
    case SETTING_TIME_LIMIT:
        print_time_setting_info();
        break;
    case SETTING_INPUT_MODE:
        print_input_setting_info();
        break;
    case ONLINE_SETTING:
        print_online_setting();
        break;
    case ONLINE_SETTING_CLIENT:

        print_client_setting();
        break;
    case SERVER_WAITING:
        print_waiting_for_player();
        break;
    
    default:
        break;
    }
    *game_state = new_state;
}

int main()
{
    system("chcp 65001");

    change_game_state(&game_state, MAIN_MENU);

    while (1)
    {
        int choice;

        switch (game_state)
        {
        case MAIN_MENU:
            choice = get_keyboard_input();
            if (choice == '1')
            {
                game_mode = LOCAL;
                init_game();
                update_info("\n目前輪到○\n請輸入座標：");
                turn_to_player(1, &game_state);
            }
            else if (choice == '2')
            {
                game_mode = ONLINE;
                change_game_state(&game_state, ONLINE_SETTING);
            }
            else if (choice == '3')
            {
                game_mode = AI;
                update_info("\n目前輪到○\n請輸入座標：");
                turn_to_player(1, &game_state);
            }
            else if (choice == '4')
            {
                change_game_state(&game_state, SETTING);
            }
            else if (choice == '5')
            {
                change_game_state(&game_state, HELP);
            }
            else if (choice == '6')
            {
                return 0;
            }
            break;
        case GAME_OVER:
            choice = get_keyboard_input();
            if (choice == '1')
            {
                change_game_state(&game_state, MAIN_MENU);
            }
            else if (choice == '2')
            {
                change_game_state(&game_state, REVIEW);
            }
            else if (choice == '3')
            {
                return 0;
            }
            break;
        case PLAYER1_ROUND:
            handle_player_turn(1, &input_state, &game_state);
            break;
        case PLAYER2_ROUND:
            handle_player_turn(2, &input_state, &game_state);
            break;
        case HELP:
            print_help_info();
            clear_input_buffer();
            getch();
            change_game_state(&game_state, MAIN_MENU);
            break;
        case SETTING:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                change_game_state(&game_state, SETTING_TIME_LIMIT);
                break;
            case '2':
                change_game_state(&game_state, SETTING_INPUT_MODE);
                break;
            case '3':
                change_game_state(&game_state, SETTING_FORBIDDEN_RULE);
                break;
            case '4':
                change_game_state(&game_state, SETTING_AI_LEVEL);

                break;
            case '5':
                change_game_state(&game_state, SETTING_FIRST_PLAYER);
                break;
            case '6':
                change_game_state(&game_state, MAIN_MENU);
                break;
            default:
                break;
            }
            break;
        case SETTING_TIME_LIMIT:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                time_limit = 15;
                change_game_state(&game_state, SETTING);
                break;
            case '2':
                time_limit = 30;
                change_game_state(&game_state, SETTING);
                break;
            case '3':
                time_limit = 60;
                change_game_state(&game_state, SETTING);
                break;
            case '4':
                time_limit = -1;
                change_game_state(&game_state, SETTING);
                break;
            case '5':
                change_game_state(&game_state, SETTING);
                break;
            default:
                break;
            }
            break;
        case SETTING_INPUT_MODE:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                input_mode = NUMBER_KEY;
                change_game_state(&game_state, SETTING);
                break;
            case '2':
                input_mode = WASD;
                change_game_state(&game_state, SETTING);
                break;
            case '3':
                change_game_state(&game_state, SETTING);
                break;
            default:
                break;
            }
            break;
        case SETTING_FORBIDDEN_RULE:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                change_forbidden_rule(1);
                change_game_state(&game_state, SETTING);
                break;
            case '2':
                change_forbidden_rule(0);
                change_game_state(&game_state, SETTING);
                break;
            case '3':
                change_game_state(&game_state, SETTING);
                break;
            default:
                break;
            }
            break;

        case SETTING_FIRST_PLAYER:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                player = 1;
                change_game_state(&game_state, SETTING);
                break;
            case '2':
                player = 2;
                change_game_state(&game_state, SETTING);
                break;
            case '3':
                change_game_state(&game_state, SETTING);
                break;
            default:
                break;
            }
            break;
        case SETTING_AI_LEVEL:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                set_ai_depth(2);
                change_game_state(&game_state, SETTING);
                break;
            case '2':
                set_ai_depth(3);
                change_game_state(&game_state, SETTING);
                break;
            case '3':
                set_ai_depth(4);
                change_game_state(&game_state, SETTING);
                break;
            case '4':
                change_game_state(&game_state, SETTING);
                break;
            default:
                break;
            }
            break;
        case ONLINE_SETTING:
            choice = get_keyboard_input();
            switch (choice)
            {
            case '1':
                
                change_game_state(&game_state, SERVER_WAITING);
                create_room();
                break;
            case '2':
                change_game_state(&game_state, ONLINE_SETTING_CLIENT);
                
                break;
            case '3':
                change_game_state(&game_state, MAIN_MENU);

                break;
            default:
                break;
            }
            break;
        case SERVER_WAITING:
            choice = get_keyboard_input();
            struct game_setting setting;
            setting.time_limit = time_limit;
            setting.player = player;
            setting.forbidden_rule = get_forbidden_rule();

            
            if (choice == '1')
            {
                change_game_state(&game_state, MAIN_MENU);
            }else {
                int result = server_wait(setting);
                if(result == SUCCESS){
                    
                    update_info("\n目前輪到○\n請輸入座標：");
                    turn_to_player(1, &game_state);

                }
                    
            }

            break;
        case ONLINE_SETTING_CLIENT:
            choice = get_keyboard_input();
            if(choice == '1'){
                print_input_ip();
                char ip[20];
                scanf("%s", ip);
                struct game_setting setting = join_room(ip);
                if(setting.is_success == SUCCESS){
                    time_limit = setting.time_limit;
                    player = 3 - setting.player;
                    change_forbidden_rule(setting.forbidden_rule);

                    update_info("\n目前輪到○\n請輸入座標：");
                    turn_to_player(1, &game_state);
                }
                else{
                    print_connection_failed();
                    clear_input_buffer();
                    getch();

                    change_game_state(&game_state, MAIN_MENU);
                }
            }
            else if(choice == '2'){
                struct game_setting setting = join_room("127.0.0.1");
                if(setting.is_success == SUCCESS){
                    time_limit = setting.time_limit;
                    player = 3 - setting.player;
                    change_forbidden_rule(setting.forbidden_rule);


                    update_info("\n目前輪到○\n請輸入座標：");
                    turn_to_player(1, &game_state);
                }
                else{
                    print_connection_failed();
                    clear_input_buffer();
                    getch();

                    change_game_state(&game_state, MAIN_MENU);
                }
                
            }
            else if (choice == '3')
            {
                change_game_state(&game_state, MAIN_MENU);
            }
            break;
        case REVIEW:
            if (review(0))
            {
                change_game_state(&game_state, MAIN_MENU);
            }
            break;
        default:
            break;
        }
    }
    return 0;
}
