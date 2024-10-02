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
	virtual ~BT_RootNode() override;

	TSharedPtr<BT_Node> Child = nullptr;

	virtual EBT_NodeState OnUpdate(UPlayerStats* Player) override;
	//static void GetAllNodes(BT_Node* Parent, TArray<BT_Node*>& ChildrenToReturn);
//	void TraverseSetOwner(AActor* InActor);
	//void CalculateCosts() const;

	//Runs the behaviour tree, returns true if it has resulted with either Success or Failure. Otherwise returns false.
	bool RunTree(UPlayerStats* Player);

	virtual void ResetNode() override;


protected:
	virtual EBT_NodeState OnStart(UPlayerStats* Player) override;
	virtual void OnExit(UPlayerStats* Player) override;
};


