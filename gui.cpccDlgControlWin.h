
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


	void enable(const bool aState) { EnableWindow(m_hWnd, aState ); }

	///////////////////////////////////////////////////////////////////
	//		check box functions
	///////////////////////////////////////////////////////////////////

	void checkBoxSetChecked(const bool aState)  {  PostMessage(m_hWnd, BM_SETCHECK, aState ? BST_CHECKED : BST_UNCHECKED, 0L); 	}


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



	std::basic_string<TCHAR> comboBoxGetSelectedItem(void) const
	{
		//   Send CB_GETCURSEL message to get the index of the selected list item.
		int ItemIndex = SendMessage(m_hWnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		//   Send CB_GETLBTEXT message to get the item.
		SendMessage( m_hWnd, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return ListItem;
	}


	void comboBoxAddVaues(const std::vector<std::basic_string<TCHAR>> &itemList, const TCHAR *preselectedValue) const
	{
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
class cpccDlgControlWinComboBox : public cpccDlgControlWin
{
public:
	cpccDlgControlWin(const HWND aWnd) : m_hWnd(aWnd)
	{ }

	cpccDlgControlWin(const HWND aParentWnd, const int aControlID) :
		m_hWnd(GetDlgItem(aParentWnd, aControlID))
	{ }
};
*/