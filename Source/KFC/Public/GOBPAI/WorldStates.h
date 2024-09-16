// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFC/Public/GOBPAI/EConditions.h"
#include "WorldStates.generated.h"


/**
 * 
 */

USTRUCT(BlueprintType)
struct KFC_API FWorldState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GOBP")
	EConditions Key;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GOBP")
	int32 Value;

	

	FWorldState()
	{
		Key = EConditions::None;
		Value = 0;
	}

	FWorldState(const EConditions& InKey, const int32& InValue)
	{
		Key = InKey;
		Value = InValue;
	}



	bool operator==(const FWorldState& Other) const
	{
		return Key == Other.Key;
	}

	
};

//Required for TSet
FORCEINLINE uint32 GetTypeHash(const FWorldState& WorldState)
{
	return GetTypeHash(WorldState.Key);
}


class KFC_API FWorldStates
{
public:
	FWorldStates()
	{
		States = TMap<EConditions, int32>();
	}

	~FWorldStates();

	TMap<EConditions, int32> States;

	bool HasState(const EConditions& Key) const;
	void AddState(const EConditions& Key, const int32& Value);
	void ModifyState(const EConditions& Key, const int32& Value);
	void RemoveState(const EConditions& Key);
	void SetState(const EConditions& Key, const int32& Value);
	TMap<EConditions, int32> GetStates() const;

	static TArray<FWorldState> TranslateMapToTArray(const TMap<EConditions, int32>& InStates);
	static TMap<EConditions, int32> TranslateTArrayToMap(const TArray<FWorldState>& InStates);
	static TArray<EConditions> ExtractConditions(const TArray<FWorldState>& InStates);
	

};
