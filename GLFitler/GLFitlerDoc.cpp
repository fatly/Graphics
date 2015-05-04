
// GLFitlerDoc.cpp : CGLFitlerDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


// CGLFitlerDoc 构造/析构

CGLFitlerDoc::CGLFitlerDoc()
{
	// TODO:  在此添加一次性构造代码

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

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CGLFitlerDoc 序列化

void CGLFitlerDoc::Serialize(CArchive& ar)
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
void CGLFitlerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CGLFitlerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
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

// CGLFitlerDoc 诊断

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


// CGLFitlerDoc 命令
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