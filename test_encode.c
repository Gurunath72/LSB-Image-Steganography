/*************************************************************************************
Name : Gurunath N
Date : 30-05-2023
Description : To encode a secret file into bmp image using LSB Image Steganography

Encoding Input :-

~/C-Project-1/4-SkeletonCode$ gcc *.c 
~/C-Project-1/4-SkeletonCode$ ./a.out -e beautiful.bmp secret.txt output.bmp

Encoding Output :-

Selected Encoding
Read and Validate encode arguments is a success
Open files is a success
width = 1920
height = 1080
check capacity is a success
copy bmp header is a success
Encoded magic string successfully
Encoded secret file extension size successfully
Encoded secret file extension successfully
Encoded secret file size
Encoded secret file data
Copied remaining data successfully
Done encoding succesfully

Verification of output bmp file size:-

~/C-Project-1/4-SkeletonCode$ ls -l *.bmp
-rwxrwx--- 1 guru guru 2359351 May 17 14:15 beautiful.bmp
-rw-rw-r-- 1 guru guru 2359350 May 30 15:35 output.bmp


Decoding Input :-

./a.out -d output.bmp 

Decoding Output :-

Selected Decoding
Read and validate decode arguments is a success
Open files is a success
The magic string is "#*"
decoded magic string successfully
The decode file extension size is 4
Decoded output  file extension size
The decode file extension is ".txt"
Decoded output file extension
The decode file size is 30
Decoded output file size
Decoded output file data
done decoding successfully

Verification of decode text file size:-

~/C-Project-1/4-SkeletonCode$ ls -l *.txt
-rw-rw-r-- 1 guru guru 30 May 30 15:37 decode.txt
-rwxrwx--- 1 guru guru 30 May 30 15:37 secret.txt

*************************************************************************************/
//inclusion of all necessary header files 
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
//main function definition
int main(int argc, char *argv[])
{	//function call to check operation type , if return value is e_encode , perform encoding
	if(( check_operation_type(argv)) == e_encode)
	{
		printf("Selected Encoding\n");
		EncodeInfo encInfo;	//declaration of local variable of EncodeInfo datatype
		//function call to check if entered files in cla  are valid to perform encoding
		if(read_and_validate_encode_args(argv, &encInfo) == e_success)
		{
			printf("Read and Validate encode arguments is a success\n");
			//if validation is success, start encoding by calling function do_encoding
			if(do_encoding(&encInfo) == e_success)//if this function returns e_success, then all necessary encoding operations are succesfull
			{
				printf("Done encoding succesfully\n");

			}
			else	//if return value of do_encoding function call is e_failure, then encoding has failed due to one or more steps in encoding
			{
				printf("Failed to do the encoding");
				return -1;	//terminate the program
			}
		}
		// if failed to validate arguments,  exit program
		else if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
		{
			printf("Read and Validate encode arguments is a failure\n");
			return -1;
		}

	}
	//if "-d" is the option in cla then perform decoding
	else if((check_operation_type(argv)) == e_decode)
	{
		printf("Selected Decoding\n");
		DecodeInfo decInfo;	//declaration of local variable of datatype DecodeInfo
		if(read_and_validate_decode_args(argv,&decInfo) == e_success)	//if this function call returns e_success, then start decoding
		{
			printf("Read and validate decode arguments is a success\n");
			if(do_decoding(&decInfo) == e_success)	//this function call returns e_success after all necessary steps of decoding is succesfully done
			{
				printf("done decoding successfully\n");
			}
			else
			{
				printf("Failed to do decoding\n");
				return -1;
			}
		}
		//if failed to validate decoce args from cla , terminate from program
		else if(read_and_validate_decode_args(argv,&decInfo) == e_failure)
		{
			printf("Read and validate decode arguments is a failure\n");
			return -1;
		}
	}
	else	//propmt user to enter proper operation option and its usage 
	{
		printf("Invalid input\nPlease pass\nFor Encoding : ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
		printf("For Decoding : ./a.out -d beautiful.bmp secret.txt stego.bmp\n");
	}

		
    return 0;

}
//function definition to check operation type
OperationType check_operation_type(char *argv[])	//it takes the argument vector passed from main()
{
	if(strcmp(argv[1],"-e") == 0)	//if cla 2 is an option matching "-e", then return e_encode to perform encoding
	{
		return e_encode;
	}
	else if(strcmp(argv[1],"-d") == 0)	//if cla 2 is an option matching "-d" then return e_decode to perform decoding
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;	//if option doesnt match either of them , e_unsupported is returned.
	}
}
