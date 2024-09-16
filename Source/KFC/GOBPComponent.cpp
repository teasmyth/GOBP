// Fill out your copyright notice in the Description page of Project Settings.


#include "GOBPComponent.h"

#include "Editor.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/SavePackage.h"


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
	FindAllActions();

	BehaviorTreeComponent = GetOwner()->FindComponentByClass<UBehaviorTreeComponent>();
	BlackboardComponent = GetOwner()->FindComponentByClass<UBlackboardComponent>();

	if (BehaviorTreeComponent && BlackboardComponent)
	{
		BehaviorTree = GOBPlanner::Plan(this, Priority, Actions, Goals);


		if (BehaviorTree)
		{
			UBlackboardData* BlackboardAsset = NewObject<UBlackboardData>();
			BehaviorTree->BlackboardAsset = BlackboardAsset;
			BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
			BehaviorTreeComponent->StartTree(*BehaviorTree);
		}
	}


	// ...
}

void UGOBPComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Actions.Empty();
	Super::EndPlay(EndPlayReason);

	BehaviorTree = nullptr;
	BehaviorTreeComponent = nullptr;
	BlackboardComponent = nullptr;
}


// Called every frame
void UGOBPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*
	if (!Planner.IsValid() || Queue.IsEmpty())
	{
		Planner->Plan(Actions);
	})
	*/
}

void UGOBPComponent::FindAllActions()
{
	Actions.Empty();
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	FARFilter Filter;
	//Filter.ClassPaths.Add(UGobpAction::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;
	Filter.PackagePaths.Add("/Game/GOBP/Actions");

	//UE_LOG(LogTemp, Warning, TEXT("Using ClassPath: %s"), *UGobpAction::StaticClass()->GetClassPathName().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Searching in path: /Game/GOBP/Actions"));

	AssetRegistry.GetAssets(Filter, AssetDataList);

	if (AssetDataList.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No assets found at path: /Game/GOBP/Actions"));
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
