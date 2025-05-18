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

		for (int i = 0; i < data.param1.Count(); i++)
		{
			Print("[ZenMap] Received map marker from server: " + data.param1.Get(i).GetMarkerText());
			mapPlugin.AddMarker(data.param1.Get(i));
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