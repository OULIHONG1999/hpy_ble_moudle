/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 09:55:57
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-23 14:29:06
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\applications\led.c
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#include "led_drive.h"
#include "system/includes.h"
#include "led.h"

#define MAX_LED_NUM 4

// 红色
#define RED 0x3f0000
// 绿色
#define GREEN 0x003f00
// 蓝色
#define BLUE 0x00003f
// 黑色
#define BLACK 0x000000

#define IS_SELECTED(x) (x & 0b00000010)
#define IS_CONNECTED(x) (x & 0b00001000)
#define IS_MATCH(x) (x & 0b00000100)
#define IS_ON(x) (x & 0b00000001)
enum LED_FLASH
{
    KEEP = 0,
    SLOW = 10,
    FAST = 5
};

enum LED_STATE
{
    POWER = 0,
    SELECTED,
    UNSELECTED,
    UNCONNECTED,
    MATCHING
};

LedInfo user_led[4];

// 初始化led结构体数组的内部默认值
static void init_led_info()
{
    for (int i = 0; i < 4; ++i)
    {
        user_led[i].led_status = 0x01; // 默认未连接 非匹配模式 未选中 开显示
        user_led[i].time_count = 0;    // 计时为0
        user_led[i].color = RED;       // 未连接状态 显示红色
    }
}

static void set_led_flash(u8 index, enum LED_FLASH flash, u32 color)
{
    // 判断是否保持亮
    if (flash == KEEP)
    {
        user_led[index].time_count = 0;
        // 显示已连接的绿色
        user_led[index].color = color;
        return;
    }

    // 判断时间计数器，时间切为两段，一段显示，一段熄灭
    if (user_led[index].time_count < flash)
    {

        // 显示已连接的绿色
        user_led[index].color = color;
    }
    else if (user_led[index].time_count < flash * 2)
    {
        // 关闭显示，执行闪烁效果
        user_led[index].color = BLACK;
    }
    else
    {
        user_led[index].time_count = 0;
    }
    // printf("led %d time_count %d\n", index, user_led[index].time_count);
    user_led[index].time_count++;
}

/**
 * @brief 使用定时器控制LED灯闪烁
 * * 该函数通过遍历LED状态数组，根据不同的状态设置LED灯的闪烁模式和颜色。
 * * 对于被选中的LED灯，根据其是否已连接以及是否处于匹配模式，设置不同的闪烁速度和颜色。
 * * 对于未被选中的LED灯，已连接状态下保持常亮绿色，未连接状态下保持常亮红色。
 *
 * * 用于定时器内调用，并将当前设备端口状态传入该函数中，更新led状态
 * @param state LED状态数组，包含每个LED灯的状态信息。
 * @param len LED状态数组的长度。
 */
void led_blink_timer(unsigned char *state, unsigned char len)
{
    // 初始化LED颜色，默认为红色
    unsigned int led_color[MAX_LED_NUM] = {RED, RED, RED, RED};

    for (u8 i = 0; i < MAX_LED_NUM; i++)
    {
        // 更新LED状态
        user_led[i].led_status = state[i];
        // printf("port %d  led_state %X", i, user_led[i].led_status);
        // 判断LED是否被选中
        if (IS_SELECTED(user_led[i].led_status))
        {
            // printf("led %d 被选中\n", i);
            // 被选中LED的处理逻辑
            if (IS_CONNECTED(user_led[i].led_status))
            {

                // 已连接且被选中，设置为缓慢闪烁绿色
                set_led_flash(i, SLOW, GREEN);
            }
            else
            {
                // 未连接但被选中，匹配模式下快速闪烁蓝色，其他情况下缓慢闪烁红色
                if (IS_MATCH(user_led[i].led_status))
                {
                    set_led_flash(i, FAST, BLUE);
                }
                else
                {
                    set_led_flash(i, SLOW, RED);
                }
            }
        }
        else
        {
            // 未被选中LED的处理逻辑
            // 已连接且未被选中，设置为常亮绿色；未连接且未被选中，设置为常亮红色
            if (IS_CONNECTED(user_led[i].led_status))
            {
                set_led_flash(i, KEEP, GREEN);
            }
            else
            {
                set_led_flash(i, KEEP, RED);
            }
        }
        // 记录当前LED的颜色
        led_color[i] = user_led[i].color;
    }

    // 将颜色设置应用到LED灯
    set_led_color(led_color, MAX_LED_NUM);
}

// 开机动画函数
void led_power_on_animation()
{
    printf("开机动画");
    // 初始化LED颜色，默认为红色
    unsigned int led_color[5][MAX_LED_NUM] =
        {
            {BLACK, BLACK, BLACK, BLACK},
            {RED, BLACK, BLACK, BLACK},
            {RED, RED, BLACK, BLACK},
            {RED, RED, RED, BLACK},
            {RED, RED, RED, RED},
        };
    set_led_color(&led_color[0][0], MAX_LED_NUM);
    os_time_dly(50);
    set_led_color(&led_color[1][0], MAX_LED_NUM);
    os_time_dly(50);
    set_led_color(&led_color[2][0], MAX_LED_NUM);
    os_time_dly(50);
    set_led_color(&led_color[3][0], MAX_LED_NUM);
    os_time_dly(50);
    set_led_color(&led_color[4][0], MAX_LED_NUM);
    os_time_dly(50);
    set_led_color(&led_color[0][0], MAX_LED_NUM);
    os_time_dly(50);
    set_led_color(&led_color[4][0], MAX_LED_NUM);
    os_time_dly(50);
}

/// @brief  初始化LED灯
/// @param  void
void init_user_led(void)
{
    init_ws2812b();
    init_led_info();
    printf("init_user_led success \r\n");
    led_power_on_animation();
}
