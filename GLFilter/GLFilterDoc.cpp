
// GLFilterDoc.cpp : CGLFilterDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "GLFilter.h"
#endif

#include "GLFilterDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGLFilterDoc

IMPLEMENT_DYNCREATE(CGLFilterDoc, CDocument)

BEGIN_MESSAGE_MAP(CGLFilterDoc, CDocument)
END_MESSAGE_MAP()


// CGLFilterDoc ����/����

CGLFilterDoc::CGLFilterDoc()
{
	// TODO:  �ڴ����һ���Թ������

}

CGLFilterDoc::~CGLFilterDoc()
{
	for (size_t i = 0; i < m_BitmapArray.size(); i++)
	{
		Bitmap* pBitmap = m_BitmapArray[i];
		delete pBitmap;
	}
}

BOOL CGLFilterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CGLFilterDoc ���л�

void CGLFilterDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  �ڴ���Ӵ洢����
	}
	else
	{
		// TODO:  �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CGLFilterDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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

// ������������֧��
void CGLFilterDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CGLFilterDoc::SetSearchContent(const CString& value)
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

// CGLFilterDoc ���

#ifdef _DEBUG
void CGLFilterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGLFilterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGLFilterDoc ����
BOOL CGLFilterDoc::OpenFile(const CString & fileName)
{
	USES_CONVERSION;
	const char* path = W2A(fileName);
	Bitmap* pBitmap = new Bitmap(path);
	assert(pBitmap != 0);

	if (m_BitmapArray.size() > 0)
	{
		Bitmap* pTemp = m_BitmapArray[0];
		delete pTemp;
		m_BitmapArray.remove(0);
	}

	m_BitmapArray.push_back(pBitmap);

	return pBitmap->IsValid() ? TRUE : FALSE;
}