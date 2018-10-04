#include "stdafx.h"
#include "Draw2D.h"
#include <math.h>

using namespace Gdiplus;
ULONG_PTR token1;
Draw2D::Draw2D()
{
	Status s;
	GdiplusStartupInput input;
	s = GdiplusStartup(&token1, &input, NULL);
	if (s != Ok)
	{
		MessageBox(L"ERROR!!!", L"Îøèáêà", MB_ICONERROR);
	}
	_isSpectr = _ellipseDrawing = false;
}


Draw2D::~Draw2D()
{
	GdiplusShutdown(token1);
}


void Draw2D::DrawItem(LPDRAWITEMSTRUCT RECT)
{
	Graphics gr(RECT->hDC);
	if (_image != nullptr && !_ellipseDrawing)
	{
		size_t width = _image[0][0].size();
		size_t height = _image->size();
		xmin = 0;
		xmax = width;
		ymin = 0;
		ymax = height;
		Bitmap bmpBuffer(width, height);

#pragma omp parallel for
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				double val = _image[0][i][j];
				Color color;
				color = Color::MakeARGB(255, val, val, val);
				bmpBuffer.SetPixel(j, height -1 - i, color);
			}
		}
		Rect rect(0, 0, RECT->rcItem.right, RECT->rcItem.bottom);
		_bmpSpect = bmpBuffer.Clone(0, 0, bmpBuffer.GetWidth(), bmpBuffer.GetHeight(), PixelFormatDontCare);
		gr.DrawImage(&bmpBuffer, rect);
	}

	if (_R != nullptr && _ellipseDrawing)
	{
		Rect rect(0, 0, RECT->rcItem.right, RECT->rcItem.bottom);
		Bitmap *bmpR= _bmpSpect->Clone(0, 0, _bmpSpect->GetWidth(), _bmpSpect->GetHeight(), PixelFormatDontCare);
		Graphics grBmpR(bmpR);
		xmax = bmpR->GetWidth();
		ymax = bmpR->GetHeight();
		Pen pen(Color::Red, 1);
		grBmpR.DrawEllipse(&pen, xmax / 2.f- *_R, ymax / 2.f - *_R-1, *_R*2.f, *_R*2.f);
		gr.DrawImage(bmpR, rect);
	}
	
}


REAL Draw2D::X(LPDRAWITEMSTRUCT RECT, float x)
{
	return (REAL)(RECT->rcItem.right) / (xmax - xmin)*((x)-xmin);
}

REAL Draw2D::Y(LPDRAWITEMSTRUCT RECT, float y)
{
	return -(REAL)(RECT->rcItem.bottom) / (ymax - ymin)*((y)-ymax);
}

REAL Draw2D::Width(LPDRAWITEMSTRUCT RECT, float width)
{
	return (REAL)(RECT->rcItem.right) / (xmax - xmin)*width;
}

REAL Draw2D::Height(LPDRAWITEMSTRUCT RECT, float height)
{
	return (REAL)(RECT->rcItem.bottom) / (ymax - ymin)*height;
}
