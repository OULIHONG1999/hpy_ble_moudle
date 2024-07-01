/*
 * @Author: OULIHONG 1756950720@qq.com
 * @Date: 2024-03-07 09:54:43
 * @LastEditors: OULIHONG 1756950720@qq.com
 * @LastEditTime: 2024-04-09 11:59:42
 * @FilePath: \蓝牙拓展模块-2024年4月8日\apps\spp_and_le\examples\hpy_ble_module\applications\user_main.c
 * @Description:
 *   初始化各个模块内容
 * 1 初始化蓝牙部分的信息
 *      1) 设备id
 *      2) mac地址
 *      3) 蓝牙名称
 * 2 初始化串口模块
 * 3 初始化按键模块
 * 4 初始化led模块
 *
 * 创建结构体 保存和关联设备的状态信息
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved.
 */

#include "ble_drive.h"
#include "system/includes.h"
#include "key.h"
#include "user_main.h"
#include "led.h"
#include "uart_drive.h"
#define BLE_DATA_BUFFER_MAX 30
#define BLE_NUMBER_MAX 4

typedef struct
{
    BleDevice *ble_device;               // 蓝牙设备信息
    u8 sensor_data[BLE_DATA_BUFFER_MAX]; // 蓝牙设备数据
    u8 sensor_data_len;                  // 蓝牙数据长度
} BleSensor;

static u8 port_cmd[10] = {0};
static u8 data_cmd[BLE_NUMBER_MAX][60] = {0};

static BleSensor ble_sensor;

void init_ble_sensor(void)
{
    // 初始化本地蓝牙信息
    init_ble_local_data();
    // 绑定蓝牙设备信息

    register_ble_device(&ble_sensor.ble_device);
}

#define SELECTED 0b00000010
#define MATCHING 0b00000100
#define CONNECT 0b00001000
void updata_port_timer()
{
    LedInfo LED[4];
    u8 port_state[4] = {0};
    u8 selected_port = 0;
    // 取得当前设备选中状态
    selected_port = get_selected_port();
    enum CONNECT_MODE connect_mode = get_connect_mode();
    // 取得当前蓝牙连接状态
    for (u8 i = 0; i < BLE_NUMBER_MAX; i++)
    {
        LED[i].led_status = 0;
        // 判断端口是否被选中
        if (selected_port == i)
        {
            // 端口被选中
            LED[i].led_status |= SELECTED;
            // 按名字连接，则为匹配模式
            if (connect_mode == MATCH)
            {
                // 设置为匹配模式
                LED[i].led_status |= MATCHING;
            }
            else
            {
                // 清除匹配模式
                LED[i].led_status &= ~MATCHING;
            }
        }
        else
        {
            // 清除选中状态和匹配状态
            LED[i].led_status &= ~(SELECTED | MATCHING);
        }

        // printf("port %d ble status is %d\n", i, ble_sensor.ble_device[i].connect_state);

        // 判断端口是否连接
        if (ble_sensor.ble_device[i].connect_state == CONNECTED)
        {
            // printf("port %d is connected\n", i);
            LED[i].led_status |= CONNECT;
        }
        else
        {
            LED[i].led_status &= ~CONNECT;
        }

        // 更新临时状态数组，便于传入led_blink_timer()函数
        port_state[i] = LED[i].led_status;
    }

    // put_buf(port_state, BLE_NUMBER_MAX);
    // 设置led闪烁
    extern void led_blink_timer(unsigned char *state, unsigned char len);
    led_blink_timer(port_state, BLE_NUMBER_MAX);
}

void init_cmd()
{
    port_cmd[0] = 0x20; // 串口识别 包头
    port_cmd[1] = 0x86; // 传感器名称id
    port_cmd[2] = 0x32; // crc
    port_cmd[3] = 0x0A; // 端口命令，用于发送当前端口状态，显示端口上有哪些传感器
    port_cmd[9] = 0xff; // 结束符，用于标识数据包的结束
    for (u8 i = 0; i < BLE_NUMBER_MAX; i++)
    {
        data_cmd[i][0] = 0x20; // 串口识别 包头
        data_cmd[i][1] = 0x86; // 传感器名称id
        data_cmd[i][2] = 0x32; // crc
        data_cmd[i][3] = 0x0B; // 传感器命令，用于发送传感器数据，显示传感器数据信息
    }
}

