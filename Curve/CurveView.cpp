
// CurveView.cpp : CCurveView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CurveApp.h"
#endif

#include "CurveDoc.h"
#include "CurveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


inline BOOL DrawBitmap(CDC* pDC, int x, int y, int w, int h, CBitmap* pBitmap, int xSrc, int ySrc)
{
	ASSERT(pDC != NULL && pBitmap != NULL);

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(pBitmap);

	return pDC->BitBlt(x, y, w, h, &dc, xSrc, ySrc, SRCCOPY);
}

// CCurveView

IMPLEMENT_DYNCREATE(CCurveView, CView)

BEGIN_MESSAGE_MAP(CCurveView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCurveView::OnFilePrintPreview)
	ON_MESSAGE(UM_CURVE_CHANGE, &CCurveView::OnCurveChange)
	ON_MESSAGE(UM_CURVE_RESET, &CCurveView::OnCurveReset)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CCurveView::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, &CCurveView::OnFileNew)
END_MESSAGE_MAP()

// CCurveView 构造/析构

CCurveView::CCurveView()
{
	// TODO:  在此处添加构造代码
	m_pSrcBitmap = new CBitmap;
	m_pDstBitmap = new CBitmap;
	m_bLoadBitmap = FALSE;

	for (int i = 0; i < 5; i++)
	{
		m_pCurveSamples[i] = new byte[256];

		for (int j = 0; j < 256; j++)
		{
			m_pCurveSamples[i][j] = j;
		}
	}
}

CCurveView::~CCurveView()
{
	SAFE_DELETE(m_pSrcBitmap);
	SAFE_DELETE(m_pDstBitmap);

	for (int i = 0; i < 5; i++)
	{
		SAFE_DELETE(m_pCurveSamples[i]);
	}
}

BOOL CCurveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCurveView 绘制

void CCurveView::OnDraw(CDC* /*pDC*/)
{
	CCurveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	// TODO:  在此处为本机数据添加绘制代码
	UpdateView();

	m_ctrlCurves.OnDraw(NULL);
}


// CCurveView 打印


void CCurveView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCurveView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCurveView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CCurveView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CCurveView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCurveView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCurveView 诊断

#ifdef _DEBUG
void CCurveView::AssertValid() const
{
	CView::AssertValid();
}

void CCurveView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCurveDoc* CCurveView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCurveDoc)));
	return (CCurveDoc*)m_pDocument;
}
#endif //_DEBUG


// CCurveView 消息处理程序


int CCurveView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	int ID = 6880;

	int w = 255;
	int h = 255;
	int x0 = 100;
	int y0 = 100;
	int x1 = x0 + w;
	int y1 = y0 + h;

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER;
// 	if (!m_ctrlCurves.Create(NULL
// 		, TEXT("Curves Config")
// 		, dwStyle
// 		, CRect(x0, y0, x1, y1)
// 		, this
// 		, ID++
// 		, NULL))
// 	{
// 		return -1;
// 	}

	dwStyle = WS_POPUP | WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_CAPTION;
	DWORD dwExStyle = WS_EX_TOPMOST;

	if (!m_ctrlCurves.CreateEx(dwExStyle
		, AfxRegisterWndClass(0)
		, TEXT("Curves Config")
		, dwStyle
		, CRect(x0, y0, x1, y1)
		, this
		, NULL
		, NULL))
	{
		return -1;
	}

	m_ctrlCurves.SetOwner(this);

	CRect rect(x0, y0, x1, y1);
	AdjustWindowRect(&rect, dwStyle, FALSE);
	m_ctrlCurves.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);

	return 0;
}

void CCurveView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect, rc;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	m_ctrlCurves.GetWindowRect(&rc);

	int w = rc.Width();
	int h = rc.Height();
	int x = rect.right - w;
	int y = rect.top + 1;

	m_ctrlCurves.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);
}

LRESULT CCurveView::OnCurveChange(WPARAM wParam, LPARAM lParam)
{
	int channel = (int)wParam;
	m_ctrlCurves.GetSample(channel, m_pCurveSamples[channel], nullptr);

	UpdateBitmap();

	UpdateView();

	return 0;
}

