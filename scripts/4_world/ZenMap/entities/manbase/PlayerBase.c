modded class PlayerBase
{
	// Get first pen found in inventory which isn't ruined or empty
	Pen_ColorBase ZenMap_GetPen()
	{
		if (!GetInventory())
			return NULL;

		Pen_ColorBase pen;
		array<EntityAI> itemsArray = new array<EntityAI>;
		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		for (int i = 0; i < itemsArray.Count(); i++)
		{
			pen = Pen_ColorBase.Cast(itemsArray.Get(i));
			if (pen != NULL && !pen.IsRuined())
			{
				// Check >= GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent? Or just let any pen with any amount of quantity work til empty, who cares?
				if (pen.GetQuantityMax() <= 1 || pen.GetQuantity() > 1)
				{
					return pen;
				}
			}
		}

		return NULL;
	}

	ItemBase ZenMap_GetItem(string itemType)
	{
		ItemBase item;
		array<EntityAI> itemsArray = new array<EntityAI>;

		if (GetItemInHands() && !GetItemInHands().IsRuined())
		{
			itemsArray.Insert(GetItemInHands());
		}

		GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		for (int i = 0; i < itemsArray.Count(); i++)
		{
			item = ItemBase.Cast(itemsArray.Get(i));
			if (item != NULL && !item.IsRuined() && item.IsKindOf(itemType))
			{
				return item;
			}
		}

		return NULL;
	}
}