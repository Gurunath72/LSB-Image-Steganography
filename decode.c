/* inclusion of all necessary built-in header files and common.h,types.h,decode.h*/
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "types.h"
#include "decode.h"

/* validate file names 
 * Input : argument vector, structure pointer
 * Outputs : stego img name, decoded file name
 * return value : e_success or e_failure depending on file name
 */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
	if(strcmp(strchr(argv[2],'.'),".bmp") == 0)	//if filename passed in cla 3 has extension ".bmp"
	{
		decInfo->stego_img_name = argv[2];	// save the file name in stego_img_name pointer
	}
	else
		return e_failure;	//return e_failure if cla 3 doesnt has .bmp extension
	/* if decoded output file name is not given as cla 
	   then save the default output decoded file name in dec_txt_fname
	   or else the given cla 4 name */
	if(argv[3] == NULL)	
	{
		decInfo->dec_txt_fname = "decode.txt";
	}
	else
		if(strcmp(strchr(argv[3],'.'),".txt") == 0)
		{
			decInfo->dec_txt_fname = argv[3];
		}
		else
			return e_failure;	//return e_failure if cla 4 doesnt have .txt extension
	return e_success;
}
//start decoding
/* each function call either returns
   e_success or e_failure, if its success then continue with 
   next steps of decoding, or else return e_failure*/
Status do_decoding(DecodeInfo *decInfo)
{
	if(open_files_dec(decInfo) == e_success)	//function call to open decode file arguments
	{
		printf("Open files is a success\n");
		if(decode_magic_string(strlen(MAGIC_STRING),decInfo) == e_success)	//functionc call to decode magic string
		{
			printf("decoded magic string successfully\n");
			if(decode_secret_file_extn_size(decInfo) == e_success)	//function call to decode secret file extension length
			{
				printf("Decoded output  file extension size\n");
				if(decode_secret_file_extn(decInfo->size_decode_file_extn,decInfo) == e_success)	//function call to decode secret file extension 
				{
					printf("Decoded output file extension\n");
					if(decode_secret_file_size(decInfo) == e_success)	//function call to decode secret file size
					{
						printf("Decoded output file size\n");
						if(decode_secret_file_data(decInfo->size_decode_file,decInfo) == e_success)	//function call to decode secret file data
						{
							printf("Decoded output file data\n");
						}
						else
						{
							printf("Failed to decode output file data\n");
							return e_failure;
						}
					}
					else
					{
						printf("Failed to decode output file size\n");
						return e_failure;
					}
				}
				else
				{
					printf("Failed to decode output file extension\n");
					return e_failure;
				}

			}
			else
			{
				printf("Failed to decode output file extension size\n");
				return e_failure;
			}

		}
		else
		{
			printf("failed to decode magic string successfully\n");
			return e_failure;
		}
	}
	else
	{
		printf("Open files is a failure\n");
		return e_failure;
	}
	return e_success;	//if all steps of decoding done succesfully then return e_success;
}



/* opening files
 * Inputs : stego image file name and output decode txt file name
 * Outputs : FILE pointers for stego image file and output decode file
 * DESCRIPTION : 10th byte position in header data gives offset i.e
 				where the rgb or raster data bytes starts
 * return value : e_success or e_failure on FILE pointers
 */

Status open_files_dec(DecodeInfo *decInfo)
{
	//open stego image file
	decInfo->fptr_stego_img = fopen(decInfo->stego_img_name, "r");
     // Do Error handling
     if (decInfo->fptr_stego_img == NULL)
     {
         perror("fopen");
         fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_img_name);
         return e_failure;
     }
 
     // open output  Decoded txt file
     decInfo->fptr_decoded_txt = fopen(decInfo->dec_txt_fname, "w");
     // Do Error handling
     if (decInfo->fptr_decoded_txt == NULL)
     {
         perror("fopen");
         fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dec_txt_fname);
		 return e_failure;
	 }
	 //read 4 bytes from 10th byte position i.e offset and save as rgb data position
	 //seek FILE pointer of stego image to rgb data
	 fseek(decInfo->fptr_stego_img,10,SEEK_SET);
     fread(&decInfo->rgb_data_pos,sizeof(int),1,decInfo->fptr_stego_img);
	 fseek(decInfo->fptr_stego_img,decInfo->rgb_data_pos,SEEK_SET);

	 return e_success;
}



/* decoding magic string
 * Inputs : FILE pointer of stego image, array of 8 bytes, 
			decode_byte(a single byte argument)
 * Outputs : magic_string
 * return value : e_success
 */ 
Status decode_magic_string(int size, DecodeInfo *decInfo)
{
	for(int i=0; i<size; i++)	//iterate over byte by byte in stego image file
    {
		fread(decInfo->image_arr,STEGO_BUFF_SIZE,1,decInfo->fptr_stego_img);	//read 8 bytes from stego image file to decode a single byte data
        decode_byte_from_lsb(decInfo->decode_byte,decInfo->image_arr);	//function call to decode an byte data from 8 bytes array
		decInfo->magic_string[i] = decInfo-> decode_byte[0];	// store the byte data in magic string array
	}
	printf("The magic string is \"");
	for(int i=0; i<size; i++)
	{
		printf("%c",decInfo->magic_string[i]);
	}
	printf("\"\n");
	return e_success;

}



