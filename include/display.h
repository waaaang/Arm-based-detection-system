#ifndef DISPLAY_H
#define DISPLAY_H

#include<stdio.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<linux/fb.h>
#include<string.h>
#include"utf.h"

// 设置颜色的宏
#define	rgb888(r,g,b)	(((r & 0xff)<<16)|((g & 0xff)<<8)|(b & 0xff))

//typedef size_t u32;
int FB_W;  // 显示屏的宽
int FB_H;  // 显示屏的高

UTF8_INFO utf8_info;  // 字模文件缓存结构体


extern u32 *init_fb();
extern void drawPoint(size_t *pmem, size_t x, size_t y, size_t colour);
extern void drawXLine(size_t *pmem, size_t x, size_t y, size_t len, size_t colour);
extern void drawYLine(size_t *pmem, size_t x, size_t y, size_t len, size_t colour);
extern void clearScreen(size_t *pmem);
extern void drawDiagonalLing(size_t *pmem, size_t x1, size_t y1, size_t x2, size_t y2, size_t colour);
extern void drawRecangle(u32 *pmem, u32 x, u32 y, u32 long_, u32 weight_, u32 colour);
extern void show_bmp(u32 *pmem, int x, int y, int width, int height, char *filename);
extern int display_show_utf8(size_t *pmem, UTF8_INFO *info, int x, int y, char *zi, u32 col, u32 col1);
extern int display_show_utf8_str(size_t *pmem, UTF8_INFO *info, int arg_x, int arg_y,  char* zi, u32 col, u32 col1);

#endif
