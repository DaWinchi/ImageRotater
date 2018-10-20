#pragma once
#include "afxwin.h"
#include <gdiplus.h>
#include <vector>
#include <iostream>
#include "Pixel.h"
using namespace Gdiplus;
using namespace std;



class FullSizeImage : public CStatic
{

public:
	double xmin, xmax, ymin, ymax;
	std::vector<std::vector<Pixel>>* _image;
	FullSizeImage();
	~FullSizeImage();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	REAL X(LPDRAWITEMSTRUCT RECT, float x);
	REAL Y(LPDRAWITEMSTRUCT RECT, float y);
	REAL Width(LPDRAWITEMSTRUCT RECT, float width);
	REAL Height(LPDRAWITEMSTRUCT RECT, float height);

};

