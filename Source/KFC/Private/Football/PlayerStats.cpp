// Fill out your copyright notice in the Description page of Project Settings.


#include "Football/PlayerStats.h"
#include "Football/FootballEventManager.h"
#include "GOBPAI/GOBPManager.h"


UPlayerStats::UPlayerStats(): bHasBall(false), PlayerID(0), MaxStamina(0), CurrentMaxStamina(0), CurrentStamina(0), Speed(0), Acceleration(0),
                              Strength(0), Passing(0), Accuracy(0), Defense(0), Tackling(0), Goalkeeping(0), BallSkills(0), Shooting(0), Dribbling(0),
                              Positioning(0), Vision(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPlayerStats::BeginPlay()
{
	Super::BeginPlay();
	if (MovementComponent == nullptr)
	{
		MovementComponent = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	}

	if (ProximitySensor == nullptr)
	{
		ProximitySensor = GetOwner()->FindComponentByClass<USphereComponent>();

		ProximitySensor->OnComponentBeginOverlap.AddDynamic(this, &UPlayerStats::OnBeginOverlap);
		ProximitySensor->OnComponentEndOverlap.AddDynamic(this, &UPlayerStats::OnEndOverlap);

		//A vision with 50 does not modify proximity sensor, while greater or smaller number does, increasing or decreasing up to by 100.
		//Maybe this should not be hardcoded here.
		const float SphereRadius = ProximitySensor->GetUnscaledSphereRadius() + MAX_STAT_NUMBER * 2 * GetVision() / (MAX_STAT_NUMBER / 2);
		ProximitySensor->SetSphereRadius(SphereRadius);
	}

	CurrentMaxStamina = MaxStamina;
	CurrentStamina = MaxStamina;
	DefaultSpeed = MovementComponent->MaxWalkSpeed;
	UFootballEventManager::GetInstance()->RegisterPlayer(this);
	OnBallOwned.AddDynamic(this, &UPlayerStats::SetHasBall);
}

void UPlayerStats::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UFootballEventManager::GetInstance()->UnregisterPlayer(this);
}

void UPlayerStats::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	InternalTimer += DeltaTime;

	if (MovementMode == EPlayerMovementMode::Sprinting)
	{
		CurrentStamina -= DeltaTime * 10; // This essentially means that a maxed out fresh stamina can sprint for 10 secs.
	}
	else
	{
		CurrentStamina += DeltaTime * 5;
	}
}

#pragma region Movement

void UPlayerStats::Run(const FVector Dir)
{
	if (MovementComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("The player %s is missing MovementComponent or null"), *GetOwner()->GetName());
		return;
	}

	if (AGOBPManager::GetGOBPManagerInstance() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GOBPManager is null"));
		return;
	}

	SetMovementMode(EPlayerMovementMode::Running);


	MovementComponent->bOrientRotationToMovement = true;
	const float AimedSpeed = DefaultSpeed + Speed * AGOBPManager::GetGOBPManagerInstance()->GetSpeedDiffMultiplier() *
		AGOBPManager::GetGOBPManagerInstance()->
		GetRunModifier();
	float ActualAcceleration = Acceleration * CurrentMaxStamina / MaxStamina / 100.0f * InternalTimer;
	ActualAcceleration = ActualAcceleration > 1.0f ? 1.0f : ActualAcceleration;


	MovementComponent->MaxWalkSpeed = AimedSpeed * ActualAcceleration;
	MovementComponent->AddInputVector(Dir);
}

