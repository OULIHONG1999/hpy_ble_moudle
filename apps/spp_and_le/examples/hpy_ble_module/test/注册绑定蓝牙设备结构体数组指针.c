#include <stdio.h>

// 假设已定义一个BleDevice结构体以及一个全局实例ble_sensor_device
typedef struct
{
    char device_name[32];
    int device_id;
    // 其他BLE设备相关的属性...
} BleDevice;

typedef struct
{
    BleDevice *ble_s;
} UserBleDevice;

// 假设在某个全局区域已经有以下定义：
BleDevice ble_sensor_device[4] = {
    {"Sensor Device 1", 1},
    {"Sensor Device 2", 2}};

// 定义register_ble_device函数，与问题中一致
void register_ble_device(BleDevice **ble_s)
{
    *ble_s = ble_sensor_device;
}

int main()
{
    UserBleDevice user_dev; // 创建一个指向BleDevice类型的指针变量，用于接管注册的BLE设备信息。
    user_dev.ble_s = NULL;  // 初始化为NULL，确保在注册之前没有指向任何设备。

    // 调用register_ble_device函数进行设备注册
    register_ble_device(&user_dev.ble_s);

    // 测试：检查注册是否成功，通过访问registered_device指向的设备信息
    printf("Registered BLE Device Name: %s\n", user_dev.ble_s[0].device_name);
    printf("Registered BLE Device ID: %d\n", user_dev.ble_s[1].device_id);
    printf("Registered BLE Device Name: %s\n", user_dev.ble_s[0].device_name);
    printf("Registered BLE Device ID: %d\n", user_dev.ble_s[1].device_id);

    user_dev.ble_s[0].device_id = 3;
    printf("Registered BLE Device ID: %d\n", user_dev.ble_s[0].device_id);
    return 0;
}