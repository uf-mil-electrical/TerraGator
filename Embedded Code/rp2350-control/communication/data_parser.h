#ifndef DATA_PARSER_H
#define DATA_PARSER_H

/******************<Summary>*****************
 * Name: data_parser.h
 * Purpose:
 *      > Parses received data using the MIL software-
 *          electrical data serialization protocol
 *      > https://milwiki.cbrxyz.com/wiki/Software-electrical_data_serialization_protocol
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "pins.h"
/******************</Dependencies>*****************/


/******************<Defines>*****************/
/******************</Defines>*****************/


/******************<Functions>*****************/
bool rover_i2c_parse_data(uint8_t* packet, uint16_t packet_len);
void rover_stop_motors(void);
void rover_i2c_extract_motor_commands(uint8_t* payload);
/******************</Functions>*****************/



#endif // DATA_PARSER_H
