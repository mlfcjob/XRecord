
#include "XVideoWriter.h"
#include <iostream>

using namespace std;

int main()
{
	char outFile[] = "rgbpcm.mp4";
	char rgbFile[] = "test.rgb";
	char pcmFile[] = "test.pcm";
	int len;

	FILE *fp = fopen(rgbFile, "rb");
	if (!fp) {
		cout << "fopen" << rgbFile << endl;
		getchar();
	}

	XVideoWriter *xw = XVideoWriter::Get(0);

	cout << xw->Init(outFile)<< endl;
	cout << xw->AddVideoStream()<< endl;

	// rgb to yuv
	int readSize = xw->inWidth * xw->inHeight * 4;
	unsigned char *rgb = new unsigned char[readSize];

	xw->WriteHeader();

	for (;;) {
		len = fread(rgb, 1, readSize, fp);
		if (len <= 0)
		{
			break;
		}

		AVPacket *pkt = xw->EncodeVideo(rgb);
		if (!pkt)
		{
			cout<<"EncodeVideo failed" << endl;
			continue;
		}


		if (!xw->WriteFrame(pkt))
		{
			cout << "WriteFrame failed";
			continue;
		}
	}

	xw->WriteEnd();

	cout << "======================end ================" << endl;

	// encode video frame
	delete rgb;
	rgb = NULL;
	getchar();
	return 0;
}