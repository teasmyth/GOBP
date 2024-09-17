// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_SequencerNode.h"


BT_SequencerNode::BT_SequencerNode()
{
}

BT_SequencerNode::~BT_SequencerNode()
{
}


void BT_SequencerNode::OnStart()
{
	if (Action != nullptr)
	{
		Action->StartAction();
		SelectedOutcome = Action->PickNextAction();
		SelectorFnEmpty = SelectedOutcome.IsEmpty();
	}
}

void BT_SequencerNode::OnExit()
{
	if (Action != nullptr)
	{
		Action->EndAction();
	}
}

EBT_NodeState BT_SequencerNode::OnUpdate()
{
	if (Children.Num() == 0 || SelectorFnEmpty)
	{
		if (SelectorFnEmpty)
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


	bool ReturnFail = true;

	if (Action == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
		return Failure;
	}


	if (Action->ActionType == EActionType::Selector)
	{
		bool IsRunning = false;
		for (const auto& Child : Children)
		{
			switch (Child->Update())
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
				return Child->Update();
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("No action was selected for %s or all actions failed."), *NodeName);
	return Failure;
}
