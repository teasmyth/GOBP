// Fill out your copyright notice in the Description page of Project Settings.

#include "GOBPComponent.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetRegistryModule.h"
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


void UGOBPComponent::PushBall()
{
	if (AGOBPManager::GetInstance() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GOBPManager is null"));
		return;
	}

	AActor* Ball = AGOBPManager::GetInstance()->GetBall();
	if ( Ball == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Ball is null"));
		return;
	}

	const FVector Dir = FVector(0,1,-0);
	
	//Cast<UStaticMeshComponent>(Ball->GetRootComponent())->ComponentVelocity = FVector::Zero();
	Cast<UStaticMeshComponent>(Ball->GetRootComponent())->AddImpulse(Dir * 1000, NAME_None, true);
}

// Called when the game starts
void UGOBPComponent::BeginPlay()
{
	Super::BeginPlay();
	FindAllActions();

	PlayerStats = GetOwner()->FindComponentByClass<UPlayerStats>();

	BehaviorTreeComponent = GetOwner()->FindComponentByClass<UBehaviorTreeComponent>();
	BlackboardComponent = GetOwner()->FindComponentByClass<UBlackboardComponent>();


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
	for (const auto& Action : Actions)
	{
		delete Action;
	}
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
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Plan finished"));
			//RootNode->ResetNode();
		}
	}
}

void UGOBPComponent::FindAllActions()
{
	Actions.Empty();
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	FARFilter Filter;
	//Filter.ClassPaths.Add(UGobpAction::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;
	Filter.PackagePaths.Add("/Game/Football/Actions");

	//UE_LOG(LogTemp, Warning, TEXT("Using ClassPath: %s"), *UGobpAction::StaticClass()->GetClassPathName().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Searching in path: /Game/GOBP/Actions"));

	AssetRegistry.GetAssets(Filter, AssetDataList);

	if (AssetDataList.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No assets found at path: /Game/Football/Actions"));
		return;
	}

	for (const auto& AssetData : AssetDataList)
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(AssetData.GetAsset());
		if (Blueprint && Blueprint->GeneratedClass->IsChildOf(UGobpAction::StaticClass()))
		{
			UGobpAction* ActionInstance = NewObject<UGobpAction>(this, Blueprint->GeneratedClass);
			if (ActionInstance)
			{
				Actions.Add(ActionInstance);
			}
		}
	}

	for (const auto& Action : Actions)
	{
		Action->SetupAction();
	}

	UE_LOG(LogTemp, Warning, TEXT("Found %d actions"), Actions.Num());
}

// ...
