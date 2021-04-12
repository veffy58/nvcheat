#include <Windows.h>
#include <iostream>
#include "driver.h"
#include <string>
#include "offsets.h"
#include <iostream>
#include <string>
#include <sstream>
#include <WinInet.h>
#include <urlmon.h>
#include <direct.h>
#include "menu/main.h""
#include "menu/globals.h"
#include "menu/cheat.h"
#include "menu/direct3d.h"
#include "menu/imgui.h"
#include "menu/imgui_impl_dx9.h"
#include "xor.hpp"
#include "thread.h"
#pragma comment(lib, "wininet.lib")

char WINNAME[16] = " "; //Overlay Window Name
char TARGETNAME[64] = "Rust"; //Target WindowName Here!

DWORD procID;
MARGINS MARGIN = { 0, 0, Globals::rWidth, Globals::rHeight };

BOOL UNLOADING = FALSE;
BOOL NOTFOUND = FALSE;
BOOL INITIALIZED = FALSE;

WNDCLASSEX wndClass;
bool skid = false;

std::string replaceAll(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

std::string DownloadString(std::string URL) {
	HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	HINTERNET urlFile;
	std::string rtn;
	if (interwebs) {
		urlFile = InternetOpenUrlA(interwebs, URL.c_str(), NULL, NULL, NULL, NULL);
		if (urlFile) {
			char buffer[2000];
			DWORD bytesRead;
			do {
				InternetReadFile(urlFile, buffer, 2000, &bytesRead);
				rtn.append(buffer, bytesRead);
				memset(buffer, 0, 2000);
			} while (bytesRead);
			InternetCloseHandle(interwebs);
			InternetCloseHandle(urlFile);
			std::string p = replaceAll(rtn, "|n", "\r\n");
			return p;
		}
	}
	InternetCloseHandle(interwebs);
	std::string p = replaceAll(rtn, "|n", "\r\n");
	return p;
}

void checkhwid() {
	setlocale(0, "TR");
	bool valid = false;
	DWORD VSnumber;
	GetVolumeInformationA("C:\\", 0, 0, &VSnumber, 0, 0, 0, 0);
	std::stringstream sosi;
	sosi << VSnumber;
	std::string str = sosi.str();
	std::string lol = DownloadString(_xor_("https://flickyazilim.com/HWID.php").c_str());
	std::string s;
	std::stringstream ss(lol);
	while (ss >> s) {
		if (s == str) {
			valid = true;
			break;
		}
	}
	if (valid == true) {
		std::cout << _xor_("OK").c_str() << std::endl;
		skid = true;
	}
	else {
		std::cout << str << std::endl;
		std::cout << _xor_("veffy e gönder knk").c_str() << std::endl;
	}
	Sleep(2000);
}

void startcitos() {
	if (skid == true)
	{
		if (driver::open_memory_handles())
			std::cout << "[-] Shared memory handles opened successfully" << std::endl;
		else
		{
			std::cout << "[!] Shared memory handles failed to open" << std::endl;
			std::cin.get();
		}

		driver::get_process_id("RustClient.exe"); // RustClient.exe

		game_assembly = (uintptr_t)driver::get_module_base_address("GameAssembly.dll");
		unity_player = (uintptr_t)driver::get_module_base_address("UnityPlayer.dll");
		if (!game_assembly || !unity_player)
		{
			std::cout << "	[+] game assembly: " << std::hex << game_assembly << std::endl;
			std::cout << "	[+] unity player: " << std::hex << unity_player << std::endl;

			std::cout << "[!] failed to aquire base address..." << std::endl;
			std::cin.get();
		}

		std::cout << "[+] aquired base address and PID" << std::endl;
		std::cout << "	[+] game assembly: " << std::hex << game_assembly << std::endl;
		std::cout << "	[+] unity player: " << std::hex << unity_player << std::endl;

	}

}

HANDLE memory_read = NULL, memory_write = NULL, memory_esp_write = NULL;

int WINAPI main(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	checkhwid();
	startcitos();
	//std::thread run_cheat(thread::features);

	//Thread Creation
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Cheat::Update, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Cheat::LateUpdate, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread::features, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UpdateWinPosition, 0, 0, 0);
	
	//Find Target Window
	while (Globals::rWidth < 640 && Globals::rHeight < 480) {
		Globals::tWnd = FindWindow(NULL, TARGETNAME);
	
		RECT wSize;
		GetWindowRect(Globals::tWnd, &wSize);
		Globals::rWidth = wSize.right - wSize.left;
		Globals::rHeight = wSize.bottom - wSize.top;
	}
	
	//Get All Access Handle
	GetWindowThreadProcessId(Globals::tWnd, &procID);
	Globals::hGame = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	
	//Initialize Overlay Window
	Globals::hWnd = InitializeWin((HINSTANCE)hInst);
	MSG uMessage;
	
	if (Globals::hWnd == NULL) { exit(1); }
	
	ShowWindow(Globals::hWnd, SW_SHOW);
	
	INITIALIZED = TRUE;
	
	while (!UNLOADING) {
		if (PeekMessage(&uMessage, Globals::hWnd, 0, 0, PM_REMOVE)) {
			DispatchMessage(&uMessage);
			TranslateMessage(&uMessage);
		}
	
		//Timed MessageBox
		if (UNLOADING) {
			HWND hMsg = FindWindow(NULL, "Info");
	
			if (hMsg) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				SendMessageA(hMsg, WM_CLOSE, 0, 0);
			}
		}
	}
	
	//Cleanup and unload our module 
	//NOTE: Leftover code from internal overlay!
	DestroyWindow(Globals::hWnd);
	UnregisterClass(WINNAME, (HINSTANCE)hInst);
	FreeLibraryAndExitThread((HMODULE)hInst, 0);
	return 0;
}

