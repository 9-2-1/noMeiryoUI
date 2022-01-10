/*
noMeiryoUI (C) 2005,2012-2022 Tatsuhiko Shoji
The sources for noMeiryoUI are distributed under the MIT open source license
*/
#include "FontSel.h"
#include "util.h"

#include <algorithm>
#include <functional>

/** �t�H���g�̃��X�g */
std::vector<struct FontInfo> fontList;
/** �����Z�b�g�ƑΉ�����X�^�C���̃��X�g */
std::vector<struct CharsetInfo> charsetList;

static bool noMeiryoUI = false;
static bool noTahoma = false;

bool operator<(const FontInfo& left, const FontInfo& right)
{
	return (_tcscmp(left.dispName, right.dispName) > 0);
}

bool operator>(const FontInfo& left, const FontInfo& right)
{
	return (_tcscmp(left.dispName, right.dispName) < 0);
}

/**
 * TypeInfo��ENUMLOGFONTEX��������R�s�[����B 
 * 
 * @param typeInfo �R�s�[��t�H���g���
 * @param lpelfe �R�s�[��ENUMLOGFONTEX
 */
void copyTypeInfo(TypeInfo& typeInfo, ENUMLOGFONTEX* lpelfe)
{
	TCHAR oldStyle[128];
	TCHAR newStyle[128];

	if (lpelfe->elfStyle[0] != _T('\0')) {
		_tcscpy(oldStyle, lpelfe->elfStyle);
	} else {
		_tcscpy(oldStyle, _T("Regular"));
	}

	// �����X�^�C���̊e����ւ̒u������
	strreplace(newStyle, oldStyle, _T("Regular"), langResource[DLG_STYLE_NORMAL].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("SemiBold"), langResource[DLG_STYLE_SEMI_BOLD].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Semibold"), langResource[DLG_STYLE_SEMI_BOLD].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Semi-bold"), langResource[DLG_STYLE_SEMI_BOLD].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Bold Italic"), langResource[DLG_STYLE_BOLD_ITALIC].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("ExtraLight"), langResource[DLG_STYLE_EXTRA_LIGHT].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("SemiLight"), langResource[DLG_STYLE_SEMI_LIGHT].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Bold"), langResource[DLG_STYLE_BOLD].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Italic"), langResource[DLG_STYLE_ITALIC].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Medium"), langResource[DLG_STYLE_MEDIUM].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Book"), langResource[DLG_STYLE_NORMAL].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Light"), langResource[DLG_STYLE_LIGHT].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Oblique"), langResource[DLG_STYLE_OBLIQUE].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	strreplace(newStyle, oldStyle, _T("Slanted"), langResource[DLG_STYLE_SLANTED].c_str(), 128);
	_tcscpy(oldStyle, newStyle);

	_tcscpy(typeInfo.typeName, newStyle);
	typeInfo.logFont = lpelfe->elfLogFont;
}

/**
 * Charset�ɑ΂���EnumFontFamiliesEx�̃R�[���o�b�N
 *
 * @param lpelfe �_���I�ȃt�H���g�f�[�^
 * @param lpntme �����I�ȃt�H���g�f�[�^
 * @param FontType �t�H���g�̎��
 * @param lParam �A�v���P�[�V������`�̃f�[�^
 * @return 0:�񋓂𒆎~���� 1:���̃t�H���g��񋓂���B
 */
int CALLBACK EnumFontCharsetProc(
	ENUMLOGFONTEX* lpelfe,    // �_���I�ȃt�H���g�f�[�^
	NEWTEXTMETRICEX* lpntme,  // �����I�ȃt�H���g�f�[�^
	DWORD FontType,           // �t�H���g�̎��
	LPARAM lParam             // �A�v���P�[�V������`�̃f�[�^
)
{
	struct TypeInfo fontInfo;
	copyTypeInfo(fontInfo, lpelfe);

	for (size_t i = 0; i < charsetList.size(); i++) {

		// ���������Z�b�g��?
		if (charsetList[i].charset == lpelfe->elfLogFont.lfCharSet) {
			// ���������Z�b�g�ŕ����X�^�C���݈̂Ⴄ
			bool found = false;
			for (size_t j = 0; j < charsetList[i].fonts.size(); j++) {
				if (!_tcscmp(charsetList[i].fonts[j].typeName, fontInfo.typeName)) {
					found = true;
				}
			}
			if (!found) {
				charsetList[i].fonts.push_back(fontInfo);
			}
			return 1;
		}
	}

	// �����Z�b�g���قȂ�
	struct CharsetInfo charset;
	charset.charset = lpelfe->elfLogFont.lfCharSet;
	charset.fonts.push_back(fontInfo);
	charsetList.push_back(charset);

	return 1;

}

