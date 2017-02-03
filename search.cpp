//#include <stdio.h>
//#include <Windows.h>
//#include <WinInet.h>
#include "search.h"

void Search()
{
	SetCurrentDirectoryA("D://test_ftp/");

	char file_name[300][300];
	int x = 0;
	WIN32_FIND_DATAA find_data;
	HANDLE find_file;
	find_file = ::FindFirstFileA("*.*", &find_data);
	if (find_file != NULL) {
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
}