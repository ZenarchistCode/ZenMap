modded class MapMenu
{
	protected ref ZenMapRightClickMenu m_ZenRightClickMenu;
	protected ref ZenMapMarkerListView m_ZenViewMarkersList;
	protected ref ZenMapErrorDialog m_ZenErrorDialog;

	override void OnShow()
	{
		super.OnShow();

		if (ZenMapStaticObjectsConfig.OPENED_MAP_BY_ZENSTATICMAP)
		{
			PlayerBase pb = PlayerBase.Cast(GetGame().GetPlayer());
			if (!pb)
				return;

			LoadMapMarkers();
			m_MapWidgetInstance.SetMapPos(pb.GetPosition());
			m_MapWidgetInstance.SetScale(0.2); // 0.85
			AddZenMapWidgetMarker(pb.GetPosition(), "", ARGB(255, 255, 0, 0), "DZ/gear/navigation/data/map_border_cross_ca.paa");
		}
	}

	override void CloseMapMenu()
	{
		super.CloseMapMenu();

		if (ZenMapStaticObjectsConfig.OPENED_MAP_BY_ZENSTATICMAP)
		{
			ZenMapStaticObjectsConfig.OPENED_MAP_BY_ZENSTATICMAP = false;
			GetGame().GetMission().RemoveActiveInputExcludes({"map"});
			GetGame().GetMission().RemoveActiveInputRestriction(EInputRestrictors.MAP);
		}

		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
			return;
		#endif

		GetZenMapClientMarkersLocalConfig().Save();
	}

	void HideZenMapMarkers(bool hide)
	{
		if (hide)
		{
			GetZenMapWidget().ClearUserMarks();
		}
		else 
		{
			LoadMapMarkers();
		}
	}

	bool IsZenMapMarkersHidden()
	{
		if (m_ZenViewMarkersList)
		{
			return m_ZenViewMarkersList.HideMarkers();
		}

		return false;
	}

	void ShowZenMapError(string text, int x, int y)
	{
		if (!m_ZenErrorDialog)
		{
			m_ZenErrorDialog = new ZenMapErrorDialog(this);
		}

		m_ZenErrorDialog.ShowError(text, x, y);
	}

	void HideZenMapError()
	{
		if (m_ZenErrorDialog)
		{
			m_ZenErrorDialog.HideErrorDialog();
		}
	}

	void CreateZenMarkerButton()
	{
		if (!m_ZenViewMarkersList)
		{
			m_ZenViewMarkersList = new ZenMapMarkerListView(this);
		}

		m_ZenViewMarkersList.RefreshMarkerList();
	}

	ZenMapMarkerListView GetZenMapMarkerList()
	{
		return m_ZenViewMarkersList;
	}

	void ShowZenRightClickMenu(int mouseX, int mouseY, bool doubleClick = false)
	{
		if (!m_ZenRightClickMenu)
		{
			m_ZenRightClickMenu = new ZenMapRightClickMenu(this, mouseX, mouseY, doubleClick);
		}

		m_ZenRightClickMenu.SetPosition(mouseX, mouseY, doubleClick);
	}

	void CloseAllZenMapMenus()
	{
		if (m_ZenRightClickMenu)
		{
			m_ZenRightClickMenu.CloseMenu();
		}

		if (m_ZenViewMarkersList)
		{
			m_ZenViewMarkersList.CloseMenu();
		}

		HideZenMapError();
	}

	bool IsZenMapMenuOpen()
	{
		return (m_ZenRightClickMenu && m_ZenRightClickMenu.GetLayoutRoot().IsVisible()) || (m_ZenErrorDialog && m_ZenErrorDialog.IsErrorVisible());
	}

	// Adds marker onto the map widget AND the map item if enabled
	void AddZenMapMarker(vector pos, string name, int color, int icon, bool saveToMap = false)
	{
		string markerText = name;

		#ifdef MAPLINK 
		if (saveToMap)
		{
			array<string> strArray = new array<string>;
			name.Split("|", strArray);

			if (strArray.Count() > 1)
			{
				markerText = strArray.Get(1);
			}
		}
		#endif

		AddZenMapWidgetMarker(pos, markerText, color, MapMarker.GetZenIconImage(icon), (saveToMap && GetZenMapConfig().ClientConfig.AllowPlayerToDecide));

		if (GetZenMapItem() && saveToMap)
		{
			GetZenMapItem().InsertMarker(pos, name, color, icon);
		}

		if (m_ZenViewMarkersList)
		{
			m_ZenViewMarkersList.RefreshMarkerList();
		}

		PlayZenMarkerSound();
		
		m_WasChanged = true;
	}

	void AddZenMapClientMarker(vector pos, string name, int color, int icon)
	{
		GetZenMapClientMarkersLocalConfig().InsertMarker(pos, name, color, icon);
		AddZenMapWidgetMarker(pos, name, color, MapMarker.GetZenIconImage(icon));

		if (m_ZenViewMarkersList)
		{
			m_ZenViewMarkersList.RefreshMarkerList();
		}

		PlayZenMarkerSound();
	}

	void PlayZenMarkerSound()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player)
		{
			SEffectManager.PlaySound(GetZenMarkerSoundSet(), player.GetPosition());
		}
	}

	string GetZenMarkerSoundSet()
	{
		return "Zen_MapMarker_SoundSet";
	}

	bool AddZenMapWidgetMarker(vector pos, string text, int color, string icon, bool displayMapPrefix = false)
	{
		icon.Replace("/", "\\");

		string prefix = " ";
		if (displayMapPrefix)
			prefix = "* ";

		m_MapWidgetInstance.AddUserMark(pos, prefix + text, color, icon);
		return true;
	}

	override void LoadMapMarkers()
	{
		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			super.LoadMapMarkers();
			return;
		}
		#endif

		// Don't pass to super, because we override how icons are loaded to allow for custom marker icons.
		GetZenMapWidget().ClearUserMarks();

		if (IsZenMapMarkersHidden())
		{
			CreateZenMarkerButton();
			return;
		}

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));

		int i;
		string iconPath;
		string serverIcon;
		string userIcon;
		MapMarker map_marker;

		if (mapPlugin)
		{
			// Load server map markers and display them on the widget first
			for (i = 0; i < mapPlugin.GetMapMarkers().Count(); i++)
			{
				map_marker = mapPlugin.GetMapMarkers().Get(i);
				AddZenMapWidgetMarker(map_marker.GetMarkerPos(), map_marker.GetMarkerText(), map_marker.GetMarkerColor(), MapMarker.GetZenIconImage(map_marker.GetMarkerIcon()));
			}
		}

		// Load client map markers directly onto map widget
		for (i = 0; i < GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Count(); i++)
		{
			map_marker = GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Get(i);
			AddZenMapWidgetMarker(map_marker.GetMarkerPos(), map_marker.GetMarkerText(), map_marker.GetMarkerColor(), MapMarker.GetZenIconImage(map_marker.GetMarkerIcon()));
		}

		// Load map makers from map item
		if (GetZenMapItem())
		{
			for (i = 0; i < GetZenMapItem().GetMarkerArray().Count(); i++)
			{
				map_marker = GetZenMapItem().GetMarkerArray().Get(i);
				string markerText = map_marker.GetMarkerText();

				#ifdef MAPLINK
				string mapName = map_marker.GetZenMapName();

				// MapLink compatibility
				if (mapName != "")
				{
					string thisMapName = GetGame().GetWorldName();

					if (mapName == thisMapName)
					{
						markerText = map_marker.GetZenMarkerText();
					}
					else 
					{
						continue;
					}
				}
				#endif

				AddZenMapWidgetMarker(map_marker.GetMarkerPos(), markerText, map_marker.GetMarkerColor(), MapMarker.GetZenIconImage(map_marker.GetMarkerIcon()), GetZenMapConfig().ClientConfig.AllowPlayerToDecide);
			}
		}

		CreateZenMarkerButton();
	}

	override void Update(float timeslice)
	{
		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			super.Update(timeslice);
			return;
		}
		#endif

		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			if (m_ZenRightClickMenu && m_ZenRightClickMenu.GetLayoutRoot().IsVisible())
			{
				m_ZenRightClickMenu.CloseMenu();
				return;
			}

			if (m_ZenViewMarkersList && m_ZenViewMarkersList.IsMarkersListVisible())
			{
				m_ZenViewMarkersList.HideMarkerList();
				return;
			}
		}

		super.Update(timeslice);
	}

	void DeleteZenMapMarker(int index)
	{
		if (index < 0)
		{
			return;
		}

		// Delete marker. Vanilla map widget does not have good methods for doing this so we need to get creative.
		int i;

		// Copy existing markers EXCEPT the one we're deleting
		array<ref MapMarker> newMarkers = new array<ref MapMarker>;

		if (index < GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Count())
		{
			GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Remove(index);
		}
		else
		if (GetZenMapItem())
		{
			index = index - GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Count();

			for (i = 0; i < GetZenMapItem().GetMarkerArray().Count(); i++)
			{
				if (i == index)
				{
					continue;
				}

				newMarkers.Insert(GetZenMapItem().GetMarkerArray().Get(i));
			}

			// Clear the map's markers
			GetZenMapItem().GetMarkerArray().Clear();

			// Add new markers back onto the map
			MapMarker newMarker;
			for (i = 0; i < newMarkers.Count(); i++)
			{
				newMarker = newMarkers.Get(i);
				GetZenMapItem().InsertMarker(newMarker.GetMarkerPos(), newMarker.GetMarkerText(), newMarker.GetMarkerColor(), newMarker.GetMarkerIcon());
			}
		}

		PlayZenMarkerSound();

		LoadMapMarkers();
	}

	ZenMapRightClickMenu GetZenMapRightClickMenu()
	{
		return m_ZenRightClickMenu;
	}

	MapHandler GetZenMapMenuHandler()
	{
		return m_MapMenuHandler;
	}

	MapWidget GetZenMapWidget()
	{
		return m_MapWidgetInstance;
	}

	ItemMap GetZenMapItem()
	{
		return m_Map;
	}
}