#pragma once
#include "GOBPAction.h"

namespace GOBPPlanner
{
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

		Node(const TSharedPtr<Node>& InParent, const float InCost, const TArray<EConditions>& InState, UGobpAction* InAction)
		{
			Parent = InParent;
			Cost = InCost;
			Action = InAction;
			ConditionStates = InState;
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
}
