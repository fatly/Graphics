// CurveWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "CurveApp.h"
#include "CurveCtrl.h"
#include "Curve.h"

#define IDC_COMBO_CHANNELS 6990
#define IDC_BUTTON_RESET   6991

#define MIN_DISTANCE 8

// CCurveCtrl

IMPLEMENT_DYNAMIC(CCurveCtrl, CWnd)

CCurveCtrl::CCurveCtrl()
{
	m_nPointCount = 9;
	m_nSampleCount = 256;
	m_bMouseDown = FALSE;
	m_nSelectIndex = -1;
	m_pCurveConfig = new CurvesConfig(m_nPointCount, m_nSampleCount);
	assert(m_pCurveConfig);

	//test data
	double red_pts[] = {
		0.000000, 0.007843,
		0.121569, 0.192157,
		0.247059, 0.372549,
		0.372549, 0.529412,
		0.498039, 0.666667,
		0.623529, 0.784314,
		0.749020, 0.874510,
		0.874510, 0.945098,
		1.000000, 0.996078,
	};
	double green_pts[] = {
		0.000000, 0.007843,
		0.121569, 0.176471,
		0.247059, 0.345098,
		0.372549, 0.498039,
		0.498039, 0.635294,
		0.623529, 0.749020,
		0.749020, 0.850980,
		0.874510, 0.933333,
		1.000000, 0.996078,
	};
	double blue_pts[] = {
		0.000000, 0.007843,
		0.121569, 0.164706,
		0.247059, 0.329412,
		0.372549, 0.482353,
		0.498039, 0.611765,
		0.623529, 0.733333,
		0.749020, 0.835294,
		0.874510, 0.921569,
		1.000000, 0.996078,
	};

	m_pCurveConfig->CreateSpline(CURVE_CHANNEL_R, 9, red_pts);
	m_pCurveConfig->CreateSpline(CURVE_CHANNEL_B, 9, blue_pts);
	m_pCurveConfig->CreateSpline(CURVE_CHANNEL_G, 9, green_pts);
	m_pCurveConfig->SetSelectCurve(CURVE_CHANNEL_R);
}

CCurveCtrl::~CCurveCtrl()
{
	SAFE_DELETE(m_pCurveConfig);
}


BEGIN_MESSAGE_MAP(CCurveCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNELS, &OnSelChangeChannels)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &OnClickReset)
END_MESSAGE_MAP()



// CCurveCtrl 消息处理程序
int CCurveCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST;
	if (!m_ctrlChannels.Create(dwStyle, CRect(0, 0, 0, 0), this, IDC_COMBO_CHANNELS))
	{
		return -1;
	}

	dwStyle = WS_CHILD | WS_VISIBLE | BS_CENTER;
	if (!m_ctrlReset.Create(TEXT("Reset"), dwStyle, CRect(0, 0, 0, 0), this, IDC_BUTTON_RESET))
	{
		return -1;
	}

	m_ctrlChannels.AddString(TEXT("Color"));
	m_ctrlChannels.AddString(TEXT("Red"));
	m_ctrlChannels.AddString(TEXT("Green"));
	m_ctrlChannels.AddString(TEXT("Blue"));
	m_ctrlChannels.AddString(TEXT("Alpha"));
	int index = m_pCurveConfig->GetSelectChannel();
	m_ctrlChannels.SetCurSel(index);

	return 0;
}


void CCurveCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	int w = 70;
	int h = 24;
	int x = cx - w;
	int y = cy - h;

	m_ctrlChannels.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	x -= w;
	h += 2;
	y -= 1;
	m_ctrlReset.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);
}

void CCurveCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);

	m_bMouseDown = TRUE;

	int x, y;
	int w = rect.Width() / (m_nPointCount - 1);
	int distance = MAXINT;
	int nSelectIndex = -1;
	Curve* curve = m_pCurveConfig->GetSelectCurve();
	for (int i = 0; i < m_nPointCount; i++)
	{
		curve->GetPoint(i, x, y);
		if (abs(x - point.x) < distance)
		{
			distance = abs(x - point.x);
			nSelectIndex = i;
		}
	}

	if (distance > MIN_DISTANCE)
	{
		nSelectIndex = (point.x + w/2) / w;
	}

	m_nLeftMost = 0;
	for (int i = nSelectIndex - 1; i >= 0; i--)
	{
		curve->GetPoint(i, x, y);
		if (x >= 0)
		{
			m_nLeftMost = x;
			break;
		}
	}

	m_nRightMost = 256;
	for (int i = nSelectIndex + 1; i < m_nPointCount; i++)
	{
		curve->GetPoint(i, x, y);
		if (x >= 0)
		{
			m_nRightMost = x;
			break;
		}
	}

	m_nSelectIndex = nSelectIndex;
	curve->SetPoint(m_nSelectIndex, (double)point.x / 255.0, (double)(255 - point.y) / 255.0);
	curve->Calculate();

	UpdateView();

	SetCapture();

	NotifyOwner();

	CWnd::OnLButtonDown(nFlags, point);
}


void CCurveCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_bMouseDown)
	{
		CRect rect;
		GetClientRect(&rect);
		point.x = clamp(point.x, 0, rect.right);
		point.y = clamp(point.y, 0, rect.bottom);

		int x, y;
		int w = rect.Width() / (m_nPointCount - 1);
		int distance = MAXINT;
		int nSelectIndex = -1;
		Curve* curve = m_pCurveConfig->GetSelectCurve();
		for (int i = 0; i < m_nPointCount; i++)
		{
			curve->GetPoint(i, x, y);
			if (abs(x - point.x) < distance)
			{
				distance = abs(x - point.x);
				nSelectIndex = i;
			}
		}

		if (distance > MIN_DISTANCE)
		{
			nSelectIndex = (point.x + w/2) / w;
		}

		curve->SetPoint(m_nSelectIndex, -1.0, -1.0);

		if (point.x>m_nLeftMost && point.x < m_nRightMost)
		{
			nSelectIndex = (point.x + w/2) / w;
			curve->GetPoint(nSelectIndex, x, y);
			if (x < 0)
			{
				m_nSelectIndex = nSelectIndex;
			}

			curve->SetPoint(m_nSelectIndex, (double)point.x / 255.0, (double)(255 - point.y) / 255.0);
			curve->Calculate();
		}

		UpdateView();

		NotifyOwner();
	}

	CWnd::OnMouseMove(nFlags, point);
}


void CCurveCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_bMouseDown = FALSE;
	m_nSelectIndex = -1;

	ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CCurveCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	static int channel = m_pCurveConfig->GetSelectChannel();

	channel += 1;

	if (channel > CURVE_CHANNEL_A)
	{
		channel = CURVE_CHANNEL_C;
	}

	m_pCurveConfig->SetSelectCurve(channel);

	UpdateView();

	TRACE("OnRButtonDown() channel = %d\n", channel);

	CWnd::OnRButtonDown(nFlags, point);
}

void CCurveCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

//	CWnd::OnRButtonUp(nFlags, point);
}

void CCurveCtrl::OnSelChangeChannels(void)
{
	int index = m_ctrlChannels.GetCurSel();
	m_pCurveConfig->SetSelectCurve(index);
	//m_pCurveConfig->GetSelectCurve()->Calculate();

	UpdateView();
}

void CCurveCtrl::OnClickReset(void)
{
	Curve* curve = m_pCurveConfig->GetSelectCurve();
	curve->Reset();
//	curve->Calculate();

	UpdateView();
}

