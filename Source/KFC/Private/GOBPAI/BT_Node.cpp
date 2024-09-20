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
		OnStart(Player);
		Started = true;
	}

	State = OnUpdate(Player);
	
	if (Action != nullptr)
	{
		if (Action->ActionType == EActionType::Null)
		{
			UE_LOG(LogTemp, Warning, TEXT("Action Type is Null at %s"), *NodeName);
			return Failure;
		}
		State = Action->ExecuteAction(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}

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

