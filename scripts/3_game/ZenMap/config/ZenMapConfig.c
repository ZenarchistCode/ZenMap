static bool OPENED_MAP_BY_ZENSTATICMAP = false;

ref ZenMapConfig g_ZenMapConfig;

static ZenMapConfig GetZenMapConfig()
{
	if (!g_ZenMapConfig) GetZenConfigRegister().RegisterConfig(ZenMapConfig);
	return g_ZenMapConfig;
}

modded class ZenConfigRegister
{
	override void RegisterPreload()
	{
		super.RegisterPreload(); 
		RegisterType(ZenMapConfig);
	}
}

class ZenMapConfig_SyncPayload
{
	ref ZenMapClientSyncConfig ClientConfig;
	
	void ZenMapConfig_SyncPayload()
	{
		ClientConfig = new ZenMapClientSyncConfig();
	}
}

class ZenMapConfig : ZenObjectHookConfigBase
{
	// -------------------------
	// CONFIG SETTINGS
	// -------------------------
	override void OnRegistered()
	{
		g_ZenMapConfig = this;
	}
	
	override ZenObjectHookDbBase GetDB()
	{
		return GetZenMapStaticObjectsDB();
	}
	
	override string    	GetCurrentVersion()   		{ return "1.29.3"; }
	override bool		ShouldLoadOnServer() 		{ return true; }
	override bool		ShouldSyncToClient()		{ return true; }
	
	override bool IsServerOnlyConfig()
	{
		return false;
	}
	
	override bool ReadJson(string path, out string err)
	{
		return JsonFileLoader<ZenMapConfig>.LoadFile(path, this, err);
	}

	override bool WriteJson(string path, out string err)
	{
		return JsonFileLoader<ZenMapConfig>.SaveFile(path, this, err);
	}

	override bool BuildSyncPayload(out string payload, out string err)
	{
		ZenMapConfig_SyncPayload snap = new ZenMapConfig_SyncPayload();

		snap.ClientConfig = ClientConfig;

		// Serialize payload only (NOT the whole config)
		return JsonFileLoader<ZenMapConfig_SyncPayload>.MakeData(snap, payload, err, false);
	}

	override bool ApplySyncPayload(string payload, out string err)
	{
		// Create default containers in case an error happens on read
		ClientConfig = new ZenMapClientSyncConfig();

		ZenMapConfig_SyncPayload snap = new ZenMapConfig_SyncPayload();
		if (!JsonFileLoader<ZenMapConfig_SyncPayload>.LoadData(payload, snap, err))
		{
			return false;
		}

		ClientConfig = snap.ClientConfig;
		
		return true;
	}
	
	// -------------------------
	// CONFIG VARIABLES
	// -------------------------
	bool ExpansionModCompatibility;
	ref ZenMapClientSyncConfig ClientConfig;
	ref array<ref ZenMapMarker> ServerMapMarkers;
	
	override void SetDefaults()
	{
		super.SetDefaults();

		ExpansionModCompatibility = false;
		
		ServerMapMarkers = new array<ref ZenMapMarker>;
		ServerMapMarkers.Insert(new ZenMapMarker("Example Marker", "8229.091797 471.185577 9031.171875", "DZ/gear/navigation/data/map_border_cross_ca.paa", ARGB(255, 255, 0, 0)));
		ClientConfig = new ZenMapClientSyncConfig();
		
		SpawnHooks.Insert(new ZenObjectHookDef("trailmap_enoch", "Zen_StaticMap", "0 0 0", "0 0 0", 1, 1));
	}
	
	override void AfterLoad()
	{
		if (g_Game.IsClient())
			return;
		
		super.AfterLoad();
	}
	
	override bool ShouldSpawn()
	{
		if (!SpawnObjects)
			return false;
		
		#ifdef ZenModPack
		if (!ZenModEnabled("ZenMap"))
			return false;
		#endif
		
		return true;
	}

	int FileToArrayIndex(string file)
	{
		if (!ClientConfig)
			return 0;

		file.ToLower();
		file.Replace("/", "");
		file.Replace(".paa", "");
		file.Replace(".edds", "");

		for (int i = 0; i < ClientConfig.IconFiles.Count(); i++)
		{
			string iconFile = ClientConfig.IconFiles.Get(i);
			iconFile.ToLower();
			iconFile.Replace("/", "");
			iconFile.Replace(".paa", "");
			iconFile.Replace(".edds", "");

			if (iconFile == file)
				return i;
		}

		return 0;
	}
}

class ZenMapMarker
{
	string Name;
	vector Position;
	string Icon;
	int Color; // https://argb-int-calculator.netlify.app/

	void ZenMapMarker(string p_name, vector p_pos, string p_icon, int p_color)
	{
		Name = p_name;
		Position = p_pos;
		Icon = p_icon;
		Color = p_color;
	}
}

class ZenMapClientSyncConfig
{
	bool TakeMapToHandsOnHotkey = true;
	bool MapsRepairableWithDuctTape = true;
	bool SaveMarkersOntoMapItem = true;
	bool AllowPlayerToDecide = true;
	bool AllowCopyMapToMap = true;
	bool RequirePenToMark = true;
	int MaxMarkerCountPerMapItem = 50;
	float PenMarkerConsumeQuantityPercent = 10;
	float DamageMapOnOpenPercent = 0;
	ref array<int> NoPenColors;
	ref array<string> IconFiles;

	void ZenMapClientSyncConfig()
	{
		NoPenColors = new array<int>;
		IconFiles = new array<string>;

		IconFiles.Insert("DZ/gear/navigation/data/map_border_cross_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/boat.paa");
		IconFiles.Insert("ZenMap/data/icons/car.paa");
		IconFiles.Insert("ZenMap/data/icons/cart.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_chapel_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_church_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/eye_closed.paa");
		IconFiles.Insert("ZenMap/data/icons/eye_open.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_factory_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/flag.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_fir_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_fuelstation_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/guns.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_hill_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_hospital_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/heli.paa");
		IconFiles.Insert("ZenMap/data/icons/house.paa");
		IconFiles.Insert("ZenMap/data/icons/info.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_lighthouse_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/marker.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_monument_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_palm_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/pin.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_quay_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_rock_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_ruin_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/shovel.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_station_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/tent.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_tourism_ca.paa");
		IconFiles.Insert("DZ/gear/navigation/data/map_waterpump_ca.paa");
		IconFiles.Insert("ZenMap/data/icons/warn.paa");

		NoPenColors.Insert(-16119286);	// black
		NoPenColors.Insert(-3664876);	// red
		NoPenColors.Insert(-15165928);	// green
		NoPenColors.Insert(-16760376);	// blue
		NoPenColors.Insert(-65374);		// pink
		NoPenColors.Insert(-16119286);	// purple
		NoPenColors.Insert(-27136);		// orange
	}
}