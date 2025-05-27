class CfgPatches
{
	class ZenMap
	{
		requiredVersion = 0.1;
		requiredAddons[] = 
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZ_Gear_Navigation"
		};
	};
};

class CfgMods
{
	class ZenMap
	{
		dir="ZenMap";
        name="ZenMap";
        credits="Zenarchist";
        author="Zenarchist";
        authorID="0";
        version="0.1";
        extra=0;
        type = "Mod";
	    dependencies[]={"Game", "World", "Mission"};
	    class defs
	    {
			class engineScriptModule
			{
				files[]=
				{
					"ZenMap/scripts/common"
				};
			};

			class gameLibScriptModule
			{
				files[]=
				{
					"ZenMap/scripts/common"
				};
			};

			class gameScriptModule
            {
				value = "";
                files[]=
				{
					"ZenMap/scripts/common",
					"ZenMap/scripts/3_game"
				};
            };
			
			class worldScriptModule
            {
                value="";
                files[]=
				{ 
					"ZenMap/scripts/common",
					"ZenMap/scripts/4_world" 
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]=
				{
					"ZenMap/scripts/common",
					"ZenMap/scripts/5_mission" 
				};
            };
        };
    };
};

class CfgVehicles
{
	class ItemMap;
	class ChernarusMap: ItemMap
	{
		repairableWithKits[] = { 5 };
		repairCosts[] = { 10 };
	};

	// Static map object (invisible object proxy)
	class HouseNoDestruct;
	class Zen_StaticMap: HouseNoDestruct
	{
		scope=1;
		physLayer="item_small";
		model="ZenMap\data\models\zen_staticmap.p3d";
	};
	class Zen_StaticMap_Debug: Zen_StaticMap
	{
		scope=1;
		physLayer="item_small";
		model="ZenMap\data\models\zen_staticmap_debug.p3d";
	};
};

class CfgSoundSets
{
	class Zen_MapMarker_SoundSet { soundShaders[] = { "Zen_MapMarker_SoundShader" }; volumeFactor = 1.0; };
};

class CfgSoundShaders
{
	class Zen_MapMarker_SoundShader_Base
	{
		samples[] = {};
		frequency = 1;
		range = 10;
		volume = 0.6;
	};

	class Zen_MapMarker_SoundShader : Zen_MapMarker_SoundShader_Base 
	{ 
		samples[] = 
		{ 
			{ "ZenMap\data\sounds\zen_mapmarker1", 1 },
			{ "ZenMap\data\sounds\zen_mapmarker2", 1 },
			{ "ZenMap\data\sounds\zen_mapmarker3", 1 },
			{ "ZenMap\data\sounds\zen_mapmarker4", 1 },
			{ "ZenMap\data\sounds\zen_mapmarker5", 1 },
			{ "ZenMap\data\sounds\zen_mapmarker6", 1 }
		};
	};
};