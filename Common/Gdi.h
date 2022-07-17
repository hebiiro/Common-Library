#pragma once

//--------------------------------------------------------------------

class ClientDC
{
public:

	HWND m_hwnd = 0;
	HDC m_dc = 0;

public:

	ClientDC(HWND hwnd)
	{
		m_hwnd = hwnd;
		m_dc = ::GetDC(m_hwnd);
	}

	~ClientDC()
	{
		if (m_dc)
			::ReleaseDC(m_hwnd, m_dc);
	}

	operator HDC()
	{
		return m_dc;
	}
};

class PaintDC
{
public:

	PAINTSTRUCT m_ps = {};
	HWND m_hwnd = 0;
	HDC m_dc = 0;

public:

	PaintDC(HWND hwnd)
	{
		m_hwnd = hwnd;
		m_dc = ::BeginPaint(m_hwnd, &m_ps);
	}

	~PaintDC()
	{
		if (m_dc)
			::EndPaint(m_hwnd, &m_ps);
	}

	operator HDC()
	{
		return m_dc;
	}
};

class UxDC
{
public:

	BP_PAINTPARAMS m_pp = { sizeof(m_pp) };
	HDC m_dc = 0;
	HPAINTBUFFER m_pb = 0;

public:

	UxDC(HDC dc, LPCRECT rc, BP_BUFFERFORMAT format = BPBF_COMPATIBLEBITMAP)
	{
		m_pb = ::BeginBufferedPaint(dc, rc, BPBF_COMPATIBLEBITMAP, &m_pp, &m_dc);
	}

	~UxDC()
	{
		if (m_pb)
			::EndBufferedPaint(m_pb, TRUE);
	}

	BOOL isValid()
	{
		return !!m_pb;
	}

	operator HDC()
	{
		return m_dc;
	}
};

//--------------------------------------------------------------------

template<class T>
class GdiObj
{
public:

	T m_gdiobj = 0;

public:

	GdiObj(T gdiobj)
	{
		m_gdiobj = gdiobj;
	}

	~GdiObj()
	{
		::DeleteObject(m_gdiobj);
	}

	operator T()
	{
		return m_gdiobj;
	}
};

class GdiObjSelector
{
public:

	HDC m_dc = 0;
	HGDIOBJ m_gdiobj = 0;

public:

	GdiObjSelector(HDC dc, HGDIOBJ gdiobj)
	{
		m_gdiobj = ::SelectObject(dc, gdiobj);
	}

	~GdiObjSelector()
	{
		::SelectObject(m_dc, m_gdiobj);
	}
};

//--------------------------------------------------------------------
