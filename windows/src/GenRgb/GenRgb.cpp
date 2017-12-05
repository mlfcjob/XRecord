

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

#pragma comment(lib, "opencv_world330d.lib")

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
	char name[16] = {0};
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

int main()
{
	VideoCapture vw;
	Mat frame;
	Mat des;
	int width, height, elemSize;
	static int frame_count = 0;

	vw.open(0);
	if (!vw.isOpened())
	{
		cout << "open camera failed" << endl;
	}

	width = vw.get(CAP_PROP_FRAME_WIDTH);
	height = vw.get(CAP_PROP_FRAME_HEIGHT);

	namedWindow("frame");

	int len = -1;

	FILE *fp = fopen("out.rgb", "w+");
	for (;;)
	{
		if (frame_count++ > 300)
		{
			elemSize = frame.elemSize();
			break;
		}
		
		vw.grab();
		vw.retrieve(frame);
	
		imshow("frame", frame);
		len = fwrite(frame.data, 1, frame.cols * frame.rows * frame.elemSize(), fp);
		WriteBitmap(frame_count, frame.data, frame.cols, frame.rows, frame.elemSize());
		waitKey(40);
	}

	cout << "total bytes size " << width * height * elemSize * (frame_count - 1) << endl;
	fclose(fp);
	return 0;

}

