
#include <iostream>
#include <d3d9.h>

using namespace std;

#pragma comment(lib, "d3d9.lib")

// capture full screen
void CaptureScreen(void *data)
{
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);

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
		CaptureScreen(buf);
		fwrite(buf, 1, size, fp);
		Sleep(100);
	}

	fclose(fp);
	getchar();
	return 0;
}