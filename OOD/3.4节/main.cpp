#include <iostream>
#include <cstdlib>
#include <cstring>

///< 例子中用到的常量
const int name_len = 50;
const int large_strlen = 128;
const int room_len = 20;

///< 输入期望温度的设备
class DesiredTempActuator {
public:
    int get_temp();
};

int DesiredTempActuator::get_temp()
{
    return (rand() % 40) + 50;
}

///< 实际温度探测器
class ActualTempSensor {
public:
    int get_temp();
};

int ActualTempSensor::get_temp()
{
    return (rand() % 40) + 50;
}

///< 探测房内是否有人
class OccupancySensor {
public:
    int anyone_in_room();
};

int OccupancySensor::anyone_in_room()
{
    return (rand() % 2);
}

///< 房间内包含上述三种设备, 并且有一个名字属性, 用来存放描述信息
class Room {
    char name[name_len];
    DesiredTempActuator dtemp;
    ActualTempSensor atemp;
    OccupancySensor occ;

public:
    Room(char*);
    int do_you_need_heat();
};

Room::Room(char* n)
{
    strncpy(name, n, name_len);
}

///< 房间对象通过计算工作(期待温度-实际温度) 并检查房内是否有人来判断是否需要供暖

int Room::do_you_need_heat()
{
    int work_temp, occupied;

    work_temp = dtemp.get_temp() - atemp.get_temp();
    occupied = occ.anyone_in_room();

    std::cout << " The " << name << " has a working temp of " << work_temp;
    std::cout << " and " << (occupied ? "someone in the room.\n" : "no one in the rom.\n");

    if (work_temp > 5 && !occupied || work_temp > 0 && occupied) {
        return 1;
    }
    return 0;
}

/* 供暖炉子不值得特别关注 */
class Furnace {
public:
    void provide_heat();
    void turnoff();
};

void Furnace::provide_heat()
{
    std::cout << "Furnamce Running \n";
}

void Furnace::turnoff()
{
    std::cout << "Furnace Turned Off \n";
}

/* 热流调节器并不包含房间的列表, 也不包含供暖的炉子. 塔筒他们是关联关系 */
class HeatFlowRegulator {
    Room* house[room_len];
    Furnace* heater;
    int room_num;

public:
    HeatFlowRegulator(Furnace*, int, Room**);
    int loop();
};

HeatFlowRegulator::HeatFlowRegulator(Furnace* f, int num, Room** rooms) 
{
    int i;

    heater = f;
    room_num = num;
    for (i = 0; i < room_num; ++i) {
        house[i] = rooms[i];
    }
}

/* 热流调节器的这个循环是为了检查每个房间是否需要供暖, 为了做到这一点,
   调节器仅仅简单的查询房间是否需要供暖, 而真正的判断交给房间类 */
int HeatFlowRegulator::loop()
{
    int any_need_heat = 0, i;

    for (i = 0; i < room_num; ++i) {
        any_need_heat += house[i]->do_you_need_heat();
    }
    if (any_need_heat) {
        heater->provide_heat();
    } else {
        heater->turnoff();
    }

    return any_need_heat;
}

int main()
{
    int room_num, i, retval;
    Furnace our_furnace;
    Room* rooms[room_len];
    char buffer[large_strlen];

    std::cout << " How many rooms in your house? ";
    std::cin >> room_num;
    std::cin.get();
    if (room_num > room_len) {
        room_num = room_len;
    }

    for (i = 0; i < room_num; ++i) {
        std::cout << " What is the name of room[ " << i + 1 << "]?";
        std::cin.getline(buffer, large_strlen, '\n');
        rooms[i] = new Room(buffer);
    }

    HeatFlowRegulator h(&our_furnace, room_num, rooms);

    do {
        retval = h.loop();
        std::cout << retval << " rooms required heat!\n";
        std::cout << "Continue? ";
        std::cin.getline(buffer, large_strlen, '\n');
    } while (buffer[0] == 'y');

    return 0;
}



