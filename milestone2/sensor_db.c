#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "sensor_db.h"
#include "logger.h"

FILE * open_db(char * filename, bool append) {
    create_log_process();

    FILE * f;
    if (append) {
        f = fopen(filename, "a");
    } else {
        f = fopen(filename, "w");
    }

    if (f == NULL) {
        write_to_log_process("Error opening CSV file");
        return NULL;
    }

    write_to_log_process("Data file opened"); // according to example.gateway.log

    return f;
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) return -1;

    // according to config.h: id is uint16_t (%hu), value is double (%f), ts is time_t (%ld)
    // for safety，convert time_t to long int
    int result = fprintf(f, "%hu, %f, %ld\n", id, value, (long int)ts);

    if (result < 0) {
        write_to_log_process("An error occurred when writing to the csv file");
        return -1;
    }

    write_to_log_process("Data inserted"); // according to example.gateway.log

    return 0;
}

int close_db(FILE * f) {
    if (f == NULL) return -1;

    write_to_log_process("Data file closed"); // according to example.gateway.log

    // close csv
    int res = fclose(f);

    // end log
    end_log_process();

    return res;
}