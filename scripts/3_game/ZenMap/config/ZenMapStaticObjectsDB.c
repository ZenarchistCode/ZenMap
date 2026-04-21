ref ZenMapStaticObjectsDB g_ZenMapStaticObjectsDB;

static ZenMapStaticObjectsDB GetZenMapStaticObjectsDB()
{
	if (!g_ZenMapStaticObjectsDB) GetZenConfigRegister().RegisterConfig(ZenMapStaticObjectsDB);
	return g_ZenMapStaticObjectsDB;
}

modded class ZenConfigRegister
{
	override void RegisterPreload()
	{
		super.RegisterPreload();
		RegisterType(ZenMapStaticObjectsDB);
	}
}

class ZenMapStaticObjectsDB: ZenObjectHookDbBase
{
	// -------------------------
	// CONFIG SETTINGS
	// -------------------------
	override void OnRegistered()
	{
		g_ZenMapStaticObjectsDB = this;
	}

	override bool ReadJson(string path, out string err)
	{
		return JsonFileLoader<ZenMapStaticObjectsDB>.LoadFile(path, this, err);
	}

	override bool WriteJson(string path, out string err)
	{
		return JsonFileLoader<ZenMapStaticObjectsDB>.SaveFile(path, this, err);
	}
}
