/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 10:58:20
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-21 16:10:22
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\hardware_drive\key_drive.h
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#pragma once
#include "system/event.h"

#define PORT_UNSELECTED 0xff

enum EVENT
{
    NULL_KEY     = 255,
    CLICK        = KEY_EVENT_CLICK,
    DOUBLE_CLICK = KEY_EVENT_DOUBLE_CLICK,
    THREE_CLICK  = KEY_EVENT_TRIPLE_CLICK,
    LONG_PRESS   = KEY_EVENT_LONG,
};

typedef struct
{
    enum EVENT event;
    void (*callback_fun)();
} key_event_callback_info;

void user_key_event_handler(struct sys_event *event);

void key_event_register(enum EVENT event, void (*callback_fun)());
