// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GOBPManager.generated.h"

UCLASS()
class KFC_API AGOBPManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGOBPManager();

	static AGOBPManager* GetInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOBP")
	bool ThisInstance = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float GetSpeedDiffMultiplier() const { return SpeedDifferenceModifier; }
	float GetRunModifier() const { return RunModifier;}
	float GetJockeyModifier() const { return JockeyModifier; }
	float GetStaminaLossRatioPerUsedStamina() const { return StaminaLossRatioPerUsedStamina; }
	AActor* GetBall() const { return Ball; }


	UPROPERTY(EditAnywhere, Category = "GOBP")
	AActor* HomeGoal;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	AActor* AwayGoal;

	UFUNCTION(BlueprintCallable, Category = "GOBP")
	AActor* GetHomeGoal() const { return HomeGoal; }

	UFUNCTION(BlueprintCallable, Category = "GOBP")
	AActor* GetAwayGoal() const { return AwayGoal; }


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOBP")
	float RunModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOBP")
	float SpeedDifferenceModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOBP")
	float JockeyModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOBP")
	float StaminaLossRatioPerUsedStamina;

private:
	static AGOBPManager* Instance;

	UPROPERTY()
    AActor* Ball;


};
