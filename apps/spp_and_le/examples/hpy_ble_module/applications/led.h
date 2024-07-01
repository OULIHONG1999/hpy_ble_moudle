/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-20 10:47:51
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-22 09:28:29
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\applications\led.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
//
// Created by Administrator on 2024/3/20.
//

typedef struct
{
    // bit for port state to change led
    // +---+---+---+---+---+---+---+---+---+---+---+---+
    // | 7 | 6 | 5 | 4 |   3   |   2   |   1   |   0   |
    // +---+---+---+---+---+---+---+---+---+---+---+---+
    // |   |   |   |   |isConnect|isMatch|isSelect|isOn|
    // +---+---+---+---+---+---+---+---+---+---+---+---+
    unsigned char led_status;

    // led flash time set
    unsigned char time_count;
    // led color
    unsigned int color;

} LedInfo;

