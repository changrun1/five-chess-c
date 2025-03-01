
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "print_utils.h"

// ANSI 顏色碼
#define NORMAL_COLOR "\x1b[47;30m"
#define HIGHLIGHT_BG "\x1b[100;30m"
#define RESET_COLOR "\x1b[0m"

char *info = NULL;
char *time_info = NULL;

int (*board)[15];

void init_print_utils(int (*_board)[15])
{
    info = (char *)malloc(1000 * sizeof(char));
    memset(info, 0, sizeof(*info));
    time_info = (char *)malloc(100 * sizeof(char));
    memset(time_info, 0, sizeof(*time_info));
    board = _board;

}

void update_info(char *str)
{
    memset(info, 0, sizeof(*info));
    strcat(info, str);
}

void append_info(char *str)
{
    strcat(info, str);
}

void append_info_char(char c)
{
    char tempStr[2] = {c, '\0'};
    strcat(info, tempStr);
}

void remove_info_last_char()
{
    info[strlen(info) - 1] = '\0';
}

void print_symbol(int i, int j, char *symbol, char *tail, int highlight)
{

    if (highlight)
        printf(HIGHLIGHT_BG);

    else
        printf(NORMAL_COLOR);

    if (board[i][j] == 0)
    {
        printf("%s", symbol);

        printf("%s", tail);
    }
    else if (board[i][j] == 1)
    {
        if (highlight)
            printf("●");
        else
            printf("●");

        printf("%s", tail);
    }
    else
    {
        if (highlight)
            printf("○");
        else
            printf("○");

        printf("%s", tail);
    }
    printf(RESET_COLOR);
}

void print_board(int highlight_row, int highlight_col)
{
    system("cls");

    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            int highlight = (i == highlight_row || j == highlight_col);

            if (i == 0)
            {
                if (j == 0)
                    print_symbol(i, j, "┌", "─", highlight);
                else if (j == 14)
                    print_symbol(i, j, "┐", "", highlight);
                else
                    print_symbol(i, j, "┬", "─", highlight);
            }

            else if (i == 14)
            {
                if (j == 0)
                    print_symbol(i, j, "└", "─", highlight);
                else if (j == 14)
                    print_symbol(i, j, "┘", "", highlight);
                else
                    print_symbol(i, j, "┴", "─", highlight);
            }

            else
            {
                if (j == 0)
                    print_symbol(i, j, "├", "─", highlight);
                else if (j == 14)
                    print_symbol(i, j, "┤", "", highlight);
                else
                    print_symbol(i, j, "┼", "─", highlight);
            }
        }

        printf("\n");
    }
    printf("%s", info);
    printf("\n");

}

void update_time_info(unsigned long long start_time, int time_limit)
{
    if(time_limit == -1){
        return;
    }
    static unsigned long long last_time = 0;
    if (clock() - last_time < 100)
    {
        return;
    }
    last_time = clock();
    memset(time_info, 0, sizeof(*time_info));
    strcat(time_info, "剩餘時間：");
    char time_str[10];
    sprintf(time_str, "%d ", (int)(time_limit - (clock() - start_time) / CLOCKS_PER_SEC));
    strcat(time_info, time_str);
    printf("\033[?25l\r%s", time_info);
}
void remove_info_coord()
{
    // 清空上一次的座標
    char coord_char[15] = {'x', 'y', ':', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\n', ' '};
    int removed;

    do
    {
        removed = 0;
        for (int i = 0; i < 15; i++)
        {
            if (info[strlen(info) - 1] == coord_char[i])
            {
                remove_info_last_char();
                removed = 1;
                break;
            }
        }
    } while (removed);
}
