
// SoftGlowDoc.h : CSoftGlowDoc ��Ľӿ�
//


#pragma once


class CSoftGlowDoc : public CDocument
{
protected: // �������л�����
	CSoftGlowDoc();
	DECLARE_DYNCREATE(CSoftGlowDoc)

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
	virtual ~CSoftGlowDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	Bitmap* GetBitmap(void) const
	{
		return m_pBitmap;
	}

	BOOL OpenFile(const CString & fileName);

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
