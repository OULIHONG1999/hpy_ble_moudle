/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 09:58:04
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-23 14:11:05
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\hardware_drive\led_drive.c
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */

#include "system/includes.h"

#include "asm/ledc.h"
#include <asm/gpio.h>
#include "led_drive.h"

#define LED_NUM_MAX (4 * 3)

static u8 led_rgb_val_buf[3 * LED_NUM_MAX] __attribute__((aligned(4)));

const struct ledc_platform_data my_ledc0_data = {
    .index = 0,
    .port = IO_PORTA_07,
    .idle_level = 0,
    .out_inv = 0,
    .bit_inv = 1,
    .t_unit = t_42ns,
    .t1h_cnt = 24,
    .t1l_cnt = 7,
    .t0h_cnt = 7,
    .t0l_cnt = 24,
    .t_rest_cnt = 20000,
    .cbfun = NULL,
};

void ledc_init(const struct ledc_platform_data *arg);
void ledc_rgb_to_buf(u8 r, u8 g, u8 b, u8 *buf, int idx);
void ledc_send_rgbbuf_isr(u8 index, u8 *rgbbuf, u32 led_num, u16 again_cnt);
/***********************************************************************************************
 *                      接口替换区
 ***********************************************************************************************/
// #define led_init() ledc_init(&my_ledc0_data)
#define led_rgb_to_buf(r, g, b, buf, idx) ledc_rgb_to_buf(r, g, b, buf, idx)
#define led_send_rgbbuf(buf, num) ledc_send_rgbbuf_isr(0, buf, num, 1)

void init_ws2812b()
{
    //    led_init();
    ledc_init(&my_ledc0_data);
}

static void hex_to_rgb(uint32_t color, RGBColor *rgb)
{
    rgb->r = (color >> 16) & 0xFF; // 取得红色分量
    rgb->g = (color >> 8) & 0xFF;  // 取得绿色分量
    rgb->b = color & 0xFF;         // 取得蓝色分量
}

void set_led_rgb_color(u8 r, u8 g, u8 b, u8 index)
{
    led_rgb_to_buf(r, g, b, led_rgb_val_buf, index);
}

/**
 * * 设置LED颜色
 * * 本函数用于根据传入的颜色缓冲区和长度，将颜色信息转换为LED能够识别的格式，并发送给LED设备。
 * * @param color_buf 颜色缓冲区，包含需要设置的LED颜色的十六进制表示。
 * @param len 颜色缓冲区的长度，表示需要设置的LED数量。
 */
void set_led_color(u32 *color_buf, u8 len)
{
    RGBColor rgb; // 定义一个RGB颜色结构体变量，用于临时存储转换后的RGB颜色。

    // printf("%06X %06X %06X %06X", color_buf[0], color_buf[1], color_buf[2], color_buf[3]);
    // 遍历颜色缓冲区，逐个处理每个颜色值。
    for (u8 i = 0; i < len; i++)
    {
        hex_to_rgb(color_buf[i], &rgb);
        // 将RGB颜色转换为LED设备需要的格式，并存储到LED的缓冲区中。
        led_rgb_to_buf(rgb.r, rgb.g, rgb.b, led_rgb_val_buf, i);
    }

    // put_buf(led_rgb_val_buf, len * 3);
    // 发送LED颜色缓冲区到LED设备。
    led_send_rgbbuf(led_rgb_val_buf, len * 3);
}
