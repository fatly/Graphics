
// GraphicsView.h : CGraphicsView 类的接口
//

#pragma once
#include "BitmapGlyph.h"
#include "ImageProcess.h"
using namespace e;

class CGraphicsView : public CView
{
protected: // 仅从序列化创建
	CGraphicsView();
	DECLARE_DYNCREATE(CGraphicsView)

// 特性
public:
	CGraphicsDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CGraphicsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CBitmap* CreateBitmap(const Bitmap* pBitmap);
	inline VOID DrawRect(CDC* pDC, const CRect* pRect, COLORREF color);
	inline BOOL DrawBitmap(CDC* pDC, int x, int y, int w, int h, CBitmap* pBitmap, int xSrc, int ySrc);
	inline BOOL DrawBitmap(CDC* pDC, const CRect* pRect, CBitmap* pBitmap, int xSrc, int ySrc);
	ImageProcess* GetImageProcesser(void) const{ return m_pProcesser;}
protected:
	CBitmap* m_pBitmap;
	CPoint m_MousePoint[2];
	CRect m_rcSelected;
	BOOL m_bMouseDown;

	ImageProcess* m_pProcesser;
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // GraphicsView.cpp 中的调试版本
inline CGraphicsDoc* CGraphicsView::GetDocument() const
   { return reinterpret_cast<CGraphicsDoc*>(m_pDocument); }
#endif

