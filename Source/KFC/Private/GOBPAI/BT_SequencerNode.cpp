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
		const auto Outcome = Action->StartAction(Player);
		if ( Outcome != Running)
		{
			return Outcome;
		}

		if (Action->ActionType == EActionType::Action && Outcome == Running)
		{
			UE_LOG(LogTemp, Warning, TEXT("Action node %s is not supposed to be running. Failing action."), *NodeName);
			return Failure;
		}
		
		return Running;
	}

	UE_LOG(LogTemp, Warning, TEXT("Chain is nullptr at %s"), *NodeName);
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
		UE_LOG(LogTemp, Warning, TEXT("Chain is nullptr at %s"), *NodeName);
	}
}

EBT_NodeState BT_SequencerNode::OnUpdate(UPlayerStats* Player)
{
	if (Action == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Chain is nullptr at %s"), *NodeName);
		return Failure;
	}


	if (Children.Num() == 0 && Action->ActionType != EActionType::Action)
	{
		UE_LOG(LogTemp, Warning, TEXT("Children are empty at %s. It must have children."), *NodeName);
		return Failure;
	}

	//Technically, I could use a switch but feels ugly.

	//To pick next action, we need to check the outcome before executing the action of this action node.
	if (Action->ActionType == EActionType::Picker)
	{
		const auto SelectedOutcome = Action->PickNextAction(Player);

		if (SelectedOutcome.IsEmpty() || SelectedOutcome.Contains(EConditions::Divider))
		{
			UE_LOG(LogTemp, Warning, TEXT("No action was selected for %s or it wrongly contains a Divider"), *NodeName);
			return Action->ExecuteAction(Player);
		}

		for (const auto& Child : Children)
		{
			if (Child->Action == nullptr) continue;

			if (Child->Action->IsAchievable(Player) && SelectedOutcome == Child->Action->PreConditions.Array())
			{
				return Child->Update(Player);
			}
		}

		//If we are here, it means that we were not able to pick any children, therefore, should execute the action.
		return Action->ExecuteAction(Player);
	}


	
	if (const auto Outcome = Action->ExecuteAction(Player); Outcome == Failure)
	{
		UE_LOG(LogTemp, Warning, TEXT("Action %s failed."), *NodeName);
		return Failure;
	}

	if (Action->ActionType == EActionType::Chain)
	{
		int SuccessChild = 0;
		int FailureChild = 0;
		for (const auto& Child : Children)
		{
			if (Child->Action == nullptr) continue;

			if (const auto ChildOutcome = Child->Update(Player); ChildOutcome == Failure)
			{
				FailureChild++;
			}
			else if (ChildOutcome == Success)
			{
				SuccessChild++;
			}
		}

		if (SuccessChild == Children.Num())
		{
			return Success;
		}
		else if (FailureChild == Children.Num())
		{
			return Failure;
		}

		return Running;
	}


	if (Action->ActionType == EActionType::Selector)
	{
		int SuccessChild = 0;
		for (const auto& Child : Children)
		{
			if (Child->Action == nullptr) continue;

			if (const auto ChildOutcome = Child->Update(Player); ChildOutcome == Failure)
			{
				return Failure;
			}
			else if (ChildOutcome == Success)
			{
				SuccessChild++;
			}
		}

		if (SuccessChild == Children.Num())
		{
			return Success;
		}
	}
	
	return Running;
}
