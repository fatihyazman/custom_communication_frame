/*
 * Crc.c
 *
 *  Created on: Jun 10, 2022
 *      Author: fatih
 */

#include "crc.h"

static uint32_t crc32_table[256];
static uint32_t checksum_total_values;

static void crc32_build_table(void);
static uint8_t checksum_base_calculate(uint32_t checksum_total_value);

static void crc32_build_table(void)
{
	for (uint32_t i = 0; i < 256; i++)
	{
		uint32_t ch = i;
		uint32_t crc = 0;
		for (size_t j = 0; j < 8; j++)
		{
			uint32_t b = (ch ^ crc) & 1;
			crc >>= 1;
			if (b)
				crc = crc ^ 0xEDB88320;
			ch >>= 1;
		}
		crc32_table[i] = crc;
	}
}

void crc_init(void)
{
	crc32_build_table();
}

//587uS
uint32_t crc32_calculator(const uint8_t *s, size_t n)
{
	uint32_t crc = 0xFFFFFFFF;

	for (size_t i = 0; i < n; i++)
	{
		uint8_t ch = s[i];
		for (size_t j = 0; j < 8; j++)
		{
			uint32_t b = (ch ^ crc) & 1;
			crc >>= 1;
			if (b)
				crc = crc ^ 0xEDB88320;
			ch >>= 1;
		}
	}

	return ~crc;
}

uint32_t crc32_fast_calculator(const uint8_t *s, size_t n)
{
	if (NULL_PTR == s)
	{
		return 0;
	}

	uint32_t crc = 0xFFFFFFFF;

	for (size_t i = 0; i < n; i++)
	{
		uint8_t ch = s[i];
		uint32_t t = (ch ^ crc) & 0xFF;
		crc = (crc >> 8) ^ crc32_table[t];
	}

	return ~crc;
}

static uint8_t checksum_base_calculate(uint32_t checksum_total_value)
{
	checksum_total_value = ((checksum_total_value ^ 255) + 1);
	return ((uint8_t*)&checksum_total_value)[3];
}

uint8_t checksum_calculate(const uint8_t *s, size_t n)
{
	if (NULL_PTR == s)
	{
		return 0;
	}

	uint32_t checksum = 0;
	for (size_t i = 0; i < n; i++)
	{
		checksum += s[i];
	}

	return checksum_base_calculate(checksum);
}

void checksum_dynamic_added_value(uint8_t value)
{
	checksum_total_values += value;
}

uint8_t checksum_dynamic_calculate(void)
{
	return checksum_base_calculate(checksum_total_values);
}

void checksum_dynamic_reset(void)
{
	checksum_total_values = 0;
}
