/*
noMeiryoUI (C) 2005,2012-2022 Tatsuhiko Shoji
The sources for noMeiryoUI are distributed under the MIT open source license
*/
#pragma once

#include <vector>
#include <windows.h>
#include "Resource.h"
#include "basedialog.h"
#include "TwrCombobox.h"
#include "TwrCheckbox.h"

/** �X�^�C���̏�� */
struct TypeInfo {
	TCHAR typeName[128];
	LOGFONT logFont;
};

/** �����Z�b�g�ƑΉ�����X�^�C���̏�� */
struct CharsetInfo {
	int charset;
	std::vector<struct TypeInfo> fonts;
};

/** �t�H���g�̏�� */
struct FontInfo {
	LOGFONT logFont;
	// std::vector<struct CharsetInfo> charsetList;
	TCHAR dispName[32];
	TCHAR fullName[LF_FULLFACESIZE];
};

class FontSel :
	public BaseDialog
{
private:
	TwrCombobox *m_fontNameList;
	TwrCombobox *m_fontSizeList;
	TwrCombobox *m_ChersetList;
	TwrCombobox *m_styleList;
	TwrCheckbox* m_bold;
	TwrCheckbox* m_italic;
	TwrCheckbox *m_underline;
	TwrCheckbox *m_strike;

	HFONT displayFont;

	LOGFONT selectedFont;
	LOGFONT *previousFont;
	double m_point;
	bool m_fontonly;

	void adjustPosition(void);
	void applyResource();
	void setCharset(void);
	void setStyle();
	INT_PTR onOK(void);
	void disableWithoutFontselect(void);

protected:
	INT_PTR OnInitDialog();
	INT_PTR OnCommand(WPARAM wParam);

public:
	FontSel(HWND parent, int resource, bool fontOnly = false);
	virtual ~FontSel(void);

	LOGFONT getSelectedFont() {
		return selectedFont;
	}
	void setPreviousFont(LOGFONT *item) {
		previousFont = item;
	}
	void setNoMeiryoUI();
	void setNoTahoma();

	/**
	 * �|�C���g�����擾����B
	 * 
	 * @return �I�����ꂽ�|�C���g��
	 */
	double getPoint(void) {
		return m_point;
	}

	/**
	 * �|�C���g����ݒ肷��B(�����\���p)
	 * 
	 * @param point �|�C���g��
	 */
	void setPoint(double point) {
		m_point = point;
	}

};

void copyTypeInfo(TypeInfo& typeInfo, ENUMLOGFONTEX* lpelfe);
