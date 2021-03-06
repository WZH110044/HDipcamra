
#include "stdafx.h" 
#include <stdio.h>  
#include <winsock2.h>  
#include <windows.h>
#include <cstring>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <atlbase.h>  
//#include <thread>
//#include <atlbase.h>  
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include "libavcodec/avcodec.h"
}
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib, "ws2_32.lib")   
#define ASSERT(X) if(!(X)){printf("####[%s:%d]assert failed:%s\n", __FUNCTION__, __LINE__, #X);}
#define _READ_BUF_SIZE (4<<10)
//广播包
BYTE buffer[] = { 0xf1, 0x30, 0x00, 0x00 };
//请求视频
BYTE buffer2[] = {
	0xf1, 0xd0, 0x00, 0xbe, 0xd1, 0x00, 0x00, 0x00,
	0x01, 0x0a, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
	0x47, 0x45, 0x54, 0x20, 0x2f, 0x6c, 0x69, 0x76,
	0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x2e,
	0x63, 0x67, 0x69, 0x3f, 0x73, 0x74, 0x72, 0x65,
	0x61, 0x6d, 0x69, 0x64, 0x3d, 0x31, 0x30, 0x26,
	0x73, 0x75, 0x62, 0x73, 0x74, 0x72, 0x65, 0x61,
	0x6d, 0x3d, 0x30, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64, 0x6d,
	0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69, 0x6e,
	0x70, 0x61, 0x73, 0x3d, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x26, 0x75, 0x73, 0x65, 0x72, 0x3d,
	0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70, 0x77,
	0x64, 0x3d, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x26, 0x01, 0x0a, 0x00, 0x00, 0x49, 0x00, 0x00,
	0x00, 0x47, 0x45, 0x54, 0x20, 0x2f, 0x63, 0x68,
	0x65, 0x63, 0x6b, 0x5f, 0x75, 0x73, 0x65, 0x72,
	0x2e, 0x63, 0x67, 0x69, 0x3f, 0x6c, 0x6f, 0x67,
	0x69, 0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64,
	0x6d, 0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x70, 0x61, 0x73, 0x3d, 0x38, 0x38, 0x38,
	0x38, 0x38, 0x38, 0x26, 0x75, 0x73, 0x65, 0x72,
	0x3d, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70,
	0x77, 0x64, 0x3d, 0x38, 0x38, 0x38, 0x38, 0x38,
	0x38, 0x26 };
//
BYTE buffer3[] = {
	0xf1, 0x41, 0x00, 0x14, 0x56, 0x53, 0x54, 0x42,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x18, 0x72,
	0x52, 0x4d, 0x53, 0x52, 0x50, 0x00, 0x00, 0x00 };
//左转
BYTE buffer4[] = {
	0xf1,0xd0,0x00,0x6e,0xd1,0x00,0x00,0x01,0x01,
	0x0a,0x00,0x00,0x62,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x64,0x65,0x63,0x6f,0x64,0x65,
	0x72,0x5f,0x63,0x6f,0x6e,0x74,0x72,0x6f,0x6c,
	0x2e,0x63,0x67,0x69,0x3f,0x63,0x6f,0x6d,0x6d,
	0x61,0x6e,0x64,0x3d,0x37,0x26,0x6f,0x6e,0x65,
	0x73,0x74,0x65,0x70,0x3d,0x30,0x26,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x35,0x32,0x31,0x35,0x32,
	0x31,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x35,
	0x32,0x31,0x35,0x32,0x31,0x26 };
BYTE buffer41[] = {
	0xf1,0xd0,0x00,0x6e,0xd1,0x00,0x00,0x04,0x01,
	0x0a,0x00,0x00,0x62,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x64,0x65,0x63,0x6f,0x64,0x65,
	0x72,0x5f,0x63,0x6f,0x6e,0x74,0x72,0x6f,0x6c,
	0x2e,0x63,0x67,0x69,0x3f,0x63,0x6f,0x6d,0x6d,
	0x61,0x6e,0x64,0x3d,0x36,0x26,0x6f,0x6e,0x65,
	0x73,0x74,0x65,0x70,0x3d,0x30,0x26,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x35,0x32,0x31,0x35,0x32,
	0x31,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x35,
	0x32,0x31,0x35,0x32,0x31,0x26 };

