class ZenMapRightClickMenu extends ScriptedWidgetEventHandler 
{
	static const int MENU_TYPE_NEW = 1;
	static const int MENU_TYPE_EDIT = 2;
	static const int MENU_TYPE_GUI = 3;

	protected int m_CurrentMenuType;

	protected int m_MouseX;
	protected int m_MouseY;

	protected ref MapMenu m_MapMenu;

	protected Widget m_RightClickRoot_New;
	protected Widget m_RightClickRoot_Edit;
	protected Widget m_RightClickRoot_GUI;

	protected TextWidget m_TitleWidget;
	protected EditBoxWidget m_MarkerName;
	protected ImageWidget m_IconImage;
	protected Widget m_ColorPanel;
	protected ButtonWidget m_SaveButton;
	protected Widget m_IconListBG;
	protected TextListboxWidget m_IconList;
	protected CheckBoxWidget m_SaveToMapCB;

	protected int m_SelectedIconIndex;

	protected ref MapMarker m_SelectedMarker;
	protected int m_SelectedMarkerArrayIndex;
	protected vector m_SelectedWorldPosition;

	protected ref array<int> m_PossibleColors;
	protected ref array<Pen_ColorBase> m_PossiblePens;
	protected int m_SelectedColorIndex;

	protected PlayerBase m_Player;
	
	void ZenMapRightClickMenu(MapMenu mapMenu, int mouseX, int mouseY, bool doubleClick = false)
	{
		m_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!m_Player)
			return;

		m_MapMenu = mapMenu;

		m_RightClickRoot_New	= GetGame().GetWorkspace().CreateWidgets("ZenMap/data/gui/layouts/RightClickOptionsMenuNew.layout", mapMenu.GetLayoutRoot());
		m_RightClickRoot_Edit	= GetGame().GetWorkspace().CreateWidgets("ZenMap/data/gui/layouts/RightClickOptionsMenuEdit.layout", mapMenu.GetLayoutRoot());
		m_RightClickRoot_GUI	= GetGame().GetWorkspace().CreateWidgets("ZenMap/data/gui/layouts/RightClickOptionsMenuGUI.layout", mapMenu.GetLayoutRoot());
		
		if (!m_RightClickRoot_New || !m_RightClickRoot_Edit || !m_RightClickRoot_GUI)
		{
			Error("[ZenMap] Big bad: failed to create right-click menu!");
			return;
		}

		// Inform widgets to send any UI interactions to this script class
		m_RightClickRoot_New.SetHandler(this);
		m_RightClickRoot_Edit.SetHandler(this);
		m_RightClickRoot_GUI.SetHandler(this);

		// Force menus to front of map GUI z-sorting or they'll be invisible buried beneath other UI elements.
		m_RightClickRoot_New.SetSort(6969);
		m_RightClickRoot_Edit.SetSort(6969);
		m_RightClickRoot_GUI.SetSort(6969);

		m_RightClickRoot_New.Show(false);
		m_RightClickRoot_Edit.Show(false);
		m_RightClickRoot_GUI.Show(false);

		m_TitleWidget = TextWidget.Cast(m_RightClickRoot_GUI.FindAnyWidget("TitleWidget"));
		m_MarkerName = EditBoxWidget.Cast(m_RightClickRoot_GUI.FindAnyWidget("NameEditBox"));
		m_IconImage = ImageWidget.Cast(m_RightClickRoot_GUI.FindAnyWidget("IconImage"));
		m_ColorPanel = m_RightClickRoot_GUI.FindAnyWidget("ColorPreviewPanel");
		m_SaveButton = ButtonWidget.Cast(m_RightClickRoot_GUI.FindAnyWidget("SaveButton"));
		m_IconListBG = m_RightClickRoot_GUI.FindAnyWidget("IconListBG");
		m_IconList = TextListboxWidget.Cast(m_RightClickRoot_GUI.FindAnyWidget("IconList"));
		m_SaveToMapCB = CheckBoxWidget.Cast(m_RightClickRoot_GUI.FindAnyWidget("SaveToMapCB"));

