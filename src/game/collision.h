/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <base/vmath.h>

class CCollision
{
	class CTile *m_pTiles;
	int m_Width;
	int m_Height;
	class CLayers *m_pLayers;

	bool IsTileSolid(int x, int y) const;
	int GetTile(int x, int y) const;

public:
	enum
	{
		COLFLAG_SOLID=1,
		COLFLAG_DEATH=2,
		COLFLAG_NOHOOK=4,
	};

	CCollision();
	void Init(class CLayers *pLayers);
	bool CheckPoint(float x, float y) const { return IsTileSolid(round_to_int(x), round_to_int(y)); }
	bool CheckPoint(vec2 Pos) const { return CheckPoint(Pos.x, Pos.y); }
	int GetCollisionAt(float x, float y) const { return GetTile(round_to_int(x), round_to_int(y)); }
	int GetWidth() const { return m_Width; };
	int GetHeight() const { return m_Height; };
	int IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	void MovePoint(vec2 *pInoutPos, vec2 *pInoutVel, float Elasticity, int *pBounces) const;
	void MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity) const;
	bool TestBox(vec2 Pos, vec2 Size) const;

	//Zomb2
	bool CheckTiles(vec2 Pos, int Deepness);
	bool CheckParable(vec2 Pos, int Wideness, int Direction);
	bool CheckPlatform(vec2 Pos, int Deepness);
    bool IntersectTile(vec2 Pos0, vec2 Pos1);

	// Tee Dark Forest.
	int GetRealTile(int x, int y)	{return unpow(GetTile(x, y), 2)+1;}
	int GetRealTile(vec2 Pos)	{return GetRealTile(Pos.x, Pos.y);}
};

#endif
