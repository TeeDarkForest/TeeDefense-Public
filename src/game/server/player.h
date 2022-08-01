/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_PLAYER_H
#define GAME_SERVER_PLAYER_H

// this include should perhaps be removed
#include "entities/character.h"
#include "gamecontext.h"

enum
{
	SWORD_TYPES=4,
	PICKAXE_TYPES=5,
	AXE_TYPES=5,

	LOG_AXE=0,
	COPPER_AXE,
	IRON_AXE,
	GOLD_AXE,
	DIAMOND_AXE,

	LOG_SWORD=0,
	IRON_SWORD,
	GOLD_SWORD,
	DIAMOND_SWORD,

	LOG_PICKAXE=0,
	COPPER_PICKAXE,
	IRON_PICKAXE,
	GOLD_PICKAXE,
	DIAMOND_PICKAXE,
};
// player object
class CPlayer
{
	MACRO_ALLOC_POOL_ID()

public:
	CPlayer(CGameContext *pGameServer, int ClientID, int Team, int Zomb);
	~CPlayer();

	void Init(int CID);

	void TryRespawn();
	void Respawn();
	void SetTeam(int Team, bool DoChatMsg=true);
	int GetTeam() const { return m_Team; };
	int GetCID() const { return m_ClientID; };

	void Tick();
	void PostTick();
	void Snap(int SnappingClient);

	void FakeSnap(int SnappingClient);
	
	void OnDirectInput(CNetObj_PlayerInput *NewInput);
	void OnPredictedInput(CNetObj_PlayerInput *NewInput);
	void OnDisconnect(const char *pReason);

	void KillCharacter(int Weapon = WEAPON_GAME);
	CCharacter *GetCharacter();

	const char* GetLanguage();
	void SetLanguage(const char* pLanguage);

	//---------------------------------------------------------
	// this is used for snapping so we know how we can clip the view for the player
	vec2 m_ViewPos;

	// states if the client is chatting, accessing a menu etc.
	int m_PlayerFlags;

	// used for snapping to just update latency if the scoreboard is active
	int m_aActLatency[MAX_CLIENTS];

	// used for spectator mode
	int m_SpectatorID;

	bool m_IsReady;

	//
	int m_Vote;
	int m_VotePos;
	//
	int m_LastVoteCall;
	int m_LastVoteTry;
	int m_LastChat;
	int m_LastSetTeam;
	int m_LastSetSpectatorMode;
	int m_LastChangeInfo;
	int m_LastEmote;
	int m_LastKill;

	// TODO: clean this up
	struct
	{
		char m_SkinName[64];
		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;
	} m_TeeInfos;

	int m_RespawnTick;
	int m_DieTick;
	int m_Score;
	int m_ScoreStartTick;
	bool m_ForceBalanced;
	int m_LastActionTick;
	int m_TeamChangeTick;
	struct
	{
		int m_TargetX;
		int m_TargetY;
	} m_LatestActivity;

	// network latency calculations
	struct
	{
		int m_Accum;
		int m_AccumMin;
		int m_AccumMax;
		int m_Avg;
		int m_Min;
		int m_Max;
	} m_Latency;

	int m_Authed;

	//Zomb2
	int m_SubZomb[3];//all Types, 3 times ( 0, 1, 2)
	int m_Zomb;
	///////////////////////////////
	//1 = Zaby
	//2 = Zoomer
	//3 = Zooker
	//4 = Zamer
	//5 = Zunner
	//6 = Zaster
	//7 = Zotter
	//8 = Zenade
	//9 = Flombie
	//10 = Zinja
	//11 = Zele
	//12 = Zinvis
	//13 = Zeater
	////////////////////////////////

	//Zomb2
	void DeleteCharacter();
	int GetZomb() { return m_Zomb; };
	bool GetZomb(int Zomb);
	

	// Tee Dark Forest.
	struct // knapsack
	{
		int m_Log; // Step 1
		int m_Coal; // Step 2
		int m_Copper; // Step 3
		int m_Iron; // Step 4
		int m_Gold; // Step 5
		int m_Diamond; // LAST STEP, DIAMOND!

		int m_Sword[SWORD_TYPES];
		int m_Pickaxe[PICKAXE_TYPES];
		int m_Axe[AXE_TYPES];
		// Developers (Special things. XD)
		int m_Shengyan; // This is special, he is SLUG lmao! hahahahahahah xaxaxaxaxaxaxaxaxa
		int m_Ninecloud;
		int m_EDreemurr;
		int m_XyCloud;
		int m_HGDio; // This is special one too, he is OOOOOOOOOOOOOOOO.
		int m_Jiuyuan; // half developer (?)
		int m_FFS; // it me, FlowerFell-Sans. lmao(why my name is soo long...)
	} m_Knapsack;

	void ResetKnapsack();
	
	int m_MiningType;
	int m_MiningTick;

private:
	CCharacter *m_pCharacter;
	CGameContext *m_pGameServer;

	CGameContext *GameServer() const { return m_pGameServer; }
	IServer *Server() const;

	//
	bool m_Spawning;
	int m_ClientID;
	int m_Team;

	char m_aLanguage[16];

private:
	CTuningParams m_PrevTuningParams;
	CTuningParams m_NextTuningParams;

	void HandleTuningParams(); //This function will send the new parameters if needed

public:
	CTuningParams* GetNextTuningParams() { return &m_NextTuningParams; };
};

#endif
