// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerStats.h"
#include "FootballBall.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KFC_API UFootballBall : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootballBall();

	


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable, Category = "Ball") //TODO rename this function to BroadcastBallOwned or something, confusing with UPlayerStats::SetHasBall
	void SetHasBall(const UPlayerStats* Player);

	UFUNCTION(BlueprintCallable)
	void PushBall(const FVector& Dir, float Force, bool bImpulse = true, bool bVelChange = false);

	UFUNCTION(BlueprintCallable)
	FVector GetBallDirection(const FVector Player) const;

	UStaticMeshComponent* GetBallMesh() const { return BallMesh; }


private:
	UPROPERTY()
	UStaticMeshComponent* BallMesh = nullptr;

	UPROPERTY()
	UPlayerStats* Owner = nullptr;

		
};
