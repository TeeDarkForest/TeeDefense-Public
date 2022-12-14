#include "turret.h"
#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include <new>
#include <engine/shared/config.h>
#include "lightning.h"
#include "plasma.h"
#include <base/math.h>
#define RAD 0.017453292519943295769236907684886f
CTurret::CTurret(CGameWorld *pGameWorld, vec2 Pos, int Owner, int Type, int Radius, int Lifes)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_TURRET)
{
    m_Pos = Pos;
    m_Owner = Owner;
    m_Type = Type;
    m_Radius = Radius;
    m_FireDelay = 0;
    m_Lifes = Lifes;

    for (unsigned i = 0; i < sizeof(m_IDs) / sizeof(int); i ++)
        m_IDs[i] = Server()->SnapNewID();

    for (unsigned i = 0; i < sizeof(m_aIDs) / sizeof(int); i ++)
        m_aIDs[i] = Server()->SnapNewID();

    m_ID = Server()->SnapNewID();

    m_ProximityRadius = PickupPhysSizeS;

    GameWorld()->InsertEntity(this);
}

CTurret::~CTurret()
{
    for (unsigned i = 0; i < sizeof(m_IDs) / sizeof(int); i ++)
	{
		if(m_IDs[i] >= 0){
			Server()->SnapFreeID(m_IDs[i]);
			m_IDs[i] = -1;
		}
	}

    for (unsigned i = 0; i < sizeof(m_aIDs) / sizeof(int); i ++)
	{
		if(m_aIDs[i] >= 0){
			Server()->SnapFreeID(m_IDs[i]);
			m_aIDs[i] = -1;
		}
	}
    if(m_ID >= 0){
        Server()->SnapFreeID(m_ID);
        m_ID = -1;
    }
}

int CTurret::GetOwner()
{
    return m_Owner;
}
int CTurret::GetRadius()
{
    return m_Radius;
}
int CTurret::GetType()
{
    return m_Type;
}

int CTurret::GetSnapType()
{
    switch (GetType())
    {
    case TURRET_GUN:
        return WEAPON_GUN;
        break;

    case TURRET_FOLLOW_GRENADE:
        return WEAPON_GRENADE;
        break;
    
    case TURRET_LASER:
        return WEAPON_RIFLE;
        break;
    
    default:
        break;
    }
}

void CTurret::Tick()
{
    for(CCharacter *pChr = (CCharacter*) GameWorld()->FindFirst(CGameWorld::ENTTYPE_CHARACTER); pChr; pChr = (CCharacter *)pChr->TypeNext())
    {
        if(!pChr->IsAlive() || !pChr->GetPlayer()->GetZomb())
            continue;

        TargetPos = pChr->m_Pos;

        if(GameServer()->Collision()->IntersectLine(m_Pos, TargetPos, NULL, NULL))
            continue;

        float Len = distance(TargetPos, m_Pos);
        vec2 Direction = normalize(TargetPos - m_Pos);
        if(Len < pChr->m_ProximityRadius+GameServer()->Tuning()->m_LaserReach)
        {
            switch (GetType())
            {
            case TURRET_GUN:
                if(m_FireDelay <= 0)
                {
                    new CProjectile(GameWorld(), WEAPON_GUN, GetOwner(), m_Pos, Direction, 5000, 1, false, 10, SOUND_GRENADE_EXPLODE, WEAPON_GUN);
                    m_FireDelay = 1;
                    m_Lifes--;
                }
                break;

            case TURRET_SHOTGUN:
                if(m_FireDelay <= 0)
                {
                    int ShotSpread = 3;

        			for(int i = -ShotSpread; i <= ShotSpread; ++i)
        			{
        				float Spreading[20 * 2 + 1];
        	    		for (int i = 0; i < 20 * 2 + 1; i++)
		    	        	Spreading[i] = -1.2f + 0.06f * i;
        				
                        float a = GetAngle(Direction);
        				a += Spreading[i+2];
        				float v = 1-(absolute(i)/(float)ShotSpread);
        				float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
        				CProjectile *pProj = new CProjectile(GameWorld(), WEAPON_SHOTGUN,
        					GetOwner(),
        					m_Pos,
        					vec2(cosf(a), sinf(a))*Speed,
        					(int)(Server()->TickSpeed()*GameServer()->Tuning()->m_ShotgunLifetime),
        					1, 0, 0, SOUND_GRENADE_EXPLODE, WEAPON_SHOTGUN);
        			}
                    m_FireDelay = 5;
                    m_Lifes--;
                }
                break;
            case TURRET_SHOTGUN_2077:
                if(m_FireDelay <= 0)
                {
                    int ShotSpread = 7;

        			for(int i = -ShotSpread; i <= ShotSpread; ++i)
        			{
        				float Spreading[20 * 2 + 1];
        	    		for (int i = 0; i < 20 * 2 + 1; i++)
		    	        	Spreading[i] = -1.2f + 0.06f * i;
        				
                        float a = GetAngle(Direction);
        				a += Spreading[i+2];
        				float v = 1-(absolute(i)/(float)ShotSpread);
        				float Speed = mix((float)GameServer()->Tuning()->m_ShotgunSpeeddiff, 1.0f, v);
        				new CPlasma(GameWorld(), m_Pos, GetOwner(), pChr->GetCID(), vec2(cosf(a), sinf(a))*Speed, false, false, WEAPON_SHOTGUN);
        			}
                    m_FireDelay = 5;
                    m_Lifes--;
                }
                break;
            case TURRET_LASER:
                if(m_FireDelay <= 0)
                {
                    new CLaser(GameWorld(), m_Pos, Direction, GameServer()->Tuning()->m_LaserReach, GetOwner());
                    m_FireDelay = 10;
                    m_Lifes--;
                }
                break;
            case TURRET_LASER_2077:
                if(m_FireDelay <= 0)
                {
	    			new CLightning(GameWorld(), m_Pos, Direction, 100, 400, GetOwner(), 10);
                    m_FireDelay = 5;
                    m_Lifes--;
                }
                break;
            case TURRET_FOLLOW_GRENADE:
                if(m_FireDelay <= 0)
                {
                    new CPlasma(GameWorld(), m_Pos, GetOwner(), pChr->GetCID(), Direction, false, true, WEAPON_GRENADE);
                    m_FireDelay = 50;
                }
                break;
            case TURRET_FREEZE_GUN:
                if(m_FireDelay <= 0)
                {
                    new CProjectile(GameWorld(), WEAPON_GUN, GetOwner(), m_Pos, Direction, (int)(Server()->TickSpeed()*GameServer()->Tuning()->m_GunLifetime*3), 5, false, 4, SOUND_GRENADE_FIRE, WEAPON_GUN, true);
                    m_FireDelay = 100;
                }
                break;

            default:
                break;

            }
        }
    }

    if(m_FireDelay >= 0)
        m_FireDelay--;
    
    if(m_Lifes <= 0 || !GameServer()->m_apPlayers[GetOwner()])
    {
        Reset();
    }
}

