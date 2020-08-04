#include "display.h"
/*屏幕初始化*/
u32 *init_fb()
{
	/*步骤*/
	// 1、open screen
	// 2、mmap,内存映射
	// 3、wirte(), 通过对映射内存的写操作,实现对显示数据的设置

	int fd = open("/dev/fb0", O_RDWR); // 设备文件打开用读写方式
	if(fd < 0)
	{
		perror("fail to open");
		exit(1);
	}

	struct fb_var_screeninfo vinfo;   // 保存屏幕信息的结构体(在#include<linux/fb.h>中)
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo); // 获取屏幕信息(保存在结构体中)
	FB_W = vinfo.xres;    // 可视画面的x、y轴分辨率
	FB_H = vinfo.yres;
                                  
	u32 size = FB_W * FB_H * vinfo.bits_per_pixel / 8; // 屏幕数据在内存中空间大小
													   // vinfo.bits_per_pixel一个像素点占的bit	
	//内存映射
	u32 *addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(addr == (void *)-1)
	{
		perror("fail to mmap");
		exit(1);
	}
	return addr;
}                                                                                      

/*画点*/
void drawPoint(size_t *pmem, size_t x, size_t y, size_t colour)
{
	size_t *p = NULL;
	
	p = pmem + x + y * FB_W;
	*p = colour;
}

/*横线*/
void drawXLine(size_t *pmem, size_t x, size_t y, size_t len, size_t colour)
{
	size_t i = 0;
	size_t *p = NULL;
	for(i = 0; i < len; ++i)
	{
		drawPoint(pmem, x + i, y, colour);
	}
}

/*竖线*/
void drawYLine(size_t *pmem, size_t x, size_t y, size_t len, size_t colour)
{
	size_t i = 0;
	size_t *p = NULL;

	for(i = 0; i < len; ++i)
	{
		drawPoint(pmem, x, y + i, colour);
	}
}

/*清屏*/
void clearScreen(size_t *pmem)
{
	int i, j;

	for(i = 0; i < FB_W; ++i)
	{
		for(j = 0; j < FB_H; ++j)
		{
		//	puts("clear");
			drawPoint(pmem, i, j, 0x00ffffff); // 遍历屏幕调画点函数,画白色
		}
	}
}

/*斜线*/
void drawDiagonalLing(size_t *pmem, size_t x1, size_t y1, size_t x2, size_t y2, size_t colour)
{
	size_t *p = NULL;	
	double i = 0,j = 0;
	double tekxx = (long)x2-(long)x1;
	double tekyy = (long)y2-(long)y1;
	double k = tekyy / tekxx;   // 计算斜率
	for(i = x1;i != x2; ++i)
	{
		j = (i - x1)* k + y1;   // 计算点的坐标
		drawPoint(pmem, i, j, colour);   // 将点的四个方向都点亮,这样画出来的线会粗用点
		drawPoint(pmem, i - 1, j, colour);
		drawPoint(pmem, i + 1, j, colour);
		drawPoint(pmem, i, j - 1, colour);
		drawPoint(pmem, i, j + 1, colour);
	}
	if(x1==x2)         // 斜率为0的情况,直接画竖线
	{
		int i=0,j=0;
		for(i=0;i<3;++i)
		{
			for(j=y1;j<y2;j++)
			{
				drawPoint(pmem,x1+i,j,colour);
			}
		}
	}
}

/*矩形*/
void drawRecangle(u32 *pmem, u32 x, u32 y, u32 long_, u32 weight_, u32 colour)
{
	drawXLine(pmem, x, y, long_, colour);
	drawYLine(pmem, x, y, weight_, colour);
	drawXLine(pmem, x, y + weight_, long_, colour);
	drawYLine(pmem, x + long_, y, weight_, colour);
}


/*显示图片*/
/*参数: x,y 图片插入的位置*/
/*		width,height图片的宽高*/
void show_bmp(u32 *pmem, int x, int y, int width, int height, char *filename)
{
	//	step 1: 打开图片
	int fd_bmp = open(filename, O_RDONLY);
	if (fd_bmp == -1) 
	{
		return;
	}
	//	54 字节 | 图像信息	
	char *p = malloc(width * height * 3); // bmp图像,一个像素点3字节
	char *p1 = p;
	read (fd_bmp, p, 54);  // 读走54字节的图片信息
	read (fd_bmp, p, width * height * 3);  // 再读图片内容

	int i, j;
	// 将3字节改为字节并点亮
	for (j = height-1; j >= 0; j--)  // 注意遍历的方向
	{
		for (i = 0; i < width; i++) 
		{
			size_t b, g, r;
			size_t col;
			b = *p++;  // p是char型指针, +1 后移1个字节
			g = *p++;
			r = *p++;
			col = (r << 16) | (g << 8) | (b << 0); // 为运算重新组合成RGB4四字节数据
			drawPoint(pmem, i + x, j + y, col);   // 调用画点,依次点亮
		}
	}
                                                                                                           
	//	malloc 在堆上申请内存
	free(p1);
	p1 = NULL;
	p = NULL;
	close(fd_bmp);
}

