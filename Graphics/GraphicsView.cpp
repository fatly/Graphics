
// GraphicsView.cpp : CGraphicsView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Graphics.h"
#endif

#include "GraphicsDoc.h"
#include "GraphicsView.h"
#include "BaseClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float sigma = 0.5f;
// CGraphicsView

IMPLEMENT_DYNCREATE(CGraphicsView, CView)

BEGIN_MESSAGE_MAP(CGraphicsView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGraphicsView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, &CGraphicsView::OnFileOpen)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CGraphicsView 构造/析构

CGraphicsView::CGraphicsView()
{
	// TODO:  在此处添加构造代码
	m_pSrcBitmap = NULL;
	m_pDstBitmap = NULL;
	m_bMouseDown = FALSE;
	m_pProcesser = new ImageProcess;
	ASSERT(m_pProcesser != NULL);
}

CGraphicsView::~CGraphicsView()
{
	SAFE_DELETE(m_pSrcBitmap);
	SAFE_DELETE(m_pDstBitmap);
	SAFE_DELETE(m_pProcesser);
}

BOOL CGraphicsView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CGraphicsView 绘制

void CGraphicsView::OnDraw(CDC* /*pDC*/)
{
	CGraphicsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	CRect rc;
	GetClientRect(&rc);

	CDC dcMem;
	dcMem.CreateCompatibleDC(NULL);
	CBitmap bmMem;
	bmMem.CreateCompatibleBitmap(GetDC(), rc.Width(), rc.Height());
	dcMem.SelectObject(&bmMem);

	CBrush fillBrush(RGB(255, 255, 255));
	dcMem.FillRect(&rc, &fillBrush);

	if (m_pSrcBitmap != NULL && m_pDstBitmap != NULL)
	{
		BITMAP bm;
		m_pSrcBitmap->GetBitmap(&bm);

		int x = (rc.Width() - bm.bmWidth * 2) / 2;
		int y = (rc.Height() - bm.bmHeight) / 2;
		if (x < 0) x = 0;
		if (y < 0) y = 0;

		DrawBitmap(&dcMem, x, y, rc.Width(), rc.Height(), m_pSrcBitmap, 0, 0);

		x += bm.bmWidth;
		DrawBitmap(&dcMem, x, y, rc.Width(), rc.Height(), m_pDstBitmap, 0, 0);
	}

	if (!m_rcSelected.IsRectEmpty())
	{
		DrawRect(&dcMem, &m_rcSelected, RGB(255, 0, 0));
	}

	if (!m_rcClient.IsRectEmpty())
	{
		DrawRect(&dcMem, &m_rcClient, RGB(0, 255, 0));
	}

	if (!m_strText.IsEmpty())
	{
		DrawString(&dcMem, 100, m_rcClient.bottom - 30, m_strText);
	}

  	CClientDC dc(this);
  	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
}


// CGraphicsView 打印


void CGraphicsView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGraphicsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CGraphicsView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CGraphicsView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}

void CGraphicsView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGraphicsView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGraphicsView 诊断

#ifdef _DEBUG
void CGraphicsView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphicsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGraphicsDoc* CGraphicsView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGraphicsDoc)));
	return (CGraphicsDoc*)m_pDocument;
}
#endif //_DEBUG

