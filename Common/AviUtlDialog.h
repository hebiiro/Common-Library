#pragma once

#include "Dialog.h"
#include "AviUtlWindow.h"

//---------------------------------------------------------------------

#define GET_THIS(theClass, member) ((theClass*)((BYTE*)this - offsetof(theClass, member)))

//---------------------------------------------------------------------

template<typename T>
class AviUtlDialog : public AviUtlWindow
{
public:

	class Dialog : public T
	{
	public:

		virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			AviUtlDialog<T>* p = GET_THIS(AviUtlDialog<T>, m_dialog);

			return p->onDlgProc(hwnd, message, wParam, lParam);
		}

		INT_PTR defDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}

	} m_dialog;

	AviUtlDialog()
	{
	}

	virtual ~AviUtlDialog()
	{
		destroy();
	}

	BOOL create(DWORD exStyle, LPCTSTR windowName, DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPCTSTR resourceName)
	{
		if (!AviUtlWindow::create(exStyle, windowName, style, x, y, w, h, parent, menu, instance))
			return FALSE;

		if (!m_dialog.create(instance, resourceName, m_hwnd))
			return FALSE;

		{
			DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);
			DWORD exStyle = ::GetWindowLong(m_hwnd, GWL_EXSTYLE);
			HMENU menu = ::GetMenu(m_hwnd);

			RECT rc; ::GetWindowRect(m_dialog, &rc);
			::AdjustWindowRectExForDpi(&rc, style, !!menu, exStyle, ::GetDpiForWindow(m_hwnd));
			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;
			::SetWindowPos(m_hwnd, 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}

		calcLayout();

		return TRUE;
	}

	BOOL calcLayout()
	{
		if (::GetWindowLong(m_hwnd, GWL_STYLE) & WS_THICKFRAME)
		{
			RECT rc; ::GetClientRect(m_hwnd, &rc);
			int x = rc.left;
			int y = rc.top;
			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;
			::SetWindowPos(m_dialog, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	LRESULT send(UINT id, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return ::SendDlgItemMessage(m_dialog, id, message, wParam, lParam);
	}

	virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_SIZE:
			{
				calcLayout();

				break;
			}
		}

		return __super::onWndProc(hwnd, message, wParam, lParam);
	}

	virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return m_dialog.defDlgProc(hwnd, message, wParam, lParam);
	}
};

//---------------------------------------------------------------------
