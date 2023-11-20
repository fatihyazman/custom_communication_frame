#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void*)0x00)
#endif /* NULL_PTR */

#ifndef COMM_PACKET_MAX_CONTENT_SIZE
#define COMM_PACKET_MAX_CONTENT_SIZE 20     /* The size of the content array in "comm_packet_struct_t". */
#endif

#define COMM_PACKET_MAX_SIZE_WITH_ALL_SECTION     sizeof(comm_packet_struct_t)     /* Maximum possible packet size. */

#ifndef COMM_BIG_ENDIAN
#define COMM_BIG_ENDIAN 1
#endif

	typedef enum comm_result_enum
	{
		RESULT_SUCCESS,
		RESULT_ERR_NULL
	}comm_result_enum_t;

	/**
 * \brief                       It is used to select the packet type.
 */
	typedef enum comm_packet_type_enum
	{
		COMM_PACKET_TYPE_WITH_VERIFICATION,
		COMM_PACKET_TYPE_WITHOUT_VERIFICATION
	}comm_packet_type_enum_t;

	/**
	 * \brief                       Information to fill the info byte during communication.
	 */
	typedef enum comm_info_bytes_enum
	{
		COMM_INFO_BYTE_WRITE_REGISTER,                  /* It means you want to write data to a register. */
		COMM_INFO_BYTE_READ_REGISTER,                   /* It means you wait to read data to a register. */
		COMM_INFO_FIRMWARE_UPDATE_CHECK_UPDATE,         /* It means check firmware update. */
		COMM_INFO_FIRMWARE_UPDATE_GET_FILE_SIZE,
		COMM_INFO_FIRMWARE_UPDATE_GET_FIRMWARE,
		COMM_INFO_STEP_MOTOR_MOVE,
		COMM_INFO_STEP_MOTOR_POSITION,
		COMM_INFO_STEP_MOTOR_RAMP_PARAMATERS,
		COMM_INFO_PID_PARAMATERS,
	}comm_info_bytes_enum_t;


	/**
	 * \brief     Union that holds definitions about the packet.
	 */
	typedef union comm_definitions_byte_union
	{
		struct
		{
#if COMM_BIG_ENDIAN == 1
			uint8_t is_high_priority : 1;       /* 0 - 2 data priority bits. */
			uint8_t is_waiting_for_answer : 1; /* The sender may or may not be expecting a response or confirmation. */
			uint8_t request_or_send : 1;       /* is the packet resquest or just data send. */
			uint8_t info : 5;                  /* The packet info byte from. comm_info_bytes_enum_t*/
#else
			uint8_t info : 5;                  /* The packet info byte from. comm_info_bytes_enum_t*/
			uint8_t request_or_send : 1;       /* is the packet resquest or just data send. */
			uint8_t is_waiting_for_answer : 1; /* The sender may or may not be expecting a response or confirmation. */
			uint8_t is_high_priority : 1;       /* 0 - 2 data priority bits. */
#endif
		} values;
		uint8_t info;
	} comm_definitions_byte_union_t;

	/**
	 * \brief     A more understandable simple buffer structure used when notifying the Library User.
	 */
	typedef struct comm_packet_struct
	{
		uint8_t content[COMM_PACKET_MAX_CONTENT_SIZE]; /* content "buffer". */
		uint32_t packet_number;                        /* Incremental package number for each package.                                 -Identity- */
		uint32_t crc;                                  /* Data package's crc except crc and "stop_byte".  for security channel only!   -securty-  */
		uint16_t total_data_len;                       /* Shows the total size of the package sent.       for security channel only!   -Identity- */
		uint16_t content_size;                         /* The size of the content buffer.                                              -Identity- */
		comm_definitions_byte_union_t definitions;     /* Data package info byte. */
		uint8_t start_byte;                            /* Byte indicating the start of packet trasfer.    for security channel only!   -Identity- */
		uint8_t device_id;                             /* The device id to which the data packet is sent. for security channel only!              */
		uint8_t header_checksum;                       /* To make sure the header is correct.                                          -security- */
		uint8_t stop_byte;                             /* Byte indicating the end of packet.              for security channel only!   -Identity- */
	} comm_packet_struct_t;

	comm_result_enum_t comm_encode(comm_packet_type_enum_t packet_type, comm_packet_struct_t *packet, uint8_t** data, uint32_t* data_len);
    comm_result_enum_t comm_decode(comm_packet_type_enum_t packet_type, comm_packet_struct_t *packet, uint8_t* data, uint32_t data_len);

#ifdef __cplusplus
}
#endif

#endif
