// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_ActionNode.h"

BT_ActionNode::BT_ActionNode()
{
}

BT_ActionNode::~BT_ActionNode()
{
}


EBT_NodeState BT_ActionNode::OnStart(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		const auto o = Action->StartAction(Player);
		//return Chain->StartAction(Player);

		return o;
	}

	UE_LOG(LogTemp, Warning, TEXT("Chain is nullptr at %s"), *NodeName);
	return Failure;
}

void BT_ActionNode::OnExit(UPlayerStats* Player)
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

EBT_NodeState BT_ActionNode::OnUpdate(UPlayerStats* Player)
{
	if (Action != nullptr)
	{
		State = Action->ExecuteAction(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Chain is nullptr at %s"), *NodeName);
		State = Failure;
	}
	return State;
}
