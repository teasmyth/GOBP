// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_SequencerNode.h"


BT_SequencerNode::BT_SequencerNode()
{
}

BT_SequencerNode::~BT_SequencerNode()
{
}


void BT_SequencerNode::OnStart(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		Action->StartAction(Player);
		SelectedOutcome = Action->PickNextAction(Player);
		SelectorFnEmpty = SelectedOutcome.IsEmpty();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}
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

	Action->ExecuteAction(Player);
	
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
		return IsRunning ? Running : Failure;
	}

	if (Action->ActionType == EActionType::Picker)
	{
		for (const auto& Child : Children)
		{
			if (Child->Action == nullptr) continue;

			if (SelectedOutcome == Child->Action->PreConditions.Array())
			{
				return Child->Update(Player);
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("No action was selected for %s or all actions failed."), *NodeName);
	return Failure;
}
