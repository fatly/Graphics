#pragma once


// CCurveCtrl
class CurvesConfig;
class CCurveCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCurveCtrl)

public:
	CCurveCtrl();
	virtual ~CCurveCtrl();
	void OnDraw(CDC* pDC);
	void SetSelectChannel(const int channel);
	int  GetSelectChannel(void) const;
	byte GetSample(const int channel, const int index);
	void GetSample(const int channel, uint8* samples, int* size = nullptr);
	void GetSmaple(const int channel, uint8* samples, const int size);
	void ExportPoint(const int channel, double* points, const int count);
	void Reset(void);
protected:
	void UpdateView(void);
	void NotifyOwner(UINT msg);
protected:
	CComboBox		m_ctrlChannels;
	CButton			m_ctrlReset;
	CurvesConfig*	m_pCurveConfig;
	BOOL			m_bMouseDown;
	int				m_nSelectIndex;
	int				m_nPointCount;
	int				m_nSampleCount;
	int				m_nLeftMost;
	int				m_nRightMost;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSelChangeChannels(void);
	afx_msg void OnClickReset(void);
};


