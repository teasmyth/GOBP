#pragma once

#include "CoreMinimal.h"
#include "WorldStates.h"
#include "KFC/Public/GOBPAI/EBT_NodeState.h"
#include "GobpAction.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EActionType : uint8
{
	Null,
	Action,
	Selector,
	Picker,
	Decorator
};

//It seems I cant wrap U-stuff in a namespace, sadly
UCLASS(Blueprintable)
class KFC_API UGobpAction : public UObject
{
	GENERATED_BODY()

public:
	UGobpAction();

	
	UPROPERTY(meta = (ToolTip = "The target location of the action, whether a person or a location to kick the ball to."))
	FVector TargetLocation = FVector::ZeroVector;
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GOBP")
	EActionType ActionType = EActionType::Null;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GOBP")
	FString Name = "Action";
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GOBP")
	TSet<EConditions> PreConditions;



	//Note: Previously this was a nested array wrapped with a struct to display on Editor. However, because it is wrapped with a struct,
	//Everytime we extend the ECondition enum, it will lose data due to how UE works. This can become very annoying very quickly.
	//As a temporary (?) solution, I added an enum called "Divider" to separate the effects of the action for each possible outcome.
	//While maintaining a linear array, which is translated into a 2D array in the code. This lets us have a simlated array of arrays, while maintaining
	//Visualization and data-loss prevention. I know it is ugly and perhaps there is a solution to this, but I am not aware of it.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GOBP")
	TArray<EConditions> AfterEffects; //This is used to publicly show the effects of the action for each possible outcome.
	TArray<TArray<EConditions>> Effects; //This is used to store the effects of the action for each possible outcome.
	
	bool Running = false;
	
	
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	bool IsAchievableBP();
	//By default, it will call the Blueprint version. This way, we can use actions both in C++ and BP with single function.
	//The purpose of this function to check if an action is achievable given the world state.
	virtual bool IsAchievable();

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	bool IsAchievableGivenConditionsBP(const TArray<EConditions>& InConditions);
	//By default, it will call the Blueprint version. This way, we can use actions both in C++ and BP with single function.
	//The purpose of this function is to evaluate whether this action can be executed given the previous action's outcome.
	virtual bool IsAchievableGivenConditions(const TArray<EConditions>& InConditions);

	
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	void StartActionBP();
	virtual void StartAction();

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	void EndActionBP();
	virtual void EndAction();
	
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	EBT_NodeState ExecuteActionBP();
	virtual EBT_NodeState ExecuteAction();

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP", meta = (DisplayName = "Pick Next Action", Tooltip = "This runs after Start action. It is used to determine the next action to be taken."))
	TArray<EConditions> PickNextActionBP();
	//This runs after Start action. It is used to determine the next action to be taken.
	virtual TArray<EConditions> PickNextAction();

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	float CalculateCostBP();
	virtual float CalculateCost();

	float GetCost() { return CalculateCost(); }

	//Checking if of the preconditions of the new node can be satisfied by the current node's effects.
	virtual bool CanBeDoneBeforeNewAction(const TArray<EConditions>& InConditions) const;
	void SetupAction();

private:
	void ConvertEffectsToTArray();
	
	//This function is used to add the preconditions of the action to the after effects of the action.
	//This way, we don't have to write every condition carrying over to the next action.
	//Otherwise, every action would have to carry over the conditions of the previous action.
	void AddConditionsToAfterEffects();

	
};
