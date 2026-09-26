#pragma once
#include <fstream>
#include <filesystem>
#include <string>

class C_outputLog
{
private:
	//static const std::string filePath;
	static std::string filePath;

	static std::string GetLogTime()
	{
		auto now_utc = std::chrono::system_clock::now();
		auto now_utc_sec = std::chrono::time_point_cast<std::chrono::seconds>(now_utc);
		auto now_jst = std::chrono::zoned_time{ "Asia/Tokyo", now_utc_sec };
		std::string nowTimeStr = std::format("{:%Y/%m/%d %H:%M:%S}", now_jst);
		nowTimeStr = "[" + nowTimeStr + "]";
		return nowTimeStr;
	}

public:
	static void SetFilePath(std::string folderPath)
	{
		filePath = folderPath + "\\LOG_CutSoundForYMM4";
		std::filesystem::create_directories(filePath);
		filePath += "\\LOG.txt";
		return;
	}

	static void DebugLog(const std::string logText)
	{
		if (filePath == "") {
			return;
		}

		std::ofstream logData;
		logData.open(filePath, std::ios::app);

		logData << GetLogTime() << "Debug: " << logText << std::endl;

		logData.close();
		return;
	}

	static void ErrorLog(const std::string logText)
	{
		if (filePath == "") {
			return;
		}

		std::ofstream logData;
		logData.open(filePath, std::ios::app);

		logData << GetLogTime() << "Error: " << logText << std::endl;

		logData.close();
		return;
	}



};

