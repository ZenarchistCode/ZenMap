class ZenMapStaticObjectsConfig extends Managed
{
	static const string CONFIG_VERSION = "1";

	static bool OPENED_MAP_BY_ZENSTATICMAP = false;

	string ConfigVersion = "";

	private static const string zenModFolder = "$profile:\\Zenarchist\\";
	private static string zenConfigName = "ZenMapStaticObjectsConfig.json";

	// Main config data
	bool DebugOn;
	bool DumpObjectLocations;
	bool SpawnMapObjects;
	ref array<ref ZenStaticMapObject> MapPositions = new array<ref ZenStaticMapObject>;

	void Load()
	{
		if (FileExist(zenModFolder + zenConfigName))
		{
			JsonFileLoader<ZenMapStaticObjectsConfig>.JsonLoadFile(zenModFolder + zenConfigName, this);

			if (ConfigVersion != CONFIG_VERSION)
			{
				JsonFileLoader<ZenMapStaticObjectsConfig>.JsonSaveFile(zenModFolder + zenConfigName + "_old", this);
				ConfigVersion = CONFIG_VERSION;
			}
			else
			{
				return;
			}
		}

		MapPositions.Clear();
		MapPositions.Insert(new ZenStaticMapObject("Land_Mil_ControlTower", "0.7 3.2 0", "90 0 0", 0.5));
		MapPositions.Insert(new ZenStaticMapObject("trailmap_", "0 0 0", "0 0 0"));
		
		ConfigVersion			= CONFIG_VERSION;
		DebugOn					= false;
		DumpObjectLocations		= true;
		SpawnMapObjects			= true;

		Save();
	}

	void Save()
	{
		if (!FileExist(zenModFolder))
		{
			MakeDirectory(zenModFolder);
		}

		JsonFileLoader<ZenMapStaticObjectsConfig>.JsonSaveFile(zenModFolder + zenConfigName, this);
	}

	void CleanUp()
	{
		MapPositions.Clear();
	}
}

class ZenStaticMapObject
{
	string TypeName;
	vector Offset = "0 0 0";
	vector Orient = "0 0 0";
	float Scale = 1.0;

	void ZenStaticMapObject(string p_type, vector p_offset, vector p_orient, float p_scale = 1.0)
	{
		TypeName = p_type;
		Offset = p_offset;
		Orient = p_orient;
		Scale = p_scale;
	}

	ref array<vector> Locations = new array<vector>;
}

static ref ZenMapStaticObjectsConfig m_ZenMapStaticObjectsConfig;

static ZenMapStaticObjectsConfig GetZenMapStaticObjectsConfig()
{
	if (!m_ZenMapStaticObjectsConfig)
	{
		Print("[ZenMapStaticObjectsConfig] Init");
		m_ZenMapStaticObjectsConfig = new ZenMapStaticObjectsConfig;

		if (GetGame().IsDedicatedServer())
			m_ZenMapStaticObjectsConfig.Load();
	}

	return m_ZenMapStaticObjectsConfig;
}