class ActionOpenStaticZenMap : ActionInteractBase
{
	void ActionOpenStaticZenMap()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_Text = "#unfold_map";
	}

	override bool IsInstant()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return true;
	}

	override void OnStartClient(ActionData action_data)
	{
		UIManager um = g_Game.GetUIManager();
		if (um && !um.IsMenuOpen(MENU_MAP))
		{
			um.CloseAll();

			OPENED_MAP_BY_ZENSTATICMAP = true;

			if (um.EnterScriptedMenu(MENU_MAP, null) != NULL)
			{
				g_Game.GetMission().AddActiveInputExcludes({"map"});
				g_Game.GetMission().AddActiveInputRestriction(EInputRestrictors.MAP);
			}
		}
	}
}