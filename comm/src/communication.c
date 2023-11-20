#include "communication.h"
#include "crc.h"

// Macro for little-endian conversion
#define LE_ARRAY_TO_UINT32(array) \
    ((uint32_t)((array)[0]) | \
    ((uint32_t)((array)[1]) << 8) | \
    ((uint32_t)((array)[2]) << 16) | \
    ((uint32_t)((array)[3]) << 24))

// Macro for big-endian conversion
#define BE_ARRAY_TO_UINT32(array) \
    ((uint32_t)((array)[3]) | \
    ((uint32_t)((array)[2]) << 8) | \
    ((uint32_t)((array)[1]) << 16) | \
    ((uint32_t)((array)[0]) << 24))

// Macro for little-endian conversion
#define LE_ARRAY_TO_UINT16(array) \
    ((uint16_t)((array)[0]) | \
    ((uint16_t)((array)[1]) << 8))

// Macro for big-endian conversion
#define BE_ARRAY_TO_UINT16(array) \
    ((uint16_t)((array)[1]) | \
    ((uint16_t)((array)[0]) << 8))

#if COMM_BIG_ENDIAN == 1
#define ARRAY_TO_UINT32(array) ((uint32_t)BE_ARRAY_TO_UINT32(&array))
#else	
#define ARRAY_TO_UINT32(array) ((uint32_t)LE_ARRAY_TO_UINT32(&array))
#endif

#if COMM_BIG_ENDIAN == 1
#define ARRAY_TO_UINT16(array) ((uint16_t)BE_ARRAY_TO_UINT16(&array))
#else	
#define ARRAY_TO_UINT16(array) ((uint16_t)LE_ARRAY_TO_UINT16(&array))
#endif

// Macros for little-endian conversion
#define UINT32_TO_LE_ARRAY(value, array) \
        (array)[0] = (uint8_t)(value); \
        (array)[1] = (uint8_t)((value) >> 8); \
        (array)[2] = (uint8_t)((value) >> 16); \
        (array)[3] = (uint8_t)((value) >> 24)

#define UINT16_TO_LE_ARRAY(value, array) \
        (array)[0] = (uint8_t)(value); \
        (array)[1] = (uint8_t)((value) >> 8)
// Macros for big-endian conversion
#define UINT32_TO_BE_ARRAY(value, array) \
        (array)[3] = (uint8_t)(value); \
        (array)[2] = (uint8_t)((value) >> 8); \
        (array)[1] = (uint8_t)((value) >> 16); \
        (array)[0] = (uint8_t)((value) >> 24)

#define UINT16_TO_BE_ARRAY(value, array) \
        (array)[1] = (uint8_t)(value); \
        (array)[0] = (uint8_t)((value) >> 8)

// Macro for selecting endianness for uint32_t
#if COMM_BIG_ENDIAN == 1
#define UINT32_TO_ARRAY(array, value) UINT32_TO_BE_ARRAY(value, &array)
#else	
#define UINT32_TO_ARRAY(array, value) UINT32_TO_LE_ARRAY(value, &array)
#endif

// Macro for selecting endianness for uint16_t
#if COMM_BIG_ENDIAN == 1
#define UINT16_TO_ARRAY(array, value) UINT16_TO_BE_ARRAY(value, &array)
#else	
#define UINT16_TO_ARRAY(array, value) UINT16_TO_LE_ARRAY(value, &array)
#endif

comm_result_enum_t comm_encode(comm_packet_type_enum_t packet_type, comm_packet_struct_t *packet, uint8_t **data, uint32_t *data_len)
{
	static uint8_t data_array[COMM_PACKET_MAX_SIZE_WITH_ALL_SECTION] = { 0 };
	uint16_t index = 0;

	if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
	{
		data_array[index++] = packet->start_byte;

		UINT16_TO_ARRAY(data_array[index], packet->total_data_len);
		index += 2;
		data_array[index++] = packet->device_id;
	}

	UINT32_TO_ARRAY(data_array[index], packet->packet_number);
	index += 4;

	data_array[index++] = packet->definitions.info;

	if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
	{
		UINT16_TO_ARRAY(data_array[index], packet->content_size);
		index += 2;
		data_array[index++] = packet->header_checksum;
	}

	for (uint16_t i = 0; i < packet->content_size; i++)
	{
		data_array[index++] = packet->content[i];
	}

	if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
	{
		UINT32_TO_ARRAY(data_array[index], packet->crc);
		index += 4;
		data_array[index++] = packet->stop_byte;
	}

	*data = &data_array[0];
	*data_len = index;

	return RESULT_SUCCESS;
}

comm_result_enum_t comm_decode(comm_packet_type_enum_t packet_type, comm_packet_struct_t* packet, uint8_t* data, uint32_t data_len)
{
	comm_result_enum_t return_value = RESULT_SUCCESS;

	if (NULL_PTR == data)
	{
		return_value = RESULT_ERR_NULL;
	}
	else
	{
		uint16_t index = 0;

		if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
		{
			packet->start_byte = data[index++];
			packet->total_data_len = ARRAY_TO_UINT16(data[index]);
			index += 2;
			packet->device_id = data[index++];
		}

		packet->packet_number = ARRAY_TO_UINT32(data[index]);
		index += 4;

		packet->definitions.info = data[index++];

		if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
		{
			packet->content_size = ARRAY_TO_UINT16(data[index]);
			index += 2;
		}
		else
		{
			packet->content_size = data_len - index;
		}

		if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
		{
			packet->header_checksum = data[index++];
		}

		for (uint16_t i = 0; i < packet->content_size; i++)
		{
			packet->content[i] = data[index++];
		}

		if (COMM_PACKET_TYPE_WITH_VERIFICATION == packet_type)
		{
			packet->crc = ARRAY_TO_UINT32(data[index]);
			index += 4;
			packet->stop_byte = data[index++];
		}
	}

	return return_value;
}