LSB Image Steganography

Steganography is the art of hiding the fact that communication is taking place, by hiding information in other information. Many carrier file formats can be used, but digital images are the most popular because of their frequency on the internet. For hiding secret information in images, there exists a large variety of steganography techniques some are more complex than others and all of them have respective strong and weak points. Different applications may require absolute invisibility of the secret information, while others require a large secret message to be hidden.

Requirement Details

1.The application accepts an image file say .bmp along with a text file that contains the message to be steged
2.Analyze the size of the message file to check whether the message could fit in the provided .bmp image
3.Provide an option to steg a magic string which could be useful to identify whether the image is steged or not
4.This has to be a command-line application and all the options have to be passed as a command-line argument

In this project, we encode the secret file data into LSB of every byte of .bmp image data, therefore for every secret byte, it reads 8 image data bytes to encode into 8 LSBs.
encoder encodes the string length as int value before encoding any characters or string so that decoder can decode size first in prior to any characters decoding to know how many characters to decode.


1.Encode operation
-> read and validate the arguments passed in comd line.

-> open files as required.

-> check image capacity : image data( pixel data or RGB bytes needs to be sufficient enough to encode magic string + size of secret file extension (".txt") + secret file extension + size of secret file data + secret data .

-> copy header bytes to new stego image, since properties of image are intact.

-> encoding starts from offset of file where pixel data starts(RGB bytes).

-> in this project .bmp has 24bpp format i.e it has pixel represented in RGB (3 intensities).

->encode magic string.

->encode size of secret file extension.

-> encode extension.

-> encode size of secret file data.

-> encode secret file data.

2. DECODE operation
-> read and validate file arguments
   
-> decode magic string

-> decode size of secret file extension

-> decode secret file extension

-> decode size of secret file

-> decode secret data
->copy remaining image data that was not encoded to stego img

Sample Output provided in Screenshots folder.

