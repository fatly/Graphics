
// HSVFilterDoc.cpp : CHSVFilterDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "HSVFilter.h"
#endif

#include "HSVFilterDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CHSVFilterDoc

IMPLEMENT_DYNCREATE(CHSVFilterDoc, CDocument)

BEGIN_MESSAGE_MAP(CHSVFilterDoc, CDocument)
END_MESSAGE_MAP()


// CHSVFilterDoc 构造/析构

CHSVFilterDoc::CHSVFilterDoc()
{
	// TODO:  在此添加一次性构造代码

}

CHSVFilterDoc::~CHSVFilterDoc()
{
	int count = m_BitmapArray.size();
	for (int i = 0; i < count; i++)
	{
		Bitmap* p = m_BitmapArray[i];
		delete p;
	}
}

BOOL CHSVFilterDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CHSVFilterDoc 序列化

void CHSVFilterDoc::Serialize(CArchive& ar)
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
void CHSVFilterDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CHSVFilterDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CHSVFilterDoc::SetSearchContent(const CString& value)
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

// CHSVFilterDoc 诊断

#ifdef _DEBUG
void CHSVFilterDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHSVFilterDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CHSVFilterDoc 命令
BOOL CHSVFilterDoc::OpenFile(const CString & fileName)
{
	USES_CONVERSION;
	const char* path = W2A(fileName);
	Bitmap* pBitmap = new Bitmap(path);

	if (m_BitmapArray.size() > 0)
	{
		Bitmap* pTemp = m_BitmapArray[0];
		delete pTemp;
		m_BitmapArray.remove(0);
	}

	m_BitmapArray.push_back(pBitmap);

	return TRUE;
}