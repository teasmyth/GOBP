// Fill out your copyright notice in the Description page of Project Settings.


#include "GOBPAI/GOBPlanner.h"

#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "GOBPAI/BT_SequencerNode.h"
#include "GOBPAI/GOBPWorld.h"

GOBPlanner::GOBPlanner()
{
}

GOBPlanner::~GOBPlanner()
{
}


bool GOBPlanner::Plan(UPlayerStats* Player, UObject* Outer, const EPriority& Priority, const TArray<UGobpAction*>& InActions, const TSet<FWorldState>& InGoals, UBehaviorTree*& OutUnrealBT, TSharedPtr<BT_RootNode>& OutRootNode)
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
		TSharedPtr<Node> GoalNode = MakeShareable(new Node(nullptr, GoalAction.Action->GetCost(Player), GoalAction.Goals, nullptr));

		if (TSharedPtr<Node> StartNode = nullptr; FindPath(Player, GoalNode, UsedActions, StartNode))
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
		
		return false;
	}


	const TSharedPtr<Node> MainPlan = StartNodes[0];

	if (StartNodes.Num() > 1)
	{
		StartNodes.RemoveAt(0);
		for (const auto& StartNode : StartNodes)
		{
			MergePlan(MainPlan, StartNode);
		}
		
		if (Priority == EPriority::PrioritizeCost)
		{
			PrioritizeCost(MainPlan);
		}
	}
	
	OutUnrealBT = ConstructUnrealBT(Outer, MainPlan);
	OutRootNode = ConstructBT(MainPlan);

	return true;
}

void GOBPlanner::MergePlan(const TSharedPtr<Node>& MainPlan, const TSharedPtr<Node>& OtherPlan)
{
	
		if (*MainPlan == *OtherPlan) //this compares the UGobpActions they have, ensuring that they are the same action.
		{
			for (const auto& NewLeaf : OtherPlan->Leaves)
			{
				bool MainLeafHasTheAction = false;
				for (const auto& MainLeaf : MainPlan->Leaves)
				{
					if (*MainLeaf == *NewLeaf)
					{
						MergePlan(MainLeaf, NewLeaf);
						MainLeafHasTheAction = true;
						break;
					}
				}
				//Meaning that the action is not present in the main plan, however because they share the same parents, it can be added to the main plan.
				if (!MainLeafHasTheAction)
				{
					MainPlan->Leaves.Add(NewLeaf);
				}
			}
		}
		else
		{
			MainPlan->Leaves.Add(OtherPlan);
		}
	}


bool GOBPlanner::FindPath(UPlayerStats* Player, const TSharedPtr<Node>& Child, TArray<UGobpAction*> UsableActions, TSharedPtr<Node>& EndNode)
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


		const TSharedPtr<Node> NewNode = MakeShareable(new Node(nullptr, Child->Cost + Action->GetCost(Player), Action->PreConditions.Array(), Action));
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
		return FindPath(Player, NewNode, UsableActions, EndNode);
	}
	return false;
}

UBehaviorTree* GOBPlanner::ConstructUnrealBT(UObject* Outer, const TSharedPtr<Node>& Node)
{
	if (Outer == nullptr)
	{
		Outer = GetTransientPackage();
	}

	UBehaviorTree* BehaviorTree = NewObject<UBehaviorTree>(Outer, UBehaviorTree::StaticClass());
	UBTCompositeNode* RootNode = NewObject<UBTComposite_Selector>(BehaviorTree, UBTComposite_Selector::StaticClass());
	BehaviorTree->RootNode = RootNode;

	RootNode->NodeName = Node->Action->Name;
	PopulateUnrealBT(Node, RootNode, BehaviorTree);


	return BehaviorTree;
}

void GOBPlanner::PopulateUnrealBT(const TSharedPtr<Node>& Node, UBTCompositeNode* ParentNode, UBehaviorTree* Tree)
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


			PopulateUnrealBT(Leaf, Selector, Tree);
		}
	}
}

TSharedPtr<BT_RootNode> GOBPlanner::ConstructBT(const TSharedPtr<Node>& InBTRootNode)
{
	const TSharedPtr<BT_RootNode> RootNode = MakeShareable(new BT_RootNode());
	const TSharedPtr<BT_SequencerNode> RootChild = MakeShareable(new BT_SequencerNode(InBTRootNode->Action));
	RootNode->Child = RootChild;
	PopulateBT(RootChild, InBTRootNode);

	UE_LOG(LogTemp, Warning, TEXT("RootNode: %s"), *RootNode->NodeName);
	return RootNode;
}

//todo rename params todo

void GOBPlanner::PopulateBT(const TSharedPtr<BT_SequencerNode>& OutRootNode, const TSharedPtr<Node>& InRootNode)
{
	/*
	if (InRootNode->Leaves.IsEmpty())
	{
		const TSharedPtr<BT_ActionNode> NewActionNode = MakeShareable(new BT_ActionNode(InRootNode->Action));
		OutRootNode->AddChild(NewActionNode);
	}
	else
	{
		const TSharedPtr<BT_SequencerNode> NewSequencerNode = MakeShareable(new BT_SequencerNode(InRootNode->Action));
		OutRootNode->AddChild(NewSequencerNode);
		for (const auto& Child : InRootNode->Leaves)
		{
			PopulateBT(NewSequencerNode, Child);
		}
	}
	*/

	if (InRootNode->Leaves.IsEmpty())
	{
		const TSharedPtr<BT_ActionNode> NewActionNode = MakeShareable(new BT_ActionNode(InRootNode->Action));
		OutRootNode->AddChild(NewActionNode);
	}
	else
	{
		for (const auto& Child : InRootNode->Leaves)
		{
			if (Child->Leaves.IsEmpty())
			{
				const TSharedPtr<BT_ActionNode> NewActionNode = MakeShareable(new BT_ActionNode(Child->Action));
				OutRootNode->AddChild(NewActionNode);
			}
			else
			{
				const TSharedPtr<BT_SequencerNode> NewSequencerNode = MakeShareable(new BT_SequencerNode(Child->Action));
				OutRootNode->AddChild(NewSequencerNode);
				for (const auto& GrandChild : Child->Leaves)
				{
					PopulateBT(NewSequencerNode, GrandChild);
				}
			}
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
