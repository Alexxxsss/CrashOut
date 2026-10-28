#pragma once

#include <Windows.h>

class Window
{
public: 
	void StartUpWindow();

private:
    //MUST be static so it matches WNDCLASSW's function pointer requirement
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HWND myHwnd = nullptr;

};