// 自模像素点一个点是8bit(没有颜色,只是点亮或没点亮)
unsigned char wang[40 / 8* 35] = 
{
	/*--  文字:  王  --*/
	/*--  Arial Narrow26;  此字体下对应的点阵为：宽x高=35x35   --*/
	/*--  宽度不是8的倍数，现调整为：宽度x高度=40x35  --*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x1E,0x00,0x1F,0xFF,
	0xFF,0xFF,0x00,0x0E,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,
	0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,
	0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x10,0x00,
	0x00,0x00,0xE0,0x38,0x00,0x07,0xFF,0xFF,0xFC,0x00,0x03,0x80,0xE0,0x00,0x00,0x00,
	0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,
	0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,
	0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x00,0xE0,0x03,
	0x00,0x00,0x00,0xE0,0x07,0x80,0x7F,0xFF,0xFF,0xFF,0xC0,0x38,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


/*打印字模函数*/
void draw_gb2312(u32 *pmem, u32 x, u32 y, unsigned char *font_yan, int high, int len)
{
	int i, j, k;
	unsigned char temp;
	for(i = 0;i < high; i++)
	{
		for(k = 0;k < len; k++)
		{
			temp = font_yan[i * len + k];
			for(j = 0; j < 8; j++)
			{
				if(temp & 0x80) // 点亮
				{
					drawPoint(pmem,8*k+x+j, y+i, rgb888(0,0,0));
				}
				else
				{
					drawPoint(pmem,8*k+x+j, y+i, rgb888(255,255,255));
				}
				temp=temp<<1;
			}
		}

	}
}

/*显示单个汉字*/
int display_show_utf8(size_t *pmem, UTF8_INFO *info, int x, int y, char *zi, u32 col, u32 col1)
{
	unsigned long  out = 0 ;

	//将一个字符的UTF8编码转换成Unicode(UCS-2和UCS-4)编码.
	int ret = enc_utf8_to_unicode_one((unsigned char*)zi, &out); 

	unsigned char* data = get_utf_data(info, out);  // 根据编号得到字模
	unsigned char temp = 0 ;
	unsigned int i,j,k;
	unsigned int num = 0;
	for(i = 0;i < info -> height; i++)
	{
		for(j = 0;j < info->width/8; j++)  // width / 8 一行所占字节数
		{
			temp = data[num++];
			for(k=0;k<8;k++)
			{
				if(0x80&temp)
				{
					drawPoint(pmem, x+k+j*8, y+i, col);
				}
				else
				{

					drawPoint(pmem, x+k+j*8, y+i, col1);
				}
				temp= temp<<1;
			}
		}

	}
	return ret;
}

/*显示汉字串*/
int display_show_utf8_str(size_t *pmem, UTF8_INFO *info, int arg_x, int arg_y,  char* zi, u32 col, u32 col1)
{
	char* temp = zi;
	unsigned int x = arg_x ;
	unsigned int y =  arg_y;

	while(*temp != '\0')
	{
		int ret = display_show_utf8(pmem, info, x, y, temp, col, col1);
		x += info->width;
		if(x > FB_W)
		{
			x = 0;
			y += info->height;
			if(y > FB_H)
			{
				y = 0;
			}
		}

		temp += ret;
	}
	return 0;
}




#if 0
int main(int argc, const char *argv[])
{
	size_t *pmem = init_fb();
	
	char ch;
	
//	clearScreen(pmem);
	while(1)
	{
		ch = getchar();
		switch(ch)
		{
		case '1':
			clearScreen(pmem);
			break;
		case '2':
			drawXLine(pmem, 100, 100, 300, 0x000000ff);
			break;
		case '3':
			drawYLine(pmem, 100, 100, 300, 0x007fff00);
			break;
		case '4':
			puts("diagonal");
			drawDiagonalLing(pmem, 100, 400, 400, 100, 0x00ff0000);
			drawDiagonalLing(pmem, 400, 100, 400, 400, 0x00ff0000);
			break;
		case '5':
			drawRecangle(pmem, 500, 100, 100, 200, 0x00ff00ff);
			break;
		case '6':
			show_bmp(pmem, 100, 100, 100, 100, "./img1.bmp");
			show_bmp(pmem, 300, 100, 100, 100, "./img2.bmp");
			break;
		case '7':
			draw_gb2312(pmem, 400, 300, wang, 35, 40/8); // 40/8是一行所占的字节数
			break;
		case '8':
			bzero(&utf8_info, sizeof(UTF8_INFO));  // 字模文件缓存结构体清0
			strcpy(utf8_info.path, ZIKUK_FILE_BIG); // 拷贝字模库文件路径
			utf8_info.width = 32;
			utf8_info.height = 32;
			init_utf8(&utf8_info);   // 将字库文件读取到缓存区文件
			display_show_utf8(pmem, &utf8_info, 400 ,400, "王", rgb888(0,0,0), rgb888(255, 255, 255));
			display_show_utf8_str(pmem, &utf8_info, 400, 450, "王栋梁",rgb888(0,0,0), rgb888(255,255,255));
			break;
			
		case 'q':
			munmap(pmem, 1024*600*4);
			close(fd);
			exit(0);
		default:
			break;
		}
	}


//	drawRecangle(pmem, 400, 100, 100, 50, 0x00ff00ff);

//	sleep(2);
//	show_bmp(pmem, 0, 0, 1024, 600, "./1.bmp");
//	close(fd);
	return 0;
}
#endif