/**
 * Charset�ɑ΂���t�H���g���擾����
 * 
 * @param lf �t�H���g����Charset�̓������t�H���g���
 */
int getCharsetFont(LOGFONT *lf)
{
	lf->lfPitchAndFamily = 0;
	lf->lfCharSet = DEFAULT_CHARSET;

	charsetList.clear();

	HDC hDC;
	hDC = GetDC(GetDesktopWindow());

	EnumFontFamiliesEx(
		hDC,
		lf,
		(FONTENUMPROC)EnumFontCharsetProc,
		(LPARAM)0,
		0
	);

	return 0;
}

/**
 * EnumFontFamiliesEx�̃R�[���o�b�N
 *
 * @param lpelfe �_���I�ȃt�H���g�f�[�^
 * @param lpntme �����I�ȃt�H���g�f�[�^
 * @param FontType �t�H���g�̎��
 * @param lParam �A�v���P�[�V������`�̃f�[�^
 * @return 0:�񋓂𒆎~���� 1:���̃t�H���g��񋓂���B
 */
int CALLBACK EnumFontFamExProc(
  ENUMLOGFONTEX *lpelfe,    // �_���I�ȃt�H���g�f�[�^
  NEWTEXTMETRICEX *lpntme,  // �����I�ȃt�H���g�f�[�^
  DWORD FontType,           // �t�H���g�̎��
  LPARAM lParam             // �A�v���P�[�V������`�̃f�[�^
)
{
	std::vector<int> charset;
	int fonts;
	struct FontInfo fontInfo;
	TCHAR dispBuf[32];

	if (lpelfe->elfLogFont.lfFaceName[0] == _T('@')) {
		// �c�����t�H���g�͔�΂��B
		return 1;
	}
	if (noMeiryoUI) {
		if (!_tcscmp(_T("Meiryo UI"), lpelfe->elfLogFont.lfFaceName)) {
			return 1;
		}
	}

	if (noTahoma) {
		if (!_tcscmp(_T("Tahoma"), lpelfe->elfLogFont.lfFaceName)) {
			return 1;
		}
	}

	fonts = fontList.size();

	for (int i = 0; i < fonts; i++) {
		// �������O��?
		if (!_tcscmp(fontList[i].logFont.lfFaceName, lpelfe->elfLogFont.lfFaceName)) {
			return 1;
		}
	}

	// ������Ȃ��ꍇ�͒ǉ�����B
	fontInfo.logFont = lpelfe->elfLogFont;
	_tcscpy(fontInfo.fullName, lpelfe->elfFullName);
	// fontInfo.charsetList.clear();

	_tcscpy(dispBuf, lpelfe->elfLogFont.lfFaceName);
	if (isKorean) {
		getKoreanFontName(dispBuf);
	}
	_tcscpy(fontInfo.dispName, dispBuf);
	fontList.push_back(fontInfo);

	return 1;
}

/**
 * �t�H���g���擾����B
 */
int getFont()
{
	LOGFONT lf;
	HDC hDC;

	fontList.clear();

	hDC = GetDC(GetDesktopWindow());

	memset(&lf, 0x00, sizeof(LOGFONT));
	lf.lfFaceName[0] = _T('\0');
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfPitchAndFamily = 0;

	EnumFontFamiliesEx(hDC,
		&lf,
		(FONTENUMPROC)EnumFontFamExProc,
		(LPARAM)0,
		0);

	std::sort(fontList.begin(), fontList.end(), std::greater<FontInfo>());

	return 0;
}

