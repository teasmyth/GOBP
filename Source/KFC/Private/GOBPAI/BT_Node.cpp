// Fill out your copyright notice in the Description page of Project Settings.

#include "KFC/Public/GOBPAI/BT_Node.h"

class BT_SequencerNode;

BT_Node::BT_Node()
{
	
}


EBT_NodeState BT_Node::Update()
{
	if (!Started)
	{
		OnStart();
		Started = true;
	}

	State = OnUpdate();
	
	if (Action != nullptr)
	{
		State = Action->ExecuteAction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}

	if (State != Running)
	{
		OnExit();
		Started = false;
	}

	return State;
}

void BT_Node::ResetNode()
{
	Started = false;
	State = Running;
}

void BT_Node::OnStart()
{
	if (Action != nullptr)
	{
		Action->StartAction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}
}

void BT_Node::OnExit()
{
	if (Action != nullptr)
	{
		Action->EndAction();
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Action is nullptr at %s"), *NodeName);
	}
}

void BT_Node::SetAction(UGobpAction* InAction)
{
	Action = InAction;
}


