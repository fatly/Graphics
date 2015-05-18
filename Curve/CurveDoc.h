
// CurveDoc.h : CCurveDoc ��Ľӿ�
//


#pragma once
using namespace e;

class CCurveDoc : public CDocument
{
protected: // �������л�����
	CCurveDoc();
	DECLARE_DYNCREATE(CCurveDoc)

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
	virtual ~CCurveDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	BOOL OpenFile(const CString & fileName);

	void SetBitmap(Bitmap* bitmap)
	{
		SAFE_DELETE(m_pBitmap);
		m_pBitmap = bitmap;
	}

	Bitmap* GetBitmap(void) const
	{
		return m_pBitmap;
	}
protected:
	Bitmap* m_pBitmap;
// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