// 实现
CBitmap* CGraphicsView::CreateBitmap(const Bitmap* pBitmap)
{
	ASSERT(pBitmap != NULL);

	CBitmap* pResult = new CBitmap;

	if (pResult != NULL)
	{
		BITMAPINFOHEADER bi;
		bi.biSize = sizeof(bi);
		bi.biWidth = pBitmap->biWidth;
		bi.biHeight = pBitmap->biHeight;
		bi.biBitCount = pBitmap->biBitCount;
		bi.biSizeImage = pBitmap->biSizeImage;
		bi.biPlanes = pBitmap->biPlanes;
		bi.biCompression = pBitmap->biCompression;
		bi.biClrImportant = pBitmap->biClrImportant;
		bi.biXPelsPerMeter = pBitmap->biXPelsPerMeter;
		bi.biYPelsPerMeter = pBitmap->biYPelsPerMeter;
		bi.biHeight = -bi.biHeight;
		//CreateDIBitmap The DDB that is created will be whatever bit depth your reference DC is.
		//CreateDIBSection To create a bitmap that is of different bit depth

		//Calling CreateDIBitmap with fdwInit as CBM_INIT is equivalent to calling the
		//CreateCompatibleBitmap function to create a DDB in the format of the device
		//and then calling the SetDIBits function to translate the DIB bits to the DDB.
		HBITMAP hBitmap = ::CreateDIBitmap(GetDC()->GetSafeHdc()
			, &bi
			, CBM_INIT
			, (void*)pBitmap->bits
			, (BITMAPINFO*)&bi
			, DIB_RGB_COLORS);

// 		uint8* bits;
// 		HBITMAP hBitmap = ::CreateDIBSection(GetDC()->GetSafeHdc()
// 			, (BITMAPINFO*)&bi
// 			, 0
// 			, (void**)&bits
// 			, NULL
// 			, 0);
//  		memcpy(bits, pBitmap->bits, pBitmap->size);

		ASSERT(hBitmap != NULL);
		pResult->Attach(hBitmap);
	}

	return pResult;
}

inline VOID CGraphicsView::DrawRect(CDC* pDC, const CRect* pRect, COLORREF color)
{
	ASSERT(pDC != NULL && pRect != NULL);

	CBrush brush(color);
	pDC->FrameRect(pRect, &brush);
}

inline BOOL CGraphicsView::DrawBitmap(CDC* pDC, int x, int y, int w, int h, CBitmap* pBitmap, int xSrc, int ySrc)
{
	ASSERT(pDC != NULL && pBitmap != NULL);

	CDC dc;
	dc.CreateCompatibleDC(NULL);
	dc.SelectObject(pBitmap);

	return pDC->BitBlt(x, y, w, h, &dc, xSrc, ySrc, SRCCOPY);
}

inline BOOL CGraphicsView::DrawBitmap(CDC* pDC, const CRect* pRect, CBitmap* pBitmap, int xSrc, int ySrc)
{
	return DrawBitmap(pDC, pRect->left, pRect->top, pRect->right, pRect->bottom, pBitmap, xSrc, ySrc);
}

inline BOOL CGraphicsView::DrawString(CDC* pDC, int x, int y, const CString & strText)
{
	ASSERT(pDC != 0);
	return pDC->TextOut(x, y, strText);
}


// CGraphicsView 消息处理程序

void CGraphicsView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	CFileDialog dlg(TRUE, TEXT(".BMP"), TEXT("*.BMP"));

	if (dlg.DoModal() == IDOK)
	{
		CGraphicsDoc* pDoc = GetDocument();

		pDoc->LoadBitmap(dlg.GetPathName());
		Bitmap* pBitmap = pDoc->GetSrcBitmap();

		Bitmap* pPPM = pBitmap->Clone();
		pPPM->SwapChannel(0, 2);
		Save2PPM(pPPM, "f:\\1.ppm");
		delete pPPM;
#if 0
		m_pBitmap = new CBitmap;
		m_pBitmap->CreateCompatibleBitmap(GetDC(), pBitmap->biWidth, pBitmap->biHeight);
		m_pBitmap->ExtendAlpha(255);
		m_pBitmap->SetBitmapBits(pBitmap->size, pBitmap->bits);
#else
		m_pSrcBitmap = CreateBitmap(pBitmap);
#endif
// 		Bitmap* pResult = m_pProcesser->SmoothBitmap(pBitmap, sigma);
// 		m_pDstBitmap = CreateBitmap(pResult);
// 		pDoc->SetDstBitmap(pResult);
// 
// 		m_strText.Format(TEXT("smooth sigma = %f"), sigma);
//		SendMessage(WM_PAINT, 0, 0);
		UpdateBitmap(sigma);
	}
}

void CGraphicsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_MousePoint[0] = point;
	m_rcSelected.left = m_rcSelected.right = point.x;
	m_rcSelected.top = m_rcSelected.bottom = point.y;
	m_bMouseDown = TRUE;

	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}