HWND WINAPI InitializeWin(HINSTANCE hInst) {

	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbClsExtra = NULL;
	wndClass.cbWndExtra = NULL;
	wndClass.hCursor = LoadCursor(0, IDC_ARROW);
	wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	wndClass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
	wndClass.hInstance = hInst;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.lpszClassName = WINNAME;
	wndClass.lpszMenuName = WINNAME;
	wndClass.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wndClass)) {
		exit(1);
	}

	Globals::hWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, WINNAME, WINNAME, WS_POPUP, 1, 1, Globals::rWidth, Globals::rHeight, 0, 0, 0, 0);
	SetLayeredWindowAttributes(Globals::hWnd, 0, 255, LWA_ALPHA);
	SetLayeredWindowAttributes(Globals::hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);

	D3DInitialize(Globals::hWnd);

	return Globals::hWnd;
}

void UpdateWinPosition() {
	while (!UNLOADING) {
		UpdateSurface(Globals::hWnd);

		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

	return;
}

void WINAPI UpdateSurface(HWND hWnd) {
	RECT wSize;
	HWND tWnd;

	tWnd = FindWindow(NULL, TARGETNAME);

	if (!tWnd && hWnd && !UNLOADING && !NOTFOUND) { ShowWindow(hWnd, SW_HIDE); MessageBox(NULL, "Game closed! Shutting down cheat client...", "Info", MB_OK | MB_ICONINFORMATION); UNLOADING = TRUE; }

	if (tWnd) {
		GetWindowRect(tWnd, &wSize);
		Globals::rWidth = wSize.right - wSize.left;
		Globals::rHeight = wSize.bottom - wSize.top;

		DWORD dwStyle = GetWindowLong(tWnd, GWL_STYLE);
		if (dwStyle & WS_BORDER)
		{
			wSize.top += 23; Globals::rHeight -= 23;
			//wSize.left += 10; rWidth -= 10;
		}

		if (hWnd) {
			MoveWindow(hWnd, wSize.left, wSize.top, Globals::rWidth, Globals::rHeight, true);
		}
	}

	return;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
	switch (uMessage) {
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &MARGIN);
		break;

	case WM_PAINT:
		D3DRender();
		break;

	case WM_DESTROY:
		//Cleanup Resources
		ImGui::Shutdown();
		DeleteObject(wndClass.hbrBackground);
		DestroyCursor(wndClass.hCursor);
		DestroyIcon(wndClass.hIcon);
		DestroyIcon(wndClass.hIconSm);

		PostQuitMessage(1);
		break;

	default:
		ImGui_ImplWin32_WndProcHandler(hWnd, uMessage, wParam, lParam);
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
		break;
	}

	return 0;
}