//拍照
BYTE buffer7[] = {
	0xf1,0xd0,0x00,0x59,0xd1,0x00,0x00,0x01,0x01,
	0x0a,0x00,0x00,0x49,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x73,0x6e,0x61,0x70,0x73,0x68,
	0x6f,0x74,0x2e,0x63,0x67,0x69,0x3f,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x35,0x32,0x31,0x35,0x32,
	0x31,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x35,
	0x32,0x31,0x35,0x32,0x31,0x26,0x72,0x65,0x73,
	0x3d,0x31,0x26 };
BYTE buffer5[] = {
	0xf1, 0xd1, 0x00, 0x06,0xd1,0x00,0x00,0x01,0x00,0x50 };
BYTE buffer8[] = {
	0xf1, 0xe1, 0x00, 0x00 };
BYTE buffer9[] = {
	0xf1, 0xe0, 0x00, 0x00 };
//get_status.cgi
BYTE buffer6[] = {
	0xf1,0xd0,0x00,0xad,0xd1,0x00,0x00,0x01,0x01,
	0x0a,0x00,0x00,0x49,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x67,0x65,0x74,0x5f,0x73,0x74,
	0x61,0x74,0x75,0x73,0x2e,0x63,0x67,0x69,0x3f,
	0x6c,0x6f,0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,
	0x61,0x64,0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,
	0x69,0x6e,0x70,0x61,0x73,0x3d,0x38,0x38,0x38,
	0x38,0x38,0x38,0x26,0x75,0x73,0x65,0x72,0x3d,
	0x61,0x64,0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,
	0x3d,0x38,0x38,0x38,0x38,0x38,0x38,0x26,0x01,
	0x0a,0x00,0x00,0x50,0x00,0x00,0x00,0x47,0x45,
	0x54,0x20,0x2f,0x67,0x65,0x74,0x5f,0x66,0x61,
	0x63,0x74,0x6f,0x72,0x79,0x5f,0x70,0x61,0x72,
	0x61,0x6d,0x2e,0x63,0x67,0x69,0x3f,0x6c,0x6f,
	0x67,0x69,0x6e,0x75,0x73,0x65,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x6c,0x6f,0x67,0x69,0x6e,
	0x70,0x61,0x73,0x3d,0x38,0x38,0x38,0x38,0x38,
	0x38,0x26,0x75,0x73,0x65,0x72,0x3d,0x61,0x64,
	0x6d,0x69,0x6e,0x26,0x70,0x77,0x64,0x3d,0x38,
	0x38,0x38,0x38,0x38,0x38,0x26 };
//521521密码的请求视频
BYTE buffer1[] = {
	0xf1, 0xd0, 0x00, 0xbe, 0xd1, 0x00, 0x00, 0x00,
	0x01, 0x0a, 0x00, 0x00, 0x61, 0x00, 0x00, 0x00,
	0x47, 0x45, 0x54, 0x20, 0x2f, 0x6c, 0x69, 0x76,
	0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x6d, 0x2e,
	0x63, 0x67, 0x69, 0x3f, 0x73, 0x74, 0x72, 0x65,
	0x61, 0x6d, 0x69, 0x64, 0x3d, 0x31, 0x30, 0x26,
	0x73, 0x75, 0x62, 0x73, 0x74, 0x72, 0x65, 0x61,
	0x6d, 0x3d, 0x30, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64, 0x6d,
	0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69, 0x6e,
	0x70, 0x61, 0x73, 0x3d, 0x35, 0x32, 0x31, 0x35,
	0x32, 0x31, 0x26, 0x75, 0x73, 0x65, 0x72, 0x3d,
	0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70, 0x77,
	0x64, 0x3d, 0x35, 0x32, 0x31, 0x35, 0x32, 0x31,
	0x26, 0x01, 0x0a, 0x00, 0x00, 0x49, 0x00, 0x00,
	0x00, 0x47, 0x45, 0x54, 0x20, 0x2f, 0x63, 0x68,
	0x65, 0x63, 0x6b, 0x5f, 0x75, 0x73, 0x65, 0x72,
	0x2e, 0x63, 0x67, 0x69, 0x3f, 0x6c, 0x6f, 0x67,
	0x69, 0x6e, 0x75, 0x73, 0x65, 0x3d, 0x61, 0x64,
	0x6d, 0x69, 0x6e, 0x26, 0x6c, 0x6f, 0x67, 0x69,
	0x6e, 0x70, 0x61, 0x73, 0x3d, 0x35, 0x32, 0x31,
	0x35, 0x32, 0x31, 0x26, 0x75, 0x73, 0x65, 0x72,
	0x3d, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x26, 0x70,
	0x77, 0x64, 0x3d, 0x35, 0x32, 0x31, 0x35, 0x32,
	0x31, 0x26
};

