// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerStats.generated.h"

/**
 * 
 */

UENUM()
enum class ETeam : uint8
{
	Home,
	Away
};

UENUM()
enum class EPlayerMovementMode : uint8
{
	Walking,
	Running,
	Sprinting,
	Jockeying
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallOwned, const UPlayerStats*, Player);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KFC_API UPlayerStats : public USceneComponent
{
	GENERATED_BODY()

public:
	UPlayerStats();

	static constexpr int32 MAX_STAT_NUMBER = 100;

	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBallOwned OnBallOwned;

	explicit UPlayerStats(const int32 PlayerID, const int32 MaxStamina, const int32 Speed, const int32 Acceleration, const int32 Strength,
	                      const int32 Passing, const int32 Accuracy, const int32 Defense, const int32 Tackling, const int32 Goalkeeping,
	                      const int32 BallSkills, const int32 Shooting, const int32 Dribbling, const int32 Positioning, const int32 Vision,
	                      const ETeam Team)
	{
		this->PlayerID = PlayerID;
		this->Team = Team;
		
		this->MaxStamina = MaxStamina;
		this->Speed = Speed;
		this->Acceleration = Acceleration;
		this->Strength = Strength;
		this->Passing = Passing;
		this->Accuracy = Accuracy;
		this->Defense = Defense;
		this->Tackling = Tackling;
		this->Goalkeeping = Goalkeeping;
		this->BallSkills = BallSkills;
		this->Shooting = Shooting;
		this->Dribbling = Dribbling;
		this->Positioning = Positioning;
		this->Vision = Vision;

		MovementComponent = Cast<UCharacterMovementComponent>(GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
		if (MovementComponent == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerStats: MovementComponent is null"));
		}
		else
		{
			DefaultSpeed = MovementComponent->MaxWalkSpeed;
		}
	}
	

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	bool HasBall() const
    {
        return bHasBall;
    }

#pragma region  Getters
	UFUNCTION(BlueprintPure)
	int32 GetPlayerID() const
	{
		return PlayerID;
	}

	UFUNCTION(BlueprintPure)
	FVector GetTargetDirection() const
	{
		return TargetDirection;
	}

	UFUNCTION(BlueprintPure)
	UPlayerStats* GetTargetPlayer() const
	{
		return TargetPlayer;
	}

	UFUNCTION(BlueprintPure)
	int32 GetMaxStamina() const
	{
		return MaxStamina;
	}

	UFUNCTION(BlueprintPure)
	int32 GetCurrentStamina() const
	{
		return CurrentStamina;
	}

	UFUNCTION(BlueprintPure)
	int32 GetCurrentMaxStamina() const
	{
		return CurrentMaxStamina;
	}

	UFUNCTION(BlueprintPure)
	int32 GetSpeed() const
	{
		return Speed;
	}

	UFUNCTION(BlueprintPure)
	int32 GetAcceleration() const
	{
		return Acceleration;
	}

	UFUNCTION(BlueprintPure)
	int32 GetStrength() const
	{
		return Strength;
	}

	UFUNCTION(BlueprintPure)
	int32 GetPassing() const
	{
		return Passing;
	}

	UFUNCTION(BlueprintPure)
	float GetAccuracy() const
	{
		return Accuracy;
	}

	UFUNCTION(BlueprintPure)
	int32 GetDefense() const
	{
		return Defense;
	}

	UFUNCTION(BlueprintPure)
	int32 GetTackling() const
	{
		return Tackling;
	}

	UFUNCTION(BlueprintPure)
	int32 GetGoalkeeping() const
	{
		return Goalkeeping;
	}

	UFUNCTION(BlueprintPure)
	int32 GetBallSkills() const
	{
		return BallSkills;
	}

	UFUNCTION(BlueprintPure)
	int32 GetShooting() const
	{
		return Shooting;
	}

	UFUNCTION(BlueprintPure)
	int32 GetDribbling() const
	{
		return Dribbling;
	}

	UFUNCTION(BlueprintPure)
	int32 GetPositioning() const
	{
		return Positioning;
	}

	UFUNCTION(BlueprintPure)
	int32 GetVision() const
	{
		return Vision;
	}

	float GetVelocity() const
	{
		return FVector(MovementComponent->Velocity.X, MovementComponent->Velocity.Y, 0).Size(); //Probably Z will be 0 but just in case.
	}

#pragma endregion 

	UFUNCTION(BlueprintCallable)
	void SetTargetDirection(const FVector Direction)
	{
		TargetDirection = Direction;
	}

	UFUNCTION(BlueprintCallable)
	void SetTargetPlayer(UPlayerStats* Player)
	{
		TargetPlayer = Player;
	}
	
	UFUNCTION(BlueprintCallable)
	void AddStamina(const int32 Stamina)
	{
		CurrentStamina += Stamina;
		if (CurrentStamina > MaxStamina)
		{
			CurrentStamina = MaxStamina;
		}
	}

	UFUNCTION(BlueprintPure)
	bool CanSprint() const
	{
		return CurrentStamina > 0;
	}

	UFUNCTION(BlueprintCallable)
	void Run(const FVector Dir);

	UFUNCTION(BlueprintCallable)
	void Sprint(const FVector Dir);

	UFUNCTION(BlueprintCallable)
	void Jockey(const FVector Dir);

	

	UFUNCTION(BlueprintCallable)
	void ResetTimer()
	{
		InternalTimer = 0.0f;
	}

	UFUNCTION(BlueprintPure)
	UCharacterMovementComponent* GetMovementComponent() const
	{
		return MovementComponent;
	}

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	//"Blackboard" variables

	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Blackboard")
	FVector TargetDirection;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Blackboard")
	UPlayerStats* TargetPlayer = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Blackboard")
	bool bHasBall;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Blackboard")
	bool bIsControllingBall = false;

	//Player stats
	
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	int32 PlayerID;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	ETeam Team = ETeam::Home;

	//UPROPERTY(EditAnywhere, Category = "Player Stats")
	//EPosition Position; TODO: Implement this

	//Movement
	UPROPERTY(EditAnywhere, Category = "Player Stats | Movement")
	int32 MaxStamina;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Movement")
	float CurrentMaxStamina;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Movement")
	float CurrentStamina;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Movement")
	int32 Speed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Movement")
	int32 Acceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Movement")
	int32 Strength;


	//Ball mechanics
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ball Mechanics")
	int32 Passing;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ball Mechanics")
	int32 Accuracy;


	//Defense
	UPROPERTY(EditAnywhere, Category = "Player Stats | Defense")
	int32 Defense;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Defense")
	int32 Tackling;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Defense")
	int32 Goalkeeping;

	//Attacking
	UPROPERTY(EditAnywhere, Category = "Player Stats | Attacking")
	int32 BallSkills;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Attacking")
	int32 Shooting;

	//General
	UPROPERTY(EditAnywhere, Category = "Player Stats | Miscellaneous")
	int32 Dribbling;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Miscellaneous")
	int32 Positioning;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Miscellaneous")
	int32 Vision;


	UPROPERTY(VisibleAnywhere)
	EPlayerMovementMode MovementMode = EPlayerMovementMode::Walking;

	float InternalTimer = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void SetControllingBall(const bool Value);
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	bool IsControllingBall() const
	{
		return bIsControllingBall;
	}
	

	//Important for normalizing the movement across all agents, so the speed stats are relative to each other.
	float DefaultSpeed = 0.0f;
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent = nullptr;


	UFUNCTION(BlueprintCallable, Category = "Player Stats")	
	AActor* GetHomeGoal() const;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
    AActor* GetOpponentGoal() const;
	

	
	UFUNCTION()
	void SetHasBall(const UPlayerStats* Player);
	


	void SetMovementMode(const EPlayerMovementMode Mode);
};
