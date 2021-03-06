#include "pch.h"
#include "iostream"
#include "vector"
#include "windows.h"
//
//#ifdef __GNUC__
//#define FOR1(a) for (int i = 0;i < a;i++)
//#define FOR2(a,b) for (int i = a;i < b;i++)
//#define FOR3(a,b,c) for (int i = a;i < b;c++)
//#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
//#define FOR(...) GET_MACRO(__VA_ARGS__, FOO3, FOO2, FOO1)(__VA_ARGS__)
//#else
//#define FOR(...) for (int i = (std::vector<int>({ __VA_ARGS__ }).size() >= 2 ?  \
//std::vector<int>({ __VA_ARGS__ })[0] : 0)\
//;i < (std::vector<int>({ __VA_ARGS__ }).size() == 1 ?  \
//std::vector<int>({ __VA_ARGS__ })[0] : std::vector<int>({ __VA_ARGS__ })[1])\
//;i += (std::vector<int>({ __VA_ARGS__ }).size() >= 3 ?  \
//std::vector<int>({ __VA_ARGS__ })[2] : 1))
//#endif // __GNUC__

using namespace std;

int main(int argc, char *argv[]) {

	bool result = true;
	HKEY hKey = NULL;
	DWORD type;
	CHAR buffer[100];
	DWORD size = 100;
	auto ret = RegGetValueA(HKEY_CURRENT_USER, "SOFTWARE\\Tencent\\WeChat", "InstallPath", RRF_RT_REG_SZ, &type, buffer, &size);
	
	if (ret != ERROR_SUCCESS) {
		result = false;
		cout << GetLastError() << ret << endl;
	}
	
	string path = buffer;
	path += "\\WeChatWin.dll";
	auto hFile = CreateFileA(path.c_str(), GENERIC_READ ,  0x1, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE) {
		result = false;
		cout << "Open File Faild:" << GetLastError() << endl;
	}
	
	int fileSize = GetFileSize(hFile, NULL);
	char* fileBuffer = new char[fileSize];
	ReadFile(hFile, fileBuffer, fileSize, NULL, NULL);
	cout << "search Pattern..." << endl;
	BYTE Pattern[7] = { 0x83,0xC4,0x14,0x84,0xC0,0x74,0x72 };
	bool flag = false;
	int offset = 0;
	for (int j = 0; j < fileSize - sizeof(Pattern); j++) {
		flag = true;

		for (int i = 0; i < sizeof(Pattern); i++) {
			BYTE b = fileBuffer[j + i];
			if (b != Pattern[i]) {
				flag = false;
				break;
			}
		}
		if (flag) {
			offset = j;
			break;
		}
	}
	CloseHandle(hFile);

	//hFile = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE | 0x1, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	//DeleteFileA(path.c_str());

	cout << "offset:" << offset << endl;
	if (offset) {
		fileBuffer[offset + 5] = 0xEB;
	}
	else {
		cout << "Get Pattern Faild,Please Check Your Version or It's Cracked" << endl;
		result = false;
	}

	hFile = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | 0x1, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		cout << "CreateFile faild!" << endl;
		cout << "lastError:" << GetLastError() << endl;
		if (GetLastError() == 0x20) {
			cout << "please Close Your WeChat.exe and try again." << endl;
		}
	}


	if (!WriteFile(hFile, fileBuffer, fileSize, 0, 0)) {
		result = false;
		cout << "write faild!" << endl;
		cout << "lastError:" << GetLastError() << endl;
	}
	
	CloseHandle(hFile);

	if (result)
		cout << "patch success" << endl;

	system("pause");
	return 1;
}
