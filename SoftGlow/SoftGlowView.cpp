
// SoftGlowView.cpp : CSoftGlowView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "SoftGlowApp.h"
#endif

#include "SoftGlowDoc.h"
#include "SoftGlowView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


inline CString I2A(int x)
{
	CString text;
	text.Format(TEXT("%d"), x);
	return text;
}

inline CString F2A(float x)
{
	CString text;
	text.Format(TEXT("%.1f"), x);
	return text;
}

inline BOOL DrawBitmap(CDC* pDC, int x, int y, int w, int h, CBitmap* pBitmap, int xSrc, int ySrc)
{
	ASSERT(pDC != NULL && pBitmap != NULL);

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(pBitmap);

	return pDC->BitBlt(x, y, w, h, &dc, xSrc, ySrc, SRCCOPY);
}

// CSoftGlowView

IMPLEMENT_DYNCREATE(CSoftGlowView, CView)

BEGIN_MESSAGE_MAP(CSoftGlowView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CSoftGlowView::OnFileOpen)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CSoftGlowView 构造/析构

CSoftGlowView::CSoftGlowView()
{
	// TODO:  在此处添加构造代码
	m_pSrcBitmap = new CBitmap;
	m_pDstBitmap = new CBitmap;
	m_pFilter = new SoftGlow;
	m_bLoadBitmap = FALSE;
	m_bNeedUpdateBitmap = FALSE;
}

CSoftGlowView::~CSoftGlowView()
{
	SAFE_DELETE(m_pSrcBitmap);
	SAFE_DELETE(m_pDstBitmap);
	SAFE_DELETE(m_pFilter);
}

BOOL CSoftGlowView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CSoftGlowView 绘制

void CSoftGlowView::OnDraw(CDC* /*pDC*/)
{
	CSoftGlowDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	// TODO:  在此处为本机数据添加绘制代码
	UpdateView();
}


// CSoftGlowView 打印

BOOL CSoftGlowView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CSoftGlowView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CSoftGlowView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CSoftGlowView 诊断

#ifdef _DEBUG
void CSoftGlowView::AssertValid() const
{
	CView::AssertValid();
}

void CSoftGlowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSoftGlowDoc* CSoftGlowView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoftGlowDoc)));
	return (CSoftGlowDoc*)m_pDocument;
}
#endif //_DEBUG


BOOL CSoftGlowView::AttachBitmap(CBitmap* pBitmap, const Bitmap* bitmap)
{
	BITMAPINFOHEADER bi;
	bi.biSize = sizeof(bi);
	bi.biWidth = bitmap->biWidth;
	bi.biHeight = bitmap->biHeight;
	bi.biBitCount = bitmap->biBitCount;
	bi.biPlanes = bitmap->biPlanes;
	bi.biSizeImage = bitmap->biSizeImage;
	bi.biCompression = bitmap->biCompression;
	bi.biClrImportant = bitmap->biClrImportant;
	bi.biClrUsed = bitmap->biClrUsed;
	bi.biHeight = -bi.biHeight;

	HBITMAP hBitmap = CreateDIBitmap(GetDC()->GetSafeHdc()
		, &bi
		, CBM_INIT
		, (void*)bitmap->bits
		, (BITMAPINFO*)&bi
		, DIB_RGB_COLORS);

	pBitmap->DeleteObject();
	pBitmap->Attach(hBitmap);

	return TRUE;
}

