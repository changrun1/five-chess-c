#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H

#include <stdio.h>

typedef struct
{
    int x;
    int y;
    int result;
    int state;//0 : 未完成 3: 完成狀態 4 : 悔棋 5 : 超時 
} InputState;



int get_keyboard_input();
void clear_input_buffer();
int get_coordinate(InputState *state, int mode, int turn, int player, int input_mode);
void init_input_state(InputState *state);



#endif // INPUT_HANDLING_H