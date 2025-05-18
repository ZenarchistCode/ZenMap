modded class Pen_ColorBase
{
    static const int ZENMAP_ACTION_RPC = 6901082;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if (!GetGame().IsDedicatedServer())
            return;

        if (rpc_type != ZENMAP_ACTION_RPC)
            return;

        if (GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent == 0)
            return;
        
        if (GetQuantityMax() > 1)
        {
            AddQuantity(Math.AbsFloat(GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent) * -1);
        }
        else 
        {
            float dmgPercent = Math.AbsFloat(GetZenMapConfig().ClientConfig.PenMarkerConsumeQuantityPercent) / 100;
            AddHealth(GetMaxHealth() * dmgPercent * -1);
        }
    }
}