ref ZenMapClientMarkersLocalConfig g_ZenMapClientMarkersLocalConfig;

static ZenMapClientMarkersLocalConfig GetZenMapClientMarkersLocalConfig()
{
	if (!g_ZenMapClientMarkersLocalConfig) GetZenConfigRegister().RegisterConfig(ZenMapClientMarkersLocalConfig);
	return g_ZenMapClientMarkersLocalConfig;
}

class ZenMapClientMarkersLocalConfig : ZenConfigBase
{
	// -------------------------
	// CONFIG SETTINGS
	// -------------------------
	override void OnRegistered()
	{
		g_ZenMapClientMarkersLocalConfig = this;
	}
	
	override string    	GetCurrentVersion()   		{ return "1.29.1"; }
	override bool		ShouldLoadOnClient() 		{ return true; }
	override bool		IsClientOnlyConfig()		{ return true; }
	
	override bool ReadJson(string path, out string err)
	{
		return JsonFileLoader<ZenMapClientMarkersLocalConfig>.LoadFile(path, this, err);
	}

	override bool WriteJson(string path, out string err)
	{
		return JsonFileLoader<ZenMapClientMarkersLocalConfig>.SaveFile(path, this, err);
	}

	bool HideMarkers = false;
	bool SaveToMap = true;
	ref array<ref MapMarker> ClientMapMarkers = new array<ref MapMarker>;

	void InsertMarker(vector pos, string name, int color, int icon)
	{
		ClientMapMarkers.Insert(new MapMarker(pos, name, color, icon));
	}
}