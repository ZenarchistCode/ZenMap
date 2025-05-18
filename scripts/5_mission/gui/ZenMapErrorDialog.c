class ZenMapErrorDialog extends ScriptedWidgetEventHandler 
{
	protected Widget m_ErrorGUI;
	protected TextWidget m_ErrorWidget;

	void ZenMapErrorDialog(MapMenu mapMenu)
	{
		m_ErrorGUI	= GetGame().GetWorkspace().CreateWidgets("ZenMap/data/gui/layouts/RightClickOptionsMenuError.layout", mapMenu.GetLayoutRoot());
		
		if (!m_ErrorGUI)
		{
			Error("[ZenMap] Big bad: failed to create error GUI!");
			return;
		}

		// Load widgets 
		m_ErrorWidget = TextWidget.Cast(m_ErrorGUI.FindAnyWidget("ErrorText"));

		// Inform widgets to send any UI interactions to this script class
		m_ErrorGUI.SetHandler(this);

		// Force menus to front of map GUI z-sorting or they'll be invisible buried beneath other UI elements.
		m_ErrorGUI.SetSort(6969);
		m_ErrorGUI.Show(false);
	}

	void ~ZenMapErrorDialog()
	{
	}

	void ShowError(string text, int x, int y)
	{
		m_ErrorWidget.SetText(text);
		m_ErrorGUI.SetPos(x, y);
		m_ErrorGUI.Show(true);

		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(HideErrorDialog);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(HideErrorDialog, 6000, false);
	}

	void HideErrorDialog()
	{
		if (m_ErrorGUI)
			m_ErrorGUI.Show(false);
	}

	bool IsErrorVisible()
	{
		return m_ErrorGUI.IsVisible();
	}
}