void CGraphicsView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_bMouseDown)
	{
		CRect rect;
		GetClientRect(&rect);
		point.x = clamp(point.x, rect.left, rect.right);
		point.y = clamp(point.y, rect.top, rect.bottom);

		m_rcSelected.left = m_MousePoint[0].x;
		m_rcSelected.top = m_MousePoint[0].y;
		m_rcSelected.right = point.x;
		m_rcSelected.bottom = point.y;

		m_rcSelected.NormalizeRect();

		SendMessage(WM_PAINT, 0, 0);
	}

	CView::OnMouseMove(nFlags, point);
}


void CGraphicsView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	point.x = clamp(point.x, rect.left, rect.right);
	point.y = clamp(point.y, rect.top, rect.bottom);

	m_rcSelected.left = m_MousePoint[0].x;
	m_rcSelected.top = m_MousePoint[0].y;
	m_rcSelected.right = point.x;
	m_rcSelected.bottom = point.y;

	m_rcSelected.NormalizeRect();

	if (m_pSrcBitmap && m_pDstBitmap)
	{
		CGraphicsDoc* pDoc = GetDocument();
		Bitmap* pBitmap = pDoc->GetSrcBitmap();

		int xOffset = (rect.Width() - pBitmap->Width()*2) / 2;
		int yOffset = (rect.Height() - pBitmap->Height()) / 2;

		if (!m_rcSelected.IsRectEmpty())
		{
			int x0 = clamp(m_rcSelected.left - xOffset, 0, (int)pBitmap->biWidth - 1);
			int y0 = clamp(m_rcSelected.top - yOffset, 0, (int)pBitmap->biHeight - 1);
			int x1 = clamp(m_rcSelected.right - xOffset, 0, (int)pBitmap->biWidth - 1);
			int y1 = clamp(m_rcSelected.bottom - yOffset, 0, (int)pBitmap->biHeight - 1);
			//test clone
			pBitmap = pDoc->CloneBitmap(pBitmap, x0, y0, x1, y1);
			assert(pBitmap != 0);
			m_pProcesser->DrawRect(pBitmap, 0, 0, pBitmap->biWidth-1, pBitmap->biHeight-1);
			pBitmap->Save("f:\\clone.bmp");
			delete pBitmap;
		}
		else
		{
			m_rcSelected.SetRect(xOffset, yOffset, xOffset + pBitmap->biWidth, yOffset + pBitmap->biHeight);
		}
	}

	m_bMouseDown = FALSE;

	ReleaseCapture();

	SendMessage(WM_PAINT, 0, 0);

	CView::OnLButtonUp(nFlags, point);
}

void CGraphicsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	static CSize size(cx, cy);

	if (cx != size.cx || cy != size.cy)
	{
		float sw = cx / max(1.0f, size.cx);
		float sh = cy / max(1.0f, size.cy);

		size.SetSize(cx, cy);
	}

	m_rcClient.right = cx;
	m_rcClient.bottom = cy;
}


BOOL CGraphicsView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (zDelta > 0)
		sigma += 0.1f;
	else
		sigma -= 0.1f;

	if (sigma < 0.0f) sigma = 0.0f;

	UpdateBitmap(sigma);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGraphicsView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nChar == 38)
	{
		sigma += 0.1f;
	}
	else if (nChar == 40)
	{
		sigma -= 0.1f;
	}

	if (sigma < 0.0f) sigma = 0.0f;

	UpdateBitmap(sigma);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGraphicsView::UpdateBitmap(float sigma)
{
	CGraphicsDoc* pDoc = GetDocument();
	Bitmap* src = pDoc->GetSrcBitmap();
	Bitmap* dst = m_pProcesser->SmoothBitmap(src, sigma);

	if (m_pDstBitmap != 0)
		delete m_pDstBitmap;
	m_pDstBitmap = CreateBitmap(dst);

	pDoc->SetDstBitmap(dst);

	m_strText.Format(TEXT("smooth sigma = %f\n"), sigma);

	SendMessage(WM_PAINT, 0, 0);
}