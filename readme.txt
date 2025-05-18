What Is This?

This mod adds a few extra immersive and quality of life features to the vanilla map functionality.

Features include:
- Adding/removing map markers
- Server-side config for adding static markers
- Plugin code for admins to add dynamic map markers (eg. missions, quests)
- Full server admin config to enable/disable certain features
- Uses mostly vanilla code so lightweight, robust & no wipe required to add/remove mod

Config Explanation:

TakeMapToHandsOnHotkey:
Enable/disable when player presses M the character takes map into hands & opens it

MapsRepairableWithDuctTape:
Enable/disable repairing damaged maps using duct tape

SaveMarkersOntoMapItem:
Enable/disable ability to save markers directly onto map item. If enabled, then map markers can be added to the map item itself - meaning that players can share maps in-game with markers persistent on the map itself, useful for treasure maps, organizing combined activities like raids etc with other players, all in-game. Just be careful marking valuable locations - if you lose your map or get killed other players can see your markers! If disabled then the markers are only saved client-side.

AllowPlayerToDecide:
Enable/disable players being able to choose where the map marker is saved (only works if SaveMarkersOntoMapItem setting is enabled). So if they want to mark treasure maps etc they can, while keeping their personal markers private. A marker added to the map item while this setting is enabled is tagged with a * symbol to differentiate them from private client-side markers.

AllowCopyMapToMap:
Enable/disable the ability to copy one map's markers onto another map item - only works if SaveMarkersOntoMapItem is enabled.

RequirePenToMark:
Enable/disable the requirement of a non-ruined pen in player inventory to be able to edit markers on maps or copy markers if AllowCopyMapToMap setting is enabled.

MaxMarkerCountPerMapItem:
Set the maximum marker limit per map item. Only applies if SaveMarkersOntoMapItem is enabled, to prevent server sync issues if a player goes silly with map markers on their map item. Client-side has no marker limit.

PenMarkerConsumeQuantityPercent:
How much quantity OR health to take from the pen for each map action. If the pen has a quantity bar (eg. you're running my ZenNotes mod) then the pen's quantity will drain. Otherwise its health will drain instead. This is the total % of health/quantity to drain - so 10% = 10 uses of a Pristine pen.

DamageMapOnOpenPercent:
How much to damage the map whenever a player opens it. Disabled by default. Also a % - so 1% would mean 100 opens of a Pristine map before ruined. I recommend enabling MapsRepairableWithDuctTape if you use this setting.

NoPenColors:
The list of usable colors for map markers if a pen is NOT required to mark maps (RequirePenToMark is disabled).

IconFiles:
The list of usable map marker icons, moddable to easily add more markers if desired.

ServerMapMarkers:
A list of static server-side markers for admins who want to add certain POI's to the vanilla map.

Custom plugin code:
To add or remove a map marker using script code, you can do this:

// Generate map markers 
PluginZenMapMarkers mapPlugin = PluginZenMapMarkers.Cast(GetPlugin(PluginZenMapMarkers));
if (mapPlugin)
{
	mapPlugin.AddMarker(new MapMarker("13154.231445 0.224814 10102.046875", "Berezino Transfer", -65536, GetZenMapConfig().FileToArrayIndex("ZenMap/data/icons/boat.paa")));
	mapPlugin.AddMarker(new MapMarker("6501.937500 -0.019806 2191.357422", "Chernogorsk Transfer", -65536, GetZenMapConfig().FileToArrayIndex("ZenMap/data/icons/boat.paa")));
	mapPlugin.AddMarker(new MapMarker("8229.091797 471.185577 9031.171875", "Altar Transfer", -65536, GetZenMapConfig().FileToArrayIndex("ZenMap/data/icons/heli.paa")));
}

// Remove map markers 
if (mapPlugin)
{
	mapPlugin.RemoveMarkerByIndex(int index) // use array index (eg. 0 = Berezino marker)
	mapPlugin.RemoveMarkerByName(string name) // remove by name (eg. "Berezino Transfer")
	mapPlugin.RemoveMarkerByPosition(vector pos) // remove by vector position (eg. "13154.231445 0.224814 10102.046875")
	mapPlugin.RemoveMarker(MapMarker marker) // remove by marker itself.
}

// Interact with map markers 
if (mapPlugin)
{
	mapPlugin.GetIconIndex("filePath") // get icon index of IconList array in config
	mapPlugin.GetMapMarkers() // get array<ref MapMarker> of map markers 
	mapPlugin.RemoveAllMarkers(bool removeStaticServerMarkers = false) // remove all custom-added map markers (except ones added by ServerMapMarkers list)
}
  
Repack & Source Code:

You can repack this mod if you like, and do anything else you want with it for that matter. Just keep in mind my future updates won't be applied so make sure to check back for new versions if you notice any bugs. The source code is on my GitHub at www.zenarchist.io

Buy Me A Coffee:

All my mods are free and open source, but it takes an enormous amount of time to put some of these mods together. If I've helped you out, please consider helping me buy my next coffee! I don't expect it, but I very much appreciate it.

https://buymeacoffee.com/zenarchist

Enjoy!