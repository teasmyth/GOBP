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


bool GOBPlanner::Plan(UPlayerStats* Player, UObject* Outer, const EPriority& Priority, const TArray<UGobpAction*>& InActions,
                      const TSet<FWorldState>& InGoals, UBehaviorTree*& OutUnrealBT, TSharedPtr<BT_RootNode>& OutRootNode)
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
			GoalNode->Parent->Leaves.Empty(); //Deleting the empty goal node.
			StartNodes.Add(StartNode);
		}
		UsedActions.Remove(GoalAction.Action); //We remove this regardless of success or failure, as it is not needed anymore.
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
			/*TODO When prioritizing goal, it should be by default that the leaves organizes themselves according to cost
			 *without messing up the goal order.
			*/
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
		/*
		//Limitation:: A node cannot have a condition that precedes for more than one node.
		//Meaning each actions' conditions should only be satisfied fully by one action prior.
		//Right now, I could not figure out how to properly carry over 'unsatisfied conditions' to the next actions without messing up the planning.
		//For example if 'Shoot' were to require both 'HasBall' and 'ClearShot' then it would not be able to pick 'LookForClearShot' as that also requires 'HasBall' but doesnt produce HasBall.
		//Yet if we bypass by carrying over unsatisfied conditions, then this opens up the floodgates for all sorts of issues, such as picking pass.
		//Tried limiting this bypass by at least matching one condition, however, this previous logic will always be present when it comes to evaluating very generic conditions such as HasBall,
		//sooner or later this issue will reproduce itself. Thus, I have made the decision to limit the planning to linear conditioning, rather than overarching conditions.

		//update 10/10 - it's been two weeks since my initial comment and I am yet to come up with a meaningful example where I would want to produce two conditions in a single outcome.
		//Perhaps removing arrays for outcomes and just producing singular outcomes is the approach. Also, I decided to double down on a design choice,
		//where each action's precondition is a single element. Having HasBall and ClearShot is meanigless as by that time I should have the ball anyway,
		//so this would just lead to unnecessary, self-implied if checks. Therefore, it is not an "issue" any more. I will remove all of these comments once I made the
		//necessary changes.
		*/

		
		if (!Action->CanBeDoneBeforeNewAction(Child->ConditionStates)) continue;

		
		const TSharedPtr<Node> NewNode = MakeShareable(
			new Node(nullptr, Child->Cost + Action->GetCost(Player), Action->PreConditions.Array(), Action));
		Child->Parent = NewNode;
		NewNode->Leaves.Add(Child);

		
		if (Action->PreConditions.IsEmpty()) // This should have an OR check, that is, either no preconditions or our current state satisfies that preconditions. For exampple we already have the ball.
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
			//Leaf->Chain->ActionType
			FBTCompositeChild ChildSelector;
			ChildSelector.ChildComposite = Selector;
			ParentNode->Children.Add(ChildSelector);


			PopulateUnrealBT(Leaf, Selector, Tree);
		}
	}
}

TSharedPtr<BT_RootNode> GOBPlanner::ConstructBT(const TSharedPtr<Node>& InPathFindRootNode)
{
	const TSharedPtr<BT_RootNode> RootNode = MakeShareable(new BT_RootNode());
	const TSharedPtr<BT_SequencerNode> RootChild = MakeShareable(new BT_SequencerNode(InPathFindRootNode->Action));
	RootNode->Child = RootChild;
	PopulateBT(RootChild, InPathFindRootNode);

	return RootNode;
}

void GOBPlanner::PopulateBT(const TSharedPtr<BT_SequencerNode>& InBT_Node, const TSharedPtr<Node>& InPathFindNode)
{
	if (InPathFindNode->Leaves.IsEmpty())
	{
		const TSharedPtr<BT_ActionNode> NewActionNode = MakeShareable(new BT_ActionNode(InPathFindNode->Action));
		InBT_Node->AddChild(NewActionNode);
	}
	else
	{
		for (const auto& Child : InPathFindNode->Leaves)
		{
			/* Keeping this 'legacy' code in case I want to use specific nodes again, instead of sequencers only.
			if (Child->Leaves.IsEmpty())
			{
				const TSharedPtr<BT_ActionNode> NewActionNode = MakeShareable(new BT_ActionNode(Child->Action));
				InBT_Node->AddChild(NewActionNode);
				
			}
			else
			{
				const TSharedPtr<BT_SequencerNode> NewSequencerNode = MakeShareable(new BT_SequencerNode(Child->Action));
				InBT_Node->AddChild(NewSequencerNode);
				PopulateBT(NewSequencerNode, Child);
			}
			*/
			const TSharedPtr<BT_SequencerNode> NewSequencerNode = MakeShareable(new BT_SequencerNode(Child->Action));
			InBT_Node->AddChild(NewSequencerNode);
			
			if (!Child->Leaves.IsEmpty())
			{
				PopulateBT(NewSequencerNode, Child);
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
