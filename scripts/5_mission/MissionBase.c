modded class MissionBase
{
	void MissionBase()
	{
		#ifdef SERVER
		//
		#else
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapConfig", this, SingeplayerExecutionType.Client);
		GetRPCManager().AddRPC("ZenMod_RPC", "RPC_ReceiveZenMapMarkers", this, SingeplayerExecutionType.Client);
		#endif
	}

	void RPC_ReceiveZenMapConfig(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<ref ZenMapClientSyncConfig> data;
		if (!ctx.Read(data))
		{
			Error("[ZenMap] RPC_ReceiveZenMapConfig: sync data read error");
			return;
		}

		GetZenMapConfig().ClientConfig = data.param1;
	}

	void RPC_ReceiveZenMapMarkers(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<ref array<ref MapMarker>> data;
		if (!ctx.Read(data))
		{
			Error("[ZenMap] RPC_ReceiveZenMapMarkers: sync data read error");
			return;
		}

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));
		if (!mapPlugin)
		{
			Error("[ZenMap] Failed to load PluginZenMapMarkers");
			return;
		}

		mapPlugin.RemoveAllMarkers();
		MapMarker marker;

		for (int i = 0; i < data.param1.Count(); i++)
		{
			marker = data.param1.Get(i);
			if (!marker)
				continue;

			Print("[ZenMap] Received map marker from server: " + marker.GetMarkerText());
			mapPlugin.AddMarker(marker);

#ifdef EXPANSIONMODNAVIGATION
			if (!marker.m_ZenPersonalMarker)
				continue;

			ExpansionMarkerModule module;
			CF_Modules<ExpansionMarkerModule>.Get(module);

			if (module)
			{
				bool skipThisOne = false;
				for (int index = 0; index < module.GetData().PersonalGet().Count(); ++index)
				{
					if (module.GetData().PersonalGet().Get(index).GetName() == marker.GetMarkerText())
						skipThisOne = true;
				}

				if (skipThisOne)
					continue;

				ExpansionMarkerData markerData = ExpansionMarkerData.Create(ExpansionMapMarkerType.PERSONAL);
				markerData.SetName(marker.GetMarkerText());
				markerData.SetIcon(marker.m_ZenIcon);
				markerData.SetColor(ARGB(255,255,240,0));
				markerData.SetPosition(marker.GetMarkerPos());
				markerData.SetLockState(true);
				module.CreateMarker(markerData);
			}
#endif
		}

		UIManager um = GetGame().GetUIManager();
		if (!um || !um.IsMenuOpen(MENU_MAP))
		{
			return;
		}

		MapMenu mapMenu = MapMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_MAP));
		if (!mapMenu)
		{
			return;
		}

		mapMenu.LoadMapMarkers();
	}
}