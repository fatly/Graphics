
// GLFitlerDoc.cpp : CGLFitlerDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "GLFitler.h"
#endif

#include "GLFitlerDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGLFitlerDoc

IMPLEMENT_DYNCREATE(CGLFitlerDoc, CDocument)

BEGIN_MESSAGE_MAP(CGLFitlerDoc, CDocument)
END_MESSAGE_MAP()


// CGLFitlerDoc ����/����

CGLFitlerDoc::CGLFitlerDoc()
{
	// TODO:  �ڴ����һ���Թ������

}

CGLFitlerDoc::~CGLFitlerDoc()
{
	for (size_t i = 0; i < m_BitmapArray.size(); i++)
	{
		Bitmap* pBitmap = m_BitmapArray[i];
		delete pBitmap;
	}
}

BOOL CGLFitlerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CGLFitlerDoc ���л�

void CGLFitlerDoc::Serialize(CArchive& ar)
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
void CGLFitlerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CGLFitlerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CGLFitlerDoc::SetSearchContent(const CString& value)
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

// CGLFitlerDoc ���

#ifdef _DEBUG
void CGLFitlerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGLFitlerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGLFitlerDoc ����
BOOL CGLFitlerDoc::OpenFile(const CString & fileName)
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