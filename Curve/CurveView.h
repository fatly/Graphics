
// CurveView.h : CCurveView 类的接口
//

#pragma once
#include "CurveCtrl.h"

class CCurveView : public CView
{
protected: // 仅从序列化创建
	CCurveView();
	DECLARE_DYNCREATE(CCurveView)

// 特性
public:
	CCurveDoc* GetDocument() const;

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
	virtual ~CCurveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void UpdateView(void);
protected:
	CCurveCtrl m_ctrlCurve;
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnCurveChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileOpen();
};

#ifndef _DEBUG  // CurveView.cpp 中的调试版本
inline CCurveDoc* CCurveView::GetDocument() const
   { return reinterpret_cast<CCurveDoc*>(m_pDocument); }
#endif