/**
 * �R���X�g���N�^
 */
FontSel::FontSel(HWND parent, int resource) : BaseDialog(parent, resource)
{
	displayFont = NULL;
	m_fontNameList = NULL;
	m_fontSizeList = NULL;
	m_ChersetList = NULL;
	m_styleList = NULL;
	m_bold = NULL;
	m_italic = NULL;
	m_underline = NULL;
	m_strike = NULL;
	previousFont = NULL;
	m_point = 0;
}

/**
 * �f�X�g���N�^
 */
FontSel::~FontSel(void)
{
	if (m_fontSizeList != NULL) {
		delete m_fontSizeList;
	}
	if (m_fontNameList != NULL) {
		delete m_fontNameList;
	}
	if (m_ChersetList != NULL) {
		delete m_ChersetList;
	}
	if (m_styleList != NULL) {
		delete m_styleList;
	}
	if (m_bold != NULL) {
		delete m_bold;
	}
	if (m_italic != NULL) {
		delete m_italic;
	}
	if (m_underline != NULL) {
		delete m_underline;
	}
	if (m_strike != NULL) {
		delete m_strike;
	}
	if (displayFont != NULL) {
		DeleteObject(displayFont);
	}
}

/**
 * �_�C�A���O����������
 */
INT_PTR FontSel::OnInitDialog()
{
	getFont();
	m_fontNameList = new TwrCombobox(::GetDlgItem(hWnd, IDC_COMBO_NAME));
	m_fontSizeList = new TwrCombobox(::GetDlgItem(hWnd, IDC_COMBO_SIZE));
	m_ChersetList =  new TwrCombobox(::GetDlgItem(hWnd, IDC_COMBO_CHARSET));
	m_styleList =  new TwrCombobox(::GetDlgItem(hWnd, IDC_COMBO_STYLE));
	m_bold = new TwrCheckbox(::GetDlgItem(hWnd, IDC_CHECK_BOLD));
	m_italic = new TwrCheckbox(::GetDlgItem(hWnd, IDC_CHECK_ITALIC));
	m_underline = new TwrCheckbox(::GetDlgItem(hWnd, IDC_CHECK_UNDERLINE));
	m_strike = new TwrCheckbox(::GetDlgItem(hWnd, IDC_CHECK_STRIKE));

	int fonts;

	fonts = fontList.size();
	for (int i = 0; i < fonts; i++) {
		m_fontNameList->addItem(fontList[i].dispName);
	}

	m_fontSizeList->addItem(_T("6"));
	m_fontSizeList->addItem(_T("7"));
	m_fontSizeList->addItem(_T("8"));
	m_fontSizeList->addItem(_T("9"));
	m_fontSizeList->addItem(_T("10"));
	m_fontSizeList->addItem(_T("11"));
	m_fontSizeList->addItem(_T("12"));
	m_fontSizeList->addItem(_T("13"));
	m_fontSizeList->addItem(_T("14"));
	m_fontSizeList->addItem(_T("15"));
	m_fontSizeList->addItem(_T("16"));
	m_fontSizeList->addItem(_T("17"));
	m_fontSizeList->addItem(_T("18"));
	m_fontSizeList->addItem(_T("19"));
	m_fontSizeList->addItem(_T("20"));
	m_fontSizeList->addItem(_T("21"));
	m_fontSizeList->addItem(_T("22"));
	m_fontSizeList->addItem(_T("23"));
	m_fontSizeList->addItem(_T("24"));
	m_fontSizeList->addItem(_T("26"));
	m_fontSizeList->addItem(_T("28"));
	m_fontSizeList->addItem(_T("36"));
	m_fontSizeList->addItem(_T("48"));
	m_fontSizeList->addItem(_T("72"));

	// �I������B
	if (previousFont != NULL) {
		// �t�H���g�T�C�Y
		int count = m_fontSizeList->getCount();
		int point = getFontPointInt(previousFont, this->getHwnd());
		int selection = 0;
		for (int i = 0; i < count; i++) {
			int itemSize = _tstoi(m_fontSizeList->getItem(i).c_str());
			if (point >= itemSize) {
				selection = i;
			}
		}
		if (selection > -1) {
			m_fontSizeList->setSelectedIndex(selection);
		}

		// �t�H���g�t�F�C�X
		for (int i = 0; i < fonts; i++) {
			if (!_tcscmp(fontList[i].dispName, previousFont->lfFaceName)) {
				m_fontNameList->setSelectedIndex(i);

				// �t�H���g�ɍ����������R�[�h�Z�b�g��ݒ肷��B
				setCharset();
				int charsetCount = charsetList.size();
				int charset = 0;
				for (int j = 0; j < charsetCount; j++) {
					if (charsetList[j].charset == previousFont->lfCharSet) {
						m_ChersetList->setSelectedIndex(j);
						charset = j;
					}
				}

				// �t�H���g�ɍ������X�^�C����ݒ肷��B
				setStyle();

				// �C�^���b�N
				if (previousFont->lfItalic) {
					m_italic->setChecked(true);
				}
				// ����
				if (previousFont->lfUnderline) {
					m_underline->setChecked(true);
				}
				// ��������
				if (previousFont->lfStrikeOut) {
					m_strike->setChecked(true);
				}

				break;
			}
		}
	}

	applyResource();
	adjustPosition();

	return (INT_PTR)FALSE;
}

