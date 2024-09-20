// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/GOBPAction.h"


UGobpAction::UGobpAction()
{
}

bool UGobpAction::IsAchievable()
{
	return IsAchievableBP();
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

void UGobpAction::StartAction(UPlayerStats* Player)
{
	StartActionBP(Player);
}

void UGobpAction::EndActionBP_Implementation(UPlayerStats* Player)
{
}

void UGobpAction::EndAction(UPlayerStats* Player)
{
	EndActionBP(Player);
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

	return TArray<EConditions>();
}

float UGobpAction::CalculateCost(UPlayerStats* Player)
{
	return CalculateCostBP(Player);
}

/*
bool UGobpAction::CanBeDoneBeforeNewAction(const TArray<EConditions>& InConditions, TArray<EConditions>& OutUnsatisfiedConditions) const
{
	bool ContainsAtLeastOne = false;
	TArray<EConditions> UnsatisfiedConditionsCopy = OutUnsatisfiedConditions;
	
	for (const auto& IndividualOutcomes : AfterEffects)
	{
		for (const auto& Condition : InConditions)
		{
			if (IndividualOutcomes.AfterEffects.Contains(Condition))
			{
				ContainsAtLeastOne = true;
				if(UnsatisfiedConditionsCopy.Contains(Condition))
				{
					UnsatisfiedConditionsCopy.Remove(Condition);
				}
			}
			else
			{
				UnsatisfiedConditionsCopy.Add(Condition);
			}
		}
	}

	if (ContainsAtLeastOne)
	{
		//We don't want to modify this unless we are using this action.
		OutUnsatisfiedConditions = UnsatisfiedConditionsCopy;
	}

	return ContainsAtLeastOne;
}
*/

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

void UGobpAction::StartActionBP_Implementation(UPlayerStats* Player)
{
}

bool UGobpAction::IsAchievableGivenConditionsBP_Implementation(const TArray<EConditions>& InConditions)
{
	return true;
}

bool UGobpAction::IsAchievableBP_Implementation()
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
