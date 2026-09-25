#pragma once
#include <fstream>
#include <string>

class C_outputLog
{
private:
	static const std::string filePath;

	static std::string GetLogTime()
	{
		time_t nowTimeData = time(nullptr);
		tm* nowTime = new tm;
		errno_t err = localtime_s(nowTime, &nowTimeData);
		if (err) {
			return "[time unknown]";
		}

		std::string nowTimeStr = "";
		nowTimeStr = "[" + std::to_string(nowTime->tm_year + 1900) + "/";
		nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_mon + 1) + "/";
		nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_mday) + " ";
		nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_hour) + ":";
		nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_min) + ":";
		nowTimeStr = nowTimeStr + std::to_string(nowTime->tm_sec) + "]";

		return nowTimeStr;
	}

public:
	static void DebugLog(const std::string logText)
	{
		std::ofstream logData;
		logData.open(filePath, std::ios::app);

		logData << GetLogTime() << "Debug: " << logText << std::endl;

		logData.close();
		return;
	}

	static void ErrorLog(const std::string logText)
	{
		std::ofstream logData;
		logData.open(filePath, std::ios::app);

		logData << GetLogTime() << "Error: " << logText << std::endl;

		logData.close();
		return;
	}



};

