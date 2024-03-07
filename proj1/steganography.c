/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "imageloader.h"

//Determines what color the cell at the given row/col should be. This should not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col)
{
	if (image == NULL || row >= image->rows || col >= image->cols || row <0 || col < 0) {
		perror("Invalid input!");
		exit(EXIT_FAILURE);
	}

	Color *res = (Color *) malloc(sizeof(Color));
	if (res == NULL) {
		perror("Error in allocating memory!");
		exit(EXIT_FAILURE);
	}

	uint16_t blue = image->image[row][col].B;
	if (blue % 2 == 0) {
		res->B = 0;
		res->G = 0;
		res->R = 0;
	} else {
		res->R = 255;
		res->G = 255;
		res->B = 255;
	}
	return res;
}

//Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image)
{
	if (image == NULL) {
		perror("Invalid input!");
		exit(EXIT_FAILURE);
	}

	Image *resImage = (Image *) malloc(sizeof(Image));
	if (resImage == NULL) {
		perror("Error in allocating memory!");
		exit(EXIT_FAILURE);
	}

	uint16_t cols = image->cols;
	uint16_t rows = image->rows;
	resImage->cols = cols;
	resImage->rows = rows;

	Color **color = (Color **) malloc(rows * sizeof(Color*));
	if (color == NULL) {
		perror("Error allocating memory for image rows");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < rows; i++) {
		color[i] = (Color *) malloc(cols * sizeof(Color));
		if (color[i] == NULL) {
			perror("Error allocating memory for image cols");
			exit(EXIT_FAILURE);
		}
	}


	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			Color *proccessedPixel = evaluateOnePixel(image, i, j);
			color[i][j] = *proccessedPixel;
			free(proccessedPixel);
		}
	}

	resImage->image = color;

	return resImage;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with printf) a new image, 
where each pixel is black if the LSB of the B channel is 0, 
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not necessarily with .ppm file extension).
If the input is not correct, a malloc fails, or any other error occurs, you should exit with code -1.
Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!
*/
int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("You should input file path!");
		return -1;
	}

	Image *image = readData(argv[1]);
	if (image == NULL) {
		printf("Error reading image data!");
		return -1;
	}

	Image *processedImage = steganography(image);
	if (processedImage == NULL) {
		printf("Error processing image!");
		freeImage(image); // 释放 readData 分配的内存
		return -1;
	}

	writeData(processedImage);

	freeImage(image);
	freeImage(processedImage);
	return 0;
}
