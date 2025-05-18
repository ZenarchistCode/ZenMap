class ZenMapClientMarkersLocalConfig extends Managed
{
	private const static string zenModFolder = "$profile:\\Zenarchist\\";
	private const static string zenConfigName = "ZenMapClientMarkersLocalConfig.json";

	bool HideMarkers = false;
	bool SaveToMap = true;
	ref array<ref MapMarker> ClientMapMarkers;

	void Load()
	{
		ClientMapMarkers = new array<ref MapMarker>;

		if (GetGame().IsDedicatedServer())
			return;

		string serverPath;
		GetCLIParam("connect", serverPath);
		serverPath.Replace(":", "");
		serverPath.Replace(".", "");

		if (FileExist(zenModFolder + serverPath + "\\" + zenConfigName))
		{
			JsonFileLoader<ZenMapClientMarkersLocalConfig>.JsonLoadFile(zenModFolder + serverPath + "\\" + zenConfigName, this);
			return;
		}
		
		Save();
	}

	void InsertMarker(vector pos, string name, int color, int icon)
	{
		ClientMapMarkers.Insert(new MapMarker(pos, name, color, icon));
	}

	void Save()
	{
		// If config folder doesn't exist, create it.
		if (!FileExist(zenModFolder))
		{
			MakeDirectory(zenModFolder);
		}

		string serverPath;
		GetCLIParam("connect", serverPath);
		serverPath.Replace(":", "");
		serverPath.Replace(".", "");

		if (!FileExist(zenModFolder + serverPath))
		{
			MakeDirectory(zenModFolder + serverPath);
		}

		string savePath = zenModFolder + serverPath + "\\" + zenConfigName;
		Print("[ZenMap] Saving " + ClientMapMarkers.Count() + " markers to " + savePath);

		JsonFileLoader<ZenMapClientMarkersLocalConfig>.JsonSaveFile(savePath, this);
	}
}

static ref ZenMapClientMarkersLocalConfig m_ZenMapClientMarkersLocalConfig;

static ZenMapClientMarkersLocalConfig GetZenMapClientMarkersLocalConfig()
{
	if (!m_ZenMapClientMarkersLocalConfig)
	{
		Print("[ZenMapClientMarkersLocalConfig] Init");
		m_ZenMapClientMarkersLocalConfig = new ZenMapClientMarkersLocalConfig;
		m_ZenMapClientMarkersLocalConfig.Load();
	}

	return m_ZenMapClientMarkersLocalConfig;
}