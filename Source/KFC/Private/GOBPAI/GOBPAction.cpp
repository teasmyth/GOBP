// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/GOBPAction.h"


UGobpAction::UGobpAction()
{
}

bool UGobpAction::IsAchievable(UPlayerStats* Player)
{
	return IsAchievableBP(Player);
}

bool UGobpAction::IsAchievableGivenConditions(const TArray<EConditions>& InConditions)
{
	bool Achievable = IsAchievableGivenConditionsBP(InConditions);

	if (!Achievable) return false;

	if (PreConditions.Num() == 0) return true;

	for (const auto& Condition : InConditions)
	{
		if (!PreConditions.Contains(Condition))
		{
			return false;
		}
	}

	return true;
}

EBT_NodeState UGobpAction::StartAction(UPlayerStats* Player)
{
	return StartActionBP(Player);
}

EBT_NodeState UGobpAction::EndActionBP_Implementation(UPlayerStats* Player)
{
	return EBT_NodeState::Running;
}

EBT_NodeState UGobpAction::EndAction(UPlayerStats* Player)
{
	return EndActionBP(Player);
}

EBT_NodeState UGobpAction::ExecuteActionBP_Implementation(UPlayerStats* Player)
{
	return Success;
}

EBT_NodeState UGobpAction::ExecuteAction(UPlayerStats* Player)
{
	return ExecuteActionBP(Player);
}

TArray<EConditions> UGobpAction::PickNextActionBP_Implementation(UPlayerStats* Player)
{
	return TArray<EConditions>();
}

TArray<EConditions> UGobpAction::PickNextAction(UPlayerStats* Player)
{
	const TArray<EConditions> Conditions = PickNextActionBP(Player); 
	if (!Conditions.IsEmpty())
	{
		return Conditions;
	}

	return TArray<EConditions>({ EConditions::Divider });;
}

float UGobpAction::CalculateCost(UPlayerStats* Player)
{
	return CalculateCostBP(Player);
}

bool UGobpAction::CanBeDoneBeforeNewAction(const TArray<EConditions>& InConditions) const
{
	bool OneIndividualOutcomeFullySatisfies = false;
	for (const auto& IndividualOutcomes : Effects)
	{
		bool ConditionsSatisfied = true;
		for (const auto& Condition : InConditions)
		{
			if (!IndividualOutcomes.Contains(Condition))
			{
				ConditionsSatisfied = false;
				break;
			}
		}
		if (ConditionsSatisfied) OneIndividualOutcomeFullySatisfies = true;
	}

	return OneIndividualOutcomeFullySatisfies;
}


float UGobpAction::CalculateCostBP_Implementation(UPlayerStats* Player)
{
	return 0;
}

EBT_NodeState UGobpAction::StartActionBP_Implementation(UPlayerStats* Player)
{
	return EBT_NodeState::Running;
}

bool UGobpAction::IsAchievableGivenConditionsBP_Implementation(const TArray<EConditions>& InConditions)
{
	return true;
}

bool UGobpAction::IsAchievableBP_Implementation(UPlayerStats* Player)
{
	return true;
}


void UGobpAction::SetupAction()
{
	ConvertEffectsToTArray();
	//AddConditionsToAfterEffects();
}

void UGobpAction::ConvertEffectsToTArray()
{
	int Index = 0;
	for (const auto& Condition : AfterEffects)
	{
		if (Condition != EConditions::Divider)
		{
			if (Effects.IsEmpty())
			{
				Effects.Add(TArray<EConditions>());
			}
			Effects[Index].Add(Condition);
		}
		else
		{
			Effects.Add(TArray<EConditions>());
			Index++;
		}
	}
}

void UGobpAction::AddConditionsToAfterEffects()
{
	for (auto& IndividualOutcome : Effects)
	{
		for (const auto& Condition : PreConditions)
		{
			IndividualOutcome.Add(Condition);
		}
	}
}

bool UGobpAction::IsStraightLineClear(UPlayerStats* Player, const AActor* Target) const
{
	FHitResult HitR;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Player->GetOwner());
	return !GetWorld()->LineTraceSingleByChannel(HitR, Player->GetComponentLocation(), Target->GetActorLocation(), ECC_Visibility, CollisionParams);
}

float UGobpAction::RandomAccuracyMultiplier(const UPlayerStats* Player) const
{
	if (const int Rand = FMath::RandRange(0, 1); Rand == 0)
	{
		return Player->GetAccuracy() / UPlayerStats::MAX_STAT_NUMBER;
	}
	else
	{
		return UPlayerStats::MAX_STAT_NUMBER / Player->GetAccuracy();
	}
}
