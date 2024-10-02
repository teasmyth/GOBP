// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GOBPAI/GOBPAction.h"
#include "GOBPAI/GOBPlanner.h"
#include "GOBPComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KFC_API UGOBPComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGOBPComponent();

	

	UPROPERTY(EditAnywhere)
	UPlayerStats* PlayerStats = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<UGobpAction*> Actions;

	UPROPERTY(EditAnywhere)
	EPriority Priority = EPriority::PrioritizeGoals;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "The goals that the AI will try to achieve. Higher values mean higher priority."))
	TSet<FWorldState> Goals;

	UPROPERTY(EditAnywhere)
	UBehaviorTreeComponent* BehaviorTreeComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComponent = nullptr;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree = nullptr;

	UFUNCTION(CallInEditor, Category = "GOBP")
	void PauseUnpauseTree()
	{
		bRunTree = !bRunTree;
	}
	
	TSharedPtr<BT_RootNode> RootNode;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bRunTree = true;

};
