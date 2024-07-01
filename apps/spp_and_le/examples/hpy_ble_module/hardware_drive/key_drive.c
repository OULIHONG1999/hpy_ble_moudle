#include "system/includes.h"
#include <asm/gpio.h>
#include "key_drive.h"

/*
1、本模块使用系统自带的按键处理程序
2、分别在 board_ac632n_demo_cfg.h  和 board_ac632n_demo_cfg.c添加和配置iokey相关的内容
3、按键事件的回调在 central_key_event_handler()函数内部
4、按键事件通过事件消息转发的形式，通过调用user_keyy_event_handler()，在本文件中对注册按键事件回调的函数，进行处理
*/

#define MAX_KEY_ENENT_CALLBACK_NUM 10
static key_event_callback_info key_cab[MAX_KEY_ENENT_CALLBACK_NUM]; // 按键事件注册列表

/// @brief 当有按键事件发生时，会调用此函数，根据按键事件类型响应对应的已注册的回调函数
/// @param event
void user_key_event_handler(struct sys_event *event)
{
    int event_type = event->u.key.event;
    // printf("user_key_event_handler event_type:%d\n", event_type);
    for (int i = 0; i < MAX_KEY_ENENT_CALLBACK_NUM; ++i)
    {
        if (key_cab[i].callback_fun != NULL && event_type == key_cab[i].event)
        {
            // printf("callback fun is %d   index %d", key_cab[i].callback_fun, i);
            key_cab[i].callback_fun();
        }
    }
}

/// @brief 按键事件的注册函数，根据需要响应的按键操作，注册不同的回调函数，当发生对应的按键消息时，
//  响应所有已经注册的回调函数
/// @param
/// @param callback_fun
void key_event_register(enum EVENT event, void (*callback_fun)())
{
    for (int i = 0; i < MAX_KEY_ENENT_CALLBACK_NUM; ++i)
    {

        if (key_cab[i].callback_fun == NULL) // 没有事件 就添加对应事件
        {

            key_cab[i].event = event;
            key_cab[i].callback_fun = callback_fun;
            printf("register callback fun %d   event %d", key_cab[i].callback_fun, key_cab[i].event);
            return;
        }
        else if (i == MAX_KEY_ENENT_CALLBACK_NUM - 1) // 如果注册的数量超过最大值，则进行报错
        {
            printf("key event callback fun full\n");
        }
    }
}