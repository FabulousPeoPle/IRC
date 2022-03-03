
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>

int main(void)
{
	FILE *img = fopen("text", "rb");
	FILE *dest = fopen("textt", "wb");
	fseek(img, 0, SEEK_END);
	unsigned long len = ftell(img);
	rewind(img);
	unsigned char *buff = (unsigned char*)malloc(sizeof(unsigned char) * len);
	char *buffer = (char*)malloc(sizeof(char) * len);
	int bytesread = fread(buff, sizeof(unsigned char), len, img);
	std::cout << std::to_string(len) << std::endl;
	int i = 0;
	while (i < len)
	{
		buffer[i] = buff[i];
		i++;
	}
	i = 0;
	while (i < len)
	{
		buff[i] = buffer[i];
		i++;
	}
	i = 0;
	while (i < len)
	{
		std::cout << buff[i];
		fputc(buffer[i], dest);
		i++;
	}
	fclose(img);
	fclose(dest);

	return 0;
}