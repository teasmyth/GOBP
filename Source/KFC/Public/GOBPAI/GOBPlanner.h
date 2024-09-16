// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOBPAction.h"
#include "Node.h"
#include "BehaviorTree/BehaviorTree.h"
using GOBPPlanner::Node;

/**
 * 
 */

UENUM(BlueprintType)
enum class EPriority : uint8
{
	PrioritizeGoals = 0,
	PrioritizeCost = 1,
};

struct FGoalAction
{
	TArray<EConditions> Goals;
	UGobpAction* Action;

	FGoalAction();
	FGoalAction(const TArray<EConditions>& InGoals, UGobpAction* InAction)
	{
		Goals = InGoals;
		Action = InAction;
	}

	bool operator==(const UGobpAction* Other) const
	{
		return Action == Other;
	}
};


class KFC_API GOBPlanner
{
public:
	GOBPlanner();
	~GOBPlanner();

	static bool GoalAchieved(const TMap<FString, int32>& Goals, const TMap<FString, int32>& States);
	static TArray<UGobpAction*> ActionSubset(TArray<UGobpAction*> NewSubset, const UGobpAction* ActionToRemove);

	//Passing in an empty node with the desired goal, and from there it works its way back to the start
	//StartNode is the start from the plan, the first action to execute
	static bool FindPath(const TSharedPtr<Node>& Child, TArray<UGobpAction*> UsableActions, TSharedPtr<Node>& EndNode);
	static UBehaviorTree* Plan(UObject* Outer, const EPriority& Priority, const TArray<UGobpAction*>& InActions, const TSet<FWorldState>& InGoals);

	static UBehaviorTree* ConstructBehaviourTree(UObject* Outer, const TSharedPtr<Node>& Node);
	static void PopulateTree(const TSharedPtr<Node>& Node, UBTCompositeNode* ParentNode, UBehaviorTree* Tree);
	static void MergePlans(const TSharedPtr<Node>& MainPlan, const TArray<TSharedPtr<Node>>& Leaves);
	static void PrioritizeCost(const TSharedPtr<Node>& Parent);


	//Higher value is more important
	static bool CompareGoalPriority(const FWorldState& A, const FWorldState& B)
	{
		return A.Value > B.Value;
	}

	static bool CompareCost(const TSharedPtr<Node>& A, const TSharedPtr<Node>& B)
	{
		return A->Cost < B->Cost;
	}
	
};
