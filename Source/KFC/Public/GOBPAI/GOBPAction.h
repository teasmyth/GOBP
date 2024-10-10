#pragma once

#include "CoreMinimal.h"
#include "WorldStates.h"
#include "KFC/Public/Football/PlayerStats.h"
#include "KFC/Public/GOBPAI/EBT_NodeState.h"
#include "GobpAction.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EActionType : uint8
{
	Null = 0 UMETA(DisplayName = "Null", ToolTip = "Null action, used for initialization."),
	Action = 1 UMETA(DisplayName = "Simple Action", ToolTip = "This action is meant to be one-time executed action. It does not have any children and uses Start Function only."),
	Chain = 2 UMETA(DisplayName = "Simple Chain Action", ToolTip = "This action executes all their children without extra condition check. Fails if all children fail."),
	Selector = 3 UMETA(DisplayName = "Selector Action", ToolTip = "This action will try running their children without extra condition check, as long as they do not fail."),
	Picker = 4 UMETA(DisplayName = "Picker Action", ToolTip = "This action will pick which child to run next based on its PickNext condition check function, first matching child is picked."),
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
	
	
	bool Running = false;
	
	
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	//The purpose of this function to check if an action is achievable given the world state run time, not in planning phase.
	bool IsAchievableBP(UPlayerStats* Player);
	//The purpose of this function to check if an action is achievable given the world state run time, not in planning phase.
	virtual bool IsAchievable(UPlayerStats* Player);

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	//The purpose of this function is to evaluate whether this action can be executed given the previous action's outcome in the planning phase.
	bool IsAchievableGivenConditionsBP(const TArray<EConditions>& InConditions);
	//The purpose of this function is to evaluate whether this action can be executed given the previous action's outcome in the planning phase.
	virtual bool IsAchievableGivenConditions(const TArray<EConditions>& InConditions);
	

	
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	EBT_NodeState StartActionBP(UPlayerStats* Player);
	virtual EBT_NodeState StartAction(UPlayerStats* Player);

	//IMPORTANT NOTE: due to BT structure, this function will only be called after all the children already executed their codes. So, unless it is a final action that leads to the goal, it is pointless to use.
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP", meta = (DisplayName = "End Action", Tooltip = "This runs after Start action. It is used to determine the next action to be taken."))
	EBT_NodeState EndActionBP(UPlayerStats* Player);
	virtual EBT_NodeState EndAction(UPlayerStats* Player);
	
	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	EBT_NodeState ExecuteActionBP(UPlayerStats* Player);
	virtual EBT_NodeState ExecuteAction(UPlayerStats* Player);

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP", meta = (DisplayName = "Pick Next Action", Tooltip = "This runs after Start action. It is used to determine the next action to be taken."))
	TArray<EConditions> PickNextActionBP(UPlayerStats* Player);
	//This runs after Start action. It is used to determine the next action to be taken.
	virtual TArray<EConditions> PickNextAction(UPlayerStats* Player);

	UFUNCTION(BlueprintNativeEvent, Category = "GOBP")
	float CalculateCostBP(UPlayerStats* Player);
	virtual float CalculateCost(UPlayerStats* Player);

	float GetCost(UPlayerStats* Player) { return CalculateCost(Player); }
	
	//The purpose of this function is to evaluate whether this action can be executed given the previous action's outcome in the planning phase.
	bool CanBeDoneBeforeNewAction(const TArray<EConditions>& InConditions) const;
	void SetupAction();

private:
	void ConvertEffectsToTArray();
	TArray<TArray<EConditions>> Effects; //This is used to store the effects of the action for each possible outcome.
	
	//This function is used to add the preconditions of the action to the after effects of the action.
	//This way, we don't have to write every condition carrying over to the next action.
	//Otherwise, every action would have to carry over the conditions of the previous action.
	void AddConditionsToAfterEffects();


protected:
	UFUNCTION(BlueprintCallable, Category = "GOBP Chain")
	bool IsStraightLineClear(UPlayerStats* Player, const AActor* Target) const;

	UFUNCTION(BlueprintCallable, Category = "GOBP Chain")
	float RandomAccuracyMultiplier(const UPlayerStats* Player) const;
};