LRESULT CCurveView::OnCurveReset(WPARAM wParam, LPARAM lParam)
{
	int channel = (int)wParam;
	m_ctrlCurves.GetSample(channel, m_pCurveSamples[channel], nullptr);

	UpdateBitmap();

	UpdateView();

	return 0;
}

void CCurveView::UpdateView(void)
{
	if (!m_bLoadBitmap) return;

	CRect rect;
	GetClientRect(&rect);

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	CBitmap bmMem;
	bmMem.CreateCompatibleBitmap(GetDC(), rect.Width(), rect.Height());
	dcMem.SelectObject(&bmMem);

	CBrush brush(RGB(255, 255, 255));
	dcMem.FillRect(&rect, &brush);

	BITMAP bm;
	m_pSrcBitmap->GetBitmap(&bm);

	int x = 0, y = 0, w = bm.bmWidth, h = bm.bmHeight;
	DrawBitmap(&dcMem, x, y, w, h, m_pSrcBitmap, 0, 0);
	x += w;
	DrawBitmap(&dcMem, x, y, w, h, m_pDstBitmap, 0, 0);

	CClientDC dc(this);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}

void CCurveView::OnFileNew()
{
	// TODO:  在此添加命令处理程序代码
	HWND hWnd = m_ctrlCurves.GetSafeHwnd();

	if (hWnd == NULL)
	{
		int w = 255;
		int h = 255;
		int x0 = 100;
		int y0 = 100;
		int x1 = x0 + w;
		int y1 = y0 + h;

		DWORD dwStyle = WS_POPUP | WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_CAPTION;
		DWORD dwExStyle = WS_EX_TOPMOST;

		if (!m_ctrlCurves.CreateEx(dwExStyle
			, AfxRegisterWndClass(0)
			, TEXT("Curves Config")
			, dwStyle
			, CRect(x0, y0, x1, y1)
			, this
			, NULL
			, NULL))
		{
			return;
		}

		CRect rect(x0, y0, x1, y1);
		AdjustWindowRect(&rect, dwStyle, FALSE);
		m_ctrlCurves.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
		m_ctrlCurves.OnDraw(NULL);
	}

	//m_ctrlCurves.Reset();
	m_ctrlCurves.OnDraw(NULL);

	UpdateBitmap();

	UpdateView();
}

void CCurveView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dlg(TRUE, TEXT("*.BMP"), TEXT("*.BMP"));
	if (dlg.DoModal() == IDOK)
	{
		CCurveDoc* pDoc = GetDocument();

		if (!pDoc->OpenFile(dlg.GetPathName())) return;

		AttachBitmap(m_pSrcBitmap, pDoc->GetBitmap());
		m_bLoadBitmap = TRUE;

		UpdateBitmap();

		UpdateView();
	}
}

inline void CCurveView::AttachBitmap(CBitmap* pBitmap, const Bitmap* bitmap)
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
}

void CCurveView::UpdateBitmap(void)
{
	if (!m_bLoadBitmap) return;

	CCurveDoc* pDoc = GetDocument();
	Bitmap* pBitmap = pDoc->GetBitmap()->Clone();
	assert(pBitmap != NULL);

	int bytesPerPixel = pBitmap->PixelBytes();

	for (int y = 0; y < pBitmap->Height(); y++)
	{
		uint8* p = pBitmap->Get(0, y);
		for (int x = 0; x < pBitmap->Width(); x++)
		{
			//set color channel
			*(p + 0) = m_pCurveSamples[0][*(p + 0)];
			*(p + 1) = m_pCurveSamples[0][*(p + 1)];
			*(p + 2) = m_pCurveSamples[0][*(p + 2)];
			//set each channel
			*(p + 0) = m_pCurveSamples[3][*(p + 0)];
			*(p + 1) = m_pCurveSamples[2][*(p + 1)];
			*(p + 2) = m_pCurveSamples[1][*(p + 2)];
			//if has alpha channel
			if (bytesPerPixel > 3)
			{
				*(p + 3) = m_pCurveSamples[4][*(p + 3)];
			}

			p += bytesPerPixel;
		}
	}

	AttachBitmap(m_pDstBitmap, pBitmap);

	SAFE_DELETE(pBitmap);
}

