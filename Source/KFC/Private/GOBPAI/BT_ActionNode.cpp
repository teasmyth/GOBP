// Fill out your copyright notice in the Description page of Project Settings.


#include "KFC/Public/GOBPAI/BT_ActionNode.h"

BT_ActionNode::BT_ActionNode()
{
}

BT_ActionNode::~BT_ActionNode()
{
}

void BT_ActionNode::DeleteNode()
{
	delete this;
}


void BT_ActionNode::OnStart()
{
}

void BT_ActionNode::OnExit()
{
}

EBT_NodeState BT_ActionNode::OnUpdate()
{
	return State;
}
