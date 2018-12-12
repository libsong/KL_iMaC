#pragma once

#include <windows.h>
#include <QString>
#include "we_types.h"

//https://blog.csdn.net/wangshubo1989/article/details/52586516

class CCrashStack
{
private:
	PEXCEPTION_POINTERS m_pException;

private:
	QString GetModuleByRetAddr(PBYTE Ret_Addr, PBYTE & Module_Addr);
	QString GetCallStack(PEXCEPTION_POINTERS pException);
	QString GetVersionStr();
	bool GetHardwareInaformation(QString &graphics_card, QString &sound_deivce);

public:
	CCrashStack(PEXCEPTION_POINTERS pException);

	QString GetExceptionInfo();
};
