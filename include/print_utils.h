#ifndef BOARD_UTILS_H
#define BOARD_UTILS_H

void print_board(int row, int col);
void update_info(char *str);
void append_info(char *str);
void append_info_char(char c);
void remove_info_last_char();
void update_time_info(unsigned long long start_time, int time_limit);
void remove_info_coord();
void init_print_utils(int board[15][15]);

#endif // BOARD_UTILS_H