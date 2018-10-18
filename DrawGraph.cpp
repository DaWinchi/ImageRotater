#include "stdafx.h"
#include "DrawGraph.h"
#include <math.h>


extern CRITICAL_SECTION cs;

using namespace Gdiplus;
ULONG_PTR token2;
DrawGraph::DrawGraph()
{
	Status s;
	GdiplusStartupInput input;
	s = GdiplusStartup(&token2, &input, NULL);
	if (s != Ok)
	{
		MessageBox(L"ERROR!!!", L"������", MB_ICONERROR);
	}
}


DrawGraph::~DrawGraph()
{
	GdiplusShutdown(token2);
}


void DrawGraph::DrawItem(LPDRAWITEMSTRUCT RECT)
{
	EnterCriticalSection(&cs);
	double maxY = 1;
	double maxX = 1;
	if (_points != nullptr&&_points[0].size() > 0)
	{
		maxY = 0;
		int size = _points->size();
		for (int i = 0; i < size; ++i)
		{
			if (fabs(_points[0][i].Y) > maxY) maxY = fabs(_points[0][i].Y);
			if (_points[0][i].X > maxX) maxX = _points[0][i].X;
		}
	}
	ymax = maxY + maxY / 3.f;
	ymin = -ymax;
	step_y = ymax / 3.f;

	xmax = maxX;
	xmin = -xmax / 100.f;
	step_x = xmax / 5.f;

	LeaveCriticalSection(&cs);

	Graphics gr(RECT->hDC);
	Bitmap bmp(RECT->rcItem.right, RECT->rcItem.bottom, &gr);
	Graphics grBmp(&bmp);

	FontFamily podpis_osi(L"Arial");
	Gdiplus::Font podpis(&podpis_osi, 12, FontStyleRegular, UnitPixel);

	SolidBrush brush(Color::White);
	Pen graph_pen(Color::Yellow, 4);
	Pen bottom_pen(Color::Aqua, 4);
	Pen osi_pen(Color::White, 2);
	Pen setka_pen(Color::White, 1);
	setka_pen.SetDashStyle(DashStyle::DashStyleDash);

	grBmp.SetSmoothingMode(SmoothingModeAntiAlias);
	grBmp.Clear(Color::Black);

	EnterCriticalSection(&cs);
	//������� ���
	grBmp.DrawLine(&osi_pen, X(RECT, xmin), Y(RECT, 0), X(RECT, xmax), Y(RECT, 0));
	grBmp.DrawLine(&osi_pen, X(RECT, 0), Y(RECT, ymin), X(RECT, 0), Y(RECT, ymax));

	//�����
	//������������ �����
	for (double x = step_x; x <= xmax; x += step_x)
	{
		grBmp.DrawLine(&setka_pen, X(RECT, x), Y(RECT, ymin), X(RECT, x), Y(RECT, ymax));
	}
	for (double x = -step_x; x >= xmin; x -= step_x)
	{
		grBmp.DrawLine(&setka_pen, X(RECT, x), Y(RECT, ymin), X(RECT, x), Y(RECT, ymax));
	}

	//�������������� �����
	for (double y = step_y; y <= ymax; y += step_y)
	{
		grBmp.DrawLine(&setka_pen, X(RECT, xmin), Y(RECT, y), X(RECT, xmax), Y(RECT, y));
	}
	for (double y = -step_y; y >= ymin; y -= step_y)
	{
		grBmp.DrawLine(&setka_pen, X(RECT, xmin), Y(RECT, y), X(RECT, xmax), Y(RECT, y));
	}

	//������� ����� �� �����������
	for (double x = step_x; x <= xmax; x += step_x)
	{
		CString str;
		str.Format(_T("%.1f"), x);
		grBmp.DrawString(str, -1, &podpis, PointF(X(RECT, x), Y(RECT, 0) + 2.f), NULL, &brush);
	}
	for (double x = -step_x; x >= xmin; x -= step_x)
	{
		CString str;
		str.Format(_T("%.1f"), x);
		grBmp.DrawString(str, -1, &podpis, PointF(X(RECT, x), Y(RECT, 0) + 2.f), NULL, &brush);
	}

	//������� ����� �� ���������
	for (double y = step_y; y <= ymax; y += step_y)
	{
		CString str;
		str.Format(_T("%.e"), y);
		grBmp.DrawString(str, -1, &podpis, PointF(X(RECT, 0), Y(RECT, y) + 2.f), NULL, &brush);
	}
	for (double y = 0; y >= ymin; y -= step_y)
	{
		CString str;
		str.Format(_T("%.e"), y);
		grBmp.DrawString(str, -1, &podpis, PointF(X(RECT, 0), Y(RECT, y) + 2.f), NULL, &brush);
	}
	LeaveCriticalSection(&cs);

	EnterCriticalSection(&cs);
	if (_points != nullptr)
	{
		int size = _points->size();
		if (size > 1)
			for (int i = 0; i < size - 1; ++i)
			{
				grBmp.DrawLine(&graph_pen, X(RECT, _points[0][i].X), Y(RECT, _points[0][i].Y),
					X(RECT, _points[0][i + 1].X), Y(RECT, _points[0][i + 1].Y));
			}
	}
	LeaveCriticalSection(&cs);
	gr.DrawImage(&bmp, 0, 0);
}


REAL DrawGraph::X(LPDRAWITEMSTRUCT RECT, float x)
{
	return (REAL)(RECT->rcItem.right) / (xmax - xmin)*((x)-xmin);
}

REAL DrawGraph::Y(LPDRAWITEMSTRUCT RECT, float y)
{
	return -(REAL)(RECT->rcItem.bottom) / (ymax - ymin)*((y)-ymax);
}

REAL DrawGraph::Width(LPDRAWITEMSTRUCT RECT, float width)
{
	return (REAL)(RECT->rcItem.right) / (xmax - xmin)*width;
}

REAL DrawGraph::Height(LPDRAWITEMSTRUCT RECT, float height)
{
	return (REAL)(RECT->rcItem.bottom) / (ymax - ymin)*height;
}
