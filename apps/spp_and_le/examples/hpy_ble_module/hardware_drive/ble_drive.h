/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 18:04:44
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-03-30 11:48:03
 * @FilePath: \蓝牙拓展模块\apps\spp_and_le\examples\hpy_ble_module\hardware_drive\ble_drive.h
 * @Description:
 *
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */
#pragma once

enum CONNECT_MODE
{
    // 匹配模式
    MATCH,
    // 普通模式
    NOMAL,
    // 通过mac地址匹配
    MAC = NOMAL,
    // 通过名称匹配
    NAME = MATCH
};

// 定义蓝牙连接状态的枚举类型
enum BLE_CONNECT_STATE
{
    DISCONNECT = 0, // 表示设备处于未连接状态
    CONNECTING,     // 表示设备正在尝试建立连接
    CONNECTED = 3,  // 表明设备已成功建立连接
    DISCONNECTING   // 表示设备正在进行断开连接的操作
};

enum VM_DATA
{
    VM_ID,
    VM_MAC,
    VM_NAME
};

typedef struct
{
    char name[50];               // 蓝牙名称，假设最长为50个字符
    unsigned char macAddress[6]; // MAC地址，通常为 6 字节十六进制表示
    unsigned char handle;        // 句柄
    unsigned char id;            // ID
    enum BLE_CONNECT_STATE connect_state;

} BleDevice;

void set_connect_mode(enum CONNECT_MODE mode);

void clear_match_mode();

void register_ble_device(BleDevice **ble_s);

void init_ble_local_data();

void disconnect_ble_device(unsigned char index);

void set_ble_device_info(unsigned char *data, unsigned char len);

enum CONNECT_MODE get_connect_mode(void);

 void register_ble_receive_callback(void (*ble_receive_callback)(unsigned char *, unsigned char, unsigned char));

void device_lost(unsigned char handle);
