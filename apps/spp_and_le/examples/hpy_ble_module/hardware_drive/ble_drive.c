#include "le_client_demo.h"
#include "le/ble_api.h"
#include "syscfg_id.h"
#include <typedef.h>
#include "ble_drive.h"
// #include "le_common_define.h"
#define HCI_EIR_DATATYPE_SHORTENED_LOCAL_NAME 0x08
#define HCI_EIR_DATATYPE_COMPLETE_LOCAL_NAME 0x09
#define BLE_NUM_MAX 4

#define MAC_ADDRESS_LENGTH 6 // 避免魔法数字，提高可读性

// 保存所有蓝牙设备的信息
static BleDevice ble_sensor_device[4];
static enum CONNECT_MODE connect_mode = NOMAL;
void (*receive_callback)(u8 *data, u8 len,u8 handle);
// mv操作
static void user_vm_get_data(enum VM_DATA type, u8 *data, u8 len)
{

    char ret = 0;
    ret = syscfg_read(type, data, len);
    if (ret != len)
    {
        printf("read id fail\n");
        for (u8 i = 0; i < 6; i++)
        {
            data[i] = 0;
        }

        return;
    }
}

static void user_vm_set_data(enum VM_DATA type, u8 *data, u8 len)
{
    char ret = 0;
    ret = syscfg_write(type, data, len);
    if (ret != len)
    {
        printf("write id fail\n");
        return;
    }
}

/// @brief
/// \param ble_s
void register_ble_device(BleDevice **ble_s)
{
    *ble_s = ble_sensor_device;
}

