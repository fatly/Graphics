
// GraphicsDoc.h : CGraphicsDoc ��Ľӿ�
//

#pragma once

#include "Bitmap.h"
using namespace e;

class CGraphicsDoc : public CDocument
{
protected: // �������л�����
	CGraphicsDoc();
	DECLARE_DYNCREATE(CGraphicsDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CGraphicsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	Bitmap* LoadBitmap(const CString & filePath);

	Bitmap* CloneBitmap(const Bitmap* bitmap, int x0, int y0, int x1, int y1);

	void SetSrcBitmap(Bitmap* bitmap)
	{
		if (m_pSrcBitmap != 0)
			delete m_pSrcBitmap;
		m_pSrcBitmap = bitmap;
	}

	void SetDstBitmap(Bitmap* bitmap)
	{
		if (m_pDstBitmap != 0)
			delete m_pDstBitmap;
		m_pDstBitmap = bitmap;
	}

	Bitmap* GetSrcBitmap(void) const
	{
		return m_pSrcBitmap;
	}

	Bitmap* GetDstBitmap(void) const
	{
		return m_pDstBitmap;
	}

protected:
	Bitmap* m_pSrcBitmap;
	Bitmap* m_pDstBitmap;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
