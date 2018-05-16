#pragma once

#include <string>

class LedgerInfo
{
public:
	enum LedgerMenu
	{
		e_Finished= 0,
		e_Name,
		e_EquipmentNum,
		e_EquipmentType,
		e_EquipmentModelNum,
		e_EquipmentProducer,
		e_Region,
		e_OperateNum,
		e_Remarks,
		e_MenuCount
	};

	LedgerInfo() {}
	~LedgerInfo() {}

	void setValue(int field, const std::string& val);
	const std::string& getValue(int field) const;
	void setID(int val) { m_id = val; }
	int getID() const { return m_id; }
private:
	std::string m_field[LedgerMenu::e_MenuCount];
	int m_id;
};