class ZenMapMarkerListView extends ScriptedWidgetEventHandler 
{
	protected ref MapMenu m_MapMenu;

	protected Widget m_ViewListButtonGUI;
	protected Widget m_ConfirmGUI;
	protected TextWidget m_ConfirmWidget;
	protected Widget m_ListPanel;
	protected ButtonWidget m_OpenButton;
	protected TextListboxWidget m_MarkerList;
	protected ImageWidget m_HideShowButtonImage;
	protected ref array<ref ZenMapMarkerListEntry> m_SortedMarkerEntries;
	protected bool m_HideMarkers = false;

	void ZenMapMarkerListView(MapMenu mapMenu)
	{
		m_MapMenu = mapMenu;

		m_ViewListButtonGUI = GetGame().GetWorkspace().CreateWidgets("ZenMap/data/gui/layouts/ViewMarkersButton.layout", mapMenu.GetLayoutRoot());
		m_ConfirmGUI = GetGame().GetWorkspace().CreateWidgets("ZenMap/data/gui/layouts/ConfirmDialog.layout", mapMenu.GetLayoutRoot());

		if (!m_ViewListButtonGUI)
		{
			Error("[ZenMap] Big bad: failed to create marker list GUI!");
			return;
		}

		// Load widgets 
		m_OpenButton = ButtonWidget.Cast(m_ViewListButtonGUI.FindAnyWidget("ViewMarkersButton"));
		m_ListPanel = m_ViewListButtonGUI.FindAnyWidget("PanelGUI");
		m_MarkerList = TextListboxWidget.Cast(m_ViewListButtonGUI.FindAnyWidget("MarkerList"));
		m_ConfirmWidget = TextWidget.Cast(m_ConfirmGUI.FindAnyWidget("ConfirmTitle"));
		m_HideShowButtonImage = ImageWidget.Cast(m_ViewListButtonGUI.FindAnyWidget("HideShowButtonImage"));

		// Set defaults 
		HideMarkerList();
		m_HideMarkers = GetZenMapClientMarkersLocalConfig().HideMarkers;

		if (m_HideMarkers)
		{
			m_HideShowButtonImage.LoadImageFile(0, "ZenMap/data/icons/eye_closed.paa");
			m_MapMenu.HideZenMapMarkers(m_HideMarkers);
		}
		else
		{
			m_HideShowButtonImage.LoadImageFile(0, "ZenMap/data/icons/eye_open.paa");
		}

		// Inform widgets to send any UI interactions to this script class
		m_ViewListButtonGUI.SetHandler(this);
		m_ConfirmGUI.SetHandler(this);

		// Force menus to front of map GUI z-sorting or they'll be invisible buried beneath other UI elements.
		m_ViewListButtonGUI.SetSort(6969);
		m_ViewListButtonGUI.Show(true);
		//m_ViewListButtonGUI.SetPos(50, 10);

		m_ConfirmGUI.SetSort(6969);
		m_ConfirmGUI.Show(false);

		RefreshMarkerList();
	}

	void ~ZenMapMarkerListView()
	{
	}

	bool IsMarkersListVisible()
	{
		return m_ListPanel.IsVisible();
	}

	void HideMarkerList()
	{
		m_OpenButton.SetText("#STR_ZenMap_ShowList");
		m_ListPanel.Show(false);
	}

	void ShowMarkerList()
	{
		m_OpenButton.SetText("#STR_ZenMap_HideList");
		m_ListPanel.Show(true);

		RefreshMarkerList();
	}

	void CloseMenu()
	{
		HideErrorDialog();
		HideConfirmDialog();
	}

