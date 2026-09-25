#include <iostream>
#include <string>
#include <filesystem>

#include "outputLog.h"
#include "wavFile.h"


int main(int argc, char* argv[])
{
	// 実行場所がカレントディレクトリになる（exeファイルの場所）
	std::filesystem::path p = std::filesystem::current_path();
	std::cout << "current: " << p << std::endl;
	std::string processPath = p.string();
	
	// ログの設定
	C_outputLog::SetFilePath(processPath);
	C_outputLog::DebugLog("start");

	C_outputLog::DebugLog("processPath: " + processPath);



	//////////////////////////////
	// tmpフォルダ内の編集対象のファイルを取得
	//////////////////////////////
	
	

	

	std::string txtFile = "";
	std::string wavFile = "";
	std::string checkWavFile = "";
	// wavファイル
	for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(processPath)) {
		C_outputLog::DebugLog(file.path().filename().string());
		if (file.path().extension().string() == ".wav") {
			wavFile = file.path().filename().string();
			checkWavFile = file.path().stem().string();
		}
	}
	// txtファイル
	for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(processPath)) {
		C_outputLog::DebugLog(file.path().filename().string());
		if (file.path().extension() == ".txt") {
			if (file.path().stem().string() == checkWavFile) {
				txtFile = file.path().filename().string();
			}
		}
	}
	C_outputLog::DebugLog("txtFile: " + txtFile);
	C_outputLog::DebugLog("wavFile: " + wavFile);

	//////////////////////////////
	// 音声ファイルの無音区間をカット
	//////////////////////////////
	// wavファイルの読み込み
	C_wavFile wavFileClass;
	if (wavFileClass.loadWaveFile(processPath + "\\" + wavFile) == false) {
		C_outputLog::ErrorLog("[結果]処理に失敗しました。");
		return -1;
	}

	// 先頭、末尾の無音をカット
	wavFileClass.CutSilenceStart();
	wavFileClass.CutSilenceEnd();

	//////////////////////////////
	// ファイルを移動
	//////////////////////////////
	std::string parentPath = p.parent_path().string();
	if (wavFileClass.wirteWaveFile(processPath + "\\" + wavFile, parentPath + "\\" + wavFile) == false) {
		C_outputLog::ErrorLog("[結果]処理に失敗しました。");
		return -1;
	}
	// txtファイルを移動
	std::filesystem::rename(processPath + "\\" + txtFile, parentPath + "\\" + txtFile);
	// 旧wavファイルを削除
	std::filesystem::remove(processPath + "\\" + wavFile);
	


	C_outputLog::DebugLog("end");

	return 0;
}




