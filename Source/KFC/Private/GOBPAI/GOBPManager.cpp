// Fill out your copyright notice in the Description page of Project Settings.


#include "GOBPAI/GOBPManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Football/FootballBall.h"
#include "Kismet/GameplayStatics.h"


AGOBPManager* AGOBPManager::Instance = nullptr;

// Sets default values
AGOBPManager::AGOBPManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (Instance != this && ThisInstance)
	{
		// Handle the error case where another instance is created
		UE_LOG(LogTemp, Error, TEXT("Multiple instances of AGOBPManager detected!"));
		//Destroy(); This would not let us make blueprints of this class. So it is commented out. That's why I have the ThisInstance bool.
	}
	else
	{
		Instance = this;
	}

}

AGOBPManager* AGOBPManager::GetGOBPManagerInstance()
{
	if (Instance == nullptr)
	{
		// Optionally, you could create the instance here if it doesn't exist
		// Instance = NewObject<AGOBPManager>();
		// But i am not using it that way.
		UE_LOG(LogTemp, Error, TEXT("AGOBPManager instance is not created yet!"));
	}
	
	
	return Instance;
}

// Called when the game starts or when spawned
void AGOBPManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetGOBPManagerInstance() != this || !ThisInstance)
	{
		Destroy();
	}
	
	FindAllActions();

	TArray<AActor*> BallActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Ball"), BallActors);
	if (BallActors.Num() == 1)
	{
		Ball = BallActors[0]->FindComponentByClass<UFootballBall>();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Ball is not found or there are multiple balls in the scene"));
	}

	bIsSetup = true;
}

// Called every frame
void AGOBPManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGOBPManager::FindAllActions()
{
	Actions.Empty();
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;
	FARFilter Filter;
	//Filter.ClassPaths.Add(UGobpAction::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;
	Filter.PackagePaths.Add("/Game/FootballContent/Actions");

	//UE_LOG(LogTemp, Warning, TEXT("Using ClassPath: %s"), *UGobpAction::StaticClass()->GetClassPathName().ToString());
	//UE_LOG(LogTemp, Warning, TEXT("Searching in path: /Game/GOBP/Actions"));

	AssetRegistry.GetAssets(Filter, AssetDataList);

	if (AssetDataList.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No assets found at path: /Game/FootballContent/Actions"));
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
