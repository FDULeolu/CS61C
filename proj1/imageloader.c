/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "imageloader.h"

//Opens a .ppm P3 image file, and constructs an Image object. 
//You may find the function fscanf useful.
//Make sure that you close the file with fclose before returning.
Image *readData(char *filename) 
{	
	//Read .ppm file
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		return NULL;
	}

	//Allocate memory for image
	Image *image = (Image *) malloc(sizeof(Image));
	if (image == NULL) {
		perror("Error allocating memory for Image\n");
		exit(EXIT_FAILURE);
	}

	uint32_t row, col;
	char header[3];
	uint32_t scale;

	//Read header and check file format
	fscanf(fp, "%2s", header);
	if (header[0] != 'P' || header[1] != '3') {
		perror("Invalid PPM file format\n");
		exit(EXIT_FAILURE);
	}

	//Read row, col and max color scale
	fscanf(fp, "%d %d", &col, &row);
	fscanf(fp, "%d", &scale);
	if (scale != 255) {
		perror("Unsupported max value in PPM file\n");
		exit(EXIT_FAILURE);
	}
	image->cols = col;
	image->rows = row;

	//Allocate memory for color
	Color **color = (Color **) malloc(row * sizeof(Color *));
	if (color == NULL) {
		perror("Error allocating memory for image rows");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < row; i++) {
		color[i] = (Color *) malloc(col * sizeof(Color));
		if (color[i] == NULL) {
			perror("Error allocating memory for image cols");
			exit(EXIT_FAILURE);
		}
	}

	//Read image data
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int r, g, b;
			fscanf(fp, "%d %d %d", &r, &g, &b);
			color[i][j].R = (uint8_t) r;
			color[i][j].G = (uint8_t) g;
			color[i][j].B = (uint8_t) b;
		}
	}

	fclose(fp);
	image->image = color;
	return image;
}

//Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the image's data.
void writeData(Image *image)
{
	printf("P3\n");
	printf("%d %d\n", image->cols, image->rows);
	printf("255\n");

	for (int i = 0; i < image->rows; i++) {
		for (int j = 0; j < image->cols; j++) {
			printf("%3d %3d %3d", image->image[i][j].R, image->image[i][j].G, image->image[i][j].B);
			if (j != image->cols - 1) {
				printf("   ");
			} 
		}
		printf("\n");
	}
}

//Frees an image
void freeImage(Image *image)
{	
	if (image == NULL) {
		return;
	}
	
	for (int i = 0; i < image->rows; i++) {
		free(image->image[i]);
	}
	free(image->image);
	free(image);
}