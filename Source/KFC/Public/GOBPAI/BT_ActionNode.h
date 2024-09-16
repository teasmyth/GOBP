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
	explicit BT_ActionNode(UGobpAction* InAction, AActor* InActor)
	{
		Action = InAction;
		Owner = InActor;
	}
	virtual ~BT_ActionNode() override;
	virtual void DeleteNode() override;


protected:
	virtual void OnStart() override;
	virtual void OnExit() override;
	virtual EBT_NodeState OnUpdate() override;
};