/**
 * �E�C���h�E�ʒu��e�E�C���h�E�̒����ɒ�������B
 */
void FontSel::adjustPosition(void)
{
	RECT parentRect;

	GetWindowRect(getParent(), &parentRect);

	adjustCenter(parentRect, getParent(), this->hWnd);
}

/**
 * �e����ɑ΂��鏈���̕���
 *
 * @param wParam WPARAM
 * @return 0:���b�Z�[�W�����������B 0�ȊO:���b�Z�[�W���������Ȃ������B
 */
INT_PTR FontSel::OnCommand(WPARAM wParam)
{
	INT_PTR result;

	switch (LOWORD(wParam)) {
		case IDC_COMBO_NAME:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				setCharset();
				setStyle();
			}
			break;
		case IDC_COMBO_CHARSET:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				setStyle();
			}
			break;
		case IDOK:
			result = onOK();
			if (result == 0) {
				return result;
			}
	}
	return BaseDialog::OnCommand(wParam);

}

/**
 * ���\�[�X���e���ڂɐݒ肷��B
 */
void FontSel::applyResource()
{
	HDC hDC = GetDC(this->hWnd);

	displayFont = CreateFont(
		-MulDiv(APP_FONTSIZE, GetDeviceCaps(hDC, LOGPIXELSY), 72),
		0,
		0,
		0,
		FW_NORMAL,
		FALSE,
		FALSE,
		FALSE,
		_tstoi(langResource[70].c_str()),
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, // CLEARTYPE_QUALITY,
		FIXED_PITCH | FF_MODERN,
		langResource[0].c_str());

	ReleaseDC(this->hWnd, hDC);


	// �A�v���^�C�g��
	setText(langResource[27].c_str());

	setChildText(IDC_STATIC_NAME, langResource[28].c_str());
	setChildFont(IDC_STATIC_NAME, displayFont);
	setChildText(IDC_STATIC_STYLE, langResource[29].c_str());
	setChildFont(IDC_STATIC_STYLE, displayFont);
	setChildText(IDC_STATIC_SIZE, langResource[30].c_str());
	setChildFont(IDC_STATIC_SIZE, displayFont);

	setChildText(IDC_CHECK_BOLD, langResource[DLG_CHECK_BOLD].c_str());
	setChildFont(IDC_CHECK_BOLD, displayFont);
	setChildText(IDC_CHECK_ITALIC, langResource[DLG_CHECK_ITALIC].c_str());
	setChildFont(IDC_CHECK_ITALIC, displayFont);
	setChildText(IDC_CHECK_UNDERLINE, langResource[31].c_str());
	setChildFont(IDC_CHECK_UNDERLINE, displayFont);
	setChildText(IDC_CHECK_STRIKE, langResource[32].c_str());
	setChildFont(IDC_CHECK_STRIKE, displayFont);
	setChildText(IDC_STATIC_CHARSET, langResource[33].c_str());
	setChildFont(IDC_STATIC_CHARSET, displayFont);

	setChildText(IDOK, langResource[34].c_str());
	setChildFont(IDOK, displayFont);
	setChildText(IDCANCEL, langResource[35].c_str());
	setChildFont(IDCANCEL, displayFont);

	setChildFont(IDC_COMBO_NAME, displayFont);
	setChildFont(IDC_COMBO_STYLE, displayFont);
	setChildFont(IDC_COMBO_SIZE, displayFont);
	setChildFont(IDC_COMBO_CHARSET, displayFont);

}

/**
 * �I�������t�H���g�ɍ����������Z�b�g�̑I������ݒ肵�܂��B
 */
void FontSel::setCharset(void)
{
	int selected = m_fontNameList->getSelectedIndex();
	int initialIndex = 0;	// �����I��

	if (selected > -1) {
		// �I�������t�H���g�ɑΉ����������Z�b�g�ƃX�^�C�����擾����
		getCharsetFont(&fontList[selected].logFont);

		m_ChersetList->clear();
		int charsetCount = charsetList.size();
		for (int i = 0; i < charsetCount; i++) {
			switch (charsetList[i].charset) {
				case ANSI_CHARSET:
					m_ChersetList->addItem(langResource[36].c_str());
					break;
				case BALTIC_CHARSET:
					m_ChersetList->addItem(langResource[37].c_str());
					break;
				case CHINESEBIG5_CHARSET:
					m_ChersetList->addItem(langResource[38].c_str());
					break;
				case DEFAULT_CHARSET:
					m_ChersetList->addItem(langResource[39].c_str());
					break;
				case EASTEUROPE_CHARSET:
					m_ChersetList->addItem(langResource[40].c_str());
					break;
				case GB2312_CHARSET:
					m_ChersetList->addItem(langResource[41].c_str());
					break;
				case GREEK_CHARSET:
					m_ChersetList->addItem(langResource[42].c_str());
					break;
				case HANGUL_CHARSET:
					m_ChersetList->addItem(langResource[43].c_str());
					break;
				case MAC_CHARSET:
					m_ChersetList->addItem(langResource[44].c_str());
					break;
				case OEM_CHARSET:
					m_ChersetList->addItem(langResource[45].c_str());
					break;
				case RUSSIAN_CHARSET:
					m_ChersetList->addItem(langResource[46].c_str());
					break;
				case SHIFTJIS_CHARSET:
					m_ChersetList->addItem(langResource[47].c_str());
					initialIndex = i;
					break;
				case SYMBOL_CHARSET:
					m_ChersetList->addItem(langResource[48].c_str());
					break;
				case TURKISH_CHARSET:
					m_ChersetList->addItem(langResource[49].c_str());
					break;
				case VIETNAMESE_CHARSET:
					m_ChersetList->addItem(langResource[50].c_str());
					break;
				case JOHAB_CHARSET:
					m_ChersetList->addItem(langResource[51].c_str());
					break;
				case ARABIC_CHARSET:
					m_ChersetList->addItem(langResource[52].c_str());
					break;
				case HEBREW_CHARSET:
					m_ChersetList->addItem(langResource[53].c_str());
					break;
				case THAI_CHARSET:
					m_ChersetList->addItem(langResource[54].c_str());
					break;
				default:
					m_ChersetList->addItem(_T(""));
					break;
			}
		}
	}
	// �����Z�b�g��I����Ԃɂ���B
	m_ChersetList->setSelectedIndex(initialIndex);
}

/**
 * �I�������t�H���g�ɍ������X�^�C���̑I������ݒ肵�܂��B
 */
void FontSel::setStyle()
{
	int selected = m_fontNameList->getSelectedIndex();
	int charset = m_ChersetList->getSelectedIndex();
	int styleCount = charsetList[charset].fonts.size();

	m_styleList->clear();
	for (int i = 0; i < styleCount; i++) {
		m_styleList->addItem(charsetList[charset].fonts[i].typeName);
	}

	m_styleList->setSelectedIndex(0);
}

/**
 * OK�{�^���������������̓���
 * 
 * @return 0:���ُ͈킠�� 1:���͂͐���
 */
INT_PTR FontSel::onOK(void)
{
	int selectedFontIndex = m_fontNameList->getSelectedIndex();
	int selectedCharset = m_ChersetList->getSelectedIndex();
	int selectedStyle = m_styleList->getSelectedIndex();

	tstring error;
	tstring message;

	error = langResource[63].c_str();

	if (selectedFontIndex < 0) {
		message = langResource[65].c_str();
		MessageBox(this->hWnd,
			message.c_str(),
			error.c_str(),
			MB_ICONEXCLAMATION | MB_OK);
		return (INT_PTR)0;
	}

	if (selectedStyle < 0) {
		message = langResource[66].c_str();
		MessageBox(this->hWnd,
			message.c_str(),
			error.c_str(),
			MB_ICONEXCLAMATION | MB_OK);
		return (INT_PTR)0;
	}

	if (selectedCharset < 0) {
		message = langResource[68].c_str();
		MessageBox(this->hWnd,
			message.c_str(),
			error.c_str(),
			MB_ICONEXCLAMATION | MB_OK);
		return (INT_PTR)0;
	}

	tstring size = m_fontSizeList->getSelectedText();
	if (size.length() < 1) {
		message = langResource[67].c_str();
		MessageBox(this->hWnd,
			message.c_str(),
			error.c_str(),
			MB_ICONEXCLAMATION | MB_OK);
		return (INT_PTR)0;
	}

	selectedFont = charsetList[selectedCharset].fonts[selectedStyle].logFont;

	/*
	if (selectedStyle > 1) {
		if (selectedFont.lfWeight < 600) {
			selectedFont.lfWeight = FW_BOLD;
		}
	} else {
		selectedFont.lfWeight = FW_NORMAL;
	}
	if ((selectedStyle == 1) || (selectedStyle == 3)) {
		selectedFont.lfItalic = TRUE;
	} else {
		selectedFont.lfItalic = FALSE;
	}
	*/

	int point = _tstoi(size.c_str());
	m_point = (double)point;

	// �T�C�Y�̐ݒ�
	HDC hDC = GetDC(this->hWnd);
	selectedFont.lfHeight = -MulDiv(point, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	selectedFont.lfWidth = 0;
	ReleaseDC(this->hWnd, hDC);

	// Windows 8�݊��̏ꍇ�̓s�N�Z��������v�Z�����|�C���g���w�肵���|�C���g��菬�����ꍇ�A
	// �t�H���g�̍����̃s�N�Z�����̐�Βl�𑝂₷�B
	if (WIN8_SIZE) {
		if (abs(getFontPointInt(&selectedFont, this->getHwnd())) < point) {
			// �s�N�Z�����̓}�C�i�X�Ŏw�肷��̂�1���炵�Đ�Βl�𑝂₷�B
			selectedFont.lfHeight--;
		}
	}

	if (m_bold->isChecked()) {
		if (selectedFont.lfWeight < 600) {
			selectedFont.lfWeight = FW_BOLD;
		}
	}
	if (m_italic->isChecked()) {
		selectedFont.lfItalic = TRUE;
	}
	if (m_underline->isChecked()) {
		selectedFont.lfUnderline = TRUE;
	} else {
		selectedFont.lfUnderline = FALSE;
	}

	if (m_strike->isChecked()) {
		selectedFont.lfStrikeOut = TRUE;
	} else {
		selectedFont.lfStrikeOut = FALSE;
	}

	return (INT_PTR)1;
}

void FontSel::setNoMeiryoUI()
{
	noMeiryoUI = true;
}

void FontSel::setNoTahoma()
{
	noTahoma = true;
}
