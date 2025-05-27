modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenMap"))
			return;
		#endif

		GetZenMapConfig();

		if (GetZenMapConfig().ClientConfig.RequirePenToMark)
		{
			// If player NEEDS a pen, clear the no-pen colors from memory as we don't need to sync that shit to clients anymore.
			GetZenMapConfig().ClientConfig.NoPenColors.Clear();
		}

		ZenStaticMapsInit();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity) 
	{
		super.InvokeOnConnect(player, identity);

		#ifdef ZENMODPACK
		if (!ZenModEnabled("ZenMap"))
			return;
		#endif

		//! SEND GENERAL CONFIG IMMEDIATELY
		GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapConfig", new Param1<ref ZenMapClientSyncConfig>(GetZenMapConfig().ClientConfig), true, identity);

		PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));
		if (mapPlugin)
		{
			mapPlugin.SyncMarkers(player);
		}
	}

	void ZenStaticMapsInit()
	{
		Print("[ZenMap] ZenStaticMapsInit");
		GetZenMapStaticObjectsConfig();

		if (GetZenMapStaticObjectsConfig().DumpObjectLocations)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DumpStaticMapObjects, 20000, false);
		}
		else
		if (GetZenMapStaticObjectsConfig().SpawnMapObjects || GetZenMapStaticObjectsConfig().DebugOn)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetupZenStaticMaps, 20000, false);
		}
	}

	void SetupZenStaticMaps()
	{
		Print("[ZenMap] Setting up static map objects.");

		string mapModel = "Zen_StaticMap";
		vector objOffset;
		vector objOrient;
		float scale;

		if (GetZenMapStaticObjectsConfig().DebugOn)
			mapModel = "Zen_StaticMap_Debug";

		foreach (ZenStaticMapObject mapObject : GetZenMapStaticObjectsConfig().MapPositions)
		{
			objOffset = mapObject.Offset;
			objOrient = mapObject.Orient;
			scale = mapObject.Scale;

			vector lastSpawn = vector.Zero;

			foreach (vector loc : mapObject.Locations)
			{
				string typeName = mapObject.TypeName;
				typeName.ToLower();

				array<Object> objectsNearby = new array<Object>;
				GetGame().GetObjectsAtPosition3D(loc, 1, objectsNearby, null);

				string debugName = "";
				string className = "";

				foreach (Object obj : objectsNearby)
				{
					debugName = obj.GetDebugNameNative();
					className = obj.ClassName();

					debugName.ToLower();
					className.ToLower();

					if (className.Contains("zen_staticmap"))
					{
						continue;
					}

					if (debugName.Contains(typeName) || className.Contains(typeName))
					{
						if (vector.Distance(lastSpawn, obj.GetPosition()) > 1)
						{
							lastSpawn = obj.GetPosition();
							CreateStaticZenMap(obj, mapModel, objOffset, objOrient, scale);
						}
					}
				}
			}
		}

		GetZenMapStaticObjectsConfig().CleanUp();
		Print("[ZenMap] Done.");
	}

	static void CreateStaticZenMap(Object parentObj, string childObjType, vector offset, vector orient, float scale)
	{
		if (!parentObj)
			return;

		childObjType.ToLower();
		Object newObj = GetGame().CreateObject(childObjType, vector.Zero);
		newObj.SetPosition(parentObj.GetPosition() + offset);
		newObj.SetOrientation(parentObj.GetOrientation() + orient);
		newObj.SetScale(scale);
		newObj.Update();
	}

	void DumpStaticMapObjects()
	{
		vector centerPos = GetGame().ConfigGetVector(string.Format("CfgWorlds %1 centerPosition", GetGame().GetWorldName()));

		array<Object> objectsOnMap = new array<Object>;
		GetGame().GetObjectsAtPosition3D(centerPos, 20000, objectsOnMap, null);
		int objCount = 0;

		foreach (ZenStaticMapObject mapType : GetZenMapStaticObjectsConfig().MapPositions)
		{
			string type = mapType.TypeName;
			type.ToLower();

			// Prepare this object
			array<vector> objLocations = new array<vector>;

			// Cycle through all map objects
			for (int x = 0; x < objectsOnMap.Count(); x++)
			{
				// Prepare classname(s)
				string debugName = "";
				string className = "";

				// Get classname(s)
				debugName = objectsOnMap[x].GetDebugNameNative();
				className = objectsOnMap[x].ClassName();

				// Convert to lower
				debugName.ToLower();
				className.ToLower();

				// If map object is what we're looking for, add it to the array
				if (debugName.Contains(type) || className.Contains(type))
				{
					objLocations.Insert(objectsOnMap[x].GetPosition());
					objCount++;
				}
			}

			// Save this wood object type & all its locations
			mapType.Locations = objLocations;
		}

		// Save objects and disable object dump for next restart
		GetZenMapStaticObjectsConfig().DumpObjectLocations = false;
		GetZenMapStaticObjectsConfig().Save();

		// Clear array & delete from memory
		objectsOnMap.Clear();
		objectsOnMap = NULL;

		// Setup wood piles
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetupZenStaticMaps, 5000, false);
	}
}