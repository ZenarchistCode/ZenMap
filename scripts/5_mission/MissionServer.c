modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenMap"))
			return;
		#endif

		GetZenMapConfig();

		if (GetZenMapConfig().ClientConfig.RequirePenToMark)
		{
			// If player NEEDS a pen, clear the no-pen colors from memory as we don't need to sync that shit to clients anymore.
			GetZenMapConfig().ClientConfig.NoPenColors.Clear();
		}
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) 
	{
		super.InvokeOnConnect(player, identity);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenMap"))
			return;
		#endif

		//! SEND GENERAL CONFIG IMMEDIATELY
		GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapConfig", new Param1<ref ZenMapClientSyncConfig>(GetZenMapConfig().ClientConfig), true, identity);

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));
		if (mapPlugin)
		{
			mapPlugin.SyncMarkers(player);
		}
	}
}