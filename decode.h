#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types


#define STEGO_BUFF_SIZE  8
#define INT_BUFF_SIZE  32
#define EXTN_SIZE  4

/* 
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */
typedef struct _DecodeInfo
{
	/* stego image information */
	char *stego_img_name;
	FILE *fptr_stego_img;
	uint rgb_data_pos;
	char image_arr[STEGO_BUFF_SIZE];
	char magic_string[2];
	char decode_byte[1];
	
	/* output decode txt info */
	char *dec_txt_fname;
	FILE *fptr_decoded_txt;
	char int_arr[INT_BUFF_SIZE];
	uint size_decode_file;
	uint size_decode_file_extn;
	int decode_int[1];
	char decode_file_extn[EXTN_SIZE+1];
	
	
}DecodeInfo;

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_dec(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(int size, DecodeInfo *decInfo);

/* Decode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extension*/
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(int size, DecodeInfo *decInfo);

/*Decode byte from LSB of image data array*/
Status decode_byte_from_lsb(char *decode_byte,char *image_buffer);

/*Decode int value from LSB of image data array*/
Status decode_size_from_lsb(int *decode_int,char *image_buffer);


#endif
