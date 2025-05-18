modded class PluginManager
{
	override void Init()
	{
		super.Init();

		RegisterPlugin("PluginZenMapMarkers", true, true);
	}
}