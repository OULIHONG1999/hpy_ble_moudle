/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 10:30:03
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-21 15:02:38
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\hardware_drive\led_drive.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

enum Color {
    RED = 0xFF0000,
    GREEN = 0x00FF00,
    BLUE = 0x0000FF,
    YELLOW = 0xFFFF00,
    CYAN = 0x00FFFF,
    MAGENTA = 0xFF00FF,
    BLACK = 0x000000,
    WHITE = 0xFFFFFF
};

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGBColor;

void init_ws2812b();
void set_led_rgb_color(unsigned char r, unsigned char g, unsigned char b, unsigned char index);

void set_led_color(unsigned int *color_buf, unsigned char len);

