// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_RootNode.h"

#include "KFC/Public/GOBPAI/BT_SequencerNode.h"

BT_RootNode::BT_RootNode()
{
	NodeName = "RootNode";
	bIsRootNode = true;
}

BT_RootNode::~BT_RootNode()
{
}

EBT_NodeState BT_RootNode::OnUpdate(UPlayerStats* Player)
{
	if (Child == nullptr)
	{
		return Failure;
	}
	return Child->Update(Player);
}

bool BT_RootNode::RunTree(UPlayerStats* Player)
{
	if (Update(Player) == Running)
	{
		return false;
	}
	return true;
}

/*
void BT_RootNode::GetAllNodes(BT_Node* Parent, TArray<BT_Node*>& ChildrenToReturn)
{
	if (const auto SequencerNode = dynamic_cast<BT_SequencerNode*>(Parent); SequencerNode != nullptr)
	{
		const auto Children = SequencerNode->GetChildren();
		for (const auto& Child : Children)
		{
			ChildrenToReturn.Add(Child);
			GetAllNodes(Child, ChildrenToReturn);
		}
	}
	
}


void BT_RootNode::TraverseSetOwner(AActor* InActor)
{
	Owner = InActor;

	TArray<BT_Node*> Children;
	Children.Add(Child);
	GetAllNodes(Child, Children);
	for (const auto& ChildNode : Children)
	{
		ChildNode->Owner = InActor;
	}
}

void BT_RootNode::CalculateCosts() const
{
	TArray<BT_Node*> Children;
	Children.Add(Child);
	GetAllNodes(Child, Children);
	for (const auto& ChildNode : Children)
	{
		if (ChildNode->Action != nullptr)
		{
			ChildNode->Action->CalculateCost();
		}
	}
}
*/

void BT_RootNode::OnStart(UPlayerStats* Player)
{
}

void BT_RootNode::OnExit(UPlayerStats* Player)
{
}

void BT_RootNode::ResetNode()
{
	BT_Node::ResetNode();
	if (Child != nullptr)
	{
		Child->ResetNode();
	}
}