		if (GetZenMapConfig().ClientConfig.SaveMarkersOntoMapItem && GetZenMapConfig().ClientConfig.AllowPlayerToDecide)
		{
			m_SaveToMapCB.SetChecked(GetZenMapClientMarkersLocalConfig().SaveToMap);
		}
		else 
		{
			m_SaveToMapCB.SetChecked(GetZenMapConfig().ClientConfig.SaveMarkersOntoMapItem);
			m_SaveToMapCB.Show(false);
			float listWidth, listHeight;
			m_IconList.GetSize(listWidth, listHeight);
			m_IconList.SetSize(listWidth, 100); // 100% height
		}

		LoadIconList();
		m_IconListBG.Show(false);
		
		SetPosition(mouseX, mouseY, doubleClick);
	}

	void ~ZenMapRightClickMenu()
	{
	}

	void LoadIconList()
	{
		string iconName;
		string iconFilePath;

		for (int i = 0; i < GetZenMapConfig().ClientConfig.IconFiles.Count(); i++)
		{
			iconFilePath = GetZenMapConfig().ClientConfig.IconFiles.Get(i);
			iconName = SanitizeIconName(iconFilePath);
			m_IconList.AddItem(iconName, NULL, 0);
		}
	}

	string SanitizeIconName(string p_iconFilePath)
	{
		string iconFileName = p_iconFilePath;
		iconFileName.ToUpper();

		int lastSlash;
		int lastDot;

		if (iconFileName.IndexOf("DZ") <= 1)
		{
			iconFileName.Replace("/MAP_", "/");
			iconFileName.Replace("_CA.", ".");
			iconFileName.Replace("_", "");
		}

		lastSlash = iconFileName.LastIndexOf("/");
		if (lastSlash == -1)
			lastSlash = iconFileName.LastIndexOf("\\");

		lastDot = iconFileName.LastIndexOf(".");
		if (lastSlash >= 0 && lastDot > lastSlash)
		{
			iconFileName = iconFileName.Substring(lastSlash + 1, lastDot - lastSlash - 1);
		}
		else
		{
			// Fallback if formatting is weird
			iconFileName = p_iconFilePath;
		}

		return iconFileName;
	}

	void SetIconIndex(int index)
	{
		if (index >= GetZenMapConfig().ClientConfig.IconFiles.Count())
			index = 0;

		if (index < 0)
		{
			if (GetZenMapConfig().ClientConfig.IconFiles.Count() > 1)
			{
				index = GetZenMapConfig().ClientConfig.IconFiles.Count() - 1;
			} else
			{
				index = 0;
			}
		}

		m_SelectedIconIndex = index;
		string iconPath = GetZenMapConfig().ClientConfig.IconFiles.Get(m_SelectedIconIndex);
		m_IconImage.LoadImageFile(0, iconPath);

		if (m_IconList.GetSelectedRow() != index)
		{
			m_IconList.SelectRow(index);
		}
	}

	int GetIconIndex()
	{
		return m_SelectedIconIndex;
	}

	Widget GetLayoutRoot()
	{
		switch (m_CurrentMenuType)
		{
			case MENU_TYPE_NEW:
				return m_RightClickRoot_New;
			case MENU_TYPE_EDIT:
				return m_RightClickRoot_Edit;
			case MENU_TYPE_GUI:
				return m_RightClickRoot_GUI;
		}

		return m_RightClickRoot_GUI;
	}

	void CloseMenu()
	{
		m_RightClickRoot_New.Show(false);
		m_RightClickRoot_Edit.Show(false);
		m_RightClickRoot_GUI.Show(false);
	}

	void SetPosition(int x, int y, bool doubleClick = false)
	{
		SetIconIndex(0);

		m_MouseX = x;
		m_MouseY = y;

		m_SelectedWorldPosition = m_MapMenu.GetZenMapWidget().ScreenToMap(Vector(x, y, 0));

		m_CurrentMenuType = PopulateMenuData(doubleClick);

		if (m_CurrentMenuType == -1)
		{
			return;
		}

		if (GetZenMapConfig().ClientConfig.RequirePenToMark && !m_Player.ZenMap_GetPen())
		{
			m_MapMenu.ShowZenMapError("#STR_ZenMap_NeedPen.", x, y);
			return;
		}

		if (GetZenMapConfig().ClientConfig.SaveMarkersOntoMapItem && !m_MapMenu.GetZenMapItem())
		{
			m_MapMenu.ShowZenMapError("#STR_ZenMap_NeedMap.", x, y);
			return;
		}

		if (m_CurrentMenuType == MENU_TYPE_NEW)
		{
			// Display "create marker" menu
			ShowMenu(m_RightClickRoot_New);
			m_RightClickRoot_Edit.Show(false);
			m_RightClickRoot_GUI.Show(false);
		}
		else 
		if (m_CurrentMenuType == MENU_TYPE_EDIT)
		{
			// Display "edit/delete marker" menu
			ShowMenu(m_RightClickRoot_Edit);
			m_RightClickRoot_New.Show(false);
			m_RightClickRoot_GUI.Show(false);
		} else 
		if (m_CurrentMenuType == MENU_TYPE_GUI)
		{
			// User clicked edit. Display edit GUI
			ShowMenu(m_RightClickRoot_GUI);
			m_RightClickRoot_Edit.Show(false);
			m_RightClickRoot_New.Show(false);
		}
	}

	void ShowMenu(Widget menu)
	{
		menu.Show(true);

		Widget widget = menu.FindAnyWidget("GridSpacer");

		int screenWidth, screenHeight
		GetScreenSize(screenWidth, screenHeight);

		float extraWidth = 0;
		float menuWidth, menuHeight;
		widget.GetSize(menuWidth, menuHeight);

		if (menu == m_RightClickRoot_GUI)
			extraWidth += 310;

		float posX = Math.Max(m_MouseX - (menuWidth / 2), 10);
		float posY = Math.Max(m_MouseY, 10);

		if (posX + menuWidth + extraWidth + 10 > screenWidth)
			posX = screenWidth - menuWidth - extraWidth - 10;

		if (posY + menuHeight + 10 > screenHeight)
			posY = screenHeight - menuHeight - 10;

		widget.SetPos(posX, posY);

		// Set current menu ID flag - this way using ShowMenu() anywhere will keep this consistent
		// rather than relying only on PopulateMenuData() to set this value.
		// This is mainly just to tell MapMenu what GUI to close if Escape button is pressed.
		if (menu == m_RightClickRoot_New)
		{
			m_CurrentMenuType = MENU_TYPE_NEW;
		} else 
		if (menu == m_RightClickRoot_Edit)
		{
			m_CurrentMenuType = MENU_TYPE_EDIT;
		} else 
		if (menu == m_RightClickRoot_GUI)
		{
			m_CurrentMenuType = MENU_TYPE_GUI;
			m_IconListBG.SetPos(posX + 310, posY);
			m_IconListBG.Show(false);
		}
	}

	int PopulateMenuData(bool doubleClick)
	{
		m_SelectedMarker = NULL;
		m_SelectedMarkerArrayIndex = -1;	

		m_SaveButton.SetText("#STR_ZenMap_CreateMarker");
		m_TitleWidget.SetText("#STR_ZenMap_CreateMarker");
		m_MarkerName.SetText("");
		
		PrepareColors();

		float scale = m_MapMenu.GetZenMapWidget().GetScale();

		int i;
		MapMarker marker;
		array<ref MapMarker> mapMarkers = new array<ref MapMarker>;

		// Client markers
		for (i = 0; i < GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Count(); i++)
		{
			marker = GetZenMapClientMarkersLocalConfig().ClientMapMarkers.Get(i);
			mapMarkers.Insert(marker);
		}

		// Item map markers
		if (m_MapMenu.GetZenMapItem())
		{
			for (i = 0; i < m_MapMenu.GetZenMapItem().GetMarkerArray().Count(); i++)
			{
				marker = m_MapMenu.GetZenMapItem().GetMarkerArray().Get(i);
				mapMarkers.Insert(marker);
			}
		}

		// Then figure out which index is selected
		for (i = 0; i < mapMarkers.Count(); i++)
		{
			marker = mapMarkers.Get(i);

			if (vector.Distance(marker.GetMarkerPos(), m_SelectedWorldPosition) < (250 * scale)) // ! Adjust selection radius based on zoom
			{
				m_SaveButton.SetText("#STR_ZenMap_EditMarker");
				m_TitleWidget.SetText("#STR_ZenMap_EditMarker");
				m_MarkerName.SetText(marker.GetMarkerText());
				SetIconIndex(marker.GetMarkerIcon());

				m_SelectedMarkerArrayIndex = i;
				m_SelectedMarker = marker;
				m_SelectedWorldPosition = marker.GetMarkerPos();

				if (doubleClick)
					return MENU_TYPE_GUI;

				return MENU_TYPE_EDIT;
			}
		}

		if (GetZenMapConfig().ClientConfig.SaveMarkersOntoMapItem && m_MapMenu.GetZenMapItem() && m_MapMenu.GetZenMapItem().GetMarkerArray().Count() >= GetZenMapConfig().ClientConfig.MaxMarkerCountPerMapItem)
		{
			m_MapMenu.ShowZenMapError("#STR_ZenMap_TooMany (" + m_MapMenu.GetZenMapItem().GetMarkerArray().Count() + "/" + GetZenMapConfig().ClientConfig.MaxMarkerCountPerMapItem + ")", m_MouseX, m_MouseY);
			return -1;
		}

		if (doubleClick)
			return MENU_TYPE_GUI;

		return MENU_TYPE_NEW;
	}

	void PrepareColors()
	{
		int i;

		if (!m_PossibleColors)
			m_PossibleColors = new array<int>;

		// No pen, no problems.
		if (!GetZenMapConfig().ClientConfig.RequirePenToMark)
		{
			for (i = 0; i < GetZenMapConfig().ClientConfig.NoPenColors.Count(); i++)
			{
				m_PossibleColors.Insert(GetZenMapConfig().ClientConfig.NoPenColors.Get(i));
			}

			return;
		}

		if (!m_PossiblePens)
			m_PossiblePens = new array<Pen_ColorBase>;

		m_PossibleColors.Clear();
		m_PossiblePens.Clear();

		ItemBase item;
		array<EntityAI> itemsArray = new array<EntityAI>;
		m_Player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
		itemsArray.Insert(m_Player.GetItemInHands());

		for (i = 0; i < itemsArray.Count(); i++)
		{
			Pen_ColorBase pen = Pen_ColorBase.Cast(itemsArray.Get(i));

			if (!pen || pen.IsRuined())
			{
				continue;
			}

			int color = GetPenColour(pen);

			// Only store unique colours. I recognize I've mixed color and colour here, it's because my wires are crossed
			// making content for Americans as an Australian on a game which uses American spelling but is made in Czech etc etc 
			// whatever who cares spelling isn't real because we all live in a simulation according to tech daddy Musk.
			if (m_PossibleColors.Find(color) == -1)
			{
				m_PossibleColors.Insert(color);
				m_PossiblePens.Insert(pen);
			}
		}

		SetSelectedColorIndex(m_PossibleColors.Get(0));
	}

	static int GetPenColour(Pen_ColorBase pen)
	{
		if (!pen)
			return ARGB(255, 0, 0, 0);

		string cfgPath = "CfgVehicles " + pen.GetType() + " ";
		int color;

		if (GetGame().ConfigIsExisting(cfgPath + "penColor"))
		{
			// ZenNotes compatibility
			// Get RGB pen color from config.cpp
			TIntArray rgbArray = new TIntArray();
			pen.ConfigGetIntArray("penColor", rgbArray);
			color = ARGB(255, rgbArray[0], rgbArray[1], rgbArray[2]);
		}
		else 
		if (GetGame().ConfigIsExisting(cfgPath + "writingColor"))
		{
			string hexColor;
			if (GetGame().ConfigGetText(cfgPath + "writingColor", hexColor))
			{
				color = HexToARGB(hexColor);
			}
		}

		return color;
	}

	// Using HexToInt() on the entire #ffffff number doesn't work for some reason - need to break it down into rgb
	static int HexToARGB(string hex)
	{
		if (!hex.Contains("#") && hex.Length() != 6)
			return 0;

		hex.Replace("#", "");
		string rs = hex.Substring(0, 2);
		string gs = hex.Substring(2, 2);
		string bs = hex.Substring(4, 2);

		int r = rs.HexToInt();
		int g = gs.HexToInt();
		int b = bs.HexToInt();

		return ARGB(255, r, g, b);
	}

	void SetSelectedColorIndex(int colorIndex)
	{
		if (colorIndex < 0 || colorIndex >= m_PossibleColors.Count())
		{
			colorIndex = 0;
		}

		m_SelectedColorIndex = colorIndex;
		int color = m_PossibleColors.Get(m_SelectedColorIndex);
		m_ColorPanel.SetColor(color);
	}

	int GetColorIndex()
	{
		return m_SelectedColorIndex;
	}

	int GetSelectedColor()
	{
		return m_PossibleColors.Get(m_SelectedColorIndex);
	}
	
	override bool OnClick(Widget w, int x, int y, int button)
	{
		// Close all types of menus
		if (w.GetName() == "CancelButton")
		{
			CloseMenu();
			return true;
		}

		// Open edit GUI
		if (w.GetName() == "EditButton" || w.GetName() == "CreateButton")
		{
			ButtonWidget clickedButton = ButtonWidget.Cast(w);
			string titleText;
			clickedButton.GetText(titleText);
			m_TitleWidget.SetText(titleText);
			m_RightClickRoot_New.Show(false);
			m_RightClickRoot_Edit.Show(false);

			ShowMenu(m_RightClickRoot_GUI);

			if (w.GetName() == "EditButton")
			{
				m_SaveButton.SetText("#layout_character_creation_save");
			}
			else
			{
				m_SaveButton.SetText("#STR_ZenMap_Create");
			}

			return true;
		}

		if (w.GetName() == "PreviousIconButton")
		{
			if (!m_IconListBG.IsVisible() && GetZenMapConfig().ClientConfig.IconFiles.Count() > 3)
			{
				// Initial show of list - set to currently selected icon
				m_IconListBG.Show(true);
				SetIconIndex(GetIconIndex());
				return true;
			}
			
			SetIconIndex(GetIconIndex() - 1);
			return true;
		}

		if (w.GetName() == "NextIconButton")
		{
			if (!m_IconListBG.IsVisible() && GetZenMapConfig().ClientConfig.IconFiles.Count() > 3)
			{
				// Initial show of list - set to currently selected icon
				m_IconListBG.Show(true);
				SetIconIndex(GetIconIndex());
				return true;
			}

			SetIconIndex(GetIconIndex() + 1);
			return true;
		}

		if (w.GetName() == "PreviousColorButton")
		{
			SetSelectedColorIndex(GetColorIndex() - 1);
			return true;
		}

		if (w.GetName() == "NextColorButton")
		{
			SetSelectedColorIndex(GetColorIndex() + 1);
			return true;
		}

		if (w.GetName() == "DeleteButton")
		{
			m_MapMenu.DeleteZenMapMarker(m_SelectedMarkerArrayIndex);

			m_SelectedMarker = NULL;
			m_SelectedMarkerArrayIndex = -1;	

			CloseMenu();

			return true;
		}

		if (w.GetName() == "SaveButton")
		{
			if (m_SelectedMarker != NULL)
			{
				// Delete the edited marker and then just add a new one with the edited values.
				// This is the only way to get around Map Widget not having a way to edit the markers.
				m_MapMenu.DeleteZenMapMarker(m_SelectedMarkerArrayIndex);
				m_SelectedMarker = NULL;
				m_SelectedMarkerArrayIndex = -1;	
			}

			string text = m_MarkerName.GetText();
			text.Replace("*", ""); // Don't allow * as that's what marks a map marker on map item.
			int color = GetSelectedColor();
			vector position = m_SelectedWorldPosition;
			int icon = GetIconIndex();

			if (m_SaveToMapCB.IsChecked())
			{
				m_MapMenu.AddZenMapMarker(position, text, color, icon, true);
			}
			else 
			{
				m_MapMenu.AddZenMapClientMarker(position, text, color, icon); 
			}
			

			CloseMenu();

			// Damage pen for each use if config'ed.
			if (m_PossiblePens && GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent != 0 && m_SelectedColorIndex >= 0 && m_SelectedColorIndex < m_PossiblePens.Count())
			{
				Pen_ColorBase pen = m_PossiblePens.Get(m_SelectedColorIndex);
				if (pen)
				{
					pen.RPCSingleParam(Pen_ColorBase.ZENMAP_ACTION_RPC, new Param1<bool>(true), true, NULL);
				}
			}

			return true;
		}

		if (w.GetName() == "SaveToMapCB")
		{
			GetZenMapClientMarkersLocalConfig().SaveToMap = m_SaveToMapCB.IsChecked();
			GetZenMapClientMarkersLocalConfig().Save();
			return true;
		}

		return super.OnClick(w, x, y, button);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_IconList)
		{
			SetIconIndex(row);
			return true;
		}
	
		return false;
	}
}