// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/Actions/DetermineDribbleDirection.h"

#include "GOBPAI/GOBPManager.h"


EBT_NodeState UDetermineDribbleDirection::ExecuteAction(UPlayerStats* Player)
{
	const auto Defenders = Player->GetNearbyOpponents();
	FVector Dir;
	if (Defenders.IsEmpty())
	{
		Dir = (Player->GetOpponentGoal()->GetActorLocation() - Player->GetComponentLocation()).GetSafeNormal();
		//Player->SetTargetDirection((Player->GetOpponentGoal()->GetActorLocation() - Player->GetComponentLocation()).GetSafeNormal());
		//return EBT_NodeState::Running;
	}
	else
	{
		Dir = CalculateDribbleDirection(Player, Defenders);
	}
	
	DrawDebugLine(GetWorld(), Player->GetOwner()->GetActorLocation(), Player->GetOwner()->GetActorLocation() + Dir * 100, FColor::Green, false, 0.001f, 0, 1.0f);
	Player->SetTargetDirection(Dir);
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

FVector UDetermineDribbleDirection::CalculateDribbleDirection(const UPlayerStats* Player, const TArray<UPlayerStats*>& NearbyOpponents) const
{
	const FVector AttackerPosition = Player->GetOwner()->GetActorLocation();
	const FVector ToGoalNormal = (Player->GetOpponentGoal()->GetActorLocation() - AttackerPosition).GetSafeNormal();
	FVector RepulsiveForceNormal = FVector::ZeroVector;

	const float SafeDistance = Player->GetProximitySensorRadius() / 2.0f; // Safe distance threshold (in units, tweak as needed) How much should it be?

	const float MinDistance = 50.0f;  // Too close, very strong repulsion
	const float MaxDistance = 500.0f;  // Beyond this distance, less repulsion
	const float RepulsionFactor = 1000.0f;

	for (const UPlayerStats* Opponent : NearbyOpponents)
	{
		const FVector OpponentPosition = Opponent->GetOwner()->GetActorLocation();
		const FVector ToOpponent = OpponentPosition - AttackerPosition;
		const FVector Perpendicular = FVector::CrossProduct(ToOpponent, FVector::UpVector).GetSafeNormal();
		const int RandomFactor = FMath::RandRange(-1, 1);
		FVector DribbleDirectionNormal = Perpendicular; //RandomFactor > 0 ? Perpendicular : -Perpendicular; cant have constant randomness in update.

		const float DistanceToOpponent = ToOpponent.Size();

		//VERY WIP

		// If within the safe distance, calculate repulsive forces
		//if (DistanceToOpponent < SafeDistance)
		{
			float ClampedDistance = FMath::Clamp(DistanceToOpponent, MinDistance, MaxDistance);
			FVector OpponentRepulsionNormal = (-ToOpponent.GetSafeNormal() * SafeDistance / DistanceToOpponent).GetSafeNormal(); //(RepulsionFactor / FMath::Square(ClampedDistance));


			if (!DefenderBlockStraightPath(Player->GetComponentLocation(), OpponentPosition))
			{
				constexpr float SomeWeight = 1.2f; //need to figure this out
				OpponentRepulsionNormal += ToGoalNormal * SomeWeight; // Drift slightly toward the goal
				OpponentRepulsionNormal = OpponentRepulsionNormal.GetSafeNormal();
			}
			
			RepulsiveForceNormal += (OpponentRepulsionNormal + DribbleDirectionNormal).GetSafeNormal();
			//RepulsiveForce += OpponentRepulsion;

		}
	}

	DrawDebugLine(GetWorld(), AttackerPosition + FVector(0,0,50), AttackerPosition + FVector(0,0,50) + RepulsiveForceNormal * 100, FColor::Red, false, 0.001f, 0, 1.0f);

	//Combine the direction to goal with repulsive force
	const  FVector MovementDirection = (ToGoalNormal * 0.5f) + RepulsiveForceNormal; //0.5f weight should depend on aggression maybe?

	// Normalize the final movement direction
	return MovementDirection.GetSafeNormal();
}
