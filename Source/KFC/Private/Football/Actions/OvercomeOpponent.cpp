// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/Actions/OvercomeOpponent.h"


TArray<EConditions> UOvercomeOpponent::PickNextAction(UPlayerStats* Player)
{
	if (!Player->GetTargetPlayer())
	{
		UE_LOG(LogTemp, Error, TEXT("Player %d has no target player"), Player->GetPlayerID());
		return {EConditions::OvercomeOpponentFailed};
	}

	const int32 PlayerDribbling = Player->GetDribbling();
	const float PlayerCurrentSpeed = Player->GetVelocity();
	const int32 PlayerVisionMultiplier = static_cast<float>(Player->GetVision()) / UPlayerStats::MAX_STAT_NUMBER;
	const int32 DefenderDefense = Player->GetTargetPlayer()->GetDefense();
	const int32 DefenderPositioning = Player->GetTargetPlayer()->GetPositioning();
	const float DefenderCurrentSpeed = Player->GetTargetPlayer()->GetVelocity();
	
	const bool DribbleSuccess = PlayerDribbling * PlayerCurrentSpeed >= (DefenderDefense + DefenderPositioning) * DefenderCurrentSpeed * PlayerVisionMultiplier;

	if (DribbleSuccess)
	{
		return {EConditions::CanOvercomeOpponent};
	}
	else
	{
		return {EConditions::OvercomeOpponentFailed};
	}
}
