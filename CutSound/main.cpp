#include <iostream>
#include <string>
#include <filesystem>
#include <thread>
#include "outputLog.h"
#include "wavFile.h"

std::string GetTimeNow();

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



	while (true) {
		// CPU使用率対策
		std::this_thread::sleep_for(std::chrono::seconds(1));

		//////////////////////////////
		// 編集対象になるファイルがあるか確認
		//////////////////////////////
		std::string txtFile = "";
		std::string wavFile = "";
		std::string checkWavFile = "";
		// wavファイル
		for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(processPath)) {
			if (file.path().extension().string() == ".wav") {
				wavFile = file.path().filename().string();
				checkWavFile = file.path().stem().string();
				break;
			}
		}
		// txtファイル
		for (const std::filesystem::directory_entry& file : std::filesystem::directory_iterator(processPath)) {
			if (file.path().extension() == ".txt") {
				if (file.path().stem().string() == checkWavFile) {
					txtFile = file.path().filename().string();
					break;
				}
			}
		}
		// 対象ファイルがない場合
		if (txtFile == "" || wavFile == "") {
			continue;
		}

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
		if (wavFileClass.wirteWaveFile(processPath + "\\" + wavFile,  parentPath + "\\" + GetTimeNow() + wavFile) == false) {
			C_outputLog::ErrorLog("[結果]処理に失敗しました。");
			return -1;
		}
		// txtファイルを移動
		std::filesystem::rename(processPath + "\\" + txtFile, parentPath + "\\" + GetTimeNow() + txtFile);
		// 旧wavファイルを削除
		std::filesystem::remove(processPath + "\\" + wavFile);

		continue;
	}


	C_outputLog::DebugLog("end");
	return 0;
}

std::string GetTimeNow()
{
	time_t nowTimeData = time(nullptr);
	tm* nowTime = new tm;
	errno_t err = localtime_s(nowTime, &nowTimeData);
	if (err) {
		return "time unknown_";
	}

	std::string nowTimeStr = "";
	nowTimeStr = std::to_string(nowTime->tm_year + 1900);
	nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_mon + 1);
	nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_mday);
	nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_hour);
	nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_min);
	nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_sec) + "_";

	return nowTimeStr;
}


