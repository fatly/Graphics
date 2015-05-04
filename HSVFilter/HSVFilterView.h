
// HSVFilterView.h : CHSVFilterView 类的接口
//

#pragma once


class CHSVFilterView : public CView
{
protected: // 仅从序列化创建
	CHSVFilterView();
	DECLARE_DYNCREATE(CHSVFilterView)

// 特性
public:
	CHSVFilterDoc* GetDocument() const;

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
// 生成的消息映射函数
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

#ifndef _DEBUG  // HSVFilterView.cpp 中的调试版本
inline CHSVFilterDoc* CHSVFilterView::GetDocument() const
   { return reinterpret_cast<CHSVFilterDoc*>(m_pDocument); }
#endif