void CTurret::Reset()
{
    GameServer()->m_World.DestroyEntity(this);
}

void CTurret::Snap(int SnappingClient)
{
    if(NetworkClipped(SnappingClient, m_Pos))
		return;

    if ( m_Degres + 1 < 360 )
		m_Degres += 1;
	else
		m_Degres = 0;

    int aIDSize = sizeof(m_aIDs) / sizeof(int);

    float AngleStep = 2.0f * pi / NumSide;

    for(int i=0; i<NumSide; i++)
	{
	    vec2 PartPosStart = m_Pos + vec2(GetRadius() * cos(AngleStep*i), GetRadius() * sin(AngleStep*i));
	    vec2 PartPosEnd = m_Pos + vec2(GetRadius() * cos(AngleStep*(i+1)), GetRadius() * sin(AngleStep*(i+1)));
	    CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(NETOBJTYPE_LASER, m_IDs[i], sizeof(CNetObj_Laser)));
	    if(!pObj)
	    	return;
	    pObj->m_X = (int)PartPosStart.x;
	    pObj->m_Y = (int)PartPosStart.y;
	    pObj->m_FromX = (int)PartPosEnd.x;
	    pObj->m_FromY = (int)PartPosEnd.y;
	    pObj->m_StartTick = Server()->Tick();
	}

    switch(GetType())
    {
    case TURRET_GUN:
    case TURRET_FOLLOW_GRENADE:
    case TURRET_LASER:
    {
        CNetObj_Projectile *pObj = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_ID, sizeof(CNetObj_Projectile)));
	    if(!pObj)
	    	return;
        pObj->m_X = m_Pos.x;
        pObj->m_Y = m_Pos.y;
        pObj->m_Type = GetSnapType();
        pObj->m_StartTick = Server()->Tick();
    }
    break;
    case TURRET_SHOTGUN:
    case TURRET_SHOTGUN_2077:
    {
        for(int i = 0; i < aIDSize; i++)
        {
            if(GetType() == TURRET_SHOTGUN_2077)
            {
                float a = frandom() * 360 * RAD;
                new CLightning(GameWorld(), m_Pos, vec2(cosf(a), sinf(a)), 1, 50, GetOwner(), 5, 9);
            }

            CNetObj_Projectile *pEff = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_aIDs[i], sizeof(CNetObj_Projectile)));
	        if(!pEff)
	        	return;
            vec2 a = m_Pos + (GetDir((m_Degres-i*22.5)*pi/180) * 48);
            pEff->m_X = a.x;
            pEff->m_Y = a.y;
            pEff->m_Type = WEAPON_SHOTGUN;
            pEff->m_StartTick = Server()->Tick();
        }
        break;
    }
    case TURRET_LASER_2077:
    {
        for (int i = 0; i < aIDSize; i++)
        {
            float a = frandom() * 360 * RAD;
            new CLightning(GameWorld(), m_Pos, vec2(cosf(a), sinf(a)), 1, 50, GetOwner(), 5, 9);
        }
        break;
    }
    case TURRET_FREEZE_GUN:
    {
        for(int i = 0; i < aIDSize; i++)
        {
            CNetObj_Projectile *pEff = static_cast<CNetObj_Projectile *>(Server()->SnapNewItem(NETOBJTYPE_PROJECTILE, m_aIDs[i], sizeof(CNetObj_Projectile)));
	        if(!pEff)
	        	return;
            vec2 a = m_Pos + (GetDir((m_Degres-i*22.5)*pi/180*22.5) * 48);
            pEff->m_X = a.x;
            pEff->m_Y = a.y;
            pEff->m_Type = WEAPON_GUN;
            pEff->m_StartTick = Server()->Tick();
        }
    }
    default:
        break;
    }
}