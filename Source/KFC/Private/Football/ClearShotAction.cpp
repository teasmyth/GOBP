// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/ClearShotAction.h"
#include "Football/FootballBall.h"
#include "GOBPAI/GOBPManager.h"

UClearShotAction::UClearShotAction()
{
	ActionType = EActionType::Action;

	PreConditions = { EConditions::ClearShot };
	AfterEffects = { EConditions::Shoot };

	Name = "Clear Shot Action";
}

EBT_NodeState UClearShotAction::StartAction(UPlayerStats* Player)
{
	
	ShootBall(Player);
	
	return Success;
}

FVector UClearShotAction::CalculateIdealShot(const UPlayerStats* Player) const
{
	if (!AGOBPManager::GetGOBPManagerInstance()) return FVector::ZeroVector;

    // Get positions of the ball and the goal
    const FVector BallPosition = AGOBPManager::GetGOBPManagerInstance()->GetBall()->GetComponentLocation();
    const FVector TargetPosition = AGOBPManager::GetGOBPManagerInstance()->GetAwayGoal()->GetActorLocation();

    // Player strength scaling
	const float PlayerStrength = static_cast<float>(Player->GetStrength()) / UPlayerStats::MAX_STAT_NUMBER;  // 0.6 for 60/100
    const float MaxVelocity = 44.0f * 100.0f;  // Convert m/s to cm/s for Unreal units - Uses 95km/h speed for 60 strength
    const float AdjustedVelocity = PlayerStrength * MaxVelocity;  // Scaled velocity based on player strength

    // Calculate horizontal displacement
    const FVector HorizontalDisplacement = FVector((TargetPosition.X - BallPosition.X) * RandomAccuracyMultiplier(Player), (TargetPosition.Y - BallPosition.Y) * RandomAccuracyMultiplier(Player), 0.0f);
    const float HorizontalDistance = HorizontalDisplacement.Size();  // Horizontal distance in cm

    // Time of flight estimate based on horizontal velocity
    const float HorizontalVelocity = AdjustedVelocity;  // Approximate horizontal velocity based on player's strength
    const float TimeOfFlight = HorizontalDistance / HorizontalVelocity;  // Estimate time to reach the goal horizontally

    // Calculate the direction vector for the horizontal movement
    const FVector VelocityXY = HorizontalDisplacement.GetSafeNormal() * HorizontalVelocity;
	
    // Calculate the vertical velocity to reach the target height
    const float TargetHeight = (TargetPosition.Z - BallPosition.Z) * RandomAccuracyMultiplier(Player);  // Height difference in cm 
    const float Gravity = 981.0f;  // Gravity in Unreal (cm/s^2)

    // Calculate vertical velocity using the kinematic equation for motion under constant acceleration:
    // Vz = (TargetHeight + 0.5 * Gravity * Time^2) / Time	
    const float VerticalVelocity = (TargetHeight + 0.5f * Gravity * FMath::Square(TimeOfFlight)) / TimeOfFlight;

    // Combine horizontal and vertical velocity components
    const FVector FinalShotVelocity = FVector(VelocityXY.X, VelocityXY.Y, VerticalVelocity);
	

    return FinalShotVelocity;  // Return the combined velocity vector
}


FVector UClearShotAction::ApplyAngularDeviation(const FVector& ShotDirection, float DeviationAngleX, float DeviationAngleY)
{
	// Rotate around Y-axis for horizontal deviation (left-right deviation)
	FVector DeviationApplied = ShotDirection.RotateAngleAxis(DeviationAngleX, FVector(0, 1, 0));

	// Rotate around X-axis for vertical deviation (up-down deviation)
	DeviationApplied = DeviationApplied.RotateAngleAxis(DeviationAngleY, FVector(1, 0, 0));

	return DeviationApplied;
}

void UClearShotAction::ShootBall(const UPlayerStats* Player)
{
	const FVector IdealShotVector = CalculateIdealShot(Player);
	float PowerMultiplier = Player->GetStrength() / UPlayerStats::MAX_STAT_NUMBER;  // Ranges from 0.0 to 1.0
	const FVector FinalShotVector = IdealShotVector * PowerMultiplier;

	const float MaxDeviationAngle = 20.0f;
	float MaxDeviation = (100 - Player->GetAccuracy()) / 100.0f * MaxDeviationAngle;
	float RandomDeviationX = FMath::RandRange(-MaxDeviation, MaxDeviation);  // Horizontal deviation
	float RandomDeviationY = FMath::RandRange(-MaxDeviation, MaxDeviation);  // Vertical deviation

	// Apply the angular deviation to the shot direction
	const FVector FinalShotVelocity = ApplyAngularDeviation(FinalShotVector, RandomDeviationX, RandomDeviationY);

	const float ShootingSkill = Player->GetShooting() / UPlayerStats::MAX_STAT_NUMBER;
	PowerMultiplier += ShootingSkill * 1; //CorrectionFactor;
	//AccuracyFactor += ShootingSkill * CorrectionFactor;

	const FVector Shot = FinalShotVelocity;// * ShootingSkill;

	const float BallMass = AGOBPManager::GetGOBPManagerInstance()->GetBall()->GetBallMesh()->GetMass();  // Ball mass in kg

	//AGOBPManager::GetGOBPManagerInstance()->GetBall()->PushBall(IdealShotVector * BallMass, 1, true,true);

	AGOBPManager::GetGOBPManagerInstance()->GetBall()->GetBallMesh()->SetPhysicsLinearVelocity(IdealShotVector);

}
