// Fill out your copyright notice in the Description page of Project Settings.

#include "KFC/Public/GOBPAI/WorldStates.h"



FWorldStates::~FWorldStates()
{
	// Destructor implementation
}

bool FWorldStates::HasState(const EConditions& Key) const
{
	return States.Contains(Key);
}

void FWorldStates::AddState(const EConditions& Key, const int32& Value)
{
	States.Add(Key, Value);
}

void FWorldStates::ModifyState(const EConditions& Key, const int32& Value)
{
	if (States.Contains(Key))
	{
		States[Key] += Value;
		if (States[Key] < 0)
		{
			States.Remove(Key);
		}
	}
	else
	{
		States.Add(Key, Value);
	}
}

void FWorldStates::RemoveState(const EConditions& Key)
{
	if (States.Contains(Key)) States.Remove(Key);
}

void FWorldStates::SetState(const EConditions& Key, const int32& Value)
{
	if (States.Contains(Key))
	{
		States[Key] = Value;
	}
	else
	{
		States.Add(Key, Value);
	}
}

TMap<EConditions, int32> FWorldStates::GetStates() const
{
	return States;
}

TArray<FWorldState> FWorldStates::TranslateMapToTArray(const TMap<EConditions, int32>& InStates)
{
	TArray<FWorldState> StatesToReturn;
	for (const auto& State : InStates)
	{
		StatesToReturn.Add(FWorldState(State.Key, State.Value));
	}
	return StatesToReturn;
}

TMap<EConditions, int32> FWorldStates::TranslateTArrayToMap(const TArray<FWorldState>& InStates)
{
	TMap<EConditions, int32> StatesToReturn;
	for (const auto& State : InStates)
	{
		//StatesToReturn.Add(State.Key, State.Value);
		StatesToReturn.Add(State.Key, 0);
	}
	return StatesToReturn;
}

TArray<EConditions> FWorldStates::ExtractConditions(const TArray<FWorldState>& InStates)
{
	TArray<EConditions> ConditionsToReturn;
	for (const auto& State : InStates)
	{
		ConditionsToReturn.Add(State.Key);
	}
	return ConditionsToReturn;
}
