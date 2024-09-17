#pragma once
#include "GOBPAction.h"

class Node
{
public:
	Node();
	~Node();

	TSharedPtr<Node> Parent = nullptr;
	float Cost = 0;
	UGobpAction* Action = nullptr;
	TArray<EConditions> ConditionStates;
	TArray<TSharedPtr<Node>> Leaves;
	EActionType ActionType = EActionType::Null;
	
	Node(const TSharedPtr<Node>& InParent, const float InCost, const TArray<EConditions>& InState, UGobpAction* InAction)
	{
		Parent = InParent;
		Cost = InCost;
		Action = InAction;
		ConditionStates = InState;
		ActionType = Action != nullptr ? Action->ActionType : EActionType::Null;
	}

	bool operator==(const Node& Other) const
	{
		if (Action == nullptr || Other.Action == nullptr)
		{
			return false;
		}
		return Action == Other.Action;
	}

	bool operator==(const TSharedPtr<Node>& Other) const
	{
		if (Action == nullptr || Other->Action == nullptr)
		{
			return false;
		}
		return Action == Other->Action;
	}
};
