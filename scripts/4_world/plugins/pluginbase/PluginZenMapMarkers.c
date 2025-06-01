class PluginZenMapMarkers extends PluginBase
{
    protected ref array<ref MapMarker> m_MapMarkers;

    //                uid,    marker array
    protected ref map<string, ref array<ref MapMarker>> m_PlayerSpecificMapMarkers;

	override void OnInit()
    {
        #ifdef ZENMODPACK
        ZMPrint("[PluginZenMapMarkers] :: OnInit");
        #else
        Print("[PluginZenMapMarkers] :: OnInit");
        #endif
        
        m_MapMarkers = new array<ref MapMarker>;
        m_PlayerSpecificMapMarkers = new map<string, ref array<ref MapMarker>>;

        AddStaticServerMarks();
    }

    void AddStaticServerMarks()
    {
        if (!GetGame().IsDedicatedServer())
            return;

        ZenMapMarker serverMarker;

        for (int i = 0; i < GetZenMapConfig().ServerMapMarkers.Count(); i++)
        {
            serverMarker = GetZenMapConfig().ServerMapMarkers.Get(i);
            m_MapMarkers.Insert(new MapMarker(serverMarker.Position, serverMarker.Name, serverMarker.Color, GetIconIndex(serverMarker.Icon)));
        }
    }

    override void OnDestroy()
    {
        m_MapMarkers.Clear();
    }

    int GetIconIndex(string iconFilePath)
    {
        string checkPath;

        iconFilePath.ToLower();
        iconFilePath.Replace("/", "");
        iconFilePath.Replace("\\", "");

        for (int i = 0; i < GetZenMapConfig().ClientConfig.IconFiles.Count(); i++)
        {
            checkPath = GetZenMapConfig().ClientConfig.IconFiles.Get(i);
            checkPath.ToLower();
            checkPath.Replace("/", "");
            checkPath.Replace("\\", "");

            if (checkPath == iconFilePath)
                return i;
        }

        return 0;
    }

    array<ref MapMarker> GetMapMarkers()
    {
        return m_MapMarkers;
    }

    map<string, ref array<ref MapMarker>> GetPlayerMapMarkers()
    {
        return m_PlayerSpecificMapMarkers;
    }

    MapMarker AddMarker(MapMarker marker)
    {
        m_MapMarkers.Insert(marker);
        ResyncMarkers();
        Print("[ZenMapPlugin] Added server-side marker " + marker.GetMarkerText() + " @ " + marker.GetMarkerPos());
        return marker;
    }

    //! SERVER STATIC MARKERS

    bool RemoveMarkerByIndex(int index)
    {
        if (index < 0 || index >= m_MapMarkers.Count())
            return false;

        m_MapMarkers.Remove(index);
        ResyncMarkers();
        return true;
    }

    bool RemoveMarkerByName(string name)
    {
        name.ToLower();
        string markerText;

        for (int i = 0; i < m_MapMarkers.Count(); i++)
        {
            markerText = m_MapMarkers.Get(i).GetMarkerText();
            markerText.ToLower();

            if (markerText == name)
            {
                m_MapMarkers.Remove(i);
                ResyncMarkers();
                return true;
            }
        }

        return false;
    }

    bool RemoveMarkerByPosition(vector pos)
    {
        for (int i = 0; i < m_MapMarkers.Count(); i++)
        {
            if (m_MapMarkers.Get(i).GetMarkerPos() == pos)
            {
                m_MapMarkers.Remove(i);
                ResyncMarkers();
                return true;
            }
        }

        return false;
    }

    bool RemoveMarker(MapMarker marker)
    {
        MapMarker checkMarker;
        string checkMarkerText;
        string markerText = marker.GetMarkerText();
        markerText.ToLower();

        for (int i = 0; i < m_MapMarkers.Count(); i++)
        {
            checkMarker = m_MapMarkers.Get(i);

            checkMarkerText = checkMarker.GetMarkerText();
            checkMarkerText.ToLower();

            if (checkMarkerText != markerText)
                continue;

            if (checkMarker.GetMarkerPos() != marker.GetMarkerPos())
                continue;

            if (checkMarker.GetMarkerColor() != marker.GetMarkerColor())
                continue;

            if (checkMarker.GetMarkerIcon() != marker.GetMarkerIcon())
                continue;

            m_MapMarkers.Remove(i);
            ResyncMarkers();
            return true;
        }

        return false;
    }

    void RemoveAllMarkers(bool removeStaticServerMarkers = false)
    {
        m_MapMarkers.Clear();

        if (!removeStaticServerMarkers)
        {
            AddStaticServerMarks();
        }

        ResyncMarkers();
    }

    //! PLAYER SPECIFIC MARKERS

    bool AddMarker(PlayerBase player, MapMarker marker)
    {
        if (!player || !player.GetIdentity() || !marker)
            return false;

        return AddMarker(player.GetIdentity(), marker);
    }

    bool AddMarker(PlayerIdentity identity, MapMarker marker)
    {
        if (!identity || !marker)
            return false;

        array<ref MapMarker> tempArray = new array<ref MapMarker>;
        string uid = identity.GetId();

        if (!m_PlayerSpecificMapMarkers.Find(uid, tempArray))
        {
            m_PlayerSpecificMapMarkers.Insert(uid, tempArray);
        }

        // Array was not found in player marker map<>, which means tempArray is now nullified and requires reset
        if (!tempArray)
        {
            tempArray = new array<ref MapMarker>;
        }

        if (GetMarker(identity, marker) != NULL)
        {
            return false; // already exists
        }

        tempArray.Insert(marker);
        m_PlayerSpecificMapMarkers.Set(uid, tempArray);

        SyncMarkers(identity);
        Print("[ZenMapPlugin] Added player marker " + marker.GetMarkerText() + " @ " + marker.GetMarkerPos() + " for " + uid);
        
        return true;
    }

    bool RemoveMarkerByIndex(PlayerBase player, int index)
    {
        if (!player || !player.GetIdentity() || index < 0 || index >= m_MapMarkers.Count())
            return false;

        string uid = player.GetIdentity().GetId();
        array<ref MapMarker> tempArray = new array<ref MapMarker>;

        if (!m_PlayerSpecificMapMarkers.Find(uid, tempArray))
        {
            // Player has no map markers. Return true or false here? What a philosophical conundrum..
            return true;
        }

        tempArray.Remove(index);
        m_PlayerSpecificMapMarkers.Set(uid, tempArray);

        SyncMarkers(player);
        return true;
    }

    bool RemoveMarkerByName(PlayerBase player, string name)
    {
        if (!player || !player.GetIdentity())
            return false;

        string uid = player.GetIdentity().GetId();
        array<ref MapMarker> tempArray = new array<ref MapMarker>;
        
        if (!m_PlayerSpecificMapMarkers.Find(uid, tempArray))
        {
            return true;
        }

        name.ToLower();
        string markerText;

        for (int i = 0; i < tempArray.Count(); i++)
        {
            markerText = tempArray.Get(i).GetMarkerText();
            markerText.ToLower();

            if (markerText == name)
            {
                tempArray.Remove(i);

                m_PlayerSpecificMapMarkers.Set(uid, tempArray);
                SyncMarkers(player);
                return true;
            }
        }

        return false;
    }

    bool RemoveMarkerByPosition(PlayerBase player, vector pos)
    {
        if (!player || !player.GetIdentity())
            return false;

        string uid = player.GetIdentity().GetId();
        array<ref MapMarker> tempArray = new array<ref MapMarker>;

        if (!m_PlayerSpecificMapMarkers.Find(uid, tempArray))
        {
            return true;
        }

        for (int i = 0; i < tempArray.Count(); i++)
        {
            if (tempArray.Get(i).GetMarkerPos() == pos)
            {
                tempArray.Remove(i);

                m_PlayerSpecificMapMarkers.Set(uid, tempArray);
                SyncMarkers(player);
                return true;
            }
        }

        return false;
    }

    bool RemoveMarker(PlayerBase player, MapMarker marker)
    {
        if (!player || !player.GetIdentity())
            return false;

        string uid = player.GetIdentity().GetId();
        array<ref MapMarker> tempArray = new array<ref MapMarker>;
        
        if (!m_PlayerSpecificMapMarkers.Find(uid, tempArray))
        {
            return true;
        }

        MapMarker checkMarker;
        string checkMarkerText;
        string markerText = marker.GetMarkerText();
        markerText.ToLower();

        for (int i = 0; i < tempArray.Count(); i++)
        {
            checkMarker = tempArray.Get(i);

            checkMarkerText = checkMarker.GetMarkerText();
            checkMarkerText.ToLower();

            if (checkMarkerText != markerText)
                continue;

            if (checkMarker.GetMarkerPos() != marker.GetMarkerPos())
                continue;

            if (checkMarker.GetMarkerColor() != marker.GetMarkerColor())
                continue;

            if (checkMarker.GetMarkerIcon() != marker.GetMarkerIcon())
                continue;

            tempArray.Remove(i);

            m_PlayerSpecificMapMarkers.Set(uid, tempArray);
            SyncMarkers(player);
            return true;
        }

        return false;
    }

    MapMarker GetMarker(PlayerBase player, MapMarker marker)
    {
        if (!player || !player.GetIdentity())
            return NULL;

        return GetMarker(player.GetIdentity(), marker);
    }

    MapMarker GetMarker(PlayerIdentity identity, MapMarker marker)
    {
        if (!identity)
            return NULL;

        string uid = identity.GetId();

        MapMarker checkMarker;
        string checkMarkerText;
        string markerText = marker.GetMarkerText();
        markerText.ToLower();

        array<ref MapMarker> tempArray = new array<ref MapMarker>;
        
        if (!m_PlayerSpecificMapMarkers.Find(uid, tempArray))
        {
            return NULL;
        }

        for (int i = 0; i < tempArray.Count(); i++)
        {
            checkMarker = tempArray.Get(i);

            checkMarkerText = checkMarker.GetMarkerText();
            checkMarkerText.ToLower();

            if (checkMarkerText != markerText)
                continue;

            if (checkMarker.GetMarkerPos() != marker.GetMarkerPos())
                continue;

            if (checkMarker.GetMarkerColor() != marker.GetMarkerColor())
                continue;

            if (checkMarker.GetMarkerIcon() != marker.GetMarkerIcon())
                continue;

            return tempArray.Get(i);
        }

        return NULL;
    }

    void ClearPlayerMarkers(PlayerBase player)
    {
        if (!player || !player.GetIdentity())
            return;

        ClearPlayerMarkers(player.GetIdentity());
    }

    void ClearPlayerMarkers(notnull PlayerIdentity identity)
    {
        if (GetPlayerMapMarkers().Contains(identity.GetId()))
		{
            Print("[ZenMapPlugin] Clearing all markers for player " + identity.GetId());
			GetPlayerMapMarkers().Get(identity.GetId()).Clear();
		}
    }

    //! SYNC

    void ResyncMarkers()
    {
        if (!GetGame().IsDedicatedServer())
            return;

        PlayerBase player;
        array<Man> onlinePlayers = new array<Man>;
		GetGame().GetPlayers(onlinePlayers);

        foreach (Man man : onlinePlayers)
        {
            player = PlayerBase.Cast(man);
            if (player)
            {
                SyncMarkers(player);
            }
        }
    }

    void SyncMarkers(PlayerBase player)
    {
        if (!player || !player.GetIdentity())
            return;

        SyncMarkers(player.GetIdentity());
    }

    void SyncMarkers(notnull PlayerIdentity identity)
    {
        if (!GetGame().IsDedicatedServer())
            return;

        if (CfgGameplayHandler.GetUse3DMap())
            return;

        int i;
        array<ref MapMarker> tempServerArray = new array<ref MapMarker>;
        array<ref MapMarker> tempPlayerArray = new array<ref MapMarker>;

        for (i = 0; i < GetMapMarkers().Count(); i++)
        {
            tempServerArray.Insert(GetMapMarkers().Get(i));
        }

        if (m_PlayerSpecificMapMarkers.Find(identity.GetId(), tempPlayerArray))
        {
            for (i = 0; i < tempPlayerArray.Count(); i++)
            {
                tempServerArray.Insert(tempPlayerArray.Get(i));
            }
        }

        Print("[ZenMap] Syncing " + tempServerArray.Count() + " server-side array markers for player " + identity.GetId());
        GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapMarkers", new Param1<ref array<ref MapMarker>>(tempServerArray), true, identity);
    }
}