
#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

void print_menu()
{
    system("cls");
    printf(HIDE_CURSOR);
    printf("*******************************************\n");
    printf("*            歡迎使用五子棋               *\n");
    printf("*            請選擇對戰方式               *\n");
    printf("*            1.人人對戰                   *\n");
    printf("*            2.遠程連線(還沒做)           *\n");
    printf("*            3.人機對戰                   *\n");
    printf("*            4.設定                       *\n");
    printf("*            5.說明                       *\n");
    printf("*            6.退出                       *\n");
    printf("*******************************************\n");
    printf("請選擇:");
}

void print_game_over_option()
{
    system("cls");

    printf("********************************************\n");
    printf("*            遊戲結束                      *\n");
    printf("*            1.返回主菜單                  *\n");
    printf("*            2.復盤                        *\n");
    printf("*            3.退出                        *\n");
    printf("********************************************\n");
}

void print_help_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*                                         *\n");
    printf("*          請當作這裡有詳細的說明         *\n");
    printf("*                                         *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入設定選項               *\n");
    printf("*            1.時間限制                   *\n");
    printf("*            2.輸入模式                   *\n");
    printf("*            3.禁手規則                   *\n");
    printf("*            4.AI設定                     *\n");
    printf("*            5.返回主菜單                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_ai_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入ai選項                 *\n");
    printf("*            1.AI難度                     *\n");
    printf("*            2.AI先後手                   *\n");
    printf("*            3.返回上一層                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_time_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入時間限制               *\n");
    printf("*            1.15秒                       *\n");
    printf("*            2.30秒                       *\n");
    printf("*            3.60秒                       *\n");
    printf("*            4.無限制                     *\n");
    printf("*            5.返回上一層                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_input_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入輸入模式               *\n");
    printf("*            1.數字鍵                     *\n");
    printf("*            2.WASD                       *\n");
    printf("*            3.返回上一層                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_forbidden_rule_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入禁手規則               *\n");
    printf("*            1.開啟                       *\n");
    printf("*            2.關閉                       *\n");
    printf("*            3.返回上一層                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_ai_level_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入ai難度                 *\n");
    printf("*            1.簡單                       *\n");
    printf("*            2.中等                       *\n");
    printf("*            3.困難(可能要比較久)         *\n");
    printf("*            4.返回上一層                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}

void print_ai_first_player_setting_info()
{
    system("cls");

    printf("*******************************************\n");
    printf("*            請輸入AI先後手               *\n");
    printf("*            1.玩家先手                   *\n");
    printf("*            2.AI先手                     *\n");
    printf("*            3.返回上一層                 *\n");
    printf("*******************************************\n");
    printf("請按任意鍵返回主菜單\n");
}