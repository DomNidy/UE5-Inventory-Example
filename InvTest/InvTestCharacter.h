// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ItemInstance.h"
#include "InvTestCharacter.generated.h"



UCLASS(Blueprintable)
class AInvTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AInvTestCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns InventoryComponent subobject **/
	FORCEINLINE class UInventoryComponent* GetInventory() const { return Inventory; }

	/**
	 * @brief Items that should be granted after creating the inventory
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray < FItemInstanceInitializer> ItemsToGrant;

private:
	virtual void PostInitializeComponents() override;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* Inventory;
};

