// Fill out your copyright notice in the Description page of Project Settings.


#include "GOBPAI/GOBPManager.h"

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

AGOBPManager* AGOBPManager::GetInstance()
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

	if (GetInstance() != this || !ThisInstance)
	{
		Destroy();
	}

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
	
}

// Called every frame
void AGOBPManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
