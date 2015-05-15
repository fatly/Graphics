
// CurveView.cpp : CCurveView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCurveView::OnFilePrintPreview)
	ON_MESSAGE(UM_CURVE_CHANGE, &CCurveView::OnCurveChange)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_OPEN, &CCurveView::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, &CCurveView::OnFileNew)
END_MESSAGE_MAP()

// CCurveView ����/����

CCurveView::CCurveView()
{
	// TODO:  �ڴ˴���ӹ������

}

CCurveView::~CCurveView()
{
}

BOOL CCurveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CCurveView ����

void CCurveView::OnDraw(CDC* /*pDC*/)
{
	CCurveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
	UpdateView();

	m_ctrlCurves.OnDraw(NULL);
}


// CCurveView ��ӡ


void CCurveView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCurveView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CCurveView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CCurveView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  ��Ӵ�ӡ����е��������
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


// CCurveView ���

#ifdef _DEBUG
void CCurveView::AssertValid() const
{
	CView::AssertValid();
}

void CCurveView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCurveDoc* CCurveView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCurveDoc)));
	return (CCurveDoc*)m_pDocument;
}
#endif //_DEBUG


// CCurveView ��Ϣ�������


int CCurveView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
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

	CRect rect(x0, y0, x1, y1);
	AdjustWindowRect(&rect, dwStyle, FALSE);
	m_ctrlCurves.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_SHOWWINDOW);

	return 0;
}

void CCurveView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
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
	// TODO:  �ڴ���������������
	int count = 9;
	double* points = new double[count * 2];
	int channel = m_ctrlCurves.GetSelectChannel();
	m_ctrlCurves.ExportPoint(channel, points, count);

	TCHAR text[256] = { 0 };
	for (int i = 0; i < count; i++)
	{
		_stprintf_s(text, TEXT("%lf, %lf,\n"), points[i * 2 + 0], points[i * 2 + 1]);
		OutputDebugString(text);
	}

	delete[] points;
}


void CCurveView::OnFileNew()
{
	// TODO:  �ڴ���������������
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
}
