
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
	, _angleStep(5)
	, _numCalc(10)
	, _scale(1)
	, _numCalc2(3)
	, _scaleStep(0.1)
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
	DDX_Text(pDX, IDC_SCALE, _scale);
	DDX_Control(pDX, IDC_RESOLUTION, _resolutionText);
	DDX_Control(pDX, IDC_EXPERIMENT2, _drawerGraph2);
	DDX_Text(pDX, IDC_COUNT2, _numCalc2);
	DDX_Text(pDX, IDC_ANGLESTEP2, _scaleStep);
	DDX_Control(pDX, IDC_RADIO1, _radioCorrect);
	DDX_Control(pDX, IDC_RADIO2, _radioNotCorrect);
}

BEGIN_MESSAGE_MAP(CImageRotaterDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, &CImageRotaterDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_BUTTON1, &CImageRotaterDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RUNEXPERIMENT, &CImageRotaterDlg::OnBnClickedRunexperiment)
	ON_BN_CLICKED(IDC_STOP, &CImageRotaterDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_DIALOG2, &CImageRotaterDlg::OnBnClickedDialog2)
	ON_BN_CLICKED(IDC_RADIO1, &CImageRotaterDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CImageRotaterDlg::OnBnClickedRadio2)
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
	_drawerGraph2._points = &_graphPoints2;
	_isCalculate = false;
	_isCorrectRotating = true;
	_radioCorrect.SetCheck(BST_CHECKED);
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

	int widthText = _imageOriginalUpper[0].size(),
		heightText = _imageOriginalUpper.size();
	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), widthText);
	arg2.Format(_T("%d"), heightText);
	str += arg1 + "x" + arg2;
	_resolutionText.SetWindowTextW(str);
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

void CImageRotaterDlg::RotateImage(double angle, const std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut)
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

void CImageRotaterDlg::RotateImageNotCorrect(double angle, const std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut)
{
	const int size = dataIn.size();
	dataOut.clear();
	std::vector<Pixel> row(size);
	dataOut.resize(size, row);

	Matrix matrix;
	Point pointNew;
	const Point center(size / 2, size / 2);
	matrix.Translate(center.X, center.Y);
	matrix.Rotate(-angle);

	Rect rect(0, 0, size, size);
	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			pointNew.X = x - center.X;
			pointNew.Y = y - center.Y;
			matrix.TransformPoints(&pointNew);
			if (!rect.Contains(pointNew)) continue;
			dataOut[pointNew.Y][pointNew.X] = dataIn[y][x];
		}
	}

}




void CImageRotaterDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	int width = _imageOriginalUpper[0].size(),
		height = _imageOriginalUpper.size();
	InterpolateImage(_scale*width, _scale*height, _imageOriginalUpper, _imageAfterScalling);
	if(_isCorrectRotating) RotateImage(_angle, _imageAfterScalling, _imageRealTime);
	else RotateImageNotCorrect(_angle, _imageAfterScalling, _imageRealTime);

	_drawer2D._image = &_imageRealTime;
	_drawer2D.RedrawWindow();
	int widthText = _imageRealTime[0].size(),
		heightText = _imageRealTime.size();
	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), widthText);
	arg2.Format(_T("%d"), heightText);
	str += arg1 + "x" + arg2;
	_resolutionText.SetWindowTextW(str);
}

double CImageRotaterDlg::GetDifference(const std::vector<std::vector<Pixel>> & dataOriginal, const std::vector<std::vector<Pixel>> & dataResult)
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
	_graphPoints2.clear();
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
	double cf = 0.1;
	while (dlg->_isCalculate)
	{
		EnterCriticalSection(&cs);
		dlg->CalculateDifferenceRotate(dlg->_angleExp,
			dlg->_imageOriginalUpper, dlg->_imageExperimental);

		dlg->_angleExp += dlg->_angleStep;

		dlg->CalculateDifferenceInterpolation(cf, dlg->_imageOriginalUpper[0].size()*cf, dlg->_imageOriginalUpper.size()*cf,
			dlg->_imageOriginalUpper, dlg->_imageExperimental);
		cf += dlg->_scaleStep;
		dlg->_angleExp += dlg->_angleStep;

		LeaveCriticalSection(&cs);
		dlg->_drawerGraph.RedrawWindow();
		dlg->_drawerGraph2.RedrawWindow();
		if (dlg->_angleExp > 360) break;
	}
	return 0;
}

void CImageRotaterDlg::CalculateDifferenceRotate(double angle,
	const std::vector<std::vector<Pixel>> & dataOriginal, std::vector<std::vector<Pixel>> & dataOutput)
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

