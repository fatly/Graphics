
// GraphicsDoc.cpp : CGraphicsDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


// CGraphicsDoc 构造/析构

CGraphicsDoc::CGraphicsDoc()
{
	// TODO:  在此添加一次性构造代码
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

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CGraphicsDoc 序列化

void CGraphicsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CGraphicsDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
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

// 搜索处理程序的支持
void CGraphicsDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
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

// CGraphicsDoc 诊断

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


// CGraphicsDoc 命令
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
