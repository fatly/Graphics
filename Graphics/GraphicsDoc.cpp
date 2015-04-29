
// GraphicsDoc.cpp : CGraphicsDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Graphics.h"
#endif

#include "GraphicsDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGraphicsDoc

IMPLEMENT_DYNCREATE(CGraphicsDoc, CDocument)

BEGIN_MESSAGE_MAP(CGraphicsDoc, CDocument)
END_MESSAGE_MAP()


// CGraphicsDoc ����/����

CGraphicsDoc::CGraphicsDoc()
{
	// TODO:  �ڴ�����һ���Թ������
	m_pBitmap = 0;
}

CGraphicsDoc::~CGraphicsDoc()
{
	SAFE_DELETE(m_pBitmap);
}

BOOL CGraphicsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  �ڴ��������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CGraphicsDoc ���л�

void CGraphicsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  �ڴ����Ӵ洢����
	}
	else
	{
		// TODO:  �ڴ����Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CGraphicsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// �������������֧��
void CGraphicsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CGraphicsDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CGraphicsDoc ���

#ifdef _DEBUG
void CGraphicsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGraphicsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGraphicsDoc ����
Bitmap* CGraphicsDoc::LoadBitmap(const CString & filePath)
{
	USES_CONVERSION;
	const char* fileName = W2A(filePath);

	m_pBitmap = new Bitmap;
	assert(m_pBitmap != 0);

	if (m_pBitmap != 0)
	{
		if (!m_pBitmap->Load(fileName))
		{
			delete m_pBitmap;
			m_pBitmap = 0;
		}
	}

	return m_pBitmap;
}

Bitmap* CGraphicsDoc::CloneBitmap(int x0, int y0, int x1, int y1)
{
	assert(m_pBitmap != 0);

	if (m_pBitmap != 0)
	{
		return m_pBitmap->Clone(x0, y0, x1, y1);
	}

	return 0;
}