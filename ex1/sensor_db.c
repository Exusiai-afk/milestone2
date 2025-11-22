//#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"
#include "sensor_db.h"

/**
 * 打开数据库文件 (CSV文件)
 * @param filename 文件名
 * @param append true则追加，false则覆盖(清空)
 * @return 打开的文件指针，如果失败返回NULL
 */
FILE * open_db(char * filename, bool append) {
    FILE * f;
    if (append) {
        f = fopen(filename, "a"); // 追加模式
    } else {
        f = fopen(filename, "w"); // 写入模式 (覆盖)
    }

    if (f == NULL) {
        perror("Error opening database file");
        return NULL;
    }
    return f;
}

/**
 * 向打开的文件中插入一条传感器记录
 * @param f 文件指针
 * @param id 传感器ID
 * @param value 传感器数值
 * @param ts 时间戳
 * @return 0 成功, -1 失败 (例如文件指针为空)
 */
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) {
        return -1;
    }

    // 根据 sensor_db.csv 的格式要求写入数据
    // 假设格式为: ID, Value, Timestamp (请根据 config.h 中的类型调整格式符)
    // 这里假设 id 是整数, value 是浮点数, ts 是 time_t (长整型)
    // 注意：如果 config.h 中定义的类型不同，请相应修改下面的 %d, %f, %ld
    int result = fprintf(f, "%d, %f, %ld\n", id, value, ts);

    if (result < 0) {
        return -1; // 写入错误
    }
    return 0;
}

/**
 * 关闭数据库文件
 * @param f 文件指针
 * @return 0 成功, EOF 失败
 */
int close_db(FILE * f) {
    if (f == NULL) {
        return -1;
    }
    return fclose(f);
}