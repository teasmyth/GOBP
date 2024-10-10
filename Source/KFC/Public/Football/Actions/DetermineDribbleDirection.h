// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOBPAI/GOBPAction.h"
#include "DetermineDribbleDirection.generated.h"

/**
 * 
 */
UCLASS()
class KFC_API UDetermineDribbleDirection : public UGobpAction
{
	GENERATED_BODY()

	UDetermineDribbleDirection()
	{
		Name = "Determine Dribble Direction";
		ActionType = EActionType::Chain;
		PreConditions.Add(EConditions::HasBall);
		AfterEffects.Add(EConditions::Dribble);
	}
	virtual EBT_NodeState ExecuteAction(UPlayerStats* Player) override;
	static bool DefenderBlockStraightPath(const FVector& AttackerPosition, const FVector& DefenderPosition);
	
};
