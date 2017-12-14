
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
		return -1;
	}

	FILE *afp = fopen(pcmFile, "rb");
	if (!afp){
		cout << "fopen " << pcmFile << endl;
		getchar();
		return -1;
	}

	XVideoWriter *xw = XVideoWriter::Get(0);

	cout << xw->Init(outFile)<< endl;
	cout << xw->AddVideoStream()<< endl;
	cout << xw->AddAudioStream() << endl;

	// rgb to yuv
	int readSize = xw->inWidth * xw->inHeight * 4;
	unsigned char *rgb = new unsigned char[readSize];

	// pcm resamlpe
	int aReadSize = xw->outChannels * xw->outNBSample * 2;
	unsigned char *pcm = new unsigned char[aReadSize];

	xw->WriteHeader();

	for (;;) {
		if (xw->IsVideoBefore()) {
			len = fread(rgb, 1, readSize, fp);
			if (len <= 0)
			{
				break;
			}

			AVPacket *pkt = xw->EncodeVideo(rgb);
			if (!pkt)
			{
				cout << "EncodeVideo failed" << endl;
				continue;
			}

			if (!xw->WriteFrame(pkt))
			{
				cout << "WriteFrame failed";
				continue;
			}
		}
		else {
			len = fread(pcm, 1, aReadSize, afp);
			if (len <= 0)
			{
				break;
			}

			AVPacket *apkt = xw->EncodeAudio(pcm);
			if (!xw->WriteFrame(apkt))
			{
				cout << "Write audio frame failed" << endl;
				continue;
			}
		}
	}

	xw->WriteEnd();

	cout << "======================end ================" << endl;

	// encode video frame
	delete rgb;
	rgb = NULL;

	delete pcm;
	pcm = NULL;

	fclose(fp);
	fclose(afp);

	getchar();
	return 0;
}