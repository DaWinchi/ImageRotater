
// ImageRotaterDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "ImageRotater.h"
#include "ImageRotaterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CImageRotaterDlg

CRITICAL_SECTION cs;

CImageRotaterDlg::CImageRotaterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGEROTATER_DIALOG, pParent)
	, _angle(45)
	, _angleStep(10)
	, _numCalc(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageRotaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, _drawer2D);
	DDX_Text(pDX, IDC_ANGLE, _angle);
	DDX_Control(pDX, IDC_EXPERIMENT, _drawerGraph);
	DDX_Text(pDX, IDC_ANGLESTEP, _angleStep);
	DDX_Text(pDX, IDC_COUNT, _numCalc);
}

BEGIN_MESSAGE_MAP(CImageRotaterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, &CImageRotaterDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_BUTTON1, &CImageRotaterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RUNEXPERIMENT, &CImageRotaterDlg::OnBnClickedRunexperiment)
	ON_BN_CLICKED(IDC_STOP, &CImageRotaterDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// Обработчики сообщений CImageRotaterDlg

BOOL CImageRotaterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	InitializeCriticalSection(&cs);
	_drawer2D._image = &_imageOriginal;
	_drawerGraph._points = &_graphPoints;
	_isCalculate = false;
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CImageRotaterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CImageRotaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CImageRotaterDlg::OnBnClickedLoad()
{
	LoadPicture();
	_drawer2D._image = &_imageOriginalUpper;
	_drawer2D.RedrawWindow();
}

void CImageRotaterDlg::LoadPicture()
{
	CFileDialog fd(true, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
		OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), NULL, 0, TRUE);

	if (fd.DoModal() != IDOK)
	{
		MessageBox(L"ERROR!!!", L"Error opening picture file.", MB_ICONERROR);
	};

	CString pathBMP = fd.GetPathName();
	Bitmap bmp(pathBMP);
	int width = bmp.GetWidth();
	int height = bmp.GetHeight();
	_imageOriginal.clear();

	for (size_t i = 0; i < height; i++)
	{
		std::vector<Pixel> bufPix;
		for (size_t j = 0; j < width; j++)
		{
			Pixel px;
			Color color;
			bmp.GetPixel(j, height - i - 1, &color);
			px.red = color.GetRed();
			px.green = color.GetGreen();
			px.blue = color.GetBlue();
			bufPix.push_back(px);
		}
		_imageOriginal.push_back(bufPix);
	}

	UpSizeImage();
}

void CImageRotaterDlg::UpSizeImage()
{
	const int sizeNew = (int)sqrt(_imageOriginal.size()*_imageOriginal.size() + _imageOriginal[0].size()*_imageOriginal[0].size());
	const int widthOld = _imageOriginal[0].size(),
		heightOld = _imageOriginal.size();
	const int sizeNewHalf = sizeNew / 2;

	_imageOriginalUpper.clear();
	std::vector<Pixel> row(sizeNew);
	_imageOriginalUpper.resize(sizeNew, row);
#pragma omp parallel for
	for (int y = 0; y < heightOld; y++)
	{
		for (int x = 0; x < widthOld; x++)
		{
			_imageOriginalUpper[y+sizeNewHalf - heightOld/2][x + sizeNewHalf - widthOld/2] = _imageOriginal[y][x];
		}
	}

	_imageRealTime = _imageOriginalUpper;

}

void CImageRotaterDlg::RotateImage(double angle, std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut)
{	
	const int size = dataIn.size();
	dataOut.clear();
	std::vector<Pixel> row(size);
	dataOut.resize(size, row);

	Matrix matrix;
	Point pointNew;
	const Point center(size / 2, size / 2);
	matrix.Translate(center.X, center.Y);
	matrix.Rotate(angle);

	Rect rect(0, 0, size, size);
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			pointNew.X = x - center.X;
			pointNew.Y = y - center.Y;
			matrix.TransformPoints(&pointNew);
			if (!rect.Contains(pointNew)) continue;
			dataOut[y][x] = dataIn[pointNew.Y][pointNew.X];
		}
	}

}


void CImageRotaterDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	RotateImage(_angle, _imageOriginalUpper, _imageRealTime);
	_drawer2D._image = &_imageRealTime;
	_drawer2D.RedrawWindow();
}

double CImageRotaterDlg::GetDifference(std::vector<std::vector<Pixel>> & dataOriginal, std::vector<std::vector<Pixel>> & dataResult)
{
	const int size = dataOriginal.size();
	double originalRed = 0, deviationRed = 0,
		originalGreen = 0, deviationGreen = 0, 
		originalBlue = 0, deviationBlue = 0;
#pragma omp parallel for reduction(+:originalRed,originalGreen,originalBlue,deviationRed,deviationGreen,deviationBlue)
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			originalRed += dataOriginal[i][j].red * dataOriginal[i][j].red;
			deviationRed += (dataOriginal[i][j].red - dataResult[i][j].red)*(dataOriginal[i][j].red - dataResult[i][j].red);

			originalGreen += dataOriginal[i][j].green * dataOriginal[i][j].green;
			deviationGreen += (dataOriginal[i][j].green - dataResult[i][j].green)*(dataOriginal[i][j].green - dataResult[i][j].green);

			originalBlue += dataOriginal[i][j].blue * dataOriginal[i][j].blue;
			deviationBlue += (dataOriginal[i][j].blue - dataResult[i][j].blue)*(dataOriginal[i][j].blue - dataResult[i][j].blue);
		}
	}

	return (deviationRed / originalRed + deviationGreen / originalGreen + deviationBlue / originalBlue) / 3;
}


void CImageRotaterDlg::OnBnClickedRunexperiment()
{
	UpdateData(TRUE);
	EnterCriticalSection(&cs);
	_isCalculate = true;
	_angleExp = 0;
	_graphPoints.clear();
	LeaveCriticalSection(&cs);
	hThreadCalculate = CreateThread(
		NULL,		// дескриптор защиты
		0,			// начальный размер стека ( Если это значение нулевое, новый поток использует по умолчанию размер стека исполняемой программы)
		(LPTHREAD_START_ROUTINE)ThreadCalculate,	 // функция потока
		this,		// параметр потока 
		0,			//oпции создания(здесь можно отложить запуск выполнения потока. Для запуска потока сразу же, передаём 0.)
		&pdwThreadCalculate);// идентификатор потока (указатель на переменную, куда будет сохранён идентификатор потока)
}

DWORD WINAPI ThreadCalculate(PVOID param)
{
	CImageRotaterDlg *dlg = (CImageRotaterDlg*)param;
	while (dlg->_isCalculate)
	{
		if (dlg->_angleExp > 360) break;
		EnterCriticalSection(&cs);
		dlg->CalculateDifference(dlg->_angleExp,
			dlg->_imageOriginalUpper, dlg->_imageExperimental);

		dlg->_angleExp += dlg->_angleStep;
		LeaveCriticalSection(&cs);
		dlg->_drawerGraph.RedrawWindow();
		if (dlg->_angleExp > 360) break;
	}
	return 0;
}

void CImageRotaterDlg::CalculateDifference(double angle,
	std::vector<std::vector<Pixel>> & dataOriginal, std::vector<std::vector<Pixel>> & dataOutput)
{
	double diff = 0;
	for (int k = 0; k < _numCalc; k++)
	{
		RotateImage(angle, dataOriginal, dataOutput);
		std::vector<std::vector<Pixel>> buffer = dataOutput;
		RotateImage(-angle, buffer, dataOutput);
		diff+= GetDifference(dataOriginal, dataOutput);
	}
	PointF pt(angle, (float)(diff/_numCalc));
	_graphPoints.push_back(pt);
}


void CImageRotaterDlg::OnBnClickedStop()
{
	_isCalculate = false;
}
