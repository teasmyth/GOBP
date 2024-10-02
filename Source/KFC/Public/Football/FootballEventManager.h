// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FootballEventManager.generated.h"

/**
 * 
 */
UCLASS()
class KFC_API UFootballEventManager : public UObject
{
	GENERATED_BODY()

public:

	static UFootballEventManager* GetInstance();

	void RegisterPlayer(class UPlayerStats* Player);
	void UnregisterPlayer(class UPlayerStats* Player);
	void BroadcastBallOwned(const UPlayerStats* Player);

	
private:
	static UFootballEventManager* Instance;
	TArray<class UPlayerStats*> RegisteredPlayers;
	
};
