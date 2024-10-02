// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOBPAI/GOBPAction.h"
#include "GOBPManager.generated.h"

class UFootballBall;

UCLASS()
class KFC_API AGOBPManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGOBPManager();

	UFUNCTION(BlueprintPure, Category = "GOBP Manager")
	static AGOBPManager* GetGOBPManagerInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GOBP")
	bool ThisInstance = false;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool IsSetup() const { return bIsSetup; }
	TArray<UGobpAction*> GetActions() const { return Actions; }
	float GetSpeedDiffMultiplier() const { return SpeedDifferenceModifier; }
	float GetRunModifier() const { return RunModifier;}
	float GetJockeyModifier() const { return JockeyModifier; }
	float GetStaminaLossRatioPerUsedStamina() const { return StaminaLossRatioPerUsedStamina; }

	UFUNCTION(BlueprintPure)
	float GetMaxStrengthShootDistance() const { return MaxStrengthShootDistance; }

	UFUNCTION(BlueprintCallable, Category = "GOBP")
	UFootballBall* GetBall() const { return Ball; }


	UPROPERTY(EditAnywhere, Category = "GOBP")
	AActor* HomeGoal;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	AActor* AwayGoal;

	UFUNCTION(BlueprintCallable, Category = "GOBP")
	AActor* GetHomeGoal() const { return HomeGoal; }

	UFUNCTION(BlueprintCallable, Category = "GOBP")
	AActor* GetAwayGoal() const { return AwayGoal; }

	


private:
	static AGOBPManager* Instance;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	TArray<UGobpAction*> Actions;

	UPROPERTY()
    UFootballBall* Ball;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	float RunModifier;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	float SpeedDifferenceModifier;
	UPROPERTY(EditAnywhere, Category = "GOBP")
	float JockeyModifier;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	float StaminaLossRatioPerUsedStamina;

	UPROPERTY(EditAnywhere, Category = "GOBP")
	float MaxStrengthShootDistance;

	bool bIsSetup = false;


	void FindAllActions();
};
