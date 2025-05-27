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
		UIManager um = GetGame().GetUIManager();
		if (um && !um.IsMenuOpen(MENU_MAP))
		{
			um.CloseAll();

			ZenMapStaticObjectsConfig.OPENED_MAP_BY_ZENSTATICMAP = true;

			if (um.EnterScriptedMenu(MENU_MAP, null) != NULL)
			{
				GetGame().GetMission().AddActiveInputExcludes({"map"});
				GetGame().GetMission().AddActiveInputRestriction(EInputRestrictors.MAP);
			}
		}
	}
}