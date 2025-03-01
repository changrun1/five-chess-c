#include "input_handling.h"
#include "print_utils.h"
#include "game_logic.h"
#include "remote.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <windows.h>
#include <process.h>


// 輸入模式
#define NUMBER_KEY 0
#define WASD 1

#define LOCAL 0
#define ONLINE 1
#define AI 2

// 結果
#define REGRET 4

int ai_done = 0;
int ai_x = -1;
int ai_y = -1;
int ai_count = 0;
HANDLE ai_count_mutex;
HANDLE ai_thread_handle;

unsigned __stdcall ai_thread(void *arg)
{
    int count;

    WaitForSingleObject(ai_count_mutex, INFINITE);
    count = ai_count;
    ai_count++;
    ReleaseMutex(ai_count_mutex);

    int * player = (int *)arg;
    ai_done = 0;
    ai_x = -1;
    ai_y = -1;
    
    get_ai_coordinate(&ai_x, &ai_y, *player);
    if(count == ai_count - 1)
        ai_done = 1;
    return 0;
}

void clear_input_buffer()
{
    while (kbhit())
    {
        getch();
    }
}

int get_keyboard_input()
{
    if (kbhit())
    {
        int key = getch();

        if (key == 13 || key == 10) // Enter
        {
            clear_input_buffer();
            return '\n';
        }
        if (key == 0 || key == 224) // 0 or 224 表示是特殊鍵，如方向鍵
        {
            key = getch();
            clear_input_buffer();
            switch (key)
            {
            case 72:
                return 'w';
            case 80:
                return 's';
            case 75:
                return 'a';
            case 77:
                return 'd';
            }
        }
        clear_input_buffer();
        return key;
    }
    return -1;
}

int get_coordinate_from_remote(InputState *state)
{
    struct remote_input input = handle_communication();
    Sleep(100);
    if (input.type == COORDINATE)
    {
        state->x = input.x + 1;
        state->y = input.y + 1;
        state->state = 3;
        return 1;
    }
    if (input.type == OUT_OF_TIME)
    {
        state->result = 5;
        return 1;
    }
    return 0;
}

void init_input_state(InputState *state)
{
    state->x = -1;
    state->y = -1;
    state->result = 0;
    state->state = 0;
    ai_done = 0;
    ai_x = -1;
    ai_y = -1;
    CloseHandle(ai_thread_handle);
    ai_thread_handle = NULL;
    clear_input_buffer();
    ai_count = 0;
    ai_count_mutex = CreateMutex(NULL, FALSE, NULL);
}


int get_coordinate_from_ai(InputState *state, int player)
{

    if(!ai_thread_handle || WaitForSingleObject(ai_thread_handle, 0) == WAIT_OBJECT_0){
        if(ai_thread_handle)
            CloseHandle(ai_thread_handle);
        ai_thread_handle = (HANDLE) _beginthreadex(NULL, 0, ai_thread, &player, 0, NULL);
        return 0;
    }
    
    
    if (ai_done)
    {

        state->x = ai_x;
        state->y = ai_y;
        state->state = 3;
        ai_done = 0;
        ai_thread_handle = NULL;
        return 1;
    }
    return 0;
}

int get_coordinate_from_local_wasd(InputState *state)
{
    int input = get_keyboard_input();
    if (!in_board(state->x, state->y))
    {
        state->x = 1;
        state->y = 1;
        print_board(state->x - 1, state->y - 1);
    }
    if (input == -1)
        return 0;
    if (input == 'r')
    {
        state->result = REGRET;
        return 1;
    }

    switch (state->state)
    {
    case 0:
        if (input == 'a')
        {
            if (in_board(state->x, state->y - 1))
                state->y--;
        }
        if (input == 'd')
        {
            if (in_board(state->x, state->y + 1))
                state->y++;
        }
        if (input == 'w')
        {
            if (in_board(state->x - 1, state->y))
                state->x--;
        }
        if (input == 's')
        {
            if (in_board(state->x + 1, state->y))
                state->x++;
        }
        if (input == '\n')
        {
            state->state = 3;
            return 1;
        }
        break;
    }

    remove_info_coord();

    char coord_info[50];
    sprintf(coord_info, "x %d  y %d\n", state->x, state->y);
    append_info(coord_info);
    print_board(state->x - 1, state->y - 1);

    return 0;
}

int get_coordinate_from_local_number_key(InputState *state)
{
    int input = get_keyboard_input();
    if (input == -1)
        return 0;
    if (input == 'r')
    {
        state->result = REGRET;
        return 1;
    }
    switch (state->state)
    {
    case 0:
        if (isdigit(input))
        {
            state->x = input - '0';
            state->state = 1;
            append_info_char(input);
        }
        break;

    case 1: // 讀取 x 座標
        if (isdigit(input))
        {
            state->x = state->x * 10 + (input - '0');
            append_info_char(input);
        }
        else if (input == ' ')
        {
            state->state = 2;
            append_info_char(input);
        }
        else if (input == '\b')
        {
            state->x /= 10;
            if (state->x == 0)
                state->state = 0;
            remove_info_last_char();
        }
        break;

    case 2: // 讀取 y 座標
        if (isdigit(input))
        {
            if (state->y == -1)
                state->y = 0;
            state->y = state->y * 10 + (input - '0');
            append_info_char(input);
        }
        else if (input == '\n')
        {
            state->state = 3; // 完成狀態
            return 1;         // 輸入完成
        }
        else if (input == '\b')
        {
            if (state->y == -1)
            {
                state->state = 1;
            }
            else
            {
                state->y /= 10;
                if (state->y == 0)
                    state->y = -1;
            }
            remove_info_last_char();
        }
        break;
    }
    print_board(state->x - 1, state->y - 1);
    return 0;
}

int get_coordinate_from_local(InputState *state, int input_mode)
{
    if (input_mode == NUMBER_KEY)
    {
        return get_coordinate_from_local_number_key(state);
    }
    else if (input_mode == WASD)
    {
        return get_coordinate_from_local_wasd(state);
    }
    return 0;
}

int get_coordinate(InputState *state, int mode, int turn, int player, int input_mode)
{
    if (state->state == 3)
    {
        state->state = 0;
        return 1;
    }
    if (turn == player)
    {
        return get_coordinate_from_local(state, input_mode);
    }
    else
    {
        switch (mode)
        {
        case LOCAL:
            return get_coordinate_from_local(state, input_mode);
        case ONLINE:
            return get_coordinate_from_remote(state);
        case AI:
            return get_coordinate_from_ai(state, turn);
        }
    }

    return 0;
}

