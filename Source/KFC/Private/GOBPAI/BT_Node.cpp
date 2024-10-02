// Fill out your copyright notice in the Description page of Project Settings.

#include "KFC/Public/GOBPAI/BT_Node.h"

class BT_SequencerNode;

BT_Node::BT_Node()
{
	
}


EBT_NodeState BT_Node::Update(UPlayerStats* Player)
{
	if (!Started)
	{
		if (const auto OutCome = OnStart(Player); OutCome != Running)
		{
			return OutCome;
		}
		Started = true;
	}

	
	if (Action != nullptr)
	{
		if (Action->ActionType == EActionType::Null)
		{
			if (!bIsRootNode) UE_LOG(LogTemp, Warning, TEXT("Action Type is Null at %s"), *NodeName);
			return Failure;
		}
	}
	else
	{
		if (!bIsRootNode) UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}

	State = OnUpdate(Player);
	
	if (State != Running)
	{
		OnExit(Player);
		Started = false;
	}

	return State;
}

void BT_Node::ResetNode()
{
	Started = false;
	State = Running;
}

