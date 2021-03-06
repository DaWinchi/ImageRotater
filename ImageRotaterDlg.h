
// ImageRotaterDlg.h: файл заголовка
//

#pragma once
#include "Draw2D.h"
#include "DrawGraph.h"
#include "DrawGraph2.h"
#include "ImageDlg.h"

// Диалоговое окно CImageRotaterDlg
class CImageRotaterDlg : public CDialogEx
{
// Создание
public:
	CImageRotaterDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEROTATER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV

	

// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	std::vector<std::vector<Pixel>> _imageOriginal;
	std::vector<std::vector<Pixel>> _imageOriginalUpper;
	std::vector<std::vector<Pixel>> _imageAfterScalling;
	std::vector<std::vector<Pixel>> _imageRealTime;
	std::vector<std::vector<Pixel>> _imageExperimental;
	std::vector<PointF> _graphPoints;
	std::vector<PointF> _graphPoints2;

	Draw2D _drawer2D;
	afx_msg void OnBnClickedLoad();
	void UpSizeImage();
	void LoadPicture();
	void RotateImage(double angle, const std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut);
	void RotateImageNotCorrect(double angle, const std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut);
	void InterpolateImage(int width, int height, const std::vector<std::vector<Pixel>> & dataIn, std::vector<std::vector<Pixel>> & dataOut);
	void NormalizeAmplitude(std::vector<std::vector<Pixel>> &pData);
	double GetDifference(const std::vector<std::vector<Pixel>> & dataOriginal, const std::vector<std::vector<Pixel>> & dataOutput);

	double _angle;
	afx_msg void OnBnClickedButton1();
	DrawGraph _drawerGraph;
	double _angleStep;
	double _angleExp;

	HANDLE hThreadCalculate;
	DWORD pdwThreadCalculate;
	afx_msg void OnBnClickedRunexperiment();
	void CalculateDifferenceRotate(double angle, const std::vector<std::vector<Pixel>> & dataOriginal, std::vector<std::vector<Pixel>> & dataOutput);
	void CalculateDifferenceInterpolation(double cf, int width, int height, const std::vector<std::vector<Pixel>> & dataOriginal, std::vector<std::vector<Pixel>> & dataOutput);
	afx_msg void OnBnClickedStop();
	bool _isCalculate;
	int _numCalc;
	double _scale;
	CStatic _resolutionText;
	afx_msg void OnBnClickedDialog2();
	DrawGraph2 _drawerGraph2;
	int _numCalc2;
	double _scaleStep;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	bool _isCorrectRotating;
	CButton _radioCorrect;
	CButton _radioNotCorrect;
};

DWORD WINAPI ThreadCalculate(PVOID param);