void CSoftGlowView::UpdateView(void)
{
	if (!m_bLoadBitmap) return;

	static int radius = 0;
	static float sharpness = 0.0f;
	static float brightness = 0.0f;

	int _radius = m_ctrlRadius.GetPos();
	float _sharpness = m_ctrlSharpness.GetPos() * 1.0f / 10.0f;
	float _brightness = m_ctrlBrightness.GetPos() * 1.0f / 10.0f;

	if (radius != _radius){ radius = _radius; m_bNeedUpdateBitmap = TRUE; }
	if (sharpness != _sharpness) { sharpness = _sharpness; m_bNeedUpdateBitmap = TRUE; }
	if (brightness != _brightness){ brightness = _brightness; m_bNeedUpdateBitmap = TRUE; }


	Bitmap* pSrcBitmap = GetDocument()->GetBitmap();

	if (m_bNeedUpdateBitmap)
	{
		Bitmap* pBitmap = m_pFilter->Process2(pSrcBitmap, radius, sharpness, brightness);
		assert(pBitmap);

		AttachBitmap(m_pDstBitmap, pBitmap);

		delete pBitmap;

		m_bNeedUpdateBitmap = FALSE;
	}

	//render bitmap
	CRect rect;
	GetClientRect(&rect);

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	CBitmap bmMem;
	bmMem.CreateCompatibleBitmap(GetDC(), rect.Width(), rect.Height());
	dcMem.SelectObject(&bmMem);

	CBrush brush(RGB(255, 255, 255));
	dcMem.FillRect(&rect, &brush);

	int x = 0;
	int y = 0;
	int w = rect.Width();
	int h = rect.Height();

	DrawBitmap(&dcMem, x, y, w, h, m_pSrcBitmap, 0, 0);
	x += pSrcBitmap->Width();
	DrawBitmap(&dcMem, x, y, w, h, m_pDstBitmap, 0, 0);

	//dcMem.TextOut(10, rect.bottom - 30, m_strParamText);

	CClientDC dc(this);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);

 	m_ctrlRadius.Invalidate(0);
	m_ctrlSharpness.Invalidate(0);
	m_ctrlBrightness.Invalidate(0);
	m_editRadius.Invalidate(0);
	m_editSharpness.Invalidate(0);
	m_editBridhtness.Invalidate(0);
}

// CSoftGlowView 消息处理程序

void CSoftGlowView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dlg(TRUE, TEXT("*.BMP"), TEXT("*.BMP"));

	if (dlg.DoModal() == IDOK)
	{
		CSoftGlowDoc* pDoc = GetDocument();
		if (!pDoc->OpenFile(dlg.GetPathName()))
		{
			return;
		}

		AttachBitmap(m_pSrcBitmap, pDoc->GetBitmap());

		m_bLoadBitmap = TRUE;
		m_bNeedUpdateBitmap = TRUE;

		UpdateView();
	}
}


int CSoftGlowView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rect;
	GetClientRect(&rect);

	int ID = 6880;

	DWORD dwStyle = TBS_HORZ | WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (!m_ctrlRadius.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	if (!m_ctrlSharpness.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	if (!m_ctrlBrightness.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	dwStyle = ES_RIGHT | ES_NUMBER | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (!m_editRadius.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	if (!m_editSharpness.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	if (!m_editBridhtness.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	m_ctrlRadius.SetRange(1, 30);
	m_ctrlSharpness.SetRange(1, 10);
	m_ctrlBrightness.SetRange(1, 10);
	m_ctrlRadius.SetPos(10);
	m_ctrlSharpness.SetPos(5);
	m_ctrlBrightness.SetPos(6);


	m_editRadius.SetWindowText(TEXT("10"));
	m_editSharpness.SetWindowText(TEXT("0.5"));
	m_editBridhtness.SetWindowText(TEXT("0.6"));

	return 0;
}


void CSoftGlowView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	if (pSlider == &m_ctrlRadius)
	{
		int nPos = m_ctrlRadius.GetPos();
		m_editRadius.SetWindowText(I2A(nPos));
	}
	else if (pSlider == &m_ctrlSharpness)
	{
		int nPos = m_ctrlSharpness.GetPos();
		m_editSharpness.SetWindowText(F2A((float)nPos / 10));
	}
	else if (pSlider == &m_ctrlBrightness)
	{
		int nPos = m_ctrlBrightness.GetPos();
		m_editBridhtness.SetWindowText(F2A((float)nPos / 10));
	}

	UpdateView();
	//SendMessage(WM_PAINT, 0, 0);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CSoftGlowView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);

	int w = 150;
	int h = 20;
	int x = rect.right - w;
	//slider ctrl
	int y = 10;
	m_ctrlRadius.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlSharpness.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlBrightness.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	//edit ctrl
	w = 50;
	x = rect.right - 150 - w - 10;
	y = 10;
	m_editRadius.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editSharpness.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editBridhtness.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);
}

