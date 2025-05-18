class PluginZenMapMarkers extends PluginBase
{
    protected ref array<ref MapMarker> m_MapMarkers;

	override void OnInit()
    {
        #ifdef ZENMODPACK
        ZMPrint("[PluginZenMapMarkers] :: OnInit");
        #else
        Print("[PluginZenMapMarkers] :: OnInit");
        #endif
        
        m_MapMarkers = new array<ref MapMarker>;

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

    MapMarker AddMarker(MapMarker marker)
    {
        Print("[ZenMapPlugin] Adding server-side marker " + marker.GetMarkerText() + " @ " + marker.GetMarkerPos());
        m_MapMarkers.Insert(marker);
        ResyncMarkers();
        return marker;
    }

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

    void SyncMarkers(notnull PlayerBase player)
    {
        if (!GetGame().IsDedicatedServer())
            return;

        if (CfgGameplayHandler.GetUse3DMap())
            return;

        if (!player.GetIdentity())
            return;

        GetRPCManager().SendRPC("ZenMod_RPC", "RPC_ReceiveZenMapMarkers", new Param1<ref array<ref MapMarker>>(m_MapMarkers), true, player.GetIdentity());
    }
}