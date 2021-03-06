
// ShakeFiguresView.cpp : implementation of the CShakeFiguresView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ShakeFigures.h"
#endif

#include "ShakeFiguresDoc.h"
#include "ShakeFiguresView.h"
#include "ShapesManager.h"
#include "ShapeSettingsDlg.h"
#include "ShapesCountSelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CShakeFiguresView

IMPLEMENT_DYNCREATE(CShakeFiguresView, CView)

BEGIN_MESSAGE_MAP(CShakeFiguresView, CView)
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_COMMAND(ID_SETTINGS_SHAPESETTINGS, &CShakeFiguresView::OnSettingsShapesettings)
	ON_COMMAND(ID_SETTINGS_SHAPESCOUNT, &CShakeFiguresView::OnSettingsShapescount)
END_MESSAGE_MAP()

// CShakeFiguresView construction/destruction
// the possible next borders

CShakeFiguresView::CShakeFiguresView()
	: m_bTimerInProgress(false)
	, m_flagShapesSwitch(true)
	, m_pShapesManager(nullptr)
	, m_nMilliseconds(500)
	, m_nShapesCount(4)
{
	//m_colorsArray = { RGB(255, 0, 0), RGB(0, 255, 0),RGB(0, 0, 255) };	
}

CShakeFiguresView::~CShakeFiguresView()
{
	delete m_pShapesManager;
}

BOOL CShakeFiguresView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CShakeFiguresView drawing

void CShakeFiguresView::OnDraw(CDC* /*pDC*/)
{
	CShakeFiguresDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CShakeFiguresView diagnostics

#ifdef _DEBUG
void CShakeFiguresView::AssertValid() const
{
	CView::AssertValid();
}

void CShakeFiguresView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShakeFiguresDoc* CShakeFiguresView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShakeFiguresDoc)));
	return (CShakeFiguresDoc*)m_pDocument;
}
#endif //_DEBUG


// CShakeFiguresView message handlers


void CShakeFiguresView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER)
	{
		MoveShapes();
	}

	CView::OnTimer(nIDEvent);
}

void CShakeFiguresView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CRect rc;	
	GetClientRect(&rc);
	CPoint center_point((int)rc.right / 2, (int)rc.bottom / 2);

	CRect moveArea(rc.left + SHAPE_WIDTH / 2, rc.top + SHAPE_HEIGHT / 2,
					rc.right - SHAPE_WIDTH / 2, rc.bottom - SHAPE_HEIGHT / 2);
	m_pShapesManager = new CShapesManager(this, center_point, moveArea, m_nShapesCount, SHAPE_WIDTH, SHAPE_HEIGHT);

	m_pShapesManager->InitalizeShapesPositions(SHAPE_TYPE::rectangle);
}


void CShakeFiguresView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	ASSERT(m_pShapesManager);

	m_pShapesManager->SetDCpointer(&dc);

	m_pShapesManager->Draw();	
}

void CShakeFiguresView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_bTimerInProgress = !m_bTimerInProgress;
	if (m_bTimerInProgress)
		SetTimer(ID_TIMER, m_nMilliseconds, NULL);
	else
		KillTimer(ID_TIMER);

	CView::OnRButtonDown(nFlags, point);
}


void CShakeFiguresView::OnLButtonDown(UINT nFlags, CPoint point)
{
	ASSERT(m_pShapesManager);
	
	if (m_pShapesManager->isPointInRectangle(point))
	{
		m_flagShapesSwitch = !m_flagShapesSwitch;

		m_pShapesManager->InitalizeShapesPositions(
			(m_flagShapesSwitch)
			? SHAPE_TYPE::rectangle
			: SHAPE_TYPE::ellipse);

		Invalidate();
	}
	

	CView::OnLButtonDown(nFlags, point);
}

void CShakeFiguresView::MoveShapes()
{
	ASSERT(m_pShapesManager);

	m_pShapesManager->MoveShapes();

	Invalidate();
}

void CShakeFiguresView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (!m_pShapesManager) return;

	CRect rc;
	GetClientRect(&rc);
	
	CRect moveAreaRect(rc.left + m_pShapesManager->GetShapesWidth() / 2, rc.top + m_pShapesManager->GetShapesHeight() / 2,
							rc.right - m_pShapesManager->GetShapesWidth() / 2, rc.bottom - m_pShapesManager->GetShapesHeight() / 2);

	if (m_pShapesManager) 
	{
		m_pShapesManager->SetShapesMoveArea(moveAreaRect);

		m_pShapesManager->RearangeShapes();
	}		

	Invalidate();
}

void CShakeFiguresView::OnSettingsShapesettings()
{
	ChangeSettings();
}

void CShakeFiguresView::ChangeSettings()
{
	CShapeSettingsDlg dlg(this);

	if (IDOK == dlg.DoModal())
	{
		auto nShapeSize = dlg.GetShapeSize();
		auto nMoveSpeed = dlg.GetMoveSpeed();
		auto nMiliseconds = dlg.GetMiliseconds();

		if (m_pShapesManager)
		{
			m_nMilliseconds = nMiliseconds;
			m_pShapesManager->SetShapesWidth(nShapeSize);
			m_pShapesManager->SetShapesHeight(nShapeSize);
			m_pShapesManager->SetShapesSpeed(nMoveSpeed);

			CRect rc;
			GetClientRect(&rc);

			CRect rc_move(rc.left + m_pShapesManager->GetShapesWidth() / 2, rc.top + m_pShapesManager->GetShapesHeight() / 2, 
							rc.right - m_pShapesManager->GetShapesWidth(), rc.bottom - m_pShapesManager->GetShapesHeight());
			m_pShapesManager->SetMoveAreaRect(rc_move);
			m_pShapesManager->SetShapesMoveArea(rc_move);

			m_pShapesManager->RearangeShapes();			

			if (m_bTimerInProgress) 
			{
				m_bTimerInProgress = false;
				KillTimer(ID_TIMER);
				SetTimer(ID_TIMER, m_nMilliseconds, NULL);
			}

			Invalidate();
		}
	}
}

void CShakeFiguresView::OnSettingsShapescount()
{
	CShapesCountSelDlg dlg(this);
	if (IDOK == dlg.DoModal())
	{
		m_nShapesCount = dlg.GetSelCount();
		m_pShapesManager->SetShapesCount(m_nShapesCount);
		m_pShapesManager->InitalizeShapesPositions(SHAPE_TYPE::rectangle);

		CRect rc;
		GetClientRect(&rc);

		CRect rc_move(rc.left + 10, rc.top + 20,
			rc.right - m_pShapesManager->GetShapesWidth(), rc.bottom - m_pShapesManager->GetShapesHeight());
		m_pShapesManager->SetMoveAreaRect(rc_move);
		m_pShapesManager->SetShapesMoveArea(rc_move);

		m_pShapesManager->RearangeShapes();
		Invalidate();
	}
}
