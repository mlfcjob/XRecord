
#include "XVideoWriter.h"
#include <iostream>

using namespace std;

int main()
{
	char outFile[] = "rgbpcm.mp4";
	char inVideoFile[] = "test.rgb";
	int len;

	FILE *fp = fopen(inVideoFile, "rb");

	

	XVideoWriter *xw = XVideoWriter::Get(0);
	int readSize = xw->inWidth * xw->inHeight * 4;
	unsigned char *rgb = new unsigned char[readSize];

	cout << xw->Init(outFile)<< endl;
	cout << xw->AddVideoStream()<< endl;

	// rgb to yuv
	for (;;) {
		len = fread(rgb, 1, readSize, fp);
		if (len <= 0)
		{
			break;
		}
		cout << "read len: " << len << endl;
		cout << xw->RgbToYuv(rgb) << endl;
	}


	// encode video frame

	delete rgb;
	getchar();
	return 0;
}