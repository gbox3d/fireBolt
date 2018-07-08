#ifndef __PARSER_H__
#define __PARSER_H__

#include <Arduino.h>

enum
{
    OP_SIN = 1,
    OP_COS,
    OP_ERR,
    OP_NONE,
    OP_NODATA
};

extern String result_cmd;
extern String result_p1_str;
extern unsigned long result_p1_number;

int checkUartCmd();



#endif
