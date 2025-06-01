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
}