void CImageRotaterDlg::InterpolateImage(int width, int height, const std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut)
{
	int widthOld = dataIn[0].size(),
		heightOld = dataIn.size();

	
	double stepX = (double)widthOld / (width + 2.0);
	double stepY = (double)heightOld / (height + 2.0);
	std::vector<std::vector<Pixel>> tempData;

	for (int h = 0; h < heightOld; h++)
	{
		std::vector<Pixel> bufLine;
		for (int i = 0; i < width; i++)
		{
			double x = i * stepX;
			for (int w = 1; w < widthOld; w++)
			{
				if ((x >= w - 1) && (x < w))
				{					
					Pixel px;
					px.red = (double)(dataIn[h][w].red - dataIn[h][w - 1].red)*x +
						(double)(dataIn[h][w - 1].red - (dataIn[h][w].red - dataIn[h][w - 1].red)*((double)w - 1.f));
					px.green = (double)(dataIn[h][w].green - dataIn[h][w - 1].green)*x +
						(double)(dataIn[h][w - 1].green - (dataIn[h][w].green - dataIn[h][w - 1].green)*((double)w - 1.f));
					px.blue = (double)(dataIn[h][w].blue - dataIn[h][w - 1].blue)*x +
						(double)(dataIn[h][w - 1].blue - (dataIn[h][w].blue - dataIn[h][w - 1].blue)*((double)w - 1.f));
					bufLine.push_back(px);
				}
			}
		}
		Pixel px;
		int dif = width - bufLine.size();
		for (int l = 0; l < dif; l++)
		{
			bufLine.push_back(px);
		}
		tempData.push_back(bufLine);
	}

	std::vector<std::vector<Pixel>> transponData;
	for (int i = 0; i < width; i++)
	{
		std::vector<Pixel> bufCol;
		for (int j = 0; j < heightOld; j++)
		{
			bufCol.push_back(tempData[j][i]);
		}
		transponData.push_back(bufCol);
	}

	tempData.clear();

	widthOld = transponData[0].size();
	heightOld = transponData.size();

	width = height;
	for (int h = 0; h < heightOld; h++)
	{
		std::vector<Pixel> bufLine;
		for (int i = 0; i < width; i++)
		{
			double x = i * stepY;
			for (int w = 1; w < widthOld; w++)
			{
				if ((x >= w - 1) && (x < w))
				{
					Pixel px;
					px.red = (double)(transponData[h][w].red - transponData[h][w - 1].red)*x +
						(double)(transponData[h][w - 1].red - (transponData[h][w].red - transponData[h][w - 1].red)*((double)w - 1.f));
					px.green = (double)(transponData[h][w].green - transponData[h][w - 1].green)*x +
						(double)(transponData[h][w - 1].green - (transponData[h][w].green - transponData[h][w - 1].green)*((double)w - 1.f));
					px.blue = (double)(transponData[h][w].blue - transponData[h][w - 1].blue)*x +
						(double)(transponData[h][w - 1].blue - (transponData[h][w].blue - transponData[h][w - 1].blue)*((double)w - 1.f));
					bufLine.push_back(px);
				}
			}
		}
		Pixel px;
		int dif = width - bufLine.size();
		for (int l = 0; l < dif; l++)
		{
			bufLine.push_back(px);
		}
		tempData.push_back(bufLine);
	}

	transponData.clear();
	for (int i = 0; i < width; i++)
	{
		std::vector<Pixel> bufCol;
		for (int j = 0; j < height; j++)
		{
			bufCol.push_back(tempData[j][i]);
		}
		transponData.push_back(bufCol);
	}

	dataOut = transponData;
}

void CImageRotaterDlg::NormalizeAmplitude(std::vector<std::vector<Pixel>> &pData)
{
	double maxR = 0, maxG = 0, maxB = 0;

	double height = pData.size(),
		width = pData[0].size();

	//#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if(pData[i][j].red > maxR) maxR = pData[i][j].red;
			if (pData[i][j].green > maxG) maxG = pData[i][j].green;
			if (pData[i][j].blue > maxB) maxB = pData[i][j].blue;
		}
	}
	//#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{			
			pData[i][j].red = pData[i][j].red / maxR * 255.f;
			pData[i][j].green = pData[i][j].green / maxG * 255.f;
			pData[i][j].blue = pData[i][j].blue / maxB * 255.f;
		}
	}
}

void CImageRotaterDlg::OnBnClickedDialog2()
{
	ImageDlg imageDlg;
	imageDlg._image = &_imageRealTime;
	imageDlg.DoModal();
}


void CImageRotaterDlg::CalculateDifferenceInterpolation(double cf, int width, int height, 
	const std::vector<std::vector<Pixel>> & dataOriginal, std::vector<std::vector<Pixel>> & dataOutput)
{
	double diff = 0;
	for (int k = 0; k < _numCalc2; k++)
	{
		int widthOriginal = dataOriginal[0].size(),
			heightOriginal = dataOriginal.size();
		InterpolateImage(width, height, dataOriginal, dataOutput);
		std::vector<std::vector<Pixel>> buffer = dataOutput;
		InterpolateImage(widthOriginal, heightOriginal, buffer, dataOutput);
		diff += GetDifference(dataOriginal, dataOutput);
	}
	PointF pt(cf, (float)(diff / _numCalc2));
	_graphPoints2.push_back(pt);

}

void CImageRotaterDlg::OnBnClickedRadio1()
{
	_isCorrectRotating = true;
	OnBnClickedButton1();
}


void CImageRotaterDlg::OnBnClickedRadio2()
{
	_isCorrectRotating = false;
	OnBnClickedButton1();

}