#include <string.h>  



#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL/SDL.h"
	FILE __iob_func[3] = { *stdin,*stdout,*stderr };
};
#else
#endif
#pragma comment(lib, "legacy_stdio_definitions.lib")
uint8_t readb[9][30001] = {0};
int n = 0;
int getdiff(char ch);
int t = 0;

//Output YUV420P 
#define OUTPUT_YUV420P 0
FILE *fp_open = NULL;
int filescount = 0;
char filepath2[] = "v260.264";
char filetxt[] = "v260.txt";
int filecount = 0;
char filepath[] = "v261.264";
int bytecount = 0;
int r = 1;
//回调函数
int j = 0;
int read_buffer(void *opaque, uint8_t *buf, int buf_size) {
	int i = 0;
	int nul = 0;
	//uint8_t *buff = (unsigned char *)malloc(32768);
pool:
	if (r <= t) { r++; }
	else goto pool;
		while (i < 30000) {
			buf[i] = readb[(filecount % 9)][i];
			i++;
		}
				filecount += 1;
		return 30000;
}
int SDLplay()
{


	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;


	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();


	//Init AVIOContext
	unsigned char *aviobuffer = (unsigned char *)av_malloc(30000);
	/*AVIOContext *avio_alloc_context(
	unsigned char *buffer,
	int buffer_size,
	int write_flag,
	void *opaque,
	int(*read_packet)(void *opaque, uint8_t *buf, int buf_size),
	int(*write_packet)(void *opaque, uint8_t *buf, int buf_size),
	int64_t(*seek)(void *opaque, int64_t offset, int whence));

	*/
	//fp_open = fopen(filepath, "wb+");

	//fopen_s(&fp_open, filepath, "wb+");
	AVIOContext *avio = avio_alloc_context(aviobuffer, 30000, 0, NULL, read_buffer, NULL, NULL);
	pFormatCtx->pb = avio;
	//uint8_t *buf =NULL;
	//std::cout<<avio->read_packet(NULL , buf, 32768);
	if (pFormatCtx->pb) { 
		//if (pFormatCtx->iformat) { std::cout << "sssss"; }
		std::cout << "dddddddddd"; }
	
	if (avformat_open_input(&pFormatCtx, NULL, NULL, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL)<0) {
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}
	if (videoindex == -1) {
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
		printf("Could not open codec.\n");
		return -1;
	}
	AVFrame	*pFrame, *pFrameYUV;
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	//uint8_t *out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	//avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	//SDL----------------------------
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	int screen_w = 0, screen_h = 0;
	SDL_Surface *screen;
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	screen = SDL_SetVideoMode(screen_w, screen_h, 0, 0);

	if (!screen) {
		printf("SDL: could not set video mode - exiting:%s\n", SDL_GetError());
		return -1;
	}
	SDL_Overlay *bmp;
	bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = screen_w;
	rect.h = screen_h;
	//SDL End--------------------------------------------------------------------------------------------------
	int ret, got_picture;

	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

#if OUTPUT_YUV420P 
	FILE *fp_yuv = fopen("output.yuv", "wb+");
#endif  
	SDL_WM_SetCaption("Simplest FFmpeg Mem Player", NULL);

	struct SwsContext *img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	//----------------------------------------------------------------------------------------------------------
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (n == 1) {
			break;
		}
		if (packet->stream_index == videoindex) {
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				return -1;
			}
			if (got_picture) {
				SDL_LockYUVOverlay(bmp);
				pFrameYUV->data[0] = bmp->pixels[0];
				pFrameYUV->data[1] = bmp->pixels[2];
				pFrameYUV->data[2] = bmp->pixels[1];
				pFrameYUV->linesize[0] = bmp->pitches[0];
				pFrameYUV->linesize[1] = bmp->pitches[2];
				pFrameYUV->linesize[2] = bmp->pitches[1];
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
#if OUTPUT_YUV420P
				int y_size = pCodecCtx->width*pCodecCtx->height;
				fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y 
				fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
				fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
#endif
				SDL_UnlockYUVOverlay(bmp);

				SDL_DisplayYUVOverlay(bmp, &rect);
				//Delay 40ms
				SDL_Delay(40);
			}
		}
		av_free_packet(packet);
	}
	sws_freeContext(img_convert_ctx);

