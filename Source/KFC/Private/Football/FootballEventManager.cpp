// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/FootballEventManager.h"
#include "Football/PlayerStats.h"

UFootballEventManager* UFootballEventManager::Instance = nullptr;

UFootballEventManager* UFootballEventManager::GetInstance()
{
	if (!Instance)
	{
		Instance = NewObject<UFootballEventManager>();
		Instance->AddToRoot(); // Prevent garbage collection
	}
	return Instance;
}

void UFootballEventManager::RegisterPlayer(UPlayerStats* Player)
{
	if (Player && !RegisteredPlayers.Contains(Player))
	{
		RegisteredPlayers.Add(Player);
	}
}

void UFootballEventManager::UnregisterPlayer(UPlayerStats* Player)
{
	if (Player)
	{
		RegisteredPlayers.Remove(Player);
	}
}

void UFootballEventManager::BroadcastBallOwned(const UPlayerStats* Player)
{
	for (const UPlayerStats* RegisteredPlayer : RegisteredPlayers)
	{
		RegisteredPlayer->OnBallOwned.Broadcast(Player);
	}
}