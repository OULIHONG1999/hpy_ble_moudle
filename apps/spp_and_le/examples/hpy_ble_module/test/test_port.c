#include <stdio.h>
#define BLE_NUMBER_MAX 4
#define uint8_t unsigned char
// 假设结构体定义
typedef struct {
    uint8_t handle;
    uint8_t id;
} BleDevice;

// 假设全局变量定义
BleDevice ble_sensor[BLE_NUMBER_MAX];

// 测试函数
void test_process_ble_data()
{
    uint8_t port_state = 0x00;
    for (uint8_t i = 0; i < BLE_NUMBER_MAX; i++)
    {
        if (ble_sensor[i].handle != 0)
        {
            port_state |= 0x08;
            printf("Device ID: %d, Existence: Present\n", ble_sensor[i].id);
        }
        else
        {
            port_state &= 0x07;
            printf("Device ID: %d, Existence: Absent\n", ble_sensor[i].id);
        }
        if (i != BLE_NUMBER_MAX - 1) // 注意这里调整为避免越界
            port_state >>= 1;
    }

    // 打印最终的 port_state，仅用于测试观察
    printf("Final port_state: 0x%02X\n", port_state);
}

// 测试用例函数
void test_case_process_ble_data()
{
    // 初始化测试数据
    for (uint8_t i = 0; i < BLE_NUMBER_MAX; i++)
    {
        ble_sensor[i].handle = i % 2; // 假设有偶数个设备存在
        ble_sensor[i].id = i;
    }

    // 调用被测试函数
    test_process_ble_data();
}

int main()
{
    // 执行测试用例
    test_case_process_ble_data();

    return 0;
}