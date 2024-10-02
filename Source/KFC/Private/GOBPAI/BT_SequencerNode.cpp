// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_SequencerNode.h"


BT_SequencerNode::BT_SequencerNode()
{
}

BT_SequencerNode::~BT_SequencerNode()
{
}


EBT_NodeState BT_SequencerNode::OnStart(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		if (const auto Outcome = Action->StartAction(Player); Outcome != Running)
		{
			return Outcome;
		}
		SelectedOutcome = Action->PickNextAction(Player);
		SelectorFnEmpty = SelectedOutcome.IsEmpty();
		return Running;
	}

	UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	return Failure;
}

void BT_SequencerNode::OnExit(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		Action->EndAction(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}
}

EBT_NodeState BT_SequencerNode::OnUpdate(UPlayerStats* Player)
{
	if (Action == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
		return Failure;
	}


	SelectedOutcome = Action->PickNextAction(Player);

	if (Children.Num() == 0 || SelectedOutcome.IsEmpty())
	{
		if (SelectedOutcome.IsEmpty())
		{
			if (Action != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Selector Function is empty for Action: %s"), *Action->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Selector Function is empty"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Children are empty"));
		}
		return Failure;
	}


	/* needs more work, need to think it through.
	if (Action->ActionType == EActionType::Selector)
	{
		bool IsRunning = false;
		for (const auto& Child : Children)
		{
			switch (Child->Update(Player))
			{
			case Success:
				return Success;
			case Running:
				IsRunning = true;
				break;
			default: break;
			}
		}
		if (IsRunning)
		{
			Action->ExecuteAction(Player);
			return Running;
		}
		return Failure;
		
	}
	*/

	if (Action->ActionType == EActionType::Picker)
	{
		for (const auto& Child : Children)
		{
			if (Child->Action == nullptr) continue;

			if (Child->Action->IsAchievable(Player) && SelectedOutcome == Child->Action->PreConditions.Array())
			{
				return Child->Update(Player);
			}
		}

		Action->ExecuteAction(Player);
	}

	//UE_LOG(LogTemp, Warning, TEXT("No action was selected for %s or all actions failed."), *NodeName);
	return Running;
}
