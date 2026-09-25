#pragma once
#include <iostream>
#include "outputLog.h"


class C_wavFile
{
private:
	struct DATA_CHUNK
	{
		long size = 0;
		double* data = new double(0.0);
	};
	DATA_CHUNK dataData;


public:

	bool loadWaveFile(std::string fileName)
	{
		FILE* fp;
		errno_t err = fopen_s(&fp, fileName.c_str(), "rb");
		if (err != 0) {
			C_outputLog::ErrorLog("編集前ファイルが開けませんでした。");
			return false;
		}

		
		while (feof(fp) == 0) {
			char id[4] = "";
			fread(id, 1, 4, fp);
			if (strncmp(id, "RIFF", 4) == 0) {
				long riffSize;
				char riffType[4];
				fread(&riffSize, 4, 1, fp);
				fread(riffType, 1, 4, fp);
			}
			else if (strncmp(id, "data", 4) == 0) {
				fread(&dataData.size, 4, 1, fp);
				dataData.data = (double*)calloc((dataData.size / 2), sizeof(double));
				for (int n = 0; n < (dataData.size / 2); n++) {
					short data;
					fread(&data, 2, 1, fp);
					dataData.data[n] = (double)data / 32768.0;
				}
				continue;
			}
			else if (strncmp(id, "JUNK", 4) == 0 || strncmp(id, "fmt ", 4) == 0) {
				long otherSize;
				fread(&otherSize, 4, 1, fp);
				char* otherData = new char[otherSize];
				fread(otherData, 1, otherSize, fp);
			}
			else {
				break;
			}

		}

		fclose(fp);

		return true;
	}

	bool wirteWaveFile(std::string beforeFilePath, std::string afterFilePath)
	{
		FILE* beforeFp;
		errno_t err = fopen_s(&beforeFp, beforeFilePath.c_str(), "rb");
		if (err != 0) {
			C_outputLog::ErrorLog("編集前ファイルが開けませんでした。");
			return false;
		}

		FILE* afterFp;
		errno_t err2 = fopen_s(&afterFp, afterFilePath.c_str(), "wb");
		if (err2 != 0) {
			C_outputLog::ErrorLog("編集後ファイルが開けませんでした。");
			return false;
		}

		while (feof(beforeFp) == 0) {
			char id[4] = "";
			fread(id, 1, 4, beforeFp);
			if (strncmp(id, "RIFF", 4) == 0) {
				fwrite(id, 1, 4, afterFp);
				long riffSize;
				char riffType[4];
				fread(&riffSize, 4, 1, beforeFp);
				fread(riffType, 1, 4, beforeFp);
				fwrite(&riffSize, 4, 1, afterFp);
				fwrite(riffType, 1, 4, afterFp);
			}
			else if (strncmp(id, "fmt ", 4) == 0) {
				// fmt_チャンク
				fwrite(id, 1, 4, afterFp);
				long otherSize;
				fread(&otherSize, 4, 1, beforeFp);
				char* otherData = new char[otherSize];
				fread(otherData, 1, otherSize, beforeFp);

				fwrite(&otherSize, 4, 1, afterFp);
				fwrite(otherData, 1, otherSize, afterFp);

				// dataチャンク
				char dataIdStr[4] = { 'd','a','t','a' };
				fwrite(dataIdStr, 1, 4, afterFp);
				fwrite(&dataData.size, 4, 1, afterFp);

				for (int n = 0; n < (dataData.size / 2); n++) {
					double s;
					s = (dataData.data[n] + 1.0) / 2.0 * 65536.0;

					if (s > 65535.0) {
						s = 65535.0;
					}
					else if (s < 0.0) {
						s = 0.0;
					}
					short sData = (short)(s + 0.5) - 32768;
					fwrite(&sData, 2, 1, afterFp);
				}
			}
			else if (strncmp(id, "data", 4) == 0) {
				DATA_CHUNK damyData;
				char damyId[4];
				fread(damyId, 1, 4, beforeFp);
				fread(&damyData.size, 4, 1, beforeFp);
				damyData.data = (double*)calloc((damyData.size / 2), sizeof(double));
				for (int n = 0; n < (damyData.size / 2); n++) {
					short data = 0;
					fread(&data, 2, 1, beforeFp);
					damyData.data[n] = (double)data / 32768.0;
				}
				continue;
			}
			else if (strncmp(id, "JUNK", 4) == 0) {
				fwrite(id, 1, 4, afterFp);
				long otherSize;
				fread(&otherSize, 4, 1, beforeFp);
				char* otherData = new char[otherSize];
				fread(otherData, 1, otherSize, beforeFp);

				fwrite(&otherSize, 4, 1, afterFp);
				fwrite(otherData, 1, otherSize, afterFp);
				delete otherData;
			}
			else {
				break;
			}

		}

		fclose(beforeFp);
		fclose(afterFp);


		return true;
	}

	// 先頭の無音をカット
	bool CutSilenceStart()
	{
		double* cutData = new double(0.0);
		// コピー
		cutData = (double*)calloc((dataData.size / 2), sizeof(double));
		for (int n = 0; n < (dataData.size / 2); n++) {
			cutData[n] = dataData.data[n];
		}

		// カット位置確認
		int cutTime = 0;
		for (int n = 0; n < (dataData.size / 2); n++) {
			if (-0.001 > dataData.data[n] || dataData.data[n] > 0.001) {
				cutTime = n * 2;
				break;
			}
		}

		// カット処理
		dataData.data = (double*)calloc((dataData.size / 2), sizeof(double));
		int dataCnt = 0;
		for (int n = (cutTime / 2); n < (dataData.size / 2); n++) {
			dataData.data[dataCnt] = cutData[n];
			dataCnt++;
		}
		// サイズ
		dataData.size -= cutTime;

		return true;
	}

	// 末尾の無音をカット
	bool CutSilenceEnd()
	{
		int start = dataData.size;
		bool flagSilence = false;
		for (int n = ((dataData.size / 2) - 1); n >= 0; n--) {
			if (-0.001 > dataData.data[n] || dataData.data[n] > 0.001) {
				start = n * 2;
				break;
			}
		}
		

		dataData.size = start;

		return true;
	}



};

