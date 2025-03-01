#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H


#define LEGAL 0
#define OUT_OF_BOARD 1
#define OCCUPIED 2
#define FORBIDDEN 3



void init_game_logic(int (*board)[15]);
int is_move_illegal(int x, int y, int player);
int check_win(int x, int y);
int in_board(int x, int y);
int get_ai_coordinate(int *x, int *y,int player);
int evaluate_board();
void change_forbidden_rule(int on);
void set_ai_depth(int depth);
int get_forbidden_rule();

#endif // GAME_LOGIC_H