
/*  *****************************************
 *  File:		gui.cpccDlgControlWin.h
 *	Purpose:	Portable (cross-platform), light-weight, win32 dialog controls 
 *	*****************************************
 *  Library:	Cross Platform C++ Classes (cpcc)
 *  Copyright: 	2018 StarMessage software.
 *  License: 	Free for opensource projects.
 *  			Commercial license for closed source projects.
 *	Web:		http://www.StarMessageSoftware.com
 *  Download:	https://github.com/starmessage/cpcc
 *	email:		sales -at- starmessage.info
 *	*****************************************
 */
 
 
#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <tchar.h>



class cpccWinPaintstructAuto
{
private:
	HWND		m_hWnd;
	PAINTSTRUCT ps;

public:
	explicit cpccWinPaintstructAuto(const HWND aHwnd): m_hWnd(aHwnd) { BeginPaint(aHwnd, &ps);  }
	
	virtual ~cpccWinPaintstructAuto(void) { EndPaint(m_hWnd, &ps);  }

	inline const HDC dc(void) const { return ps.hdc;  }
};



/// C++ wrapper class for WIN32 controls
/// Provides methods to replace message sending to the HWND
class cpccDlgControlWin
{
protected:
    HWND m_hWnd;
    
public:
	cpccDlgControlWin(void) : m_hWnd(0)
	{ }

    cpccDlgControlWin(const HWND aWnd): m_hWnd(aWnd)
    { }
    
    cpccDlgControlWin(const HWND aParentWnd, const int aControlID):
        m_hWnd( GetDlgItem(aParentWnd, aControlID) )
    { }
    
    void assignHandle(const HWND aWnd) {  m_hWnd=aWnd; }
	void assignHandle(const HWND aParentWnd, const int aControlID) {  m_hWnd=GetDlgItem(aParentWnd, aControlID); }


	void setEnabled(const bool aState) { EnableWindow(m_hWnd, aState ); }
	BOOL isEnabled(void) { return IsWindowEnabled(m_hWnd); }

	void setVisibility(const bool aVisibility) { ShowWindow(m_hWnd, aVisibility? SW_SHOW: SW_HIDE); }

	void setControlStyle(const LONG dwNewStyle) const 
	{ 
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644898(v=vs.85).aspx
		// To write code that is compatible with both 32-bit and 64-bit versions of Windows, use SetWindowLongPtr. 
		// When compiling for 32-bit Windows, SetWindowLongPtr is defined as a call to the SetWindowLong function
		SetWindowLongPtr(m_hWnd, GWL_STYLE, dwNewStyle);
	}