void UPlayerStats::Sprint(const FVector Dir)
{
	if (MovementComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("The player %s is missing MovementComponent or null"), *GetOwner()->GetName());
		return;
	}

	if (AGOBPManager::GetGOBPManagerInstance() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GOBPManager is null"));
		return;
	}

	if (CurrentStamina <= 0 && MovementMode == EPlayerMovementMode::Sprinting || CurrentStamina <= CurrentMaxStamina * 0.2f)
	{
		Run(Dir);
		return;
	}

	SetMovementMode(EPlayerMovementMode::Sprinting);

	MovementComponent->bOrientRotationToMovement = true;
	const float AimedSpeed = DefaultSpeed + Speed * (AGOBPManager::GetGOBPManagerInstance()->GetSpeedDiffMultiplier() * (1 +
		AGOBPManager::GetGOBPManagerInstance()->
		GetRunModifier()));
	float ActualAcceleration = Acceleration * CurrentStamina / CurrentMaxStamina / 100.0f * InternalTimer;
	ActualAcceleration = ActualAcceleration > 1.0f ? 1.0f : ActualAcceleration;

	MovementComponent->MaxWalkSpeed = AimedSpeed * ActualAcceleration;
	MovementComponent->AddInputVector(Dir);
}

void UPlayerStats::Jockey(const FVector Dir)
{
}

void UPlayerStats::SetMovementMode(const EPlayerMovementMode Mode)
{
	if (MovementMode == Mode)
	{
		return;
	}

	switch (MovementMode)
	{
	case EPlayerMovementMode::Running:
		break;
	case EPlayerMovementMode::Sprinting:
		CurrentMaxStamina = (CurrentMaxStamina - CurrentStamina) * AGOBPManager::GetGOBPManagerInstance()->GetStaminaLossRatioPerUsedStamina();;
		break;
	case EPlayerMovementMode::Jockeying:
		break;

	case EPlayerMovementMode::Walking:
		break;

	default: break;
	}

	switch (Mode)
	{
	case EPlayerMovementMode::Running:
		if (MovementMode != EPlayerMovementMode::Sprinting)
		{
			InternalTimer = 0.0f;
		}
		break;
	case EPlayerMovementMode::Sprinting:
		if (MovementMode != EPlayerMovementMode::Running)
		{
			InternalTimer = 0.0f;
		}
		break;
	case EPlayerMovementMode::Jockeying:
		InternalTimer = 0.0f;
		break;
	case EPlayerMovementMode::Walking:
		break;
	default: break;
	}

	MovementMode = Mode;
}

#pragma endregion

#pragma region Sensors

void UPlayerStats::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UPlayerStats* OtherPlayer = OtherActor->FindComponentByClass<UPlayerStats>();
	
	if (OtherPlayer != nullptr && OtherPlayer->Team != Team && !NearbyOpponents.Contains(OtherPlayer))
	{
		NearbyOpponents.Add(OtherPlayer);
		UE_LOG(LogTemp, Warning, TEXT("Player %s is near player %s"), *GetOwner()->GetName(), *OtherActor->GetName());
	}
}

void UPlayerStats::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UPlayerStats* OtherPlayer = OtherActor->FindComponentByClass<UPlayerStats>();
	if (OtherPlayer != nullptr && NearbyOpponents.Contains(OtherPlayer))
	{
		NearbyOpponents.Remove(OtherPlayer);
	}
}

#pragma endregion

AActor* UPlayerStats::GetHomeGoal() const
{
	if (AGOBPManager::GetGOBPManagerInstance() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GOBPManager is null"));
		return nullptr;
	}

	if (Team == ETeam::Home)
	{
		return AGOBPManager::GetGOBPManagerInstance()->GetHomeGoal();
	}
	else
	{
		return AGOBPManager::GetGOBPManagerInstance()->GetAwayGoal();
	}
}

AActor* UPlayerStats::GetOpponentGoal() const
{
	if (AGOBPManager::GetGOBPManagerInstance() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GOBPManager is null"));
		return nullptr;
	}

	if (Team == ETeam::Away)
	{
		return AGOBPManager::GetGOBPManagerInstance()->GetHomeGoal();
	}
	else
	{
		return AGOBPManager::GetGOBPManagerInstance()->GetAwayGoal();
	}
}

void UPlayerStats::SetHasBall(const UPlayerStats* Player)
{
	if (Player == this)
	{
		bHasBall = true;
	}
	else
	{
		bHasBall = false;
	}
}
