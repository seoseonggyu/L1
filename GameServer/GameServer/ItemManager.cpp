#include "pch.h"
#include "ItemManager.h"

void ItemManager::Add(int32 itemID, ItemInfo itemInfo)
{
	_items.insert(make_pair(itemID, itemInfo));
}

ItemInfo ItemManager::GetItem(int32 itemID)
{
	auto it = _items.find(itemID);
	if (it != _items.end())
	{
		return it->second;
	}
	return ItemInfo();
}