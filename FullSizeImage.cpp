#include "stdafx.h"
#include "FullSizeImage.h"
#include <math.h>

using namespace Gdiplus;
ULONG_PTR token3;
FullSizeImage::FullSizeImage()
{
	Status s;
	GdiplusStartupInput input;
	s = GdiplusStartup(&token3, &input, NULL);
	if (s != Ok)
	{
		MessageBox(L"ERROR!!!", L"Îøèáêà", MB_ICONERROR);
	}
}


FullSizeImage::~FullSizeImage()
{
	GdiplusShutdown(token3);
}


void FullSizeImage::DrawItem(LPDRAWITEMSTRUCT RECT)
{
	Graphics gr(RECT->hDC);
	if (_image != nullptr && !_image->empty())
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
		Rect rect(0, 0, width, height);
		gr.DrawImage(&bmpBuffer, rect);
	}
}


REAL FullSizeImage::X(LPDRAWITEMSTRUCT RECT, float x)
{
	return (REAL)(RECT->rcItem.right) / (xmax - xmin)*((x)-xmin);
}

REAL FullSizeImage::Y(LPDRAWITEMSTRUCT RECT, float y)
{
	return -(REAL)(RECT->rcItem.bottom) / (ymax - ymin)*((y)-ymax);
}

REAL FullSizeImage::Width(LPDRAWITEMSTRUCT RECT, float width)
{
	return (REAL)(RECT->rcItem.right) / (xmax - xmin)*width;
}

REAL FullSizeImage::Height(LPDRAWITEMSTRUCT RECT, float height)
{
	return (REAL)(RECT->rcItem.bottom) / (ymax - ymin)*height;
}
