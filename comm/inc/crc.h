/*
 * Crc.h
 *
 *  Created on: Jun 10, 2022
 *      Author: fatih
 *
 *      source: https://lxp32.github.io/docs/a-simple-example-crc32-calculation/
 */

#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>
#include <stdio.h>


#ifndef NULL_PTR
#define NULL_PTR ((void*)0x00)
#endif /* NULL_PTR */

void crc_init(void);
uint32_t crc32_calculator(const uint8_t *s, size_t n);
uint32_t crc32_fast_calculator(const uint8_t *s, size_t n);

uint8_t checksum_calculate(const uint8_t *s, size_t n);

uint8_t checksum_dynamic_calculate(void);
void checksum_dynamic_reset(void);
void checksum_dynamic_added_value(uint8_t value);

#endif /* _CRC_H_ */
