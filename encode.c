//include necessary builtin header files and encode.h,common.h,types.h
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* all members of EncodeInfo datatype(structure) are accessed through derefencing  structure adress passed  during function call*/
/* Function Definitions */

/* read and validate encode arguments
 * Input : argument vector , structure pointer
 * Output : source image name, secret file name and output image name
 * return values : e_success, e_failure depending on file names
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if(strcmp(strchr(argv[2],'.'),".bmp") == 0)	//if file passed in command line as cla 3 has extension ".bmp"
	{
		encInfo->src_image_fname = argv[2];	//save the bmp file name in src_image_fname
	}
	else
		return e_failure;
	if(strcmp(strchr(argv[3],'.'),".txt") == 0)	//if file passed in command line as cla 4 has extension ".txt"
	{
		encInfo->secret_fname = argv[3];	//save the txt file name in secret_fname
	}
	else
		return e_failure;
	/* if no output bmp file name is mentioned in command line 
	   save default output bmp name in stego_image_fname ,
	   else save the given output bmp file name in stego_image_fname*/
	if(argv[4] == NULL)
	{
		encInfo->stego_image_fname = "stego.bmp";
	}
	else
		if(strcmp(strchr(argv[4],'.'),".bmp") == 0)
      	{
          encInfo->stego_image_fname = argv[4];
      	}
		else
			return e_failure;
	return e_success;
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    //open  Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // open Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    //open  Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure,then  return e_success
    return e_success;
}
/* copy the header bytes in bmp file 
 * to output bmp file
 * Inputs : FILE pointers of source image and output image file
 * Outputs : rgb_data byte position, Output image file with header data
 * Return value : e_success
 */
Status copy_bmp_header(EncodeInfo *encInfo)
{
	/*char str[54];
	fseek(fptr_src_image,0,SEEK_SET);
	fread(str,54,1,fptr_src_image);
	fwrite(str,54,1,fptr_dest_image);*/

	fseek(encInfo->fptr_src_image,10,SEEK_SET);	//seek the file pointer in source image to 10th byte position which has rgb data position as int value of 4bytes
	fread(&encInfo->rgb_data_pos,sizeof(int),1,encInfo->fptr_src_image);//read 4 bytes of int value from 10th byte position and save it in rgb_data_pos
	char str[encInfo->rgb_data_pos];	//declaration of array of header bytes size
	fseek(encInfo->fptr_src_image,0,SEEK_SET);	//bring back the file pointer of src image to beginning of file
	fread(str,encInfo->rgb_data_pos,1,encInfo->fptr_src_image);	//read header bytes from src image file and copy to output image file 
	fwrite(str,encInfo->rgb_data_pos,1,encInfo->fptr_stego_image);


	return e_success;
}

/* Get secret file size
 * Inputs : FILE pointer of secret file
 * Outputs : file size 
 * Return value : uint value of secret file size
 */
uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	return ftell(fptr);
}


/* check image capacity
 * Inputs : structure pointer (EncodeInfo *encInfo)
 * Outputs : image_capacity, size of secret file
 * Return values : e_success or e_failure on image_capacity checking
 */
Status check_capacity(EncodeInfo *encInfo)
{
	//calculate the image capacity and size of secret.txt file by calling respective functions
	encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);	
	encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
	//image capacity must be greater than (magic string length*8 + secret file extension length *8 + secret file extension name* 8+ secret file size *8 + secret file data * 8)
	if(encInfo->image_capacity > (16+32+32+32+(encInfo->size_secret_file * 8)))
	{
		return e_success;
	}
	else
		return e_failure;
}

//start encoding 
/* each function call either returns a e_success or e_failure
   if e_success is returned, then  perform the next step of encoding
   or else return e_failure 
 */
Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)	// function call to open  all valid files 
	{
		printf("Open files is a success\n");	
		if(check_capacity(encInfo) == e_success)	//function call to check capacity of raster data(rgb data) of image
		{
			printf("check capacity is a success\n");
			if(copy_bmp_header(encInfo) == e_success)	//function call to copy bmp file header bytes into output bmp file
			{
				printf("copy bmp header is a success\n");
				if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)//function call to encode magic string
				{
					printf("Encoded magic string successfully\n");
					strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname,"."));	//copy the extension of secret file into extn_secret_file
					if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)	//function call to encode secret file extension length
					{
						printf("Encoded secret file extension size successfully\n");
						if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)	//function call to encode secret file extension
						{
							printf("Encoded secret file extension successfully\n");
							if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)	//function call to encode secret_file_size
							{
								printf("Encoded secret file size\n");
								if(encode_secret_file_data(encInfo) == e_success)	//function call to encode secret file data
								{
									printf("Encoded secret file data\n");
									if(copy_remaining_img_data(encInfo) == e_success)	//functionc call to copy the remaining data from source image file to output image file
									{
										printf("Copied remaining data successfully\n");
									}
									else
									{
										printf("failed to copy remaining data\n");
										return e_failure;
									}
								}
								else
								{
									printf("failed to encode secret file data\n");
								}
							}
							else
							{
								printf("Failed to encode secret file size\n");
								return e_failure;
							}

						}
						else
						{
							printf("failed to encode secrert file extension\n");
							return e_failure;
						}
					}
					else
					{
						printf("failed to encode secret file extension size\n");
						return e_failure;
					}
				}
				else
				{
					printf("Failed to encode Magic string\n");
					return e_failure;
				}
			}
			else
			{
				printf("copy bmp header is a failure\n");
				return e_failure;
			}

		}
		else
		{
			printf("check capacity is a failure\n");
			return e_failure;
		}
	}
	else
	{
		printf("Open files is a failure\n");
		return e_failure;
	}
	return e_success;	//return e_success if all steps in do_encoding is successful 
}

