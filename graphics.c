/*
smallHex - hexadecimal editor for Windows and PS Vita
Copyright (C) 2015  Luciano Ciccariello (Xeeynamo)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "defines.h"
#include "graphics.h"

static int _GraphicsRefCount = 0;

#if defined(_WIN32)
#include <windows.h>

static HWND curHwnd = NULL;
static HDC curDc = NULL;
static HDC backDc = NULL;
static HBITMAP hBitmap = NULL;
static Surface curSurface;

void GetConsoleBufferSize(COORD *dwSize)
{
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	CONSOLE_FONT_INFO fontInfo;
	HANDLE conOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(conOut, &screenInfo);
	GetCurrentConsoleFont(conOut, FALSE, &fontInfo);
	dwSize->X = (screenInfo.srWindow.Right - screenInfo.srWindow.Left + 1) * fontInfo.dwFontSize.X;
	dwSize->Y = (screenInfo.srWindow.Bottom - screenInfo.srWindow.Top + 1) * fontInfo.dwFontSize.Y;
}
void OptimizeConsole()
{
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	HANDLE conOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(conOut, &screenInfo);
	screenInfo.dwSize.X = (screenInfo.srWindow.Right - screenInfo.srWindow.Left + 1);
	screenInfo.dwSize.Y = (screenInfo.srWindow.Bottom - screenInfo.srWindow.Top + 1);
	SetConsoleScreenBufferSize(conOut, screenInfo.dwSize);
}
#elif defined(PLATFORM_PSP2)
#include <psp2/display.h>
#include <psp2/gxm.h>

static SceDisplayFrameBuf fb[2];
static SceUID fb_memuid[2];
static int cur_fb = 0;

#define PSP2_SCREEN_WIDTH 960
#define PSP2_SCREEN_HEIGHT 544

static void *alloc_gpu_mem(uint32_t type, uint32_t size, uint32_t attribs, SceUID *uid)
{
	int ret;
	void *mem = NULL;

	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW)
		size = align_mem(size, 256 * 1024);
	else
		size = align_mem(size, 4 * 1024);

	*uid = sceKernelAllocMemBlock("gxm", type, size, NULL);

	printf("MemBlock uid: 0x%08X\n", *uid);

	ret = sceKernelGetMemBlockBase(*uid, &mem);
	printf("sceKernelGetMemBlockBase(): 0x%08X\n", ret);
	printf("MemBlockBase addr: %p\n", mem);
	if (ret != 0) {
		return NULL;
	}

	ret = sceGxmMapMemory(mem, size, attribs);
	printf("sceGxmMapMemory(): 0x%08X\n", ret);
	if (ret != 0) {
		return NULL;
	}

	return mem;
}
static int create_framebuffer(SceUID *uid, SceDisplayFrameBuf *fb)
{
	fb->size = sizeof(fb[0]);
	fb->pitch = PSP2_SCREEN_WIDTH;
	fb->pixelformat = PSP2_DISPLAY_PIXELFORMAT_A8B8G8R8;
	fb->width = PSP2_SCREEN_WIDTH;
	fb->height = PSP2_SCREEN_HEIGHT;

	fb->base = alloc_gpu_mem(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
		PSP2_SCREEN_WIDTH * PSP2_SCREEN_HEIGHT * sizeof(int), SCE_GXM_MEMORY_ATTRIB_RW, uid);
	return fb->base == NULL ? -1 : 0;
}
#endif

int GraphicsInit()
{
	if (_GraphicsRefCount <= 0)
	{
#if defined(_WIN32)
		COORD coord;
		BITMAPINFO bmi;

		curHwnd = GetConsoleWindow();
		curDc = GetDC(curHwnd);

		OptimizeConsole();
		GetConsoleBufferSize(&coord);
		curSurface.width = coord.X;
		curSurface.height = coord.Y;
		curSurface.pitch = curSurface.width;

		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
		bmi.bmiHeader.biWidth = curSurface.width;
		bmi.bmiHeader.biHeight = -curSurface.height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		backDc = CreateCompatibleDC(curDc);
		hBitmap = CreateDIBSection(backDc, &bmi, DIB_RGB_COLORS, (void**)&curSurface.data, NULL, 0);
		SelectObject(backDc, hBitmap);

#elif defined(PLATFORM_PSP2)
		SceGxmInitializeParams params;
		params.flags = 0x0;
		params.displayQueueMaxPendingCount = 0x2;
		params.displayQueueCallback = 0x0;
		params.displayQueueCallbackDataSize = 0x0;
		params.parameterBufferSize = (16 * 1024 * 1024);

		// initialize the GXM
		sceGxmInitialize(&params);

		// set-up and allocation of framebuffer
		if (CALL_FAILED(create_framebuffer(&fb_memuid[0], &fb[0]))) {
			printf("Could not allocate memory for fb[0]. %p", fb[0].base);
			return -1;
		}
		if (CALL_FAILED(create_framebuffer(&fb_memuid[1], &fb[1]))) {
			printf("Could not allocate memory for fb[1]. %p", fb[1].base);
			return - 1;
		}

		cur_fb = 0;
		GraphicsSwapBuffers(false);
#endif
	}
	return ++_GraphicsRefCount;
}

int GraphicsDestroy()
{
	if (--_GraphicsRefCount == 0)
	{
#if defined(_WIN32)
		SelectObject(backDc, NULL);
		DeleteObject(hBitmap);
		ReleaseDC(curHwnd, curDc);
		ReleaseDC(curHwnd, backDc);
		free(curSurface.data);
#elif defined(PLATFORM_PSP2)
		sceGxmUnmapMemory(fb[0].base);
		sceGxmUnmapMemory(fb[1].base);
		sceGxmTerminate();
#endif
	}
	return _GraphicsRefCount;
}

void GraphicsSetTitle(const char *title)
{
#if defined(_WIN32)
	SetConsoleTitle(title);
#endif
}

void GraphicsSwapBuffers(bool waitvsync)
{
#if defined(_WIN32)
	BitBlt(curDc, 0, 0, curSurface.width, curSurface.height, backDc, 0, 0, SRCCOPY);
	if (waitvsync)
		Sleep(1);

#elif defined(PLATFORM_PSP2)
	sceDisplaySetFrameBuf(&fb[cur_fb], PSP2_DISPLAY_SETBUF_NEXTFRAME);
	cur_fb ^= 1;
	if (waitvsync == true)
		sceDisplayWaitVblankStart();
#endif
}
void GetCurrentBuffer(Surface *surface)
{
#if defined(_WIN32)
	surface->width = curSurface.width;
	surface->height = curSurface.height;
	surface->pitch = curSurface.pitch;
	surface->data = curSurface.data;
#elif defined(PLATFORM_PSP2)
	surface->width = fb[cur_fb].width;
	surface->height = fb[cur_fb].height;
	surface->pitch = fb[cur_fb].pitch;
	surface->data = (unsigned int *)fb[cur_fb].base;
#endif
}