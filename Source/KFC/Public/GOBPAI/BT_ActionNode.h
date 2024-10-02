// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BT_Node.h"

/**
 * 
 */
class KFC_API BT_ActionNode : public BT_Node
{
public:
	BT_ActionNode();
	explicit BT_ActionNode(UGobpAction* InAction)
	{
		Action = InAction;
		if (Action == nullptr) return;
		NodeName = Action->Name;
	}
	virtual ~BT_ActionNode() override;


protected:
	virtual EBT_NodeState OnStart(UPlayerStats* Player) override;
	virtual void OnExit(UPlayerStats* Player) override;
	virtual EBT_NodeState OnUpdate(UPlayerStats* Player) override;
};
