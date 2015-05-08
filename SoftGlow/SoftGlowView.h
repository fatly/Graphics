
// SoftGlowView.h : CSoftGlowView ��Ľӿ�
//
#pragma once
#include "SoftGlow.h"
using namespace e;

class CSoftGlowView : public CView
{
protected: // �������л�����
	CSoftGlowView();
	DECLARE_DYNCREATE(CSoftGlowView)

// ����
public:
	CSoftGlowDoc* GetDocument() const;

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
	virtual ~CSoftGlowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL AttachBitmap(CBitmap* pBitmap, const Bitmap* bm);
	void UpdateView(void);
protected:
	CSliderCtrl m_ctrlRadius;
	CSliderCtrl m_ctrlSharpness;
	CSliderCtrl m_ctrlBrightness;
	CEdit		m_editRadius;
	CEdit       m_editSharpness;
	CEdit		m_editBridhtness;

	CBitmap*	m_pSrcBitmap;
	CBitmap*	m_pDstBitmap;
	BOOL		m_bLoadBitmap;
	BOOL		m_bNeedUpdateBitmap;
	SoftGlow* m_pFilter;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // SoftGlowView.cpp �еĵ��԰汾
inline CSoftGlowDoc* CSoftGlowView::GetDocument() const
   { return reinterpret_cast<CSoftGlowDoc*>(m_pDocument); }
#endif

