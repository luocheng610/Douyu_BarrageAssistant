
// stdafx.cpp : 只包括标准包含文件的源文件
// BarrageAssistant.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"



void DoEvent()
{
	static MSG msg;
	PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

	::TranslateMessage(&msg);
	::DispatchMessage(&msg);
}
