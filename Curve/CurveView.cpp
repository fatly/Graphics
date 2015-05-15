
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


// CCurveView

IMPLEMENT_DYNCREATE(CCurveView, CView)

BEGIN_MESSAGE_MAP(CCurveView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCurveView::OnFilePrintPreview)
	ON_MESSAGE(UM_CURVE_CHANGE, &CCurveView::OnCurveChange)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CCurveView::OnFileOpen)
END_MESSAGE_MAP()

// CCurveView 构造/析构

CCurveView::CCurveView()
{
	// TODO:  在此处添加构造代码

}

CCurveView::~CCurveView()
{
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

	m_ctrlCurve.OnDraw(NULL);
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
	if (!m_ctrlCurve.Create(NULL
		, TEXT("Curve Config")
		, dwStyle
		, CRect(x0, y0, x1, y1)
		, this
		, ID++
		, NULL))
	{
		return -1;
	}

	CRect rect(x0, y0, x1, y1);
	AdjustWindowRect(&rect, dwStyle, FALSE);
	m_ctrlCurve.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);
	m_ctrlCurve.ShowWindow(SW_SHOW);

	return 0;
}

void CCurveView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	CRect rect;
	m_ctrlCurve.GetWindowRect(&rect);

	int w = rect.Width();
	int h = rect.Height();
	int x = cx - w;
	int y = 1;

	m_ctrlCurve.SetWindowPos(NULL, x, y, w, h, SWP_SHOWWINDOW);
}

LRESULT CCurveView::OnCurveChange(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CCurveView::UpdateView(void)
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

	CClientDC dc(this);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
}


void CCurveView::OnFileOpen()
{
	// TODO:  在此添加命令处理程序代码
	int count = 17;
	double* points = new double[count * 2];
	int channel = m_ctrlCurve.GetSelectChannel();
	m_ctrlCurve.ExportPoint(channel, points, count);

	TCHAR text[256] = { 0 };
	for (int i = 0; i < count; i++)
	{
		_stprintf_s(text, TEXT("[%02d] x = %lf, y = %lf\n"), i, points[i * 2 + 0], points[i * 2 + 1]);
		OutputDebugString(text);
	}
}
