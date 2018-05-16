#pragma once

#include <string>

class LedgerInfo
{
public:
	enum class VoltageClass : unsigned int
	{
		e_10KV,
		e_35KV,
		e_66KV,
		e_100KV,
		e_220KV,
		e_500KV,
		e_1000KV,

		e_Undefined
	};

	enum LedgerMenu
	{
		e_Name = 0,
		e_EquipmentNum,
		e_EquipmentType,
		e_EquipmentModelNum,
		e_EquipmentProducer,
		e_VoltageClass,
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