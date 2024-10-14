// Fill out your copyright notice in the Description page of Project Settings.

#include "KFC/Public/GOBPAI/BT_Node.h"


BT_Node::BT_Node()
{
}


EBT_NodeState BT_Node::Update(UPlayerStats* Player)
{
	if (Action != nullptr || bIsRootNode)
	{
		if (!bIsRootNode && Action->ActionType == EActionType::Null)
		{
			if (!bIsRootNode) UE_LOG(LogTemp, Warning, TEXT("Action Type is Null at %s"), *NodeName);
			return Failure;
		}
		Player->SetCurrentActionName(NodeName);
	}
	else
	{
		if (!bIsRootNode) UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
		Player->SetCurrentActionName("Null");
		return Failure;
	}

	if (!Started)
	{
		if (const auto OutCome = OnStart(Player); OutCome != Running)
		{
			return OutCome;
		}
		Started = true;
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
