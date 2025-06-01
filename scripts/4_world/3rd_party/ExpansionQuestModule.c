#ifdef EXPANSIONMODQUESTS
modded class ExpansionQuestModule
{
	override void CreateClientMarker(vector pos, string text, int questID, PlayerIdentity identity, int objectiveIndex, int visibility = -1)
	{
		super.CreateClientMarker(pos, text, questID, identity, objectiveIndex, visibility);

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));
		if (mapPlugin)
		{
			// For some reason vanilla map widget doesn't allow automatic stringtable translations
			if (text.Contains("#"))
				text = "QUEST TARGET";

			int color = GetQuestColor(GetQuestConfigByID(questID));

			MapMarker newMarker = new MapMarker(pos, text, color, GetZenMapConfig().FileToArrayIndex("ZenMap/data/icons/pin.paa"));
			newMarker.m_QuestID = questID;
			newMarker.m_ObjectiveID = objectiveIndex;
			mapPlugin.AddMarker(identity, newMarker);
		}
	}

	override void RemoveClientMarkers(int questID, PlayerIdentity identity, int objectiveIndex)
	{
		super.RemoveClientMarkers(questID, identity, objectiveIndex);

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));
		if (mapPlugin)
		{
			if (mapPlugin.GetPlayerMapMarkers().Contains(identity.GetId()))
			{
				for (int i = 0; i < mapPlugin.GetPlayerMapMarkers().Get(identity.GetId()).Count(); i++)
				{
					if (mapPlugin.GetPlayerMapMarkers().Get(identity.GetId()).Get(i).m_QuestID == questID)
					{
						if (mapPlugin.GetPlayerMapMarkers().Get(identity.GetId()).Get(i).m_ObjectiveID == objectiveIndex)
						{
							mapPlugin.GetPlayerMapMarkers().Get(identity.GetId()).Remove(i);
							mapPlugin.SyncMarkers(identity);
							break;
						}
					}
				}
			}
		}
	}
}
#endif