#if OUTPUT_YUV420P 
	fclose(fp_yuv);
#endif 

	//fclose(fp_open);

	SDL_Quit();

	//av_free(out_buffer);
	av_free(pFrameYUV);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}
//
void change()
{
	filepath2[3] = ('0' + (filescount % 9));
	filetxt[3] = ('0' + (filescount % 9));
	char msg[256];
	FILE *rfp, *wfp;
	rfp = fopen(filetxt, "r");
	wfp = fopen(filepath2, "w+b");


	if (rfp == NULL || wfp == NULL)
		return;

	int i, num;
	while (!feof(rfp))
	{
		fgets(msg, 4, rfp);
		i = 0;
		while (msg[i] != 0x0a && i<1)//0x0a代表换行符，应为该文本打印出来是16进制的格式所以读取的ASCII的值是16进制的，而16进制中0x0a正好代表换行符  
		{ // if(msg[i]==0x20){i++;}
			num = 0;
			num = getdiff(msg[i]) * 16;//*16，  
			num += getdiff(msg[i + 1]);//加上后一位  

			i += 3;//跳过空格  
			fwrite(&num, 1, 1, wfp);//fwrite（指针存放输出数据的首地址，数据块的字节数，读取数据块的个数，文本指针）？不确定打印的结果1个字节1个字节打印所以用1  
		}
	}
	fclose(wfp);
	fclose(rfp);
}
//将字符转换成整数  
int getdiff(char ch)
{
	int diff = 0;
	if (ch >= '0' && ch <= '9')
		diff = ch - '0';
	else if (ch >= 'A' && ch <= 'F')
		diff = ch - 'A' + 10;//换位11到15的数字  
	else printf("不是0-15的数字");
	return diff;

}
//Callback


//std::vector<char *> fpath;
//char *filepathpath;
//filepathpath[0] = filepath;

