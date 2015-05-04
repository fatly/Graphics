
// GLFitlerView.h : CGLFitlerView 类的接口
//

#pragma once

class CGLFitlerView : public CView
{
protected: // 仅从序列化创建
	CGLFitlerView();
	DECLARE_DYNCREATE(CGLFitlerView)

// 特性
public:
	CGLFitlerDoc* GetDocument() const;

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
	virtual ~CGLFitlerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void UpdateView(void);
protected:
	CSliderCtrl m_ctrlRadius;
	CSliderCtrl m_ctrlSigma;
	CEdit		m_editRadius;
	CEdit		m_editSigma;
	CBitmap*    m_pSrcBitmap;
	CBitmap*	m_pDstBitmap;
	BOOL		m_bLoadBitmap;
	BOOL		m_bNeedUpdateBitmap;
	e::ImageFilter* m_pImagerFitler;
	CString		m_strParamText;
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

#ifndef _DEBUG  // GLFitlerView.cpp 中的调试版本
inline CGLFitlerDoc* CGLFitlerView::GetDocument() const
   { return reinterpret_cast<CGLFitlerDoc*>(m_pDocument); }
#endif

