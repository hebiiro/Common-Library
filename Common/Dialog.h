#pragma once

//---------------------------------------------------------------------

class Dialog
{
public:

	static constexpr LPCTSTR PROP_NAME = _T("Dialog.This");

	HWND m_hdlg = 0;
	int m_result = 0;

	operator HWND() const { return m_hdlg; }

	Dialog()
	{
	}

	Dialog(HINSTANCE instance, LPCTSTR name, HWND parent)
	{
		create(instance, name, parent);
	}

	virtual ~Dialog()
	{
		destroy();
	}

	virtual BOOL create(HINSTANCE instance, LPCTSTR name, HWND parent)
	{
		m_hdlg = ::CreateDialogParam(instance, name, parent, dlgProc, (LPARAM)this);
		return !!m_hdlg;
	}

	virtual BOOL destroy()
	{
		if (!m_hdlg) return FALSE;
		::DestroyWindow(m_hdlg), m_hdlg = 0;
		return TRUE;
	}

	virtual BOOL close(int result)
	{
		if (!m_hdlg) return FALSE;
		::ShowWindow(m_hdlg, SW_HIDE);
		::PostMessage(m_hdlg, WM_NULL, 0, 0);
		m_result = result;
		return TRUE;
	}

	virtual int doModal()
	{
		HWND parent = ::GetWindow(m_hdlg, GW_OWNER);
		::EnableWindow(parent, FALSE);
		::ShowWindow(m_hdlg, SW_SHOW);

		MSG msg = {};
		while (::IsWindowVisible(m_hdlg) && ::GetMessage(&msg, 0, 0, 0))
		{
			if (!::IsDialogMessage(m_hdlg, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}

		::EnableWindow(parent, TRUE);
		::SetForegroundWindow(parent);

		return m_result;
	}

	virtual void onOK()
	{
		close(IDOK);
	}

	virtual void onCancel()
	{
		close(IDCANCEL);
	}

	virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
			{
				switch (LOWORD(wParam))
				{
				case IDOK:
					{
						onOK();
						return TRUE;
					}
				case IDCANCEL:
					{
						onCancel();
						return TRUE;
					}
				}

				break;
			}
		}

		return FALSE;
	}

	static Dialog* getThis(HWND  hwnd)
	{
		return (Dialog*)::GetProp(hwnd, PROP_NAME);
	}

	static INT_PTR CALLBACK dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Dialog* dialog = getThis(hwnd);

		if (dialog) return dialog->onDlgProc(hwnd, message, wParam, lParam);

		switch (message)
		{
		case WM_INITDIALOG:
			{
				::SetProp(hwnd, PROP_NAME, (HANDLE)lParam);
				break;
			}
		}

		return FALSE;
	}
};

class NoCloseDialog : public Dialog
{
public:

	virtual BOOL close(int result)
	{
		return FALSE;
	}
};

//---------------------------------------------------------------------
