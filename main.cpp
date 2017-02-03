// for orthophoto generation
#include "ortho.h"
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>

#include <chrono>
#include <iostream>
#include <ctime>
#include "stdafx.h"
#include "ApxModifier.h"
#include <string.h>
#include <iostream>

void LogFile(char* filename, int st, char* st_hms, int et, char* et_hms, int pt)
{
	FILE* pFile = NULL;
	fopen_s(&pFile, "C:\\Log_file/TimeOrthophoto.csv", "a+");
	fseek(pFile, 0, SEEK_SET);

	int nLine = 1;
	while (!feof(pFile))
	{
		if (fgetc(pFile) == 10) // 라인 끝에 도착 Line 증가
		{
			nLine++;
		}
	}
	fprintf(pFile, "%s,%.3f,'%s,%.3f,'%s,%.3f\n", filename, double(st*0.001), st_hms, double(et*0.001), et_hms, double((pt)*0.001));
	fclose(pFile);
}

void main() {

	SetCurrentDirectoryA("C://uav_image/");
	const int timeWait = 500;

	char file_name[500][500];
	int x;
	WIN32_FIND_DATAA find_data;
	HANDLE find_file;

	char sttime_hms[30], edtime_hms[30];
	std::time_t tNow = std::time(NULL);
	std::tm tmHMS;
	localtime_s(&tmHMS, &tNow);
	tmHMS.tm_hour = 0;
	tmHMS.tm_min = 0;
	tmHMS.tm_sec = 0;

	std::time_t theDay000 = std::mktime(&tmHMS);

	for (int k = 0; k < 10000; k++)
	{
		x = 0;
		//처리할 영상에 대한 영상 위치자세 데이터의 파일명 읽어오기
		find_file = ::FindFirstFileA("*.txt", &find_data);
		if (find_file != INVALID_HANDLE_VALUE) {
			do {

				if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
				else
				{
					char *c = find_data.cFileName;
					printf("%s \n", c);

					strcpy_s(file_name[x], c);
					printf("");
					x++;
				};
			} while (::FindNextFileA(find_file, &find_data));

			::FindClose(find_file);
		}
		if (x == 0)
		{
			Sleep(timeWait);
		}
		else
		{
			bool fProcessed = false;
			for (int i = 0; i < x; i++)
			{
				std::chrono::system_clock::time_point tpDay000 = std::chrono::system_clock::from_time_t(theDay000);

				std::chrono::system_clock::time_point tpStart = std::chrono::system_clock::now();
				int stms = std::chrono::duration_cast<std::chrono::milliseconds>(tpStart - tpDay000).count();
				int sthour = std::chrono::duration_cast<std::chrono::hours>(tpStart - tpDay000).count();
				int stminute = std::chrono::duration_cast<std::chrono::minutes>(tpStart - tpDay000).count();
				int stsecond = std::chrono::duration_cast<std::chrono::seconds>(tpStart - tpDay000).count();
				sprintf_s(sttime_hms, "%d:%d:%.3lf", sthour, int(stminute - sthour * 60), double(stsecond - sthour * 3600 - (stminute - sthour * 60) * 60 + (stms*0.001 - stsecond)));
				printf("%s\n", sttime_hms);				

				char* EOname = file_name[i];

				if ( ortho(EOname) )//txt 파일을 넘겨 받아 영상 처리
				{					
					//처리된 txt 파일 저장 경로 변경
					char Dpath[500] = "C://uav_image/Done/";
					strcat_s(Dpath, file_name[i]);
					rename(file_name[i], Dpath);
					fProcessed = true;
				}

				//Sleep(4000);

				std::chrono::system_clock::time_point tpEnd = std::chrono::system_clock::now();

				int edms = std::chrono::duration_cast<std::chrono::milliseconds>(tpEnd - tpDay000).count();
				int edhour = std::chrono::duration_cast<std::chrono::hours>(tpEnd - tpDay000).count();
				int edminute = std::chrono::duration_cast<std::chrono::minutes>(tpEnd - tpDay000).count();
				int edsecond = std::chrono::duration_cast<std::chrono::seconds>(tpEnd - tpDay000).count();
				int processtime = std::chrono::duration_cast<std::chrono::milliseconds>(tpEnd - tpStart).count();
				sprintf_s(edtime_hms, "%d:%d:%.3lf", edhour, int(edminute - edhour * 60), double(edsecond - edhour * 3600 - (edminute - edhour * 60) * 60 + (edms*0.001 - edsecond)));
				printf("%s\n", edtime_hms);
				LogFile(EOname, stms, sttime_hms, edms, edtime_hms, processtime);
			}
			if (!fProcessed)
			{
				//처리할 영상이 없는 경우 코드가 잠시 쉬었다가 동작
				Sleep(timeWait);
			}
		}
	}	
}