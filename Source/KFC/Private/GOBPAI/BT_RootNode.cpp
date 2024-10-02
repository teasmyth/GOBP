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

EBT_NodeState BT_RootNode::OnStart(UPlayerStats* Player)
{
	return Running;
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
