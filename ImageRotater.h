
// ImageRotater.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CImageRotaterApp:
// Сведения о реализации этого класса: ImageRotater.cpp
//

class CImageRotaterApp : public CWinApp
{
public:
	CImageRotaterApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CImageRotaterApp theApp;
