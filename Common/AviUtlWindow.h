#pragma once

//---------------------------------------------------------------------

class AviUtlWindow
{
public:

	static constexpr LPCTSTR PROP_NAME = _T("AviUtlWindow.This");

	HWND m_hwnd = 0;

	operator HWND() const { return m_hwnd; }

	AviUtlWindow()
	{
	}

	virtual ~AviUtlWindow()
	{
		destroy();
	}

	virtual BOOL create(DWORD exStyle, LPCTSTR windowName, DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance)
	{
		WNDCLASS wc = {};
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.hCursor = ::LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
		wc.lpfnWndProc = wndProc;
		wc.hInstance = instance;
		wc.lpszClassName = _T("AviUtl");
		::RegisterClass(&wc);

		m_hwnd = ::CreateWindowEx(exStyle, _T("AviUtl"), windowName, style, x, y, w, h, parent, menu, instance, this);
		return !!m_hwnd;
	}

	virtual BOOL destroy()
	{
		if (!m_hwnd) return FALSE;
		::DestroyWindow(m_hwnd), m_hwnd = 0;
		return TRUE;
	}

	virtual BOOL show()
	{
		return ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
	}

	virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
/*		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC dc = ::BeginPaint(hwnd, &ps);
				RECT rc = ps.rcPaint;

				HBRUSH brush = (HBRUSH)::SendMessage(hwnd, WM_CTLCOLORDLG, 0, 0);
				::FillRect(dc, &rc, brush);

				::EndPaint(hwnd, &ps);
				return 0;
			}*/
		case WM_CLOSE:
			{
				if (::IsWindowVisible(hwnd))
					::ShowWindow(hwnd, SW_HIDE);
				else
					::ShowWindow(hwnd, SW_SHOW);

				return  0;
			}
		}

		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	static AviUtlWindow* getThis(HWND hwnd)
	{
		return (AviUtlWindow*)::GetProp(hwnd, PROP_NAME);
	}

	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		AviUtlWindow* window = getThis(hwnd);

		if (window) return window->onWndProc(hwnd, message, wParam, lParam);

		switch (message)
		{
		case WM_NCCREATE:
			{
				CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
				::SetProp(hwnd, PROP_NAME, cs->lpCreateParams);
				break;
			}
		}

		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}
};

//---------------------------------------------------------------------
