
// HSVFilterView.cpp : CHSVFilterView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "HSVFilter.h"
#endif

#include "HSVFilterDoc.h"
#include "HSVFilterView.h"

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
	text.Format(TEXT("%.2f"), x);
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

// CHSVFilterView

IMPLEMENT_DYNCREATE(CHSVFilterView, CView)

BEGIN_MESSAGE_MAP(CHSVFilterView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CHSVFilterView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_FILE_OPEN, &CHSVFilterView::OnFileOpen)
END_MESSAGE_MAP()

// CHSVFilterView 构造/析构

CHSVFilterView::CHSVFilterView()
{
	// TODO:  在此处添加构造代码
	m_pSrcBitmap = new CBitmap;
	m_pHSVBitmap = new CBitmap;
	m_bLoadBitmap = FALSE;
	m_bNeedUpdateBitmap = TRUE;
}

CHSVFilterView::~CHSVFilterView()
{
	if (m_pSrcBitmap)
		delete m_pSrcBitmap;
	if (m_pHSVBitmap)
		delete m_pHSVBitmap;
}

BOOL CHSVFilterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CHSVFilterView 绘制

void CHSVFilterView::OnDraw(CDC* /*pDC*/)
{
	CHSVFilterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	UpdateView();
}


// CHSVFilterView 打印


void CHSVFilterView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CHSVFilterView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CHSVFilterView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CHSVFilterView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CHSVFilterView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CHSVFilterView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CHSVFilterView 诊断

#ifdef _DEBUG
void CHSVFilterView::AssertValid() const
{
	CView::AssertValid();
}

void CHSVFilterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CHSVFilterDoc* CHSVFilterView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHSVFilterDoc)));
	return (CHSVFilterDoc*)m_pDocument;
}
#endif //_DEBUG


// CHSVFilterView 消息处理程序


int CHSVFilterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rect;
	GetClientRect(&rect);

	int id = 6880;
	int y = 10;
	int w = 150;
	int h = 20;

	DWORD dwStyle = TBS_HORZ | WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (!m_ctrlMinH.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, id++))
	{
		return -1;
	}

	y += h + 10;
	if (!m_ctrlMaxH.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, id++))
	{
		return -1;
	}

	y += h + 10;
	if (!m_ctrlMinS.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, id++))
	{
		return -1;
	}

	y += h + 10;
	if (!m_ctrlMaxS.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, id++))
	{
		return -1;
	}

	y += h + 10;
	if (!m_ctrlMinV.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, id++))
	{
		return -1;
	}

	y += h + 10;
	if (!m_ctrlMaxV.Create(dwStyle, CRect(rect.right - w, y, rect.right, y + h), this, id++))
	{
		return -1;
	}

	m_ctrlMinH.SetRange(0, 360);
	m_ctrlMaxH.SetRange(0, 360);
	m_ctrlMinS.SetRange(0, 100);
	m_ctrlMaxS.SetRange(0, 100);
	m_ctrlMinV.SetRange(0, 100);
	m_ctrlMaxV.SetRange(0, 100);

	m_ctrlMinH.SetPos(180);
	m_ctrlMaxH.SetPos(250);

	m_ctrlMinS.SetPos(40);
	m_ctrlMaxS.SetPos(100);

	m_ctrlMinV.SetPos(10);
	m_ctrlMaxV.SetPos(100);

	dwStyle = ES_RIGHT | ES_NUMBER | ES_READONLY | WS_CHILD | WS_VISIBLE | WS_BORDER;
	if (!m_editMinH.Create(dwStyle, CRect(0, 0, 0, 0), this, id++))
	{
		return -1;
	}

	if (!m_editMaxH.Create(dwStyle, CRect(0, 0, 0, 0), this, id++))
	{
		return -1;
	}

	if (!m_editMinS.Create(dwStyle, CRect(0, 0, 0, 0), this, id++))
	{
		return -1;
	}

	if (!m_editMaxS.Create(dwStyle, CRect(0, 0, 0, 0), this, id++))
	{
		return -1;
	}

	if (!m_editMinV.Create(dwStyle, CRect(0, 0, 0, 0), this, id++))
	{
		return -1;
	}

	if (!m_editMaxV.Create(dwStyle, CRect(0, 0, 0, 0), this, id++))
	{
		return -1;
	}

	m_editMinH.SetWindowText(TEXT("180"));
	m_editMaxH.SetWindowText(TEXT("250"));
	m_editMinS.SetWindowText(TEXT("0.40"));
	m_editMaxS.SetWindowText(TEXT("1.00"));
	m_editMinV.SetWindowText(TEXT("0.10"));
	m_editMaxV.SetWindowText(TEXT("1.00"));

	return 0;
}


