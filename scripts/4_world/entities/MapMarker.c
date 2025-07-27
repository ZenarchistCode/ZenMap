modded class MapMarker
{
	#ifdef EXPANSIONMODQUESTS
	int m_QuestID;
	int m_ObjectiveID;
	#endif

	static string GetZenIconImage(int iconID)
	{
		if (GetZenMapConfig().ClientConfig.IconFiles.Count() == 0)
			return "";

		if (iconID < 0 || iconID >= GetZenMapConfig().ClientConfig.IconFiles.Count())
			return GetZenMapConfig().ClientConfig.IconFiles.Get(0);

		return GetZenMapConfig().ClientConfig.IconFiles.Get(iconID);
	}

	string GetZenMapName()
	{
		if (!GetMarkerText().Contains("|"))
			return "";

		array<string> strArray = new array<string>;
		GetMarkerText().Split("|", strArray);

		if (strArray && strArray.Count() > 0)
		{
			return strArray.Get(0);
		}
		
		return "";
	}

	string GetZenMarkerText()
	{
		if (!GetMarkerText().Contains("|"))
			return "";

		array<string> strArray = new array<string>;
		GetMarkerText().Split("|", strArray);

		if (strArray && strArray.Count() > 1)
		{
			return strArray.Get(1);
		}
		
		return "";
	}
}