// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/FootballBall.h"

#include "Football/FootballEventManager.h"
#include "Football/PlayerStats.h"

// Sets default values for this component's properties
UFootballBall::UFootballBall()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFootballBall::BeginPlay()
{
	Super::BeginPlay();

	BallMesh = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent());

	// ...
}


// Called every frame
void UFootballBall::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFootballBall::SetHasBall(const UPlayerStats* Player)
{
	UFootballEventManager::GetInstance()->BroadcastBallOwned(Player);
}

void UFootballBall::PushBall(const FVector& Dir, const float Force, const bool bImpulse, const bool bVelChange)
{
	if (BallMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("BallMesh is null"));
		return;
	}

	BallMesh->ComponentVelocity = FVector::ZeroVector;

	if (bImpulse)
	{
		BallMesh->AddImpulse(Dir * Force, NAME_None, bVelChange);
	}
	else
	{
		BallMesh->AddForce(Dir * Force, NAME_None, bVelChange);
	}
}

FVector UFootballBall::GetBallDirection(const FVector Player) const
{
	return (GetOwner()->GetActorLocation() - Player).GetSafeNormal2D();
}

void UFootballBall::DisableBallCollisionWithPlayers(const bool bDisable)
{
	if (BallMesh == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("BallMesh is null"));
		return;
	}

	if (bDisable)
	{
		BallMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	else
	{
		BallMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}
