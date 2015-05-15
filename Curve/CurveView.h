
// CurveView.h : CCurveView ��Ľӿ�
//

#pragma once
#include "CurveCtrl.h"

class CCurveView : public CView
{
protected: // �������л�����
	CCurveView();
	DECLARE_DYNCREATE(CCurveView)

// ����
public:
	CCurveDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CCurveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void UpdateView(void);
protected:
	CCurveCtrl m_ctrlCurve;
// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // CurveView.cpp �еĵ��԰汾
inline CCurveDoc* CCurveView::GetDocument() const
   { return reinterpret_cast<CCurveDoc*>(m_pDocument); }
#endif

