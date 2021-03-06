/**************************************************************************
 *                                                                        *
 *                         More TF2 Stocks                                *
 *                   Author: Ross Bemrose (Powerlord)                     *
 *                       Contributions by Leonardo                        *
 *                          Version: 1.0                                  *
 *                                                                        *
 **************************************************************************/

#if defined _tf2_morestocks_included
 #endinput
#endif
#define _tf2_morestocks_included

#include <sourcemod>
#include <sdktools>
#include <tf2>

// Taken from TF2ItemsInfo
enum
{
	TF2ItemSlot_Primary = 0,
	TF2ItemSlot_Secondary = 1,
	TF2ItemSlot_Sapper = 1,
	TF2ItemSlot_Melee = 2,
	TF2ItemSlot_PDA = 3,
	TF2ItemSlot_PDA1 = 3,
	TF2ItemSlot_PDA2 = 4,
	TF2ItemSlot_Building = 5,
	TF2ItemSlot_Hat = 5,
	TF2ItemSlot_Head = 5,
	TF2ItemSlot_Misc = 6,
	TF2ItemSlot_Action = 7
};

// This enum represents the order of values in DT_TFPlayerScoringDataExclusive
// Since the first offset is 4 (not 0), start the enum at 1
enum TF2ScoreData
{
	TF2Score_Captures = 1, // m_iCaptures
	TF2Score_Defenses, // m_iDefenses
	TF2Score_Kills, // m_iKilla
	TF2Score_Deaths, // m_iDeaths
	TF2Score_Suicides, // m_iSuicides
	TF2Score_Dominations, // m_iDominations
	TF2Score_Revenge, // m_iRevenge
	TF2Score_BuildingsBuilt, // m_iBuildingsBuilt
	TF2Score_BuildingDestroyed, // m_iBuildingsDestroyed
	TF2Score_Headshots, // m_iHeadshots
	TF2Score_Backstabs, // m_iBackstabs
	TF2Score_HealPoints, // m_iHealPoints
	TF2Score_Invulns, // m_iInvulns
	TF2Score_Teleports, // m_iTeleports
	TF2Score_DamageDone, // m_iDamageDone
	TF2Score_Crits, // m_iCrits
	TF2Score_ResupplyPoints, // m_iResupplyPoints
	TF2Score_KillAssists, // m_iKillAssists
	TF2Score_BonusPoints, // m_iBonusPoints
	TF2Score_Points, // m_iPoints
};

new TF2Hitpoints[] = {
	0, // TFClass_Unknown
	125, // TFClass_Scout
	125, // TFClass_Sniper
	200, // TFClass_Soldier
	175, // TFClass_DemoMan
	150, // TFClass_Medic
	300, // TFClass_Heavy
	175, // TFClass_Pyro
	125, // TFClass_Spy
	125 // TFClass_Engineer
}

/**
 * Get a class's base health
 * 
 * Note: You probably want TF2_GetMaxHealth instead.
 * 
 * This is used if you want to know what a class's base health is before attributes modify it
 */
stock TF2_GetBaseHealth(TFClassType:class)
{
	return TF2Hitpoints[class];
}
/**
 * Get a client's current health
 * This may duplicate SourceMod's GetClientHealth
 * 
 * @return client's current health
 */
stock TF2_GetHealth(client)
{
	return GetResourceProp(resourceName, "m_iHealth");
}

/**
 * Get a client's max health.
 * 
 * @param client	A client index
 * @param overheal	If true, return the max overheal health.
 * NOTE: The overheal health is the standard overheal health, not the Quick-Fix's half overheal health.
 * 
 * @return client's max health
 */
stock TF2_GetMaxHealth(client, bool:overheal=false)
{
	new String:resourceName[32];
	
	if (overheal)
	{
		strcopy(resourceName, sizeof(resourceName), "m_iMaxBuffedHealth");
	}
	else
	{
		strcopy(resourceName, sizeof(resourceName), "m_iMaxHealth");
	}
	
	return GetResourceProp(resourceName, client);
}

/**
 * Internal function for getting player resource data
 */
static stock GetResourceProp(const String:resourceName[], client)
{
	if (client < 1 || client > MaxClients)
	{
		ThrowError("Client index %d is out of bounds", client);
		return -1;
	}
	
	if (!IsClientInGame(client))
	{
		ThrowError("Client index %d is not in game", client);
		return -1;
	}
	
	new playerResource = GetPlayerResourceEntity();
	if (playerResource == -1 || !IsValidEntity(playerResource))
	{
		LogError("Player Resource Entity doesn't exist.")
		return -1;
	}

	return GetEntProp(playerResource, Prop_Send, resourceName, _, client);
}


/**
 * Return a value from a player's Score Data
 * This function exists because m_iCaptures, etc... can't be looked up directly
 * 
 * @param client	A valid client index
 * @param dataType	A TF2ScoreData type
 * 
 * @returns Value of resource or -1 if the player isn't in game
 */
