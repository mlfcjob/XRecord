
#include <iostream>
#include <d3d9.h>

using namespace std;

#pragma comment(lib, "d3d9.lib")

typedef struct
{
	//unsigned short bfType;  // 2
	unsigned long  bfSize;    // 4
	unsigned short bfReserved1;  // 2
	unsigned short bfReserved2;  // 2
	unsigned long  bfOffBits;  // 4
}BitmapFileHeader;

typedef struct
{
	unsigned long biSize;   // 4
	long  biWidth;          // 4 
	long  biHeight;         // 4
	unsigned short biPlanes; // 2
	unsigned short biBitCount; // 2
	unsigned long biCompression; // 4
	unsigned long biSizeImage;  // 4
	long biXPelsPerMeter; // 4
	long biYPlesPerMeter;  //4
	unsigned long biClrUsed; // 4
	unsigned long biClrImportant; //4
}BitmapInfoHeader;


bool WriteBitmapHeader(FILE *fp, int width, int height, int channels)
{
	BitmapFileHeader header;
	BitmapInfoHeader info;
	int step;
	int offset;

	unsigned short fileType = 0x4D42;

	step = width * channels;

	fwrite(&fileType, sizeof(unsigned short), 1, fp);

	header.bfSize = height * step + 54;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = 54;
	fwrite(&header, 1, sizeof(BitmapFileHeader), fp);

	info.biSize = 40;
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = channels * 8;
	info.biCompression = 0;
	info.biSizeImage = height * step;
	info.biXPelsPerMeter = 0;
	info.biYPlesPerMeter = 0;
	info.biClrUsed = 0;
	info.biClrImportant = 0;
	fwrite(&info, 1, sizeof(BitmapInfoHeader), fp);

	return true;
}

int WriteBitmap(unsigned char index, unsigned char *rgb, int width, int height, int channels) {
	char name[16] = { 0 };
	sprintf(name, "%d.bmp", index);

	FILE *fp = fopen(name, "w+");
	if (!fp) {
		cout << "open " << name << "failed: " << strerror(errno) << endl;
		return -1;
	}

	WriteBitmapHeader(fp, width, height, channels);

	fwrite(rgb, 1, width * height * channels, fp);

	fclose(fp);
}


// capture full screen
void CaptureScreen(void *data, int index)
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

	cout << "w: " << w << ", h: " << h << endl;
	// 1 create d3d object
	static IDirect3D9 *d3d = NULL;

	if (!d3d) {
		d3d = Direct3DCreate9(D3D_SDK_VERSION);
	}

	if (!d3d) {
		return;
	}

	//2 create  graphics card device
	static IDirect3DDevice9 *device = NULL;
	if (!device)
	{
		D3DPRESENT_PARAMETERS pa;
		ZeroMemory(&pa,sizeof(D3DPRESENT_PARAMETERS));
		pa.Windowed = true;
		pa.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
		pa.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pa.hDeviceWindow = GetDesktopWindow();

		d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,0, 
			              D3DCREATE_HARDWARE_VERTEXPROCESSING, &pa, &device);
	}

	if (!device) {
		return;
	}
		
	// 3 create a offscreen surface
	static IDirect3DSurface9  *sur = NULL;
	if (!sur) {
		device->CreateOffscreenPlainSurface(w, h, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &sur, 0);
	}

	if (!sur) {
		return;
	}


	// 4 capture screen
	device->GetFrontBufferData(0, sur);

	// 5 pull out data
	D3DLOCKED_RECT rect;
	ZeroMemory(&rect, sizeof(rect));
	if (sur->LockRect(&rect, 0, 0) != S_OK)
	{
		return;
	}

	memcpy(data, rect.pBits, w * h * 4);
	sur->UnlockRect();

	WriteBitmap(index, (unsigned char*)data, w, h, 4);
	cout << " . " << endl;

}

int main(int argc, char *argv[])
{
	FILE *fp = fopen("out.rgb", "wb");
	if (!fp) {
		cout << "fopen failed" << strerror(errno) << endl;
		return -1;
	}

	int size = 1366 * 768 * 4;

	char *buf = new char[size];
	for (int i = 0; i < 100; i++)
	{
		CaptureScreen(buf, i+1);
		fwrite(buf, 1, size, fp);
		Sleep(100);
	}

	fclose(fp);
	getchar();
	return 0;
}