void CHSVFilterView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);

	int w = 150;
	int h = 20;
	int x = rect.right - w;

	int y = 10;
	m_ctrlMinH.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlMaxH.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlMinS.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlMaxS.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlMinV.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_ctrlMaxV.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	w = 50;
	x = rect.right - 150 - w - 10;
	y = 10;
	m_editMinH.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editMaxH.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editMinS.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editMaxS.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editMinV.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);

	y += 25;
	m_editMaxV.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);
}

void CHSVFilterView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CSliderCtrl* pSlider = (CSliderCtrl*)pScrollBar;
	if (pSlider == &m_ctrlMinH)
	{
 		int nPos = m_ctrlMinH.GetPos();
// 		if (nPos > m_ctrlMaxH.GetPos())
// 		{
// 			nPos = m_ctrlMaxH.GetPos();
// 			m_ctrlMinH.SetPos(nPos);
// 		}
		m_editMinH.SetWindowText(I2A(nPos));
	}
	else if (pSlider == &m_ctrlMaxH)
	{
 		int nPos = m_ctrlMaxH.GetPos();
// 		if (nPos < m_ctrlMinH.GetPos())
// 		{
// 			nPos = m_ctrlMinH.GetPos();
// 			m_ctrlMaxH.SetPos(nPos);
// 		}
		m_editMaxH.SetWindowText(I2A(nPos));
	}
	else if (pSlider == &m_ctrlMinS)
	{
 		int nPos = m_ctrlMinS.GetPos();
// 		if (nPos > m_ctrlMaxS.GetPos())
// 		{
// 			nPos = m_ctrlMaxS.GetPos();
// 			m_ctrlMinS.SetPos(nPos);
// 		}
		m_editMinS.SetWindowText(F2A((float)nPos / 100));
	}
	else if (pSlider == &m_ctrlMaxS)
	{
 		int nPos = m_ctrlMaxS.GetPos();
// 		if (nPos < m_ctrlMinS.GetPos())
// 		{
// 			nPos = m_ctrlMinS.GetPos();
// 			m_ctrlMaxS.SetPos(nPos);
// 		}
		m_editMaxS.SetWindowText(F2A((float)nPos / 100));
	}
	else if (pSlider == &m_ctrlMinV)
	{
 		int nPos = m_ctrlMinV.GetPos();
// 		if (nPos > m_ctrlMaxV.GetPos())
// 		{
// 			nPos = m_ctrlMaxV.GetPos();
// 			m_ctrlMinV.SetPos(nPos);
// 		}
		m_editMinV.SetWindowText(F2A((float)nPos / 100));
	}
	else if (pSlider == &m_ctrlMaxV)
	{
 		int nPos = m_ctrlMaxV.GetPos();
// 		if (nPos < m_ctrlMinV.GetPos())
// 		{
// 			nPos = m_ctrlMinV.GetPos();
// 			m_ctrlMaxV.SetPos(nPos);
// 		}
		m_editMaxV.SetWindowText(F2A((float)nPos / 100));
	}

	UpdateView();

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHSVFilterView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dlg(TRUE, TEXT("*.BMP"), TEXT("*.BMP"));

	if (dlg.DoModal() == IDOK)
	{
		CHSVFilterDoc* pDoc = GetDocument();
		pDoc->OpenFile(dlg.GetPathName());
		Bitmap* pBitmap = pDoc->GetBitmap(0);
		
		BITMAPINFOHEADER bi;
		bi.biSize = sizeof(bi);
		bi.biWidth = pBitmap->biWidth;
		bi.biHeight = pBitmap->biHeight;
		bi.biBitCount = pBitmap->biBitCount;
		bi.biPlanes = pBitmap->biPlanes;
		bi.biSizeImage = pBitmap->biSizeImage;
		bi.biCompression = pBitmap->biCompression;
		bi.biClrImportant = pBitmap->biClrImportant;
		bi.biClrUsed = pBitmap->biClrUsed;
		bi.biHeight = -bi.biHeight;

		HBITMAP hBitmap = CreateDIBitmap(GetDC()->GetSafeHdc()
			, &bi
			, CBM_INIT
			, (void*)pBitmap->bits
			, (BITMAPINFO*)&bi
			, DIB_RGB_COLORS);

		m_pSrcBitmap->DeleteObject();

		m_pSrcBitmap->Attach(hBitmap);

		m_bLoadBitmap = TRUE;
		m_bNeedUpdateBitmap = TRUE;

		UpdateView();
	}
}