stock TF2_GetScoreData(client, TF2ScoreData:dataType)
{
	if (client < 1 || client > MaxClients)
	{
		ThrowError("Client index %d is out of bounds", client);
		return -1;
	}
	
	if (!IsClientInGame(client))
	{
		ThrowError("Client index %d is not in game", client);
		return -1;
	}

	return GetEntProp(client, Prop_Send, "m_ScoreData", 4, _:dataType);
}

/**
 * Return a value from a player's Round Score Data
 * This function exists because m_iCaptures, etc... can't be looked up directly
 * 
 * @param client	A valid client index
 * @param dataType	A TF2ScoreData type
 * 
 * @returns Value of resource or -1 if the player isn't in game
 */
stock TF2_GetRoundScoreData(client, TF2ScoreData:dataType)
{
	if (client < 1 || client > MaxClients)
	{
		ThrowError("Client %d is out of bounds", client);
		return -1;
	}
	
	if (!IsClientInGame(client))
	{
		ThrowError("Client %d is not in game", client);
		return -1;
	}

	return GetEntProp(client, Prop_Send, "m_RoundScoreData", 4, _:dataType);
}

/**
 * Print to chat matching a target mask.
 * 
 * @param pattern	The target mask
 * @param format	Formatting rules
 * @param ,,,		Variable number of format parameters
 */
stock PrintToChatTarget(const String:pattern[], const String:format[], any:...)
{
	decl String:buffer[192];
	
	new targets[MaxClients];
	decl String:target_name[128];
	new bool:tn_is_ml;
	
	new targetCount = ProcessTargetString(pattern, 0, targets, MaxClients, COMMAND_FILTER_NO_IMMUNITY, target_name, sizeof(target_name), tn_is_ml);

	for (new i = 0; i < targetCount; ++i)
	{
		if (IsClientInGame(targets[i]))
		{
			SetGlobalTransTarget(targets[i]);
			VFormat(buffer, sizeof(buffer), format, 3);
			PrintToChat(i, "%s", buffer);
		}
	}
}

/**
 * Prints a message to all clients on a team
 * 
 * @param team		Team to print message to.
 * @param format	Formatting rules.
 * @param ...		Variable number of format parameters.
 * @noreturn
 */
stock PrintToChatTeam(team, const String:format[], any:...)
{
	decl String:buffer[192];
	
	for (new i = 1; i <= MaxClients; ++i)
	{
		if (IsClientInGame(i) && GetClientTeam(i) == team)
		{
			SetGlobalTransTarget(i);
			VFormat(buffer, sizeof(buffer), format, 3);
			PrintToChat(i, "%s", buffer);
		}
	}
}

/**
 * Prints a message to all clients but one in the chat area
 * 
 * @param client	Client index to skip.
 * @param format	Formatting rules.
 * @param ...		Variable number of format parameters.
 * @noreturn
 */
stock PrintToChatAllButOne(client, const String:format[], any:...)
{
	decl String:buffer[192];
	
	for (new i = 1; i <= MaxClients; ++i)
	{
		if (IsClientInGame(i) && i != client)
		{
			SetGlobalTransTarget(i);
			VFormat(buffer, sizeof(buffer), format, 3);
			PrintToChat(i, "%s", buffer);
		}
	}
}

stock PrintHintTextToTarget(const String:pattern[], const String:format[], any:...)
{
	decl String:buffer[192];
	
	new targets[MaxClients];
	decl String:target_name[128];
	new bool:tn_is_ml;
	
	new targetCount = ProcessTargetString(pattern, 0, targets, MaxClients, COMMAND_FILTER_NO_IMMUNITY, target_name, sizeof(target_name), tn_is_ml);

	for (new i = 0; i < targetCount; ++i)
	{
		if (IsClientInGame(targets[i]))
		{
			SetGlobalTransTarget(targets[i]);
			VFormat(buffer, sizeof(buffer), format, 3);
			PrintHintText(i, "%s", buffer);
		}
	}
}

stock PrintHintTextToTeam(team, const String:format[], any:...)
{
	decl String:buffer[192];
	
	for (new i = 1; i <= MaxClients; ++i)
	{
		if (IsClientInGame(i) && GetClientTeam(i) == team)
		{
			SetGlobalTransTarget(i);
			VFormat(buffer, sizeof(buffer), format, 3);
			PrintHintText(i, "%s", buffer);
		}
	}
}

stock PrintHintTextToAllButOne(client, const String:format[], any:...)
{
	decl String:buffer[192];
	
	for (new i = 1; i <= MaxClients; ++i)
	{
		if (IsClientInGame(i) && i != client)
		{
			SetGlobalTransTarget(i);
			VFormat(buffer, sizeof(buffer), format, 3);
			PrintHintText(i, "%s", buffer);
		}
	}
}
