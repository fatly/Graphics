
// HSVFilterView.h : CHSVFilterView ��Ľӿ�
//

#pragma once


class CHSVFilterView : public CView
{
protected: // �������л�����
	CHSVFilterView();
	DECLARE_DYNCREATE(CHSVFilterView)

// ����
public:
	CHSVFilterDoc* GetDocument() const;

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
	virtual ~CHSVFilterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void UpdateView(void);
protected:
	CSliderCtrl m_ctrlMinH;
	CSliderCtrl m_ctrlMaxH;
	CSliderCtrl m_ctrlMinS;
	CSliderCtrl m_ctrlMaxS;
	CSliderCtrl m_ctrlMinV;
	CSliderCtrl m_ctrlMaxV;
	CEdit m_editMinH;
	CEdit m_editMaxH;
	CEdit m_editMinS;
	CEdit m_editMaxS;
	CEdit m_editMinV;
	CEdit m_editMaxV;

	CBitmap* m_pSrcBitmap;
	CBitmap* m_pHSVBitmap;
	BOOL m_bLoadBitmap;
	BOOL m_bNeedUpdateBitmap;
// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFileOpen();
};

#ifndef _DEBUG  // HSVFilterView.cpp �еĵ��԰汾
inline CHSVFilterDoc* CHSVFilterView::GetDocument() const
   { return reinterpret_cast<CHSVFilterDoc*>(m_pDocument); }
#endif

