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

	//--------------------------------------------
	// Item instances: Creating
	//--------------------------------------------
	/**
	 * @brief Creates an item instance, adding it to this inventory
	 * @param ItemClass UItemInstance subclass to create a new instance of
	 * @param ItemData Data to initialize the new instance with
	 * @return if executed on server, a pointer to the newly created instance, or nullptr if on client
	 */
	UFUNCTION(BlueprintCallable)
	const UItemInstance* CreateItemInInventory(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData);

	UFUNCTION(Server, Reliable)
	void ServerCreateItemInInventory(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData);

	/**
	 * @brief Get const reference to all items in the inventory
	 */
	UFUNCTION(BlueprintCallable)
	const TArray<UItemInstance*>& GetItemInstances() { return Items; }

public:
	//--------------------------------------------
	// Item actors: Spawning & Destroying 
	//--------------------------------------------
	/**
	 * @brief Performs authority checks and validation before spawning
	 * an ItemActor.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Items")
	virtual void ServerSpawnItemActor(UItemInstance* InItemInstance);
	/**
	 * @brief Performs authority checks and validation before destroying
	 * an ItemActor.
	 */
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Items")
	virtual void ServerDestroyItemActor(UItemInstance* InItemInstance);

	/** Indicates whether or not the ItemActor is already in the scene (true if yes, false if not)*/
	virtual bool IsItemActorSpawned(UItemInstance* InItemInstance) const;
private:
	/**
	 * @brief Set of all item instances that have a currently spawned actor
	 */
	TSet<UItemInstance*> SpawnedItemActors;
private:
	UPROPERTY(ReplicatedUsing = OnRep_Items)
	TArray<TObjectPtr<UItemInstance>> Items;

	UFUNCTION()
	virtual void OnRep_Items();
};