/* encode magic string into output image file 
 * Inputs :- MAGIC_STRING, structure pointer
 * Outputs :- magic string encoded Output image file
 * return value : e_success
 */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
     encode_data_to_image(magic_string,strlen(magic_string),encInfo);	//function call to encode string argument or set of characters into output image file
     return e_success;
}

/* encoding the secret file extension length
 * Inputs : size of secret file extension
 * Outputs : extension length encoded output image file
 * return value : e_success
 */
Status encode_secret_file_extn_size(uint size,EncodeInfo *encInfo)
{
	encode_size_to_lsb(size,encInfo);	//function call to encode int value of size into output image file
	return e_success;
}

/* encoding secret file extension
 * Inputs : secret file extension , structure pointer
 * Outputs : secret file extension encoded Output image file
 * return value : e_success
 */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn,strlen(file_extn),encInfo);	//function call to encode string or char array into output image file
	return e_success;
}

/* encoding secret file size 
 * Inputs : file size, structure pointer 
 * Outputs : file size encoded Output image file
 * return value : e_success
 */
Status encode_secret_file_size(uint file_size, EncodeInfo *encInfo)
{
	encode_size_to_lsb(file_size,encInfo);	//function call to encode int value into output image file
	return e_success;
}

/* encoding secret file data 
 * Inputs : size of secret file, FILE pointers of secret file
 * Output : secret file data encoded Output image file
 * DESCRIPTION : all secret file data is stored as bytes of an array and then array
 				 is passed to encode_data_to_image function
 * return value : e_success
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char str[encInfo->size_secret_file];	//declaration of array of size equal to no of  bytes of secret file data
	fseek(encInfo->fptr_secret,0,SEEK_SET);	//seek the FILE pointer of secret file to beginning of file
	fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);//read as many bytes as size of secret file data from secret file
	encode_data_to_image(str,encInfo->size_secret_file,encInfo);//function call to encode secret file data 
	return e_success;
}


/* Generic function to encode char array or string data to output image file
 * Inputs : char array or string argument, length of string or characters,
 * 			FILE pointers of source image file and output image file,
 			image_data array of size 8 bytes
 * Outputs : output image file with encoded string or char array data
 * Return value : e_success
 */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
	for(int i=0; i<size; i++)	//iterate over byte by byte of string or char array
	{
		fread(encInfo->image_data,8,1,encInfo->fptr_src_image);	//read 8 bytes from source image file and save them in image_data array
		encode_byte_to_lsb(data[i],encInfo->image_data);	//generic function call to encode a byte into LSB of  8 bytes of image_data array
		fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);	//copy the encoded 8 bytes into output image file
	}
	return e_success;
}

/* Generic function to encode a byte to LSBs
 * of rgb data of output image file
 * Inputs : one byte data, array of 8 bytes
 * Outputs : byte data encoded array of 8 bytes
 * Return value : e_success
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i=0; i<8; i++)//iterate over 8 bytes of array
	{
		image_buffer[i] = ((image_buffer[i] & 0xFE) | (data>>i & 1));	//clear the LSB of byte and set it with current bit of byte data
	}
	return e_success;
}

/* Generic function to encode int value into output image file
 * Inputs : size as int value, FILE pointer of source image file
 			and output image file
 * Outputs : size encoded output image file
 * return value : e_success
 */
Status encode_size_to_lsb(uint size, EncodeInfo *encInfo)
{
    char str[32];	//declaration of an array of 32 bytes to store 4bytes int value
    fread(str,32,1,encInfo->fptr_src_image);	//read 32 bytes from source image file and store in array of 32 bytes
    for(int i=0; i<32; i++)	//iterate over byte by byte of array
    {
        str[i] = (str[i] & 0xFE) | (size>>i & 1);	//clear the LSB of byte and set the LSB of byte with current bit of the byte
    }
    fwrite(str,32,1,encInfo->fptr_stego_image);	//copy the encoded 32 bytes into output image file
    return e_success;
}
 
/* copying remaining data of source image file 
 * to output image file after encoding secret file data completely
 * Inputs : image_capacity, rgb data position, FILE pointers of source image and output image files
 * Outputs : remaining data encoded output image file
 * return value : e_success
 */

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
	// header bytes count + rgb bytes count - current byte position of source image file == remaining bytes count

	int len = (encInfo->image_capacity + encInfo->rgb_data_pos ) -ftell(encInfo->fptr_src_image);
    char arr[len];	
	fread(arr,len,1,encInfo->fptr_src_image);	//read remaining bytes from source image file and store in arr
	fwrite(arr,len,1,encInfo->fptr_stego_image);	//copy the remaining bytes to output image file
	return e_success;
}                                
