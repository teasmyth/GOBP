// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/Actions/DetermineDribbleDirection.h"

#include "GOBPAI/GOBPManager.h"


EBT_NodeState UDetermineDribbleDirection::ExecuteAction(UPlayerStats* Player)
{
	const UPlayerStats* Defender = Player->GetTargetPlayer();
	if (!Defender)
	{
		//UE_LOG(LogTemp, Error, TEXT("Player %d has no target player"), Player->GetPlayerID());
		const FVector TargetDir = (AGOBPManager::GetGOBPManagerInstance()->GetAwayGoal()->GetActorLocation() - Player->GetComponentLocation()).GetSafeNormal();
		Player->SetTargetDirection(TargetDir);
		return EBT_NodeState::Running;
	}

	const FVector ToDefender = Defender->GetComponentLocation() - Player->GetComponentLocation(); // Vector from attacker to defender
	const FVector ToGoal = (AGOBPManager::GetGOBPManagerInstance()->GetAwayGoal()->GetActorLocation() - Player->GetComponentLocation()).
		GetSafeNormal(); // General direction towards the goal

	// Get a perpendicular direction to the defender (either left or right)
	const FVector Perpendicular = FVector::CrossProduct(ToDefender, FVector::UpVector).GetSafeNormal();

	// Randomly decide to go left or right based on the situation (attacker's vision and defender's positioning)
	const int RandomFactor = FMath::RandRange(-1, 1);
	FVector DribbleDirection = RandomFactor > 0 ? Perpendicular : -Perpendicular;

	// Adjust the dribble direction based on how far off-center the defender is
	if (DefenderBlockStraightPath(Player->GetComponentLocation(), Player->GetTargetPlayer()->GetComponentLocation()))
	{
		constexpr float SomeWeight = 1.2f;
		DribbleDirection += ToGoal * SomeWeight; // Drift slightly toward the goal
	}

	Player->SetTargetDirection(DribbleDirection.GetSafeNormal());
	return EBT_NodeState::Running;
}

bool UDetermineDribbleDirection::DefenderBlockStraightPath(const FVector& AttackerPosition, const FVector& DefenderPosition)
{
	// Calculate the direction from attacker to the goal
	const FVector ToGoal = (AGOBPManager::GetGOBPManagerInstance()->GetAwayGoal()->GetActorLocation() - AttackerPosition).GetSafeNormal();

	// Calculate the direction from attacker to the defender
	const FVector ToDefender = (DefenderPosition - AttackerPosition).GetSafeNormal();

	// Calculate the dot product between the two directions
	// If the defender is perfectly in line between attacker and goal, the dot product will be close to 1
	const float Alignment = FVector::DotProduct(ToGoal, ToDefender);

	// If the alignment is close to 1, the defender is centered; otherwise, they're off-center
	// Threshold determines how much off-center is considered "significant" for the attacker to react
	constexpr float Threshold = 0.9f; // Tweak this value based on your game logic

	return Alignment < Threshold; // If Alignment is less than the threshold, the defender is off-center
}
