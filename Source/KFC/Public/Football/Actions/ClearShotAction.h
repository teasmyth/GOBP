// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOBPAI/GOBPAction.h"
#include "ClearShotAction.generated.h"

/**
 * 
 */
UCLASS()
class KFC_API UClearShotAction : public UGobpAction
{
	GENERATED_BODY()

	UClearShotAction();

	virtual EBT_NodeState StartAction(UPlayerStats* Player) override;
	

private:

	FVector CalculateIdealShot(const UPlayerStats* Player) const;
	FVector ApplyAngularDeviation(const FVector& ShotDirection, float DeviationAngleX, float DeviationAngleY);
	void ShootBall(const UPlayerStats* Player);
	
};