	void RefreshMarkerList()
	{
		if (!m_SortedMarkerEntries)
		{
			m_SortedMarkerEntries = new array<ref ZenMapMarkerListEntry>;
		}
		
		m_SortedMarkerEntries.Clear();
		m_MarkerList.ClearItems();

		int untitledNumber = 1;
		int i;
		string label;
		vector pos;
		MapMarker marker;

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));

		// Add server markers
		if (mapPlugin)
		{
			for (i = 0; i < mapPlugin.GetMapMarkers().Count(); i++)
			{
				marker = mapPlugin.GetMapMarkers().Get(i);
				label = "\#" + marker.GetMarkerText(); // # ensures these sort first based on ASCII value, but doesn't display due to being DayZ's stringtable reserved char
				pos = marker.GetMarkerPos();
				m_SortedMarkerEntries.Insert(new ZenMapMarkerListEntry(label, pos, 0, i));
			}
		}

		// Add client config markers
		for (i = 0; i < GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Count(); i++)
		{
			marker = GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Get(i);
			label = marker.GetMarkerText();
			if (label == "")
			{
				untitledNumber++;
				label = "#STR_ZenMap_Untitled " + untitledNumber;
			}

			pos = marker.GetMarkerPos();
			m_SortedMarkerEntries.Insert(new ZenMapMarkerListEntry(label, pos, 1, i));
		}

		// Add map item markers
		if (m_MapMenu.GetZenMapItem())
		{
			for (i = 0; i < m_MapMenu.GetZenMapItem().GetMarkerArray().Count(); i++)
			{
				marker = m_MapMenu.GetZenMapItem().GetMarkerArray().Get(i);
				label = marker.GetMarkerText();
				if (label == "")
				{
					label = "#STR_ZenMap_Untitled " + untitledNumber++;
				}
				if (GetZenMapConfig().ClientConfig.AllowPlayerToDecide)
					label = "* " + label;

				pos = marker.GetMarkerPos();
				m_SortedMarkerEntries.Insert(new ZenMapMarkerListEntry(label, pos, 2, i));
			}
		}

		SortMarkerList();

		for (i = 0; i < m_SortedMarkerEntries.Count(); i++)
		{
			string markerLabel = m_SortedMarkerEntries.Get(i).GetLabel().Trim();

			#ifdef MAPLINK
			array<string> strArray = new array<string>;
			markerLabel.Split("|", strArray);
			if (strArray && strArray.Count() > 1)
				markerLabel = strArray.Get(1);
			#endif

			m_MarkerList.AddItem(markerLabel, NULL, 0);
		}
	}

	void SortMarkerList()
	{
		int count = m_SortedMarkerEntries.Count();
		ZenMapMarkerListEntry temp;
		string label1;
		string label2;

		for (int i = 0; i < count - 1; i++)
		{
			for (int j = i + 1; j < count; j++)
			{
				label1 = m_SortedMarkerEntries.Get(i).GetLabel();
				label2 = m_SortedMarkerEntries.Get(j).GetLabel();
				label1.ToLower();
				label2.ToLower();
				label1.Replace(" ", "");
				label2.Replace(" ", "");

				if (label1 > label2)
				{
					temp = m_SortedMarkerEntries.Get(i);
					m_SortedMarkerEntries.Set(i, m_SortedMarkerEntries.Get(j));
					m_SortedMarkerEntries.Set(j, temp);
				}
			}
		}
	}

	void HideErrorDialog()
	{
		if (m_MapMenu)
			m_MapMenu.HideZenMapError();
	}

	void HideConfirmDialog()
	{
		if (m_ConfirmGUI)
			m_ConfirmGUI.Show(false);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		HideErrorDialog();

		// Close all types of menus
		if (w.GetName() == "ViewMarkersButton")
		{
			if (!m_ListPanel.IsVisible())
			{
				if (m_MapMenu.GetZenMapRightClickMenu() && m_MapMenu.GetZenMapRightClickMenu().GetLayoutRoot().IsVisible())
				{
					m_MapMenu.GetZenMapRightClickMenu().CloseMenu();
				}

				ShowMarkerList();
			}
			else 
			{
				HideMarkerList();
			}

			return true;
		}

		if (w.GetName() == "DeleteSelectedMarkerButton")
		{
			int selectedRow = m_MarkerList.GetSelectedRow();
			if (selectedRow < 0 || selectedRow >= m_SortedMarkerEntries.Count())
				return true;

			ZenMapMarkerListEntry entry = m_SortedMarkerEntries.Get(selectedRow);

			if (entry.GetSourceArray() == 0)
			{
				m_MapMenu.ShowZenMapError("#STR_ZenMap_CantDeleteServer", x + 50, y);
				return true;
			}

			int deletionIndex = entry.GetArrayIndex();
			m_MapMenu.DeleteZenMapMarker(deletionIndex);
			RefreshMarkerList();
		}

		if (w.GetName() == "DeleteAllMarkersButton")
		{
			int totalMarkers = GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Count();

			if (m_MapMenu.GetZenMapItem())
			{
				totalMarkers = totalMarkers + m_MapMenu.GetZenMapItem().GetMarkerArray().Count();
			}

			if (totalMarkers == 0)
			{
				m_MapMenu.ShowZenMapError("#STR_ZenMap_NoMarkers", x + 50, y);
				return true;
			}

			m_ConfirmWidget.SetText("#STR_ZenMap_Confirm");
			m_ConfirmGUI.Show(true);
			return true;
		}

		if (w.GetName() == "ConfirmYesButton")
		{
			HideConfirmDialog();

			if (m_MapMenu.GetZenMapItem())
			{
				m_MapMenu.GetZenMapItem().GetMarkerArray().Clear();
			}

			GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Clear();

			m_MapMenu.GetZenMapWidget().ClearUserMarks();
			m_MapMenu.LoadMapMarkers();
			m_MapMenu.PlayZenMarkerSound();
			RefreshMarkerList();

			return true;
		}

		if (w.GetName() == "ConfirmCancelButton")
		{
			HideConfirmDialog();
			return true;
		}

		if (w.GetName() == "HideShowButton")
		{
			m_HideMarkers = !m_HideMarkers;

			if (m_HideMarkers)
			{
				m_HideShowButtonImage.LoadImageFile(0, "ZenMap/data/icons/eye_closed.paa");
			}
			else 
			{
				m_HideShowButtonImage.LoadImageFile(0, "ZenMap/data/icons/eye_open.paa");
			}

			m_MapMenu.HideZenMapMarkers(m_HideMarkers);

			GetZenMapClientMarkersLocalConfig().HideMarkers = m_HideMarkers;
			GetZenMapClientMarkersLocalConfig().Save();
		}

		return super.OnClick(w, x, y, button);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_MarkerList)
		{
			m_MapMenu.GetZenMapWidget().SetMapPos(m_SortedMarkerEntries.Get(row).GetPosition());
			return true;
		}
	
		return false;
	}

	bool HideMarkers()
	{
		return m_HideMarkers;
	}
}