void recvw() {
	FILE *FSPOINTER;
	//filetxt[3] =( '0'+(filescount % 9));
	fopen_s(&FSPOINTER, filepath2, "ab+");
	int lent = 0;
	int i = 8;
	char recvbuf[1200];

	WORD socketVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(socketVersion, &wsaData) != 0)
	{
		return;
	}
	SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN addrSrv;
	// addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); 
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(32108);
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.199.255");
	//INADDR_BROADCAST inet_addr("192.168.1.255");  
	int len = sizeof(addrSrv);

	//char * sendData = "来自客户端的数据包.\n";  
	//send( sclient,(LPCSTR)buffer, sizeof(buffer), 0); 
	//发送寻找局域网内摄像头的buffer数据包
	printf("发送广播包\n");
	sendto(sclient, (LPCSTR)buffer, sizeof(buffer), 0, (sockaddr *)&addrSrv, len);
	SOCKADDR_IN addrClient;
	//接收客户端回应

	recvfrom(sclient, recvbuf, sizeof(recvbuf), 0, (SOCKADDR *)&addrClient, &len);

	printf("接收到客户端回应\n");

	for (int q = 0; q<10; q++) { printf("%x\n", (unsigned char)recvbuf[q]); }
	closesocket(sclient);
	sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(32108);
	bind(sclient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	len = sizeof(SOCKADDR);
	//recvfrom(sclient, recvbuf, sizeof(recvbuf), 0, (SOCKADDR *)&addrClient, &len);



	Sleep(200);
	//发送buffer3（UID）信息，类似于登录
	sendto(sclient, (LPCSTR)buffer3, sizeof(buffer3), 0, (sockaddr *)&addrClient, len); //回应
	Sleep(2000);

	sendto(sclient, (LPCSTR)buffer1, sizeof(buffer1), 0, (sockaddr *)&addrClient, len); //请求视频

	BYTE bufferack[] = {
		0xf1, 0xd1, 0x00, 0x08,0xd1,0x01,0x00,
		0x02,
		0x00,0x00,0x00,0x01,
	};
	BYTE bufferackff[] = {
		0xf1, 0xd1, 0x00, 0x06,0xd1,0x01,0x00,
		0x01,
		0x00,0xff,
	};

	//loop:
	while (1) {
		if (n == 1) {
			break;
		}
		//接受
		recvfrom(sclient, recvbuf, sizeof(recvbuf), 0, (SOCKADDR *)&addrClient, &len);
		//判断是否是视频帧，获取帧号，判断是否是最后一帧，不是就继续接受

		if ((unsigned char)recvbuf[4] == 0xd1 && (unsigned char)recvbuf[5] == 0x01) {
			if ((unsigned char)recvbuf[8] == 0x55 && (unsigned char)recvbuf[9] == 0xaa) {
				//i是读取数据的起始点
				i = 41;


			}
			else {
				i = 8;
				//fopen_s(&FSPOINTER, "C:/Users/36970/Documents/software/ipcamera/v264.txt", "a+");
			}

			if ((unsigned char)recvbuf[7] % 2 == 0x01 && (unsigned char)recvbuf[7] != 0xFF)
			{
				int p = 11;
				unsigned char var = recvbuf[7];
				for (char q = 0x00; q < 0x02; q++)
				{
					bufferack[p] = var;
					var = var - 0x01;
					p = p - 2;
				}
				bufferack[8] = recvbuf[6];
				bufferack[10] = recvbuf[6];
				sendto(sclient, (LPCSTR)bufferack, sizeof(bufferack), 0, (sockaddr *)&addrClient, len); //回应
																										//q++;
			}
			else if ((unsigned char)recvbuf[7] == 0xFF) {
				bufferackff[8] = recvbuf[6];
				bufferackff[9] = recvbuf[7];
				sendto(sclient, (LPCSTR)bufferackff, sizeof(bufferackff), 0, (sockaddr *)&addrClient, len); //回应
				bufferackff[9] = 0xfe;
				sendto(sclient, (LPCSTR)bufferackff, sizeof(bufferackff), 0, (sockaddr *)&addrClient, len); //回应
			}
			//fclose(FSPOINTER);
			//fopen_s(&FSPOINTER, filepath2, "ab+");
			int nu;

			while (i<(((unsigned char)recvbuf[2] - 0x00) * 16 * 16 + ((unsigned char)recvbuf[3] - 0x00)) + 4) {
				
				//1032是最大数据长度 
				while (bytecount == 30000) {
					//t表示已写文件数
					t += 1;
					filescount += 1;
					bytecount = 0;
					readb[(filescount % 9)][bytecount] = (unsigned char)recvbuf[i];
		
					goto ii;
				}

				readb[(filescount%9)][bytecount] = (unsigned char)recvbuf[i];
			ii:
				i++;
				bytecount += 1;
			}

		}



		//记录视频帧数量，发送确认帧
		//视频帧头f1 d0 04 04 d1 01 00 a9（视频帧的序号）
		//确认帧格式：f1 d1 00 16（后面帧长度） d1 01 00 09（收到的帧的数量） 00 a6 00 a7 00 a8 00 a9 00 aa 00 ab 00 ac 00 ad 00 ae（帧编号）

	}
	//fclose(FSPOINTER);
	closesocket(sclient);
	WSACleanup();
}
//size_t fread(void *buffer, size_t size, size_t count, FILE *stream);
//参 数
//buffer
//用于接收数据的内存地址
//size
//要读的每个数据项的字节数，单位是字节
//count
//要读count个数据项，每个数据项size个字节.
//stream
//输入流
int main(int argc, char* argv[]) {
	boost::thread thrd(recvw);
	thrd.detach();
loop:
	if (t > 0) {
		std::cout << "hhhhhhhhhhhhhh";
		boost::thread thrd2(SDLplay);
		thrd2.detach();
	}
	else goto loop;
	//SDLplay();
	while (1) {
		std::cin >> n;
		if (n == 1) { break; }
	}
	return 0;
}