void CHSVFilterView::UpdateView(void)
{
	if (!m_bLoadBitmap) return;

	static float fMinH = 0.0f;
	static float fMaxH = 0.0f;
	static float fMinS = 0.0f;
	static float fMaxS = 0.0f;
	static float fMinV = 0.0f;
	static float fMaxV = 0.0f;

	float _fMinH = m_ctrlMinH.GetPos() * 1.0f;
	float _fMaxH = m_ctrlMaxH.GetPos() * 1.0f;
	float _fMinS = m_ctrlMinS.GetPos() * 1.0f / 100.0f;
	float _fMaxS = m_ctrlMaxS.GetPos() * 1.0f / 100.0f;
	float _fMinV = m_ctrlMinV.GetPos() * 1.0f / 100.0f;
	float _fMaxV = m_ctrlMaxV.GetPos() * 1.0f / 100.0f;

	if (_fMinH != fMinH){ fMinH = _fMinH; m_bNeedUpdateBitmap = TRUE; }
	if (_fMaxH != fMaxH){ fMaxH = _fMaxH; m_bNeedUpdateBitmap = TRUE; }
	if (_fMinS != fMinS){ fMinS = _fMinS; m_bNeedUpdateBitmap = TRUE; }
	if (_fMaxS != fMaxS){ fMaxS = _fMaxS; m_bNeedUpdateBitmap = TRUE; }
	if (_fMinV != fMinV){ fMinV = _fMinV; m_bNeedUpdateBitmap = TRUE; }
	if (_fMaxV != fMaxV){ fMaxV = _fMaxV; m_bNeedUpdateBitmap = TRUE; }

	if (fMinH > fMaxH) swap(fMinH, fMaxH);
	if (fMinS > fMaxS) swap(fMinS, fMaxS);
	if (fMinV > fMaxV) swap(fMinV, fMaxV);

	Bitmap* pSrcBitmap = GetDocument()->GetBitmap(0);

	if (m_bNeedUpdateBitmap)
	{
		Bitmap* pDstBitmap = new Bitmap(pSrcBitmap->Width(), pSrcBitmap->Height(), 24);
		HSVFilter24(pDstBitmap, pSrcBitmap, fMinH, fMaxH, fMinS, fMaxS, fMinV, fMaxV);

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
		m_pHSVBitmap->DeleteObject();
		m_pHSVBitmap->Attach(hBitmap);
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
	DrawBitmap(&dcMem, x, y, w, h, m_pHSVBitmap, 0, 0);

	CClientDC dc(this);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}