	void setControlExtendedStyle(const LONG dwNewStyle) const
	{
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms644898(v=vs.85).aspx
		// To write code that is compatible with both 32-bit and 64-bit versions of Windows, use SetWindowLongPtr. 
		// When compiling for 32-bit Windows, SetWindowLongPtr is defined as a call to the SetWindowLong function
		SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, dwNewStyle);
	}

	LONG getControlExtendedStyle(void) const { return GetWindowLongPtr(m_hWnd, GWL_EXSTYLE); }

	LONG getControlStyle(void) const { return GetWindowLongPtr(m_hWnd, GWL_STYLE); }

	void removeControlStyle(const LONG dwStyleToRemove) const
	{
		LONG style = getControlStyle();
		setControlStyle(style & (~dwStyleToRemove));
	}

	void removeControlExtendedStyle(const LONG dwStyleToRemove) const
	{
		LONG style = getControlExtendedStyle();
		setControlExtendedStyle(style & (~dwStyleToRemove));
	}

	/*
	void richEditSetBackgroundColor(const COLORREF crColor) const
	{
		//	wParam
		//	Specifies whether to use the system color.If this parameter is a nonzero value, the background is set to the window background system color.Otherwise, the background is set to the specified color.
		//	lParam
		//	A COLORREF structure specifying the color if wParam is zero.To generate a COLORREF, use the RGB macro.

		SendMessage(m_hWnd, EM_SETBKGNDCOLOR, 0, crColor);
	}
	*/

	BOOL getWindowRect_absolute(LPRECT aRect)
	{
		// The dimensions are given in screen coordinates that are relative to the upper-left corner of the screen.
		return GetWindowRect(m_hWnd, aRect);
	}

	void getWindowRect_relative( LPRECT aRect)
	{
		// The dimensions are given in screen coordinates that are relative to the upper-left corner of the screen.
		GetWindowRect(m_hWnd, aRect);
		// first parameter: A handle to the window from which points are converted. 
		// If this parameter is NULL or HWND_DESKTOP, the points are presumed to be in screen coordinates.
		MapWindowPoints(HWND_DESKTOP, GetParent(m_hWnd), (LPPOINT)aRect, 2);
	}

	///////////////////////////////////////////////////////////////////
	//		check box functions
	///////////////////////////////////////////////////////////////////

	void checkBoxSetChecked(const bool aState)  {  SendMessage(m_hWnd, BM_SETCHECK, aState ? BST_CHECKED : BST_UNCHECKED, 0L); 	}


	bool checkBoxGetChecked(void) const
	{
		// The return value from a button created with the BS_AUTOCHECKBOX, BS_AUTORADIOBUTTON, BS_AUTO3STATE, BS_CHECKBOX, BS_RADIOBUTTON, or BS_3STATE 
		// style can be one of the following.
		//LRESULT chkState = SendMessage(m_hWnd, BM_GETCHECK, 0, 0L);
		return (SendMessage(m_hWnd, BM_GETCHECK, 0, 0L) == BST_CHECKED);
	}


	///////////////////////////////////////////////////////////////////
	//		combo box functions
	///////////////////////////////////////////////////////////////////

	


	void comboBoxRemoveItems(void) const {	SendMessage(m_hWnd, (UINT)CB_RESETCONTENT, 0, 0L); 	}


	std::basic_string<TCHAR> comboBoxGetSelectedItem(void) const
	{
		// maybe the same as a simple call to GetWindowText() ?

		//   Send CB_GETCURSEL message to get the index of the selected list item.
		int ItemIndex = SendMessage(m_hWnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		//   Send CB_GETLBTEXT message to get the item.
		SendMessage( m_hWnd, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return ListItem;
	}


	void comboBoxAddVaues(const std::vector<std::basic_string<TCHAR>> &itemList, const TCHAR *preselectedValue) const
	{
		// ComboBox style: CBS_SORT : Automatically sorts strings added to the list box.
		// CBS_SORT is the creation time style that is style that control use only during self creation.
		// Changing such styles after creation will not change control mode.
		// removeControlStyle(CBS_SORT);

		// https://stackoverflow.com/questions/10047846/c-win32-adding-values-to-combobox
		// https://msdn.microsoft.com/en-us/library/windows/desktop/hh298364(v=vs.85).aspx
		int preselectedIndex = -1;
		int i = 0;
		for (std::vector<std::basic_string<TCHAR>>::const_iterator it = itemList.begin(); it != itemList.end(); ++it, ++i)
		{
			// Add string to combobox.
			SendMessage(m_hWnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)(it->c_str()));
			if (it->compare(preselectedValue) == 0)
				preselectedIndex = i;
		}

		// Send the CB_SETCURSEL message to display an initial item in the selection field  
		// wParam:
		// 	Specifies the zero - based index of the string to select.If this parameter is –1, any current selection in the list is removed and the edit control is cleared.
		PostMessage(m_hWnd, CB_SETCURSEL, (WPARAM)preselectedIndex, (LPARAM)0);
	}
};





/*
void selectFont(const HWND hWndOwner, DWORD &aColor, cpcc_string &fontName, float &fontSize)
{
	CHOOSEFONT cf;            // common dialog box structure
	LOGFONT lf;        // logical font structure
					   // DWORD rgbCurrent;  // current text color
					   // HFONT hfont, hfontPrev;
					   // DWORD rgbPrev = aColor;

					   // Initialize CHOOSEFONT
					   //	https://msdn.microsoft.com/en-us/library/windows/desktop/ms646832(v=vs.85).aspx
	ZeroMemory(&cf, sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = hWndOwner;
	cf.lpLogFont = &lf;
	cf.rgbColors = aColor;
	cf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_NOSCRIPTSEL | CF_NOSTYLESEL;

	if (ChooseFont(&cf) == TRUE)
	{
		aColor = cf.rgbColors;
		fontSize = cf.iPointSize / 10.0f;

	}
}
*/