modded class PluginRepairing
{
	override bool CanRepair(ItemBase repair_kit, Object item, string damage_zone = "")
	{
		if (!super.CanRepair(repair_kit, item, damage_zone))
			return false;

		if (item.IsKindOf("ItemMap") && repair_kit.IsKindOf("DuctTape"))
		{
			return GetZenMapConfig().ClientConfig.MapsRepairableWithDuctTape;
		}

		return true;
	}
}