// 初始化外设
void init_ble_drive(void)
{
    // 初始化蓝牙设备
    init_ble_sensor();
    // 初始化串口
    user_uart_init(1000000);
    // 初始化led灯
    init_user_led();
    // 初始化按键
    init_user_key_event();
    // 初始化串口数据包
    init_cmd();
}
extern void ble_send_data(u8 *data, u8 len, u8 handle);
void send_ble_data_timer()
{

    u8 test_data[20] = {0};
    for (u8 i = 0; i < 20; i++)
    {
        test_data[i] = i;
    }

    for (u8 i = 0; i < BLE_NUMBER_MAX; i++)
    {
        if (ble_sensor.ble_device[i].handle != 0)
        {
            ble_send_data(ble_sensor.sensor_data, 20, ble_sensor.ble_device[i].handle);
        }
    }
}

void send_uart_data_timer()
{
    static u8 flag = 0;
    u8 data_len = 0;
    if (flag == 4)
    {
        printf("send port cmd\n");

        // 实际需要计算crc，再进行发送
        user_uart_send(port_cmd, 10);
        flag = 0;
    }
    else
    {
        if (port_cmd[flag + 4] != 0)
        {
            printf("send data cmd %02X", port_cmd[flag + 4]);

            data_len = data_cmd[flag][5] + 7;    // 数据长度，包括id和crc和数据长度和数据本身和结束符0xff，其中数据长度本身占一个字节，数据本身占data_cmd[flag][5]个字节，id和crc各占一个字节，结束符占一个字节，共7个字节。
            data_cmd[flag][data_len - 1] = 0xff; // 结束符，用于标识数据包的结束。
            // 实际需要计算crc，再进行发送
            user_uart_send(&data_cmd[flag][0], data_len);
        }
    }
    flag++;
}

void uart_command(u8 *data, u8 len)
{

    if (data[0] == 0xab && ble_sensor.ble_device[data[1]].handle != 0)
    {
        ble_send_data(data, len, ble_sensor.ble_device[data[1]].handle);
    }
}

/**
 * @brief 蓝牙接收回调函数
 *
 * 此函数用于处理接收到的蓝牙数据。当蓝牙模块接收到数据时，会调用此回调函数。
 * 函数会根据接收到的数据的handle（句柄），将数据存储到相应的传感器数据结构中。
 *
 * @param data 接收到的蓝牙数据，类型为u8数组。
 * @param len 接收到的数据长度，单位为字节。
 * @param handle 数据的句柄，用于标识数据的来源或目的地。
 */
void ble_receive_callback(u8 *data, u8 len, u8 handle)
{

    /* 
    data: 包头 传感器名称
     */

    // printf("handle:%d\n", handle);
    put_buf(data, len);
    // 根据handle寻找对应的传感器数据存储在data_cmd的位置
    for (u8 i = 0; i < BLE_NUMBER_MAX; i++)
    {
        if (handle == ble_sensor.ble_device[i].handle)
        {
            // 设置id，传感器在数组中的序号
            data_cmd[i][4] = i;
            // 设置长度
            data_cmd[i][5] = 1;
            // 取得数据
            memcpy(&data_cmd[i][6], &data[2], len);
            // 取得传感端口名称
            port_cmd[i + 4] = data[1];
            break;
        }
    }
}

void user_main_thread(void *arg)
{
    // 初始化硬件和软件驱动
    init_ble_drive();

    uart_callback_register(uart_command);

    register_ble_receive_callback(ble_receive_callback);

    // 创建定时器，刷新端口状态，并刷新led灯
    sys_hi_timer_add(NULL, updata_port_timer, 50);
    // 创建定时器，发送数据
    // sys_hi_timer_add(NULL, send_ble_data_timer, 30);
    // 创建定时器，发送串口数据
    sys_hi_timer_add(NULL, send_uart_data_timer, 1000);
    while (1)
    {
        wdt_clear();
        os_time_dly(1);
    }
}
