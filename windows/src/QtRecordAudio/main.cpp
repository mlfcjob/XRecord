#include <QAudioInput>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	QAudioFormat fmt;
	fmt.setSampleRate(44100);
	fmt.setChannelCount(2);
	fmt.setSampleSize(16);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setCodec("audio/pcm");

	QAudioInput *input = new QAudioInput(fmt);

	QIODevice *io = input->start();

	FILE *fp = fopen("out.pcm", "wb");
	char *buf = new char[1024];
	int total = 0;

	for (;;)
	{
		int br = input->bytesReady();
		if (br < 1024) continue;
		int len = io->read(buf, 1024);
		cout << len << " ";
		fwrite(buf, 1, len, fp);

		total += len;
		if (total > 1024 * 1024)
			break;
	}


	fclose(fp);
	return 0;
}
