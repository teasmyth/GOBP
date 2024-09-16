// Fill out your copyright notice in the Description page of Project Settings.


#include "GOBPAI/GOBPlanner.h"

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "GOBPAI/GOBPWorld.h"

GOBPlanner::GOBPlanner()
{
}

GOBPlanner::~GOBPlanner()
{
}


UBehaviorTree* GOBPlanner::Plan(UObject* Outer, const EPriority& Priority, const TArray<UGobpAction*>& InActions, const TSet<FWorldState>& InGoals)
{
	TArray<FWorldState> SortedGoals = InGoals.Array();

	if (Priority == EPriority::PrioritizeGoals)
	{
		SortedGoals.Sort(&CompareGoalPriority);
	}

	
	
	TArray<TSharedPtr<Node>> StartNodes;

	//This ensures that all actions that can lead to a goal are stored.
	//Without this, FindPath will focus on single action that leads to a specific goal.
	//We loop through sorted goal first to maintain priority order.
	TArray<FGoalAction> GoalActions;
	for (const auto& Goal : SortedGoals)
	{
		for (const auto& Action : InActions)
		{
			if (Action->AfterEffects.Contains(Goal.Key))
			{
				GoalActions.Add(FGoalAction({Goal.Key}, Action));
			}
		}
	}
	
	TArray<UGobpAction*> UsedActions = InActions;

	for (const auto GoalAction : GoalActions)
	{
		//We pass in an empty node, whose conditions will be satisfied by the first 'final' action. E.g. Shoot, Pass, etc.
		TSharedPtr<Node> GoalNode = MakeShareable(new Node(nullptr, GoalAction.Action->GetCost(), GoalAction.Goals, nullptr));

		if (TSharedPtr<Node> StartNode = nullptr; FindPath(GoalNode, UsedActions, StartNode))
		{
			UsedActions.Remove(GoalAction.Action);
			GoalNode->Parent->Leaves.Empty(); //Deleting the empty goal node.
			StartNodes.Add(StartNode);
		}
	}

	if (StartNodes.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Plan Found"));
		UE_LOG(LogTemp, Warning, TEXT("No Plan Found"));
		
		return nullptr;
	}


	const TSharedPtr<Node> MainPlan = StartNodes[0];

	if (StartNodes.Num() > 1)
	{
		StartNodes.RemoveAt(0);
		MergePlans(MainPlan, StartNodes);
		if (Priority == EPriority::PrioritizeCost)
		{
			PrioritizeCost(MainPlan);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Plan Found"));
	return ConstructBehaviourTree(Outer, MainPlan);
}

bool GOBPlanner::FindPath(const TSharedPtr<Node>& Child, TArray<UGobpAction*> UsableActions, TSharedPtr<Node>& EndNode)
{
	for (const auto Action : UsableActions)
	{
		//Limitation:: A node cannot have a condition that precedes for more than one node.
		//Meaning each actions' conditions should only be satisfied fully by one action prior.
		//Right now, I could not figure out how to properly carry over 'unsatisfied conditions' to the next actions without messing up the planning.
		//For example if 'Shoot' were to require both 'HasBall' and 'ClearShot' then it would not be able to pick 'LookForClearShot' as that also requires 'HasBall' but doesnt produce HasBall.
		//Yet if we bypass by carrying over unsatisfied conditions, then this opens up the floodgates for all sorts of issues, such as picking pass.
		//Tried limiting this bypass by at least matching one condition, however, this previous logic will always be present when it comes to evaluating very generic conditions such as HasBall,
		//sooner or later this issue will reproduce itself. Thus, I have made the decision to limit the planning to linear conditioning, rather than overarching conditions.
		//if (!Action->CanBeDoneBeforeNewAction(Child->ConditionStates, UnsatisfiedConditions)) continue;
		if (!Action->CanBeDoneBeforeNewAction(Child->ConditionStates)) continue;


		/*
		 *Could help with global conditions to carry over.
		TMap<FString, int32> CurrentState = Child->Conditions;
		for (const auto& Effect : Action->PreConditions)
		{
			if (CurrentState.Contains(Effect.Key))
			{
				CurrentState[Effect.Key] += Effect.Value;
			}
			else
			{
				CurrentState.Add(Effect.Key, Effect.Value);
			}
		}
		*/


		const TSharedPtr<Node> NewNode = MakeShareable(new Node(nullptr, Child->Cost + Action->GetCost(), Action->PreConditions.Array(), Action));
		Child->Parent = NewNode;
		NewNode->Leaves.Add(Child);

		/*
		if (Action->IsAchievableGivenConditions(FWorldStates::TranslateMapToTArray(GOBPWorld::Instance->GetWorld().GetStates())))
		{
			FoundPath = true;
		}
		*/
		if (Action->PreConditions.IsEmpty()) // for the time being im not checking for anything
		{
			EndNode = NewNode;
			return true;
		}


		UsableActions.Remove(Action); //This removes the action from this plan only, but other plans still have it.
		return FindPath(NewNode, UsableActions, EndNode);
	}
	return false;
}

UBehaviorTree* GOBPlanner::ConstructBehaviourTree(UObject* Outer, const TSharedPtr<Node>& Node)
{
	if (Outer == nullptr)
	{
		Outer = GetTransientPackage();
	}

	UBehaviorTree* BehaviorTree = NewObject<UBehaviorTree>(Outer, UBehaviorTree::StaticClass());
	UBTCompositeNode* RootNode = NewObject<UBTComposite_Selector>(BehaviorTree, UBTComposite_Selector::StaticClass());
	BehaviorTree->RootNode = RootNode;

	RootNode->NodeName = Node->Action->Name;
	PopulateTree(Node, RootNode, BehaviorTree);


	return BehaviorTree;
}

void GOBPlanner::PopulateTree(const TSharedPtr<Node>& Node, UBTCompositeNode* ParentNode, UBehaviorTree* Tree)
{
	for (const auto& Leaf : Node->Leaves)
	{
		if (Leaf->Action == nullptr) continue;

		if (Leaf->Leaves.IsEmpty())
		{
			UBTTaskNode* Task = NewObject<UBTTaskNode>(Tree, UBTTaskNode::StaticClass());
			Task->NodeName = Leaf->Action->Name;

			FBTCompositeChild ChildTask;
			ChildTask.ChildTask = Task;
			ParentNode->Children.Add(ChildTask);
		}
		else
		{
			UBTComposite_Selector* Selector = NewObject<UBTComposite_Selector>(Tree, UBTComposite_Selector::StaticClass());
			Selector->NodeName = Leaf->Action->Name;
			//Leaf->Action->ActionType
			FBTCompositeChild ChildSelector;
			ChildSelector.ChildComposite = Selector;
			ParentNode->Children.Add(ChildSelector);


			PopulateTree(Leaf, Selector, Tree);
		}
	}
}

void GOBPlanner::MergePlans(const TSharedPtr<Node>& MainPlan, const TArray<TSharedPtr<Node>>& Leaves)
{
	for (const auto& Leaf : Leaves)
	{
		if (*MainPlan == *Leaf) //this compares the UGobpActions they have, ensuring that they are the same action.
		{
			for (const auto& NewLeaf : Leaf->Leaves)
			{
				bool MainLeafHasAction = false;
				for (const auto& MainLeaf : MainPlan->Leaves)
				{
					if (*MainLeaf == *NewLeaf)
					{
						MergePlans(MainLeaf, NewLeaf->Leaves);
						MainLeafHasAction = true;
						break;
					}
				}
				if (!MainLeafHasAction)
				{
					MainPlan->Leaves.Add(NewLeaf);
				}
			}
		}
		else
		{
			MainPlan->Leaves.Add(Leaf);
		}
	}
}

void GOBPlanner::PrioritizeCost(const TSharedPtr<Node>& Parent)
{
	if (Parent->Leaves.Num() == 0) return;

	Parent->Leaves.Sort(&CompareCost);

	for (const auto& Leaf : Parent->Leaves)
	{
		PrioritizeCost(Leaf);
	}
}


bool GOBPlanner::GoalAchieved(const TMap<FString, int32>& Goals, const TMap<FString, int32>& States)
{
	for (auto Goal : Goals)
	{
		if (!States.Contains(Goal.Key)) //|| States[Goal.Key] != Goal.Value)
		{
			return false;
		}
	}
	return true;
}

TArray<UGobpAction*> GOBPlanner::ActionSubset(TArray<UGobpAction*> NewSubset, const UGobpAction* ActionToRemove)
{
	TArray<UGobpAction*> Subset = TArray<UGobpAction*>();
	for (const auto& Action : NewSubset)
	{
		if (Action != ActionToRemove)
		{
			Subset.Add(Action);
		}
	}
	return Subset;
}
