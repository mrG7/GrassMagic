// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GrassMagic.h"

#include "GMSpellComponent.generated.h"


UENUM(BlueprintType)
enum class ESpellComponentCurrentAction : uint8
{
	/* Misc States */
	None UMETA(DisplayName = "None"),
	/* Resource Acquire States */
	AcquireResource	UMETA(DisplayName = "Acquire Resource"),
	/* Spell Cast States */
	CastDamageGesture UMETA(DisplayName = "Cast Damage Gesture"),
	CastControlGesture UMETA(DisplayName = "Cast Control Gesture"),
	CastChangeGesture UMETA(DisplayName = "Cast Change Gesture"),
	/* Spell Release States */
	Release UMETA(DisplayName = "Release Spell")
};

UENUM(BlueprintType)
enum class ESpellComponentActionState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Prepare UMETA(DisplayName = "Prepare"),
	InProgress UMETA(DisplayName = "In Progress")
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRASSMAGIC_API UGMSpellComponent : public UActorComponent
{
	GENERATED_BODY()

	const static float Movement_Adjust_Rate;
	const static float Movement_Adjust_Timer_Interval;
	const static float Movement_Adjust_Cuttoff;


public:	

	// Sets default values for this component's properties
	UGMSpellComponent();
	~UGMSpellComponent() = default;

	void BeginPlay() override;

	void Init(float ExpectedMovementInput);
	float AdjustMovement(float Value);

	// This function must be called in the end of any Input release Sequence
	void GenericInputRelease();

	/* Resource acquire interface */
	void HandleAcquireResource_Pressed();
	void HandleAcquireResource_Released();


	/* Spell cast Interface */
	void HandleDamageGesture_Pressed();
	void HandleDamageGesture_Released();

	void HandleControlGesture_Pressed();
	void HandleControlGesture_Released();

	void HandleChangeGesture_Pressed();
	void HandleChangeGesture_Released();

	/* Spell release interface */
	void HandleReleaseSpell_Pressed();
	void HandleReleaseSpell_Released();
	
	UFUNCTION(BlueprintCallable, Category = "Spell System")
	int GetResources() const;

	UFUNCTION(BlueprintCallable, Category = "Spell System")
	ESpellComponentCurrentAction GetCurrentAction() const noexcept { return CurrentAction; }

	UFUNCTION(BlueprintCallable, Category = "Spell System")
	ESpellComponentActionState GetCurrentActionState() const noexcept { return CurrentActionState; }

	UFUNCTION(BlueprintCallable, Category = "Spell System")
	float GetFocus() const;

	UFUNCTION(BlueprintCallable, Category = "Spell System")
	float GetFocusMin() const;

	UFUNCTION(BlueprintCallable, Category = "Spell System")
	float GetFocusMax() const;

	UFUNCTION(BlueprintCallable, Category = "Spell System")
	void SetSpellProjectileBPType(TSubclassOf<AGMSpellProjectile> BPProjectileClass);



private:

	template<typename T>
	void HandleInputGeneric_Pressed( T* Component, void (T::*CallBackStart)(), ESpellComponentCurrentAction ComponentAction)
	{
		check(Component);
		
		// We should not do few action in the same time. That's why we check current action state
		if (CurrentActionState != ESpellComponentActionState::Idle)
			return;

		CurrentActionState = ESpellComponentActionState::Prepare;
		CurrentAction = ComponentAction;
		PendingAction.BindUObject(Component, CallBackStart);
	}

	template<typename T>
	void HandleInputGeneric_Released(T* Component, void (T::*CallBackStop)(), ESpellComponentCurrentAction ComponentAction)
	{
		check(Component);

		// We react only on release if the same action that currently in progress got release command
		if (ComponentAction != CurrentAction)
			return;

		// Call end of input callback. God, please forgive me this ugly syntax
		(Component->*(CallBackStop))();
	}

	float Prepare(float InpurValue);

	DelVoidVoid PendingAction;

	// Use UPROPERTY to avoid garbage collection of this object
	UPROPERTY()
	class UGMResourceAcquirer* ResAcq;

	// Use UPROPERTY to avoid garbage collection of this object
	UPROPERTY()
	class UGMSpellCaster* SpellCaster;

	UPROPERTY()
	class UGMSpellReleaser* SpellReleaser;

	ESpellComponentCurrentAction CurrentAction;
	ESpellComponentActionState CurrentActionState;

	float MaximumMovmentInput;
	float MovementOffset;
};