void CCurveCtrl::UpdateView(void)
{
	CRect rect;
	GetClientRect(&rect);

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	CBitmap bmMem;
	bmMem.CreateCompatibleBitmap(GetDC(), rect.Width(), rect.Height());
	dcMem.SelectObject(&bmMem);

	CBrush brush(RGB(255, 255, 255));
	dcMem.FillRect(&rect, &brush);

	int width = 1;
	COLORREF colors[5] = {
		RGB(45, 45, 45),
		RGB(255, 0, 0),
		RGB(0, 255, 0),
		RGB(0, 0, 255),
		RGB(240, 170, 30)
	};

	CPen pens[5] = {
		CPen(PS_SOLID, width, colors[0]),
		CPen(PS_SOLID, width, colors[1]),
		CPen(PS_SOLID, width, colors[2]),
		CPen(PS_SOLID, width, colors[3]),
		CPen(PS_SOLID, width, colors[4])
	};

	int selectIndex = m_pCurveConfig->GetSelectChannel();
	//draw B line of no select channels
	{
		for (int i = CURVE_CHANNEL_A; i >= CURVE_CHANNEL_C; i--)
		{
			dcMem.SelectObject(&pens[i]);

			Curve* curve = m_pCurveConfig->GetCurve(i);
			if (curve->IsIdentity() || i==selectIndex) continue;

			int y = 255 - curve->GetSample(0);
			if (y >= 255) y -= 1;
			dcMem.MoveTo(0, y);

			for (int x = 1; x < curve->GetSampleCount(); x++)
			{
				y = 255 - curve->GetSample(x);				
				if (y >= 255) y -= 1;
				dcMem.LineTo(x, y);
			}
		}
	}

	//draw the B line of select channel
	{
		dcMem.SelectObject(&pens[selectIndex]);
		Curve* curve = m_pCurveConfig->GetCurve(selectIndex);

		int y = 255 - curve->GetSample(0);
		if (y >= 255) y -= 1;

		dcMem.MoveTo(0, y);

		for (int x = 1; x < curve->GetSampleCount(); x++)
		{
			y = 255 - curve->GetSample(x);
			if (y >= 255) y -= 1;
			dcMem.LineTo(x, y);
		}
	}

	//draw points of select channel
	CPen pen(PS_SOLID, 4, RGB(0, 0, 0));
	dcMem.SelectObject(&pen);
	{
		Curve* curve = m_pCurveConfig->GetCurve(selectIndex);
		int x = 0, y = 0;
		for (int j = 0; j < curve->GetPointCount(); j++)
		{
			curve->GetPoint(j, x, y);
			y = 255 - y;

			dcMem.MoveTo(x, y);
			dcMem.LineTo(x, y);
		}
	}

	CClientDC dc(this);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);

	m_ctrlChannels.Invalidate(FALSE);
	m_ctrlReset.Invalidate(FALSE);
}

void CCurveCtrl::OnDraw(CDC* pDC)
{
	if (GetSafeHwnd() == NULL) return;

	UpdateView();
}

void CCurveCtrl::NotifyOwner(void)
{
	CWnd* pOwner = GetOwner();
	assert(pOwner);
	pOwner->PostMessage(UM_CURVE_CHANGE, (WPARAM)m_pCurveConfig->GetSelectChannel(), (LPARAM)m_nSelectIndex);
}

int CCurveCtrl::GetSelectChannel(void) const
{
	return m_pCurveConfig->GetSelectChannel();
}

byte CCurveCtrl::GetSample(const int channel, const int index)
{
	assert(channel >= 0 && channel <= 4);
	return m_pCurveConfig->GetCurve(channel)->GetSample(index);
}

void CCurveCtrl::GetSample(const int channel, uint8* samples, int* size)
{
	assert(channel >= 0 && channel <= 4);
	Curve* curve = m_pCurveConfig->GetCurve(channel);
	int _size = curve->GetSampleCount();
	curve->CopySample(samples, _size);

	if (size != nullptr) *size = _size;
}

void CCurveCtrl::GetSmaple(const int channel, uint8* samples, const int size)
{
	assert(channel >= 0 && channel <= 4);
	m_pCurveConfig->GetCurve(channel)->CopySample(samples, size);
}

void CCurveCtrl::ExportPoint(const int channel, double* points, const int count)
{
	assert(channel >= 0 && channel <= 4);
	assert(count >= 2);
	Curve* curve = m_pCurveConfig->GetCurve(channel);
	int sampleCount = curve->GetSampleCount();
	int dx = sampleCount / (count - 1);

	points[0] = 0.0;
	points[1] = curve->GetSample(0.0);

	for (int i = 1; i < count-1; i++)
	{
		int x = i * dx;
		points[i * 2 + 0] = (double)x / sampleCount;
		points[i * 2 + 1] = curve->GetSample(points[i * 2]);
	}

	int i = count - 1;
	points[i * 2 + 0] = 1.0;
	points[i * 2 + 1] = curve->GetSample(1.0);
}
