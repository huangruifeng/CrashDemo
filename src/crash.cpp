#include <Windows.h>
#include <DbgHelp.h>
#include <tchar.h>
#include <shlobj_core.h>
#include <io.h>
#include <direct.h>
 
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
 
// 生成 dump 文件的目录名称
const std::string& kDUMPDir = "\\MyDumpTest";
typedef BOOL(WINAPI * MiniDumpWriteDumpT)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
	PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);
 
int CreateDump(PEXCEPTION_POINTERS pointers) {
	HMODULE dbg_help = LoadLibrary("DbgHelp.dll");
	if (NULL == dbg_help) {
		return EXCEPTION_CONTINUE_EXECUTION;
	}
 
	MiniDumpWriteDumpT dump_writer = (MiniDumpWriteDumpT)GetProcAddress(dbg_help, "MiniDumpWriteDump");
	if (NULL == dump_writer) {
		FreeLibrary(dbg_help);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
 
	// 获取到 %localappdata% 目录
	char buffer[256] = "";
	SHGetSpecialFolderPathA(NULL, buffer, CSIDL_LOCAL_APPDATA, FALSE);
 
	std::string dump_dir = buffer;
	dump_dir += kDUMPDir;
	std::cout << "dump_dir:" << dump_dir << std::endl;
	// 判断文件夹是否存在
	if (_access(dump_dir.c_str(), 0) == -1) {
		// 如果不存在，那么就创建
		_mkdir(dump_dir.c_str());
	}
 
	// dmp 文件名
	SYSTEMTIME local_time;
	GetLocalTime(&local_time);
 
	std::stringstream ss;
	ss << dump_dir << "\\dump-" << local_time.wYear << "-" <<
		std::setw(2) << std::setfill('0') << local_time.wMonth << "-" <<
		std::setw(2) << std::setfill('0') << local_time.wDay << "-" <<
		std::setw(2) << std::setfill('0') << local_time.wHour <<
		std::setw(2) << std::setfill('0') << local_time.wMinute <<
		std::setw(2) << std::setfill('0') << local_time.wSecond << ".dmp";
 
	HANDLE dump_file = CreateFile(ss.str().c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if (INVALID_HANDLE_VALUE == dump_file) {
		FreeLibrary(dbg_help);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
 
	// 写入 dmp 文件
	MINIDUMP_EXCEPTION_INFORMATION param;
	param.ThreadId = GetCurrentThreadId();
	param.ExceptionPointers = pointers;
	param.ClientPointers = FALSE;
 
    // MiniDumpWithDataSegs : dump 文件的类型，一般这个就够了
    // MiniDumpNormal 比较小 MiniDumpWithFullMemory 比较大
	dump_writer(GetCurrentProcess(), GetCurrentProcessId(),
		dump_file, MiniDumpWithDataSegs, (pointers ? &param : NULL), NULL, NULL);
 
	// 释放文件
	CloseHandle(dump_file);
	FreeLibrary(dbg_help);
	return 0;
}
 
LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS pointers) {
	// 这里做一些异常的过滤或提示
	if (IsDebuggerPresent()) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	return CreateDump(pointers);
}
int main(int argc, char *argv[]) {
    std::cout << "start" << std::endl;
	SetUnhandledExceptionFilter(ExceptionFilter);
	// 执行程序
	int* p = nullptr;
	p[0] = 1;
	std::cout << "end" << std::endl;
	getchar();
	return 0;
}