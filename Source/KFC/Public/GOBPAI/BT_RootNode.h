// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BT_Node.h"

/**
 * 
 */
class KFC_API BT_RootNode : public BT_Node
{
public:
	BT_RootNode();

	explicit BT_RootNode(UGobpAction* InAction)
	{
		Action = InAction;
	}

	virtual ~BT_RootNode() override;

	BT_Node* Child = nullptr;

	virtual EBT_NodeState OnUpdate() override;
	virtual void DeleteNode() override;
	static void GetAllNodes(BT_Node* Parent, TArray<BT_Node*>& ChildrenToReturn);
	void TraverseSetOwner(AActor* InActor);
	void CalculateCosts() const;



protected:
	virtual void OnStart() override;
	virtual void OnExit() override;
	virtual void ResetNode() override;
};


