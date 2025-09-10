#pragma once

struct ItemInfo
{
	int32 itemID = 0;
	String itemName;

	optional<float>    damage;
	optional<float>    strength;
	optional<float>    defense;
	optional<float>    agility;
	optional<float>    intelligence;
};

class ItemManager
{
public:
	void Add(int32 itemID, ItemInfo itemInfo);
	ItemInfo GetItem(int32 itemID);


private:
	unordered_map<int32, ItemInfo> _items;
};

