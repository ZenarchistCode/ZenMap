modded class MapHandler
{
	protected ref MapMenu m_ZenMapMenu;

	void MapHandler(Widget w)
	{
		m_ZenMapMenu = MapMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_MAP));
	}

	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			return super.OnMouseButtonDown(w, x, y, button);
		}
		#endif

		if (!m_ZenMapMenu || m_ZenMapMenu.IsZenMapMarkersHidden())
		{
			return super.OnMouseButtonDown(w, x, y, button);
		}

		if (button == MouseState.RIGHT)
		{
			m_ZenMapMenu.ShowZenRightClickMenu(x, y);
			return true;
		}
		else 
		if (m_ZenMapMenu.IsZenMapMenuOpen())
		{
			m_ZenMapMenu.CloseAllZenMapMenus();
			return true;
		}

		return super.OnMouseButtonDown(w, x, y, button);
	}

	override bool OnDoubleClick(Widget w, int x, int y, int button)
	{
		#ifdef ZENMODPACK 
		if (!ZenModEnabled("ZenMap"))
		{
			return super.OnDoubleClick(w, x, y, button);
		}
		#endif

		if (!m_ZenMapMenu || m_ZenMapMenu.IsZenMapMarkersHidden())
		{
			return super.OnDoubleClick(w, x, y, button);
		}
	
		m_ZenMapMenu.ShowZenRightClickMenu(x, y, true);

		return super.OnDoubleClick(w, x, y, button);
	}
}