/* decoding secret file extension length
 * Inputs : FILE pointer of stego image, array of 32 bytes,
 			decode_int ( a 4 byte argument)
 * Outputs : size_decode_file_extension
 * return values : e_success
 */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	fread(decInfo->int_arr,INT_BUFF_SIZE,1,decInfo->fptr_stego_img);	//read 32 bytes from stego image file and cpy to int_arr
	decode_size_from_lsb(decInfo->decode_int,decInfo->int_arr);	//function call to decode an int value from 32 bytes array
	decInfo->size_decode_file_extn = decInfo->decode_int[0];	//decoded int value is stored in size_decode_file_extn
	printf("The decode file extension size is %u\n",decInfo->size_decode_file_extn);
	return e_success;
}


/* decoding secret file extension
 * Inputs : size (size_decode_file_extn), FILE pointer of stego image
 * Outputs : decode_file_extn (output decoded file extension)
 * return value : e_success
 */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
	int i;
	for(i=0; i<size; i++)	//iterate over size no of bytes in image file
	{
		fread(decInfo->image_arr,STEGO_BUFF_SIZE,1,decInfo->fptr_stego_img);	//read 8 bytes from stego image file and save in image_arr
		decode_byte_from_lsb(decInfo->decode_byte,decInfo->image_arr);	//function call to decode a byte data from array of 8 bytes
		decInfo->decode_file_extn[i] = decInfo->decode_byte[0];	// decoded byte data is stored in respective byte position of decode file extension array
	}
	decInfo->decode_file_extn[i] = '\0';
	if(strncmp(decInfo->decode_file_extn , strstr(decInfo->dec_txt_fname,"."),4) != 0)
	{
		decInfo->dec_txt_fname = decInfo->decode_file_extn;
	}
	printf("The decode file extension is \"");		

	for( i=0;i<size;i++)
	{
		printf("%c",decInfo->decode_file_extn[i]);
	}
	printf("\"\n");
	return e_success;
}


/* decoding secret file size 
 * Inputs : FILE pointer of stego image, array of 32 bytes
 			decode_int( 4 byte argument)
 * Outputs : size of output decode file
 * return value : e_success
 */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	fread(decInfo->int_arr,INT_BUFF_SIZE,1,decInfo->fptr_stego_img);	//read 32 bytes from stego image file and save in int_arr
	decode_size_from_lsb(decInfo->decode_int,decInfo->int_arr);		//function call to decode int value from 32 bytes array
	decInfo->size_decode_file = decInfo->decode_int[0];	// save the decoded int value as size_decode_file
	printf("The decode file size is %u\n",decInfo->size_decode_file);
	return e_success;
}


/* decoding secret file data
 * Inputs : FILE pointers of output decoded file,stego image file
 			array of 8 bytes, decode_byte( single byte argument)
			size of output decode file
 * Outputs : decoded secret file
 * return value : e_success
 */
Status decode_secret_file_data(int size, DecodeInfo *decInfo)
{
	char arr[size];
	fseek(decInfo->fptr_decoded_txt,0,SEEK_SET);
	for(int i=0; i<size; i++)
	{
		fread(decInfo->image_arr,STEGO_BUFF_SIZE,1,decInfo->fptr_stego_img);
		decode_byte_from_lsb(decInfo->decode_byte,decInfo->image_arr);
		arr[i] = decInfo->decode_byte[0];
	}
	int i=0;
	while(arr[i] != EOF && i<size)
	{
	
		fputc(arr[i],decInfo->fptr_decoded_txt);
		i++;
	}
	return e_success;
}


/* Generic function to decode an byte from LSB of 8bytes array
 * Inputs : decode_byte(1 byte argument), 8 bytes from stego image file
 * Outputs : decoded byte from 8bytes of stego image file
 * return value : e_success
 */
Status decode_byte_from_lsb(char *decode_byte,char *image_buffer)
{
	char mask = 0x01;	//define an mask of 1byte with value 0x01
	decode_byte[0] = 0;	//clear all the bits in byte data to 0
	for(int i=0; i<STEGO_BUFF_SIZE; i++)	//iterate over 8 bytes of array
	{
		if(image_buffer[i] & 0x01)	//if LSB of i'th  byte of array is 1
		{
			decode_byte[0] = decode_byte[0] & (~(mask<<i));	//then, clear the ith position bit in decode_byte so the same bit is set to 1
			decode_byte[0] |= (mask<<i);
		}
		else	//if LSB of i'th byte of array is 0
		{
			decode_byte[0] = decode_byte[0] & (~(mask<<i));	//then, clear the i'th bit position in decode_byte so same bit is set to 0
		}
	}
	return e_success;
}


/* Generic function to decode int value from 32 bytes array
 * Inputs : decode_int (4 byte argument), 32 bytes array
 * Outputs : decoded int value from 32 bytes of image file
 * return value : e_success
 */
Status decode_size_from_lsb(int *decode_int,char *image_buffer)
{
	int mask = 0x00000001;	//define an mask of 4 bytes with value 0x01
	decode_int[0] = 0;	//clear all bits in integer argument to 0
	for(int i=0; i<INT_BUFF_SIZE; i++)	//iterate over 32 bytes of array
	{
		if(image_buffer[i] & 0x01)	//if LSB of i'th byte of array is 1
		{
			decode_int[0] = decode_int[0] & (~(mask<<i));	//then clear the i'th position bit in decode_int so same bit is set to 1
			decode_int[0] |= (mask<<i);
		}
		else	//if LSB of i'th byte of array is 0
		{
			decode_int[0] |= decode_int[0] & (~(mask<<i));	//then clear the i'th position bit in decode_int , so same bit is set to 0
		}
	}
	return e_success;
}


		




