// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_ActionNode.h"

BT_ActionNode::BT_ActionNode()
{
}

BT_ActionNode::~BT_ActionNode()
{
}


void BT_ActionNode::OnStart(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		Action->StartAction(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}
}

void BT_ActionNode::OnExit(UPlayerStats* Player)
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

EBT_NodeState BT_ActionNode::OnUpdate(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		State = Action->ExecuteAction(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
		State = Failure;
	}
	return State;
}
