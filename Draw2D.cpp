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
		MessageBox(L"ERROR!!!", L"������", MB_ICONERROR);
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
	if (!_image->empty())
	{
		size_t width = _image[0][0].size();
		size_t height = _image->size();
		xmin = 0;
		xmax = width;
		ymin = 0;
		ymax = height;
		Bitmap bmpBuffer(width, height);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Color color;
				color = Color::MakeARGB(255, _image[0][i][j].red, _image[0][i][j].green, _image[0][i][j].blue);
				bmpBuffer.SetPixel(j, height - 1 - i, color);
			}
		}
		Rect rect(0, 0, RECT->rcItem.right, RECT->rcItem.bottom);
		gr.DrawImage(&bmpBuffer, rect);
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
