// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOBPAI/GOBPAction.h"
#include "OvercomeOpponent.generated.h"

/**
 * 
 */
UCLASS()
class KFC_API UOvercomeOpponent : public UGobpAction
{
	GENERATED_BODY()

	UOvercomeOpponent()
	{
		Name = "Overcome Opponent";
		ActionType = EActionType::Picker;
		PreConditions.Add(EConditions::ObstructedDribble);
		AfterEffects.Add(EConditions::CanOvercomeOpponent);
		AfterEffects.Add(EConditions::Divider);
		AfterEffects.Add(EConditions::OvercomeOpponentFailed);
	}

	virtual TArray<EConditions> PickNextAction(UPlayerStats* Player) override;
	
};
