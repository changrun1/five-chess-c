#ifndef MENU_H
#define MENU_H

// ANSI取消光標
#define HIDE_CURSOR "\033[?25l"

void print_menu();
void print_game_over_option();
void print_help_info();
void print_setting_info();
void print_time_setting_info();
void print_input_setting_info();
void print_forbidden_rule_setting_info();
void print_first_player_setting_info();
void print_ai_level_setting_info();
void print_online_setting();
void print_client_setting();
void print_waiting_for_player();
void print_connection_failed();
void print_input_ip();



#endif // MENU_H