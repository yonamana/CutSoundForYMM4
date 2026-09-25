#include <iostream>
#include <string>
#include "outputLog.h"
#include "wavFile.h"

int main(int argc, char* argv[])
{
	std::string beforeFilePath = argv[1];
	std::string afterFilePath = argv[2];

	// wavファイルの読み込み
	C_wavFile wavFile;
	if (wavFile.loadWaveFile(beforeFilePath) == false) {
		C_outputLog::ErrorLog("[結果]処理に失敗しました。");
		return -1;
	}
	
	// 先頭、末尾の無音をカット
	wavFile.CutSilenceStart();
	wavFile.CutSilenceEnd();

	// カットした音声ファイルの保存
	if (wavFile.wirteWaveFile(beforeFilePath, afterFilePath) == false) {
		C_outputLog::ErrorLog("[結果]処理に失敗しました。");
		return -1;
	}
	


	return 0;
}




