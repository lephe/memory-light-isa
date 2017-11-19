/* Quick and dirty font converter. Assumes 8 rows of 16 glyphs */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define	GW 5	/* Glyph width */
#define GH 7	/* Glyph height */

int pixel(unsigned char *px, int width, int height, int x, int y)
{
	int bpr = ((width + 31) & ~31) >> 3;
	int off = (height - y - 1) * bpr + (x >> 3);
	int byte = px[off];

	return (byte >> (~x & 7)) & 1;
}

int main(int argc, char **argv)
{
	if(argc != 2)
		fprintf(stderr, "usage: %s <bmp_file>\n", argv[0]), exit(1);

	FILE *fp = fopen(argv[1], "r");
	if(!fp) exit(1);

	fseek(fp, 0, SEEK_END);
	unsigned long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char *data = malloc(size);
	if(!data) fclose(fp), exit(1);

	int x = fread(data, size, 1, fp);
	fclose(fp);
	if(x != 1) free(data), exit(1);

	uint32_t off =
		(data[13]<<24) | (data[12]<<16) | (data[11]<<8) | data[10];
	uint32_t w =
		(data[21]<<24) | (data[20]<<16) | (data[19]<<8) | data[18];
	uint32_t h =
		(data[25]<<24) | (data[24]<<16) | (data[23]<<8) | data[22];
	unsigned char *px = data + off;

	for(int y = 0; y < 8; y++)
	for(int x = 0; x < 16; x++)
	for(int n = 0; n < GH; n++)
	for(int p = 0; p < GW; p++)
	{
		int k = pixel(px, w, h, x * (GW + 2) + p,
			y * (GH + 2) + n);
		putchar(k ? '1' : '0');
	}

	free(data);
	return 0;
}