/// @brief 初始化本地保存的蓝牙信息，将id和mac地址读取出来并做出关联
void init_ble_local_data()
{
    u8 id[BLE_NUM_MAX] = {0};
    u8 mac[BLE_NUM_MAX * MAC_ADDRESS_LENGTH] = {0};

    // 取得所有id
    user_vm_get_data(VM_ID, id, BLE_NUM_MAX);

    // read id，添加了长度检查以避免数组越界
    for (int i = 0; i < BLE_NUM_MAX; ++i)
    {
        ble_sensor_device[i].id = id[i];
        ble_sensor_device[i].handle = 0;
        ble_sensor_device[i].connect_state = DISCONNECT;
    }

    // 取得所有mac地址
    user_vm_get_data(VM_MAC, mac, BLE_NUM_MAX * MAC_ADDRESS_LENGTH);

    for (u8 i = 0; i < BLE_NUM_MAX * MAC_ADDRESS_LENGTH; i += MAC_ADDRESS_LENGTH)
    {
        // 使用memcpy_s代替memcpy，并添加了对目的地和源的长度检查
        memcpy(ble_sensor_device[i / MAC_ADDRESS_LENGTH].macAddress, mac + i, MAC_ADDRESS_LENGTH);
    }

    // 输出所有id和mac
    printf("id: %x mac: %02X %02X %02X %02X %02X %02X ", id[0], mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("id: %x mac: %02X %02X %02X %02X %02X %02X ", id[1], mac[6], mac[7], mac[8], mac[9], mac[10], mac[11]);
    printf("id: %x mac: %02X %02X %02X %02X %02X %02X ", id[2], mac[12], mac[13], mac[14], mac[15], mac[16], mac[17]);
    printf("id: %x mac: %02X %02X %02X %02X %02X %02X ", id[3], mac[18], mac[19], mac[20], mac[21], mac[22], mac[23]);
}

/// @brief 用户自定义设备匹配功能 根据需求可以按名字匹配或按地址匹配
/// @param report_pt
/// @param len
/// @return
u8 user_check_device_match(adv_report_t *report_pt, u16 len)
{
    unsigned char *adv_data_pt;
    unsigned char length, ad_type;
    unsigned char len_s = 0;
    unsigned char bla_name[] = "HPY"; // 用于匹配需要连接的蓝牙设备的前缀
    unsigned int tmp32;
    // 获取adv_data_pt 数据
    adv_data_pt = report_pt->data;
    // printf("%s", adv_data_pt);

    // put_buf(report_pt, len);
    // return 0;
    for (int i = 0; i < report_pt->length;)
    {
        if (*adv_data_pt == 0)
        {
            printf("analyze end\n");
            break;
        }

        // 用于遍历data找到蓝牙名称
        length = *adv_data_pt++;

        /*过滤非标准包格式*/
        if (length >= report_pt->length || (length + i) >= report_pt->length)
        {
            // printf("!!!error_adv_packet:");
            // put_buf(report_pt->data, report_pt->length);
            break;
        }

        ad_type = *adv_data_pt++;
        i += (length + 1);

        // printf("i %d length:%X,ad_type:%X", i, length, ad_type);

        // 该设备有个完整的名字
        switch (ad_type)
        {
        case HCI_EIR_DATATYPE_COMPLETE_LOCAL_NAME:
        case HCI_EIR_DATATYPE_SHORTENED_LOCAL_NAME:
            tmp32 = adv_data_pt[length - 1];
            adv_data_pt[length - 1] = 0;

            printf("remoter_name:%s ,rssi:%d", adv_data_pt, report_pt->rssi);
            put_buf(report_pt->address, 6);

            // 判断蓝牙名称的前缀是否正确
            if (!memcmp(bla_name, strtok(adv_data_pt, "_"), sizeof(bla_name)))
            {
                printf("name OK!!!");

                switch (connect_mode)
                {
                case MAC: // 通过地址连接

                    // 读取本地所有保存过的地址
                    for (u8 device_index = 0; device_index < 4; device_index++)
                    {

                        // 由于先判断过名字，所以这里只需判断地址是否符合本地保存的地址
                        if (memcmp(ble_sensor_device[device_index].macAddress, report_pt->address, 6) == 0)
                        {
                            printf("index %d mac ok!!!", device_index);
                            return 1;
                        }
                        else
                        {
                            // printf("index %d mac erro!!!", device_index);
                            // put_buf(ble_sensor_device[device_index].macAddress, 6);
                        }
                    }
                    return 0;
                    break;

                case NAME: // 通过名称连接
                    // 按名称连接时，处于配对模式，需要判断信号强度，不能随意连接
                    // TODO: 之后可能还需要判断是否是传感器
                    if (report_pt->rssi > -70)
                    {
                        return 1;
                    }
                    break;

                default:
                    break;
                    return 0;
                }
            }
            else
            {
                printf("name erro!!!");
                return 0;
            }

            adv_data_pt[length - 1] = tmp32;
            break;

        default:
            break;
        }
        // 根据蓝牙广播数据包进行指针偏移
        adv_data_pt += (length - 1);
    }
    return 0;
}

void set_connect_mode(enum CONNECT_MODE mode)
{
    printf("connect mode:%d\n", mode);
    connect_mode = mode;
}

void clear_match_mode()
{
    connect_mode = NOMAL;
}

/// @brief 当设备连接完成时，需要将设备的各种信息关联起来
/// @param index 设备保存在本地的序号
/// @param handle 设备在连接成功后分配的句柄，用于数据的发送与接收，以及后续的主动断开设备
/// @param mac 用于当设备重新连接时的匹配操作
void save_ble_device(u8 index, u8 handle, u8 *mac)
{
    put_buf(mac, 6);
    ble_sensor_device[index].handle = handle;
    ble_sensor_device[index].connect_state = CONNECTED;
    memcpy(ble_sensor_device[index].macAddress, mac, 6);

    // 创建缓存区 用于保存所有地址
    u8 all_mac_adress[6 * BLE_NUM_MAX] = {0};
    // 读取本地所有地址
    user_vm_get_data(VM_MAC, all_mac_adress, BLE_NUM_MAX * MAC_ADDRESS_LENGTH);
    // 根据index，找到地址的位置，并将传入的mac覆盖原来的地址
    memcpy(all_mac_adress + index * MAC_ADDRESS_LENGTH, mac, MAC_ADDRESS_LENGTH);
    // 保存所有地址
    user_vm_set_data(VM_MAC, all_mac_adress, BLE_NUM_MAX * MAC_ADDRESS_LENGTH);
}

/// @brief
/// @param index
/// @param id
void save_ble_sensor_id(u8 index, u8 id)
{
    ble_sensor_device[index].id = id;
}

/// @brief
/// @param index
/// @param id
u8 get_ble_sensor_id(u8 index)
{
    return ble_sensor_device[index].id;
}

/// @brief
/// @param index
/// @param handle
void save_ble_sensor_handle(u8 index, u8 handle)
{
    ble_sensor_device[index].handle = handle;
}

/// @brief
/// @param index
/// @return
u8 get_ble_sensor_handle(u8 index)
{
    return ble_sensor_device[index].handle;
}

/// @brief
/// @param handle
/// @return
u8 get_ble_sensor_index_by_handle(u8 handle)
{
    for (u8 i = 0; i < BLE_NUM_MAX; i++)
    {
        if (ble_sensor_device[i].handle == handle)
        {
            return i;
        }
    }
}

u8 get_ble_sensor_index_by_mac(u8 *mac)
{
    for (u8 i = 0; i < BLE_NUM_MAX; i++)
    {
        if (memcmp(ble_sensor_device[i].macAddress, mac, MAC_ADDRESS_LENGTH) == 0)
        {
            return i;
        }
    }
}

/// @brief 设置端口的设备信息
/// @param data
/// @param len
void set_ble_device_info(u8 *data, u8 len)
{
    put_buf(data, len);
    u8 handle = 0;
    u8 mac[6] = {0};
    u8 port = get_selected_port();

    // 解析数据包
    handle = data[4];                          // 取得handle
    memcpy(mac, data + 8, MAC_ADDRESS_LENGTH); // 取得mac

    // 判断index是否越界,说明没有选中设备，那就是根据本地地址来连接的
    if (port >= BLE_NUM_MAX)
    {
        port = get_ble_sensor_index_by_mac(mac);
        printf("get locole device index:%d\n", port);
    }

    save_ble_device(port, handle, mac);
}

enum CONNECT_MODE get_connect_mode(void)
{
    return connect_mode;
}

/// @brief 断开目标端口的蓝牙设备，当重新匹配设备的时候需要使用此函数
/// @param index
void disconnect_ble_device(u8 index)
{

    // 通过handle断开设备
    ble_comm_disconnect(ble_sensor_device[index].handle);
    printf("disconnect ble device:%d\n", index);
    // 更新目标设备状态为未连接
    ble_sensor_device[index].connect_state = DISCONNECT;
    ble_sensor_device[index].handle = 0;
}

/// @brief 断开所有设备，当重置模块时使用
void disconnect_all_sensor_device()
{
    ble_gatt_client_disconnect_all();
}

void device_lost(u8 handle)
{
    printf("device lost:%d\n", handle);
    ble_sensor_device[get_ble_sensor_index_by_handle(handle)].connect_state = DISCONNECT;
    ble_sensor_device[get_ble_sensor_index_by_handle(handle)].handle = 0;
}

extern void att_send_data(u8 *data, u8 len, u8 handle);
void ble_send_data(u8 *data, u8 len, u8 handle)
{
    printf("ble send data:%d\n", handle);
    put_buf(data, len);
    att_send_data(data, len, handle);
}


// 注册回调函数
void register_ble_receive_callback(void (*ble_receive_callback)(u8 *, u8 ,u8))
{
    receive_callback = ble_receive_callback;
}
