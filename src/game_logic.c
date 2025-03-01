#include "game_logic.h"
#include <stdlib.h>
#include <windows.h>

/*
#define DEBUG
*/
#ifdef DEBUG
#include "print_utils.h"

int count = 0;

#endif

#define INFINITY 1000000000

struct Point
{
    int x;
    int y;
};

int (*board)[15];

int forbidden_pattern_on = 0;

int depth = 3;

void change_forbidden_rule(int on)
{
    forbidden_pattern_on = on;
}

int get_forbidden_rule()
{
    return forbidden_pattern_on;
}

void init_game_logic(int (*_board)[15])
{
    board = _board;
}

int in_board(int x, int y) // 1到15
{
    return x > 0 && x <= 15 && y > 0 && y <= 15;
}

int check_forbidden(int x, int y)
{
    board[x][y] = 1;
    int dir[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    int count3 = 0, count4 = 0;
    int have3 = 0, have4 = 0;
    for (int i = 0; i < 4; i++)
    {
        have3 = 0;
        have4 = 0;
        for (int j = -4; j <= 0; j++)
        {
            int count = 0;
            for (int k = 0; k < 5; k++)
            {
                int new_x = x + dir[i][0] * (j + k);
                int new_y = y + dir[i][1] * (j + k);
                if (new_x < 0 || new_x >= 15 || new_y < 0 || new_y >= 15)
                {
                    continue;
                }
                if(board[new_x][new_y] == 1){
                    count++;
                }
            }

            if (count == 3)
            {
                have3 = 1;
            }
            else if (count == 4)
            {
                have4 = 1;
            }
        }
        if (have4)
        {
            count4++;
        }
        else if (have3)
        {
            count3++;
        }
    }
    board[x][y] = 0;

    if (count3 >= 2 || count4 >= 2)
    {
        return 1;
    }

    return 0;
}

int is_move_illegal(int x, int y, int player)
{
    if (!in_board(x + 1, y + 1))
        return OUT_OF_BOARD;
    else if (board[x][y] != 0)
        return OCCUPIED;
    else if (forbidden_pattern_on && check_forbidden(x, y) && player == 1)
        return FORBIDDEN;
    return LEGAL;
}

int check_win(int x, int y)
{
    if(board[x][y] == 0){
        return 0;
    }
    int dir[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (int i = 0; i < 4; i++)
    {
        int count = 1;
        for (int j = 1; j < 5; j++)
        {
            int new_x = x + dir[i][0] * j;
            int new_y = y + dir[i][1] * j;
            if (new_x < 0 || new_x >= 15 || new_y < 0 || new_y >= 15 || board[new_x][new_y] != board[x][y])
            {
                break;
            }
            count++;

            
        }

        for (int j = 1; j < 5; j++)
        {
            int new_x = x - dir[i][0] * j;
            int new_y = y - dir[i][1] * j;
            if (new_x < 0 || new_x >= 15 || new_y < 0 || new_y >= 15 || board[new_x][new_y] != board[x][y])
            {
                break;
            }

            count++;

        }
        if (count >= 5)
        {

            return 1;
        }
    }
    return 0;
}

int check_win_all_board()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (board[i][j] != 0)
            {
                if (check_win(i, j))
                    return 1;
            }
        }
    }
    return 0;
}

int is_board_full()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (board[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

int is_board_empty()
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (board[i][j] != 0)
                return 0;
        }
    }
    return 1;
}

int evaluate_board()
{
    int score = 0;
    int dir[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {

            for (int k = 0; k < 4; k++)
            {
                int count = 0;
                int player = 0;
                for (int l = 0; l < 4; l++)
                {
                    int new_x = i + dir[k][0] * l;
                    int new_y = j + dir[k][1] * l;
                    if (new_x < 0 || new_x >= 15 || new_y < 0 || new_y >= 15)
                    {
                        count = 0;
                        break;
                    }
                    if (player == 0)
                    {
                        if (board[new_x][new_y] != 0)
                        {
                            player = board[new_x][new_y];
                            count++;
                            continue;
                        }
                    }
                    else if (board[new_x][new_y] == player)
                    {
                        count++;
                    }
                    else if (board[new_x][new_y] == 0)
                    {
                        continue;
                    }
                    else
                    {
                        count = 0;
                        break;
                    }
                }
                if (player == 1)
                    score += count * count * count;
                else
                    score -= count * count * count;

                if (count >= 5)
                {
                    if (board[i][j] == 1)
                        return INFINITY;
                    else
                        return -INFINITY;
                }
            }
        }
    }
    return score;
}

int has_neighbor(int x, int y)
{
    int dir[8][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}, {-1, 0}, {0, -1}, {-1, -1}, {-1, 1}};
    for (int i = 0; i < 8; i++)
    {
        for (int j = 1; j < 4; j++)
        {
            int new_x = x + dir[i][0] * j;
            int new_y = y + dir[i][1] * j;
            if (new_x < 0 || new_x >= 15 || new_y < 0 || new_y >= 15)
            {
                break;
            }
            if (board[new_x][new_y] != 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

int maxmin(struct Point *point, int player, int depth, int layer, int alpha, int beta)
{
    if (layer == depth)
    {
        return evaluate_board();
    }
    int best_score;
    best_score = (player == 1) ? -INFINITY : INFINITY;
    for (int i = 0; i <= 15; i++)
    {
        for (int j = 0; j <= 15; j++)
        {
            if (is_move_illegal(i, j, player) != LEGAL)
            {
                continue;
            }
            if (!has_neighbor(i, j))
            {
                continue;
            }

            board[i][j] = player;

            if (check_win(i, j))
            {
                board[i][j] = 0;
                if (layer == 0)
                {
                    point->x = i;
                    point->y = j;
                }
                return (player == 1) ? INFINITY : -INFINITY;
            }

            int score;
            score = maxmin(point, 3 - player, depth, layer + 1, alpha, beta);

            board[i][j] = 0;

            if (player == 1)
            {

                if (score > best_score)
                {
                    best_score = score;
                    if (layer == 0)
                    {
                        point->x = i;
                        point->y = j;
                    }
                }
                if (best_score > alpha)
                {
                    alpha = best_score;
                }
                if (alpha >= beta)
                {
                    return best_score;
                }
            }
            else
            {
                if (score < best_score)
                {
                    best_score = score;
                    if (layer == 0)
                    {
                        point->x = i;
                        point->y = j;
                    }
                }
                if (best_score < beta)
                {
                    beta = best_score;
                }
                if (alpha >= beta)
                {
                    return best_score;
                }
            }
        }
    }
    return best_score;
}

int get_ai_coordinate(int *x, int *y, int player)
{

    struct Point point;
    if(!is_board_empty()){
        maxmin(&point, player, depth, 0, -INFINITY, INFINITY);

        *x = point.x + 1;
        *y = point.y + 1;
    }else{
        *x = 8;
        *y = 8;
    }
        

    return 1;
}

void set_ai_depth(int _depth)
{
    depth = _depth;
}