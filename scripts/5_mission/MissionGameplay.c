modded class MissionGameplay
{
	protected int m_ZenMapLoopTimer = 0;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			return;
		}
		#endif

		if (!GetZenMapConfig().ClientConfig)
		{
			return;
		}

		if (!GetZenMapConfig().ClientConfig.TakeMapToHandsOnHotkey)
		{
			return;
		}

		if (GetUApi().GetInputByID(UAMapToggle).LocalPress())
		{
			MapMenu mapMenu = MapMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_MAP));
			if (GetGame().GetUIManager().GetMenu() != NULL && GetGame().GetUIManager().GetMenu() != mapMenu)
			{
				return;
			}

			PlayerBase playerBase = PlayerBase.Cast(GetGame().GetPlayer());
			if (!playerBase)
			{
				return;
			}

			ChernarusMap mapItem = ChernarusMap.Cast(playerBase.ZenMap_GetItem("ItemMap"));

			if (!mapItem)
			{
				return;
			}

			m_ZenMapLoopTimer = 0;

			if (playerBase.GetItemInHands() == NULL)
			{
				ZenMap_TakeMapToHands(mapItem);
			} else
			if (playerBase.GetItemInHands().IsKindOf("ItemMap"))
			{
				ZenMap_TriggerAction();
			} else
			{
				ZenMap_EmptyHands(mapItem);
			}
		}
	}

	override void HandleMapToggleByKeyboardShortcut(Man player)
	{
		PlayerBase playerBase = PlayerBase.Cast(player);
		if (!playerBase)
		{
			return;
		}

		ChernarusMap mapItem = ChernarusMap.Cast(playerBase.ZenMap_GetItem("ItemMap"));
		
		// If we're not taking a map to hands or we have no map, then we can just use vanilla code and update markers if necessary.
		if (!CfgGameplayHandler.GetUse3DMap() && (!GetZenMapConfig().ClientConfig.TakeMapToHandsOnHotkey || (!mapItem && CfgGameplayHandler.GetMapIgnoreMapOwnership())))
		{
			super.HandleMapToggleByKeyboardShortcut(player);

			ZenMap_HandleMapToggleByKeyboardShortcut(player);

			MapMenu mapMenu = MapMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_MAP));
			if (mapMenu)
			{
				mapMenu.LoadMapMarkers();
			}
		}
	}

	void ZenMap_RepeatCheckHands()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
	}

	void ZenMap_HandleMapToggleByKeyboardShortcut(Man player)
	{
		// Update the map if it's open and attach a map object to it if possible.
		UIManager um = GetGame().GetUIManager();
		if (!um || !um.IsMenuOpen(MENU_MAP))
		{
			return;
		}

		PlayerBase pb = PlayerBase.Cast(player);
		if (!pb || !pb.GetIdentity())
		{
			return;
		}

		// Map menu is open - request sync if we have a map in our inventory!
		ChernarusMap mapItem = ChernarusMap.Cast(pb.ZenMap_GetItem("ItemMap"));
		if (!mapItem)
		{
			return;
		}

		MapMenu mapMenu = MapMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_MAP));
		if (!mapMenu)
		{
			return;
		}

		mapItem.RequestZenSync();
		mapMenu.InitMapItem(mapItem);
		mapMenu.LoadMapMarkers();

		// Zero time between requesting map marker sync and displaying map menu GUI means we need to delay update of map markers til AFTER menu has opened, 
		// 1 sec ought to be enough 99.9% of the time. If it takes longer than 1 sec then the server has much bigger issues than map marker desync.
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(UpdateZenMapMarkers, 1000, false);
	}

	void ZenMap_EmptyHands(notnull ItemBase mapItem)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		if (!player.GetItemInHands())
		{
			m_ZenMapLoopTimer = 0;
			ZenMap_TakeMapToHands(mapItem);
			return;
		}

		ItemBase item = ItemBase.Cast(player.GetItemInHands());
		if (item)
		{
			if (item.GetInventory().CanRemoveEntity() && player.GetInventory().CanAddEntityToInventory(item) && player.GetHumanInventory().CanRemoveEntityInHands())
			{
				// Get reserved location in inventory
                InventoryLocation loc_dst = new InventoryLocation;
                int reservedIndex = player.GetHumanInventory().FindUserReservedLocationIndex(item);
                player.GetHumanInventory().GetUserReservedLocation(reservedIndex, loc_dst);

				// Get current item inventory location
                InventoryLocation loc_src = new InventoryLocation;
                item.GetInventory().GetCurrentInventoryLocation(loc_src);

				if (reservedIndex == -1 || !player.PredictiveTakeToDst(loc_src, loc_dst))
				{
					player.PredictiveTakeEntityToInventory(FindInventoryLocationType.ANY, item);
				}
			}
		}

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ZenMap_EmptyHands, 100, false, mapItem);
	}

	void ZenMap_TakeMapToHands(notnull ItemBase item)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		if (player.GetItemInHands() && player.GetItemInHands() == item)
		{
			m_ZenMapLoopTimer = 0;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ZenMap_TriggerAction, 100, false);
			return;
		}

		m_ZenMapLoopTimer++;
		if (m_ZenMapLoopTimer > 50)
			return;
		
		if (item.GetInventory().CanRemoveEntity() && player.GetInventory().HasEntityInInventory(item) && player.GetHumanInventory().CanAddEntityInHands(item))
		{
			player.PredictiveTakeEntityToHands(item);
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ZenMap_TakeMapToHands, 100, false, item);
	}

	void ZenMap_TriggerAction()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		ActionManagerClient actionMgr = ActionManagerClient.Cast(player.GetActionManager());

		m_ZenMapLoopTimer++;
		if (m_ZenMapLoopTimer > 50 || actionMgr.GetRunningAction() != NULL)
			return;

		ActionBase action = ActionManagerClient.GetAction(ActionUnfoldMap);
		ItemBase item = ItemBase.Cast(player.GetItemInHands());
		ActionTarget target = null;

		if (action && item && action.Can(player, target, item))
		{
			actionMgr.InjectAction(action, target, item);
			actionMgr.PerformActionStart(action, target, item);
			actionMgr.EjectAction(action);
		}

		// Keep trying until action timeout or action is running
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(ZenMap_TriggerAction, 100, false, player);
	}

	void UpdateZenMapMarkers()
	{
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