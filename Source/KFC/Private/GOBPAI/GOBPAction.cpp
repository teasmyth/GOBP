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

void UGobpAction::StartAction()
{
	StartActionBP();
}

void UGobpAction::EndActionBP_Implementation()
{
}

void UGobpAction::EndAction()
{
	EndActionBP();
}

EBT_NodeState UGobpAction::ExecuteActionBP_Implementation()
{
	return Success;
}

EBT_NodeState UGobpAction::ExecuteAction()
{
	return ExecuteActionBP();
}

TArray<EConditions> UGobpAction::PickNextAction()
{
	if (TArray<EConditions> Conditions = PickNextActionBP(); !Conditions.IsEmpty())
	{
		return Conditions;
	}

	return TArray<EConditions>();
}

TArray<EConditions> UGobpAction::PickNextActionBP_Implementation()
{
	return TArray<EConditions>();
}

float UGobpAction::CalculateCost()
{
	return CalculateCostBP();
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


float UGobpAction::CalculateCostBP_Implementation()
{
	return 0;
}

void UGobpAction::StartActionBP_Implementation()
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
