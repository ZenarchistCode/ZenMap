modded class ChernarusMap
{
	void ZenMapDamageOnOpen()
	{
		if (GetZenMapConfig().ClientConfig.DamageMapOnOpenPercent != 0)
		{
			float dmgPercent = Math.AbsFloat(GetZenMapConfig().ClientConfig.DamageMapOnOpenPercent) / 100;
            AddHealth(GetMaxHealth() * dmgPercent * -1);
		}
	}

	// Copies all markers from target map onto this map.
	void ZenCopyMarkers(ItemMap secondMap)
	{
		ChernarusMap cmap = ChernarusMap.Cast(secondMap);
		if (!cmap)
			return;

		for (int i = 0; i < cmap.GetMarkerArray().Count(); i++)
		{
			bool skipThisMarker = false;
			MapMarker copyMarker = cmap.GetMarkerArray().Get(i);

			for (int x = 0; x < GetMarkerArray().Count(); x++)
			{
				if (vector.Distance(GetMarkerArray().Get(x).GetMarkerPos(), copyMarker.GetMarkerPos()) < 1)
				{
					skipThisMarker = true;
				}

				if (GetMarkerArray().Count() + 1 >= GetZenMapConfig().ClientConfig.MaxMarkerCountPerMapItem)
				{
					skipThisMarker = true;
					break; // not sure if this breaks both loops?
				}
			}

			if (skipThisMarker)
				continue;

			GetMarkerArray().Insert(copyMarker);
		}
	}

	// Custom sync method as vanilla's sync is janky being a defunct function and all. Plus this allows for client-side sync option.
	void SyncZenMapMarkers()
	{
		Param1<ref array<ref MapMarker>> syncData = new Param1<ref array<ref MapMarker>>(m_MapMarkerArray);

		if (GetGame().IsDedicatedServer())
		{
			ZenMapDamageOnOpen();

			// SERVER -> CLIENT SYNC
			PlayerBase playerOwner = PlayerBase.Cast(GetHierarchyRootPlayer());
			if (!playerOwner || !playerOwner.GetIdentity())
				return;

			RPCSingleParam(ERPCs.RPC_SEND_MAP_MARKERS, syncData, true, playerOwner.GetIdentity());
		}
		else 
		{
			// CLIENT -> SERVER SYNC
			RPCSingleParam(ERPCs.RPC_SEND_MAP_MARKERS, syncData, true);
		}
	}

	void RequestZenSync()
	{
		if (GetGame().IsClient())
		{
			RPCSingleParam(ERPCs.RPC_SEND_MAP_MARKERS + 1000, new Param1<bool>(true), true);
		}
	}

	// Vanilla overrides:

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		if (GetGame().IsDedicatedServer() && rpc_type == ERPCs.RPC_SEND_MAP_MARKERS + 1000)
		{
			SyncMapMarkers();
		}
	}

	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionZenMergeMapMarkers);
	}

	override void InsertMarker(vector pos, string text, int color, int idx)
	{
		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			super.InsertMarker(pos, text, color, idx);
			return;
		}
		#endif

		m_MapMarkerArray.Insert(new MapMarker(pos, text, color, idx));
	}

	override void SyncMapMarkers()
	{
		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			super.SyncMapMarkers();
			return;
		}
		#endif

		SyncZenMapMarkers();
	}
}