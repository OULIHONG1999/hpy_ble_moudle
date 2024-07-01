/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 09:55:05
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-23 09:24:11
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\applications\key.c
 * @Description: 按键操作管理
 * 1、按键的初始化
 * 2、不同按键功能的实现
 * 3、按键事件回调的注册
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */

#include "key_drive.h"
#include "ble_drive.h"
#define IS_KEY_KOCK()     \
    if (is_lock_key == 1) \
    {                     \
        return;           \
    }

static u8 current_port = 0;
static u32 time_count = 0;
static u8 is_lock_key = 1;
static void reset_time_count()
{
    time_count = 0;
}

u8 get_selected_port()
{
    time_count++;
    // printf("time_count:%d\n", time_count);

    // 计时，当超过阈值就锁定键
    if (time_count >= 1000 && is_lock_key != 1)
    {
        reset_time_count();
        is_lock_key = 1;
    }

    // 如果锁定了按键，就返回0Xff
    if (is_lock_key == 1)
    {
        // printf("KEY IS LOCK\n");
        return PORT_UNSELECTED;
    }

    return current_port;
}
/// @brief  端口选择函数，通过单击切换当前选择的端口
void select_port()
{
    printf("select port %d\n", current_port);
    IS_KEY_KOCK();
    if (get_connect_mode() == MATCH)
    {
        printf("match mode\n");
        return;
    }

    // 响应一次，端口加1，端口为0~3，四个端口
    // 当端口移到最后一个时，下一次响应就回到首个端口
    current_port++;
    if (current_port >= 4)
    {
        current_port = 0;
    }
    reset_time_count();
}

/// @brief 退出配对模式
void exit_pairing_mode()
{
    printf("exit pairing mode\n");
    IS_KEY_KOCK();
    // 设置蓝牙连接方式为按地址连接
    set_connect_mode(NOMAL);
    // 设置当前端口变为选中状态 且未连接状态
    reset_time_count();
}

/// @brief 开启配对模式
void enable_match_mode()
{
    printf("enable pairing mode\n");
    IS_KEY_KOCK();
    // 设置蓝牙连接方式为按名称连接
    set_connect_mode(MATCH);
    // 断开当前端口的蓝牙连接
    disconnect_ble_device(current_port);
    reset_time_count();
}

/// @brief 解锁按键误触，让按键可以操作
void unlock_key()
{
    printf("unlock key\n");
    reset_time_count();
    is_lock_key = 0;
}

void init_user_key_event()
{
    // 按键的初始化由自带的SDK实现

    // 注册按键的事件回调函数
    key_event_register(CLICK, select_port);
    key_event_register(DOUBLE_CLICK, exit_pairing_mode);
    key_event_register(THREE_CLICK, unlock_key);
    key_event_register(LONG_PRESS, enable_match_mode);

    printf("key event init ok\n");
}