#include "LedgerInfo.h"

using namespace std;

void LedgerInfo::setValue(int field, const string& val)
{
	if (field >= 0 && field < LedgerMenu::e_MenuCount)
	{
		m_field[field] = val;
	}
}

const string& LedgerInfo::getValue(int field) const
{
	if (field >= 0 && field < LedgerMenu::e_MenuCount)
	{
		return m_field[field];
	}
	return m_field[LedgerMenu::e_Name];
}