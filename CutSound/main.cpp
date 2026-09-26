#include <iostream>
#include <string>
#include <filesystem>
#include <thread>
#include <format>
#include <chrono>
#include "outputLog.h"
#include "wavFile.h"

std::string GetTimeNow();

int main(int argc, char* argv[])
{
	// 実行場所がカレントディレクトリになる（exeファイルの場所）
	std::filesystem::path p = std::filesystem::current_path();
	std::string processPath = p.string();
	
	// ログの設定
	C_outputLog::SetFilePath(processPath);
	//C_outputLog::DebugLog("start");

	//C_outputLog::DebugLog("processPath: " + processPath);

	std::cout << "CutSoundForYMM4" << std::endl;
	std::cout << "start process" << std::endl;

	int errCnt = 0;
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

		// 待機させる
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//////////////////////////////
		// 音声ファイルの無音区間をカット
		//////////////////////////////
		// wavファイルの読み込み
		C_wavFile wavFileClass;
		if (wavFileClass.loadWaveFile(processPath + "\\" + wavFile) == false) {
			C_outputLog::ErrorLog("[結果]wavファイルの読み込みに失敗しました。");
			errCnt++;
			if (errCnt > 5) {
				break;
			}
			continue;
		}

		// 先頭、末尾の無音をカット
		wavFileClass.CutSilenceStart();
		wavFileClass.CutSilenceEnd();

		//////////////////////////////
		// ファイルを移動
		//////////////////////////////
		std::string parentPath = p.parent_path().string();
		if (wavFileClass.wirteWaveFile(processPath + "\\" + wavFile,  parentPath + "\\" + GetTimeNow() + wavFile) == false) {
			C_outputLog::ErrorLog("[結果]wavファイルの移動に失敗しました。");
			errCnt++;
			if (errCnt > 5) {
				break;
			}
			continue;
		}
		// txtファイルを移動
		std::filesystem::rename(processPath + "\\" + txtFile, parentPath + "\\" + GetTimeNow() + txtFile);
		// 旧wavファイルを削除
		std::filesystem::remove(processPath + "\\" + wavFile);

		continue;
	}


	//C_outputLog::DebugLog("end");
	return 0;
}

std::string GetTimeNow()
{

	auto now_utc = std::chrono::system_clock::now();
	auto now_utc_sec = std::chrono::time_point_cast<std::chrono::seconds>(now_utc);
	auto now_jst = std::chrono::zoned_time{ "Asia/Tokyo", now_utc_sec };
	std::string nowTimeStr = std::format("{:%Y%m%d_%H%M%S_}", now_jst);

	return nowTimeStr;
}


