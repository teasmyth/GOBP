// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BT_ActionNode.h"
#include "BT_Node.h"

/**
 * 
 */
class KFC_API BT_SequencerNode : public BT_Node
{
public:
	BT_SequencerNode();
	explicit BT_SequencerNode(UGobpAction* InAction)
	{
		Action = InAction;
		if (Action == nullptr) return;
		NodeName = Action->Name;
	}
	
	virtual ~BT_SequencerNode() override;

	TArray<TSharedPtr<BT_Node>> GetChildren() { return Children; }

	void AddChild(const TSharedPtr<BT_Node>& InChild)
	{
		Children.Add(InChild);
	}

protected:
	virtual void OnStart() override;
	virtual void OnExit() override;
	virtual EBT_NodeState OnUpdate() override;

private:
	TArray<TSharedPtr<BT_Node>> Children;
	TArray<EConditions> SelectedOutcome;
	bool SelectorFnEmpty = false;
	/*
static bool CompareNodeCost(const BT_Node& A, const BT_Node& B)
{
	if (A.Action == nullptr) return false;
	if (B.Action == nullptr) return true;
	return A.Action->Cost < B.Action->Cost;
}
*/
};


