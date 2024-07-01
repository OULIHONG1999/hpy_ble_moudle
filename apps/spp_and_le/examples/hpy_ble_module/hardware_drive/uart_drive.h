/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 14:11:03
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-07 14:11:03
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\hardware_drive\uart_drive.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */

#pragma once
void user_uart_init(u32 baud);

void user_uart_send(u8 *buf, u8 len);

void uart_callback_register(void (*callback)(unsigned char *, unsigned char));
