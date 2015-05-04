
// GLFitlerView.cpp : CGLFitlerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "GLFitler.h"
#endif

#include "GLFitlerDoc.h"
#include "GLFitlerView.h"
#include "ImageFilter.h"

using namespace e;

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

// CGLFitlerView

IMPLEMENT_DYNCREATE(CGLFitlerView, CView)

BEGIN_MESSAGE_MAP(CGLFitlerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGLFitlerView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_FILE_OPEN, &CGLFitlerView::OnFileOpen)
END_MESSAGE_MAP()

// CGLFitlerView 构造/析构

CGLFitlerView::CGLFitlerView()
{
	// TODO:  在此处添加构造代码
	m_pSrcBitmap = new CBitmap;
	m_pDstBitmap = new CBitmap;
	m_bLoadBitmap = FALSE;
	m_bNeedUpdateBitmap = FALSE;
	m_pImagerFitler = NULL;
}

CGLFitlerView::~CGLFitlerView()
{
	if (m_pSrcBitmap) delete m_pSrcBitmap;
	if (m_pDstBitmap) delete m_pDstBitmap;
	if (m_pImagerFitler) delete m_pImagerFitler;
}

BOOL CGLFitlerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CGLFitlerView 绘制

void CGLFitlerView::OnDraw(CDC* /*pDC*/)
{
	CGLFitlerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	UpdateView();
}


// CGLFitlerView 打印


void CGLFitlerView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGLFitlerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGLFitlerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CGLFitlerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CGLFitlerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGLFitlerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGLFitlerView 诊断

#ifdef _DEBUG
void CGLFitlerView::AssertValid() const
{
	CView::AssertValid();
}

void CGLFitlerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGLFitlerDoc* CGLFitlerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLFitlerDoc)));
	return (CGLFitlerDoc*)m_pDocument;
}
#endif //_DEBUG


// CGLFitlerView 消息处理程序


int CGLFitlerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rect;
	GetClientRect(&rect);

	int ID = 6880;
	int y = 10;
	int w = 150;
	int h = 20;

	DWORD dwStyle = TBS_HORZ | WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (!m_ctrlRadius.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, ID++))
	{
		return -1;
	}

	y += h + 10;
	if (!m_ctrlSigma.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, ID++))
	{
		return -1;
	}

	dwStyle = ES_RIGHT | ES_NUMBER | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (!m_editRadius.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	if (!m_editSigma.Create(dwStyle, CRect(0, 0, 0, 0), this, ID++))
	{
		return -1;
	}

	m_ctrlRadius.SetRange(1, 10);
	m_ctrlSigma.SetRange(1, 200);
	m_ctrlRadius.SetPos(2);
	m_ctrlSigma.SetPos(5);

	m_editRadius.SetWindowText(TEXT("2"));
	m_editSigma.SetWindowText(TEXT("0.5"));

	return 0;
}


void CGLFitlerView::OnSize(UINT nType, int cx, int cy)
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
	m_ctrlSigma.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	//edit ctrl
	w = 50;
	x = rect.right - 150 - w - 10;
	y = 10;
	m_editRadius.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editSigma.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);
}


void CGLFitlerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	if (pSlider == &m_ctrlRadius)
	{
		int nPos = m_ctrlRadius.GetPos();
		m_editRadius.SetWindowText(I2A(nPos));
	}
	else if (pSlider == &m_ctrlSigma)
	{
		int nPos = m_ctrlSigma.GetPos();
		m_editSigma.SetWindowText(F2A((float)nPos / 10));
	}

	SendMessage(WM_PAINT, 0, 0);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CGLFitlerView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dlg(TRUE, TEXT("*.BMP"), TEXT("*.BMP"));

	if (dlg.DoModal() == IDOK)
	{
		CGLFitlerDoc* pDoc = GetDocument();
		if (!pDoc->OpenFile(dlg.GetPathName())) return;

		Bitmap* pBitmap = pDoc->GetBitmap(0);
		BITMAPINFOHEADER bi;
		bi.biSize			= sizeof(bi);
		bi.biWidth			= pBitmap->biWidth;
		bi.biHeight			= pBitmap->biHeight;
		bi.biBitCount		= pBitmap->biBitCount;
		bi.biPlanes			= pBitmap->biPlanes;
		bi.biSizeImage		= pBitmap->biSizeImage;
		bi.biCompression	= pBitmap->biCompression;
		bi.biClrImportant	= pBitmap->biClrImportant;
		bi.biClrUsed		= pBitmap->biClrUsed;
		bi.biHeight			= -bi.biHeight;

		HBITMAP hBitmap = CreateDIBitmap(GetDC()->GetSafeHdc()
			, &bi
			, CBM_INIT
			, (void*)pBitmap->bits
			, (BITMAPINFO*)&bi
			, DIB_RGB_COLORS);

		m_pSrcBitmap->DeleteObject();

		m_pSrcBitmap->Attach(hBitmap);

		if (m_pImagerFitler != NULL)
			delete m_pImagerFitler;

		m_pImagerFitler = new e::ImageFilter(GetDC()->GetSafeHdc());
		assert(m_pImagerFitler != NULL);

		m_bLoadBitmap = TRUE;
		m_bNeedUpdateBitmap = TRUE;

		UpdateView();
	}
}

void CGLFitlerView::UpdateView(void)
{
	if (!m_bLoadBitmap) return;

	static int radius = 0;
	static float sigma = 0.0f;

	int _radius = m_ctrlRadius.GetPos();
	float _sigma = m_ctrlSigma.GetPos() * 1.0f / 10.0f;

	if (radius != _radius){ radius = _radius; m_bNeedUpdateBitmap = TRUE; }
	if (sigma != _sigma) { sigma = _sigma; m_bNeedUpdateBitmap = TRUE; }

	m_strParamText.Format(TEXT("radius = %d, sigma = %f"), radius, sigma);

	Bitmap* pSrcBitmap = GetDocument()->GetBitmap(0);

	if (m_bNeedUpdateBitmap)
	{
		Bitmap* pDstBitmap = pSrcBitmap->Clone();
		assert(pDstBitmap != 0);
		//call opengl fitler
		m_pImagerFitler->SetParam(radius, sigma);
		m_pImagerFitler->SetFilterType(1);
		m_pImagerFitler->Process(pDstBitmap->bits, pDstBitmap->Width(), pDstBitmap->Height());

		BITMAPINFOHEADER bi;
		bi.biSize = sizeof(bi);
		bi.biWidth = pDstBitmap->biWidth;
		bi.biHeight = pDstBitmap->biHeight;
		bi.biBitCount = pDstBitmap->biBitCount;
		bi.biPlanes = pDstBitmap->biPlanes;
		bi.biSizeImage = pDstBitmap->biSizeImage;
		bi.biCompression = pDstBitmap->biCompression;
		bi.biClrImportant = pDstBitmap->biClrImportant;
		bi.biClrUsed = pDstBitmap->biClrUsed;
		bi.biHeight = -bi.biHeight;

		HBITMAP hBitmap = CreateDIBitmap(GetDC()->GetSafeHdc()
			, &bi
			, CBM_INIT
			, (void*)pDstBitmap->bits
			, (BITMAPINFO*)&bi
			, DIB_RGB_COLORS);

		delete pDstBitmap;
		m_pDstBitmap->DeleteObject();
		m_pDstBitmap->Attach(hBitmap);
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

	dcMem.TextOut(10, rect.bottom - 30, m_strParamText);

	CClientDC dc(this);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}
