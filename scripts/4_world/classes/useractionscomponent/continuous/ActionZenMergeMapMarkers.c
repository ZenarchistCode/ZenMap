class ActionZenMergeMapMarkersCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(5);
	}
}

class ActionZenMergeMapMarkers : ActionContinuousBase
{
	void ActionZenMergeMapMarkers()
	{
		m_CallbackClass = ActionZenMergeMapMarkersCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_PRONE;
		m_Text = "#STR_ZenMap_MergeMapMarkers";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNonRuined();
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!target || !target.GetObject())
			return false;

		if (!target.GetObject().IsKindOf("ItemMap"))
			return false;

		if (!GetZenMapConfig().ClientConfig.SaveMarkersOntoMapItem)
			return false;

		if (GetZenMapConfig().ClientConfig.AllowCopyMapToMap)
		{
			if (GetGame().IsDedicatedServer())
				return true; // Don't bother using resources searching for pen on server, it's not like this needs to be a super secure anti-hack feature.

			if (GetZenMapConfig().ClientConfig.RequirePenToMark)
			{
				return player.ZenMap_GetPen() != NULL;
			}

			return true;
		}

		return false;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		ChernarusMap originMap = ChernarusMap.Cast(action_data.m_MainItem);
		if (!originMap)
			return;

		ChernarusMap secondMap = ChernarusMap.Cast(action_data.m_Target.GetObject());
		if (!secondMap)
			return;

		int markerCount = originMap.GetMarkerArray().Count();
		originMap.ZenCopyMarkers(secondMap);
		secondMap.ZenCopyMarkers(originMap);

		originMap.AddHealth(-1);
		secondMap.AddHealth(-1);
		
		if (GetZenMapConfig().ClientConfig.RequirePenToMark && GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent != 0)
		{
			Pen_ColorBase pen = action_data.m_Player.ZenMap_GetPen();
			if (!pen)
				return; // This shouldn't happen as action condition checks for pen but better be safe.

			if (pen.GetQuantityMax() > 1)
			{
				pen.AddQuantity(Math.AbsFloat(GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent) * markerCount * -1);
			}
			else 
			{
				float dmgPercent = Math.AbsFloat(GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent) / 100;
				pen.AddHealth(pen.GetMaxHealth() * markerCount * dmgPercent * -1);
			}
		}
	}
}