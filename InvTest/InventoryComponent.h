// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVTEST_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(UInventoryComponent, Items);
	}

	// Creates an item instance using ItemData
	UFUNCTION(BlueprintCallable)
	void CreateItemInInventory(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::CreateItemInInventory called on %s"), GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client"));

		FItemInstanceInitializer ItemInitializer;
		ItemInitializer.Outer = this;
		ItemInitializer.OwnerActor = GetOwner();
		ItemInitializer.ItemClass = ItemClass;
		ItemInitializer.ItemData = ItemData;

		UItemInstance* Item = UItemInstance::CreateItemInstance(ItemInitializer);
		Items.Add(Item);

		// For fun try spawn actor
		Item->SpawnItemActor();
	}

private:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<UItemInstance>> Items;
};
