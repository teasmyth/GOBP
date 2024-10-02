// Fill out your copyright notice in the Description page of Project Settings.

#include "GOBPAI/GOBPComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GOBPAI/GOBPManager.h"


// Sets default values for this component's properties
UGOBPComponent::UGOBPComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UGOBPComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerStats = GetOwner()->FindComponentByClass<UPlayerStats>();

	BehaviorTreeComponent = GetOwner()->FindComponentByClass<UBehaviorTreeComponent>();
	BlackboardComponent = GetOwner()->FindComponentByClass<UBlackboardComponent>();

	if (AGOBPManager::GetGOBPManagerInstance())
    {
		while (!AGOBPManager::GetGOBPManagerInstance()->IsSetup())
		{
			//Do I wait?
		}
        Actions = AGOBPManager::GetGOBPManagerInstance()->GetActions();
    }
	else return;

	if (BehaviorTreeComponent && BlackboardComponent)
	{
		if (BehaviorTree != nullptr)
		{
			RootNode.Reset();
			BehaviorTree = nullptr;
		}
		if (GOBPlanner::Plan(PlayerStats, this, Priority, Actions, Goals, BehaviorTree, RootNode))
		{
			UBlackboardData* BlackboardAsset = NewObject<UBlackboardData>();
			BehaviorTree->BlackboardAsset = BlackboardAsset;
			BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			BehaviorTreeComponent->StartTree(*BehaviorTree);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to plan tree"));
		}
	}

	// ...
}

void UGOBPComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	BehaviorTree = nullptr;
	BehaviorTreeComponent = nullptr;
	BlackboardComponent = nullptr;
	RootNode = nullptr;
	Actions.Empty();
}


// Called every frame
void UGOBPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (RootNode.IsValid() && bRunTree)
	{
		if (RootNode->RunTree(PlayerStats))
		{
			bRunTree = false;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Plan finished"));
			//RootNode->ResetNode();
		}
	}
}

// ...
