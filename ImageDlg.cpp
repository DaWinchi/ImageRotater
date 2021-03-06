// ImageDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "ImageRotater.h"
#include "ImageDlg.h"
#include "afxdialogex.h"


// Диалоговое окно ImageDlg

IMPLEMENT_DYNAMIC(ImageDlg, CDialogEx)

ImageDlg::ImageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGE_DLG, pParent)
{
}

ImageDlg::~ImageDlg()
{
}

void ImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, _drawerImage);
}


BEGIN_MESSAGE_MAP(ImageDlg, CDialogEx)
END_MESSAGE_MAP()


// Обработчики сообщений ImageDlg


BOOL ImageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Добавить дополнительную инициализацию
	_drawerImage._image = _image;
	if (_image != nullptr && !_image->empty())
	{
		int height = _image->size(),
			width = _image[0][0].size();
		this->SetWindowPos(NULL, 0, 0, width + 17, height + 47, SWP_NOMOVE | SWP_NOZORDER);
		_drawerImage.SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
		CString str1, str2;
		str1.Format(_T("%d"), width);
		str2.Format(_T("%d"), height);
		this->SetWindowTextW(str1 + "x" + str2);
	}
	_drawerImage.RedrawWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // Исключение: страница свойств OCX должна возвращать